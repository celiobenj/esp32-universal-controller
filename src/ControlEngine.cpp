#include "ControlEngine.h"

ControlEngine::ControlEngine() {
    _mutex = xSemaphoreCreateMutex();
}

ControlEngine::~ControlEngine() {
    stop();
    if (_mutex != nullptr) {
        vSemaphoreDelete(_mutex);
        _mutex = nullptr;
    }
}

void ControlEngine::start() {
    // Initialize HAL with current IO configuration
    _hal.init(_ioCfg);
    resetState();
    
    if (_taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            _taskFunc, 
            "CtrlEngine", 
            8192, 
            this, 
            10, 
            &_taskHandle, 
            1
        );
    }
}

void ControlEngine::stop() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }
    _hal.writeOutput(0.0f);
    resetState();
}

void ControlEngine::setControlConfig(const ControlConfig& config) {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _ctrlCfg = config;
    xSemaphoreGive(_mutex);
}

ControlConfig ControlEngine::getControlConfig() {
    ControlConfig config;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    config = _ctrlCfg;
    xSemaphoreGive(_mutex);
    return config;
}

void ControlEngine::setIOConfig(const IOConfig& config) {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _ioCfg = config;
    _hal.updateConfig(config);
    xSemaphoreGive(_mutex);
}

IOConfig ControlEngine::getIOConfig() {
    IOConfig config;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    config = _ioCfg;
    xSemaphoreGive(_mutex);
    return config;
}

void ControlEngine::setSetpoint(float sp) {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    _ctrlCfg.setpoint = sp;
    xSemaphoreGive(_mutex);
}

void ControlEngine::resetState() {
    _integral = 0.0f;
    _prevError = 0.0f;
    _bangState = false;
    
    for (int i = 0; i < MAX_DIFF_ORDER + 1; i++) {
        _errorHist[i] = 0.0f;
    }
    for (int i = 0; i < MAX_DIFF_ORDER; i++) {
        _outputHist[i] = 0.0f;
    }
    
    _decimCounter = 0;
}

bool ControlEngine::hasTelemetry() {
    return _telemetryReady;
}

TelemetryPacket ControlEngine::getTelemetry() {
    TelemetryPacket pkt = _telemetryPkt;
    _telemetryReady = false;
    return pkt;
}

void ControlEngine::_taskFunc(void* param) {
    ControlEngine* engine = static_cast<ControlEngine*>(param);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(CONTROL_PERIOD_MS);

    while (true) {
        engine->_loop();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void ControlEngine::_loop() {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    ControlConfig ctrlCfg = _ctrlCfg;
    IOConfig ioCfg = _ioCfg;
    xSemaphoreGive(_mutex);

    if (!ctrlCfg.running) {
        _hal.writeOutput(0.0f);
        return;
    }

    float input = _hal.readInput();
    float setpoint = (ioCfg.setpointSource == SetpointSrc_t::EXTERNAL_GPIO) ? _hal.readSetpointExternal() : ctrlCfg.setpoint;
    float error = setpoint - input;
    float output = 0.0f;

    switch (ctrlCfg.strategy) {
        case Strategy_t::DIRECT:
            output = _computeDirect(input);
            break;
        case Strategy_t::BANG_BANG:
            output = _computeBangBang(input, setpoint);
            break;
        case Strategy_t::PID:
            output = _computePID(error);
            break;
        case Strategy_t::DIFF_EQUATION:
            output = _computeDiffEquation(error);
            break;
    }

    output = _clampOutput(output);
    _hal.writeOutput(output);

    _decimCounter++;
    if (_decimCounter >= TELEMETRY_DECIMATION) {
        _telemetryPkt.timestamp = millis();
        _telemetryPkt.input = input;
        _telemetryPkt.output = output;
        _telemetryPkt.setpoint = setpoint;
        _telemetryPkt.error = error;
        _telemetryReady = true;
        _decimCounter = 0;
    }
}

float ControlEngine::_computeDirect(float input) {
    return input;
}

float ControlEngine::_computeBangBang(float input, float setpoint) {
    float bangLow, bangHigh, outMax, outMin;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    bangLow = _ctrlCfg.bangLow;
    bangHigh = _ctrlCfg.bangHigh;
    outMax = _ctrlCfg.outputMax;
    outMin = _ctrlCfg.outputMin;
    xSemaphoreGive(_mutex);
    
    if (input < bangLow) {
        _bangState = true;
    } else if (input > bangHigh) {
        _bangState = false;
    }
    
    return _bangState ? outMax : outMin;
}

float ControlEngine::_computePID(float error) {
    float kp, ki, kd, outMax, outMin;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    kp = _ctrlCfg.kp;
    ki = _ctrlCfg.ki;
    kd = _ctrlCfg.kd;
    outMax = _ctrlCfg.outputMax;
    outMin = _ctrlCfg.outputMin;
    xSemaphoreGive(_mutex);

    float P = kp * error;
    _integral += ki * error * CONTROL_DT;
    float D = kd * (error - _prevError) / CONTROL_DT;
    
    float output_raw = P + _integral + D;
    
    // Anti-windup (clamping)
    if (output_raw > outMax || output_raw < outMin) {
        _integral -= ki * error * CONTROL_DT;
    }
    
    _prevError = error;
    return output_raw;
}

float ControlEngine::_computeDiffEquation(float error) {
    float b[MAX_DIFF_ORDER + 1];
    float a[MAX_DIFF_ORDER];
    xSemaphoreTake(_mutex, portMAX_DELAY);
    for (int i = 0; i < MAX_DIFF_ORDER + 1; i++) {
        b[i] = _ctrlCfg.b[i];
    }
    for (int i = 0; i < MAX_DIFF_ORDER; i++) {
        a[i] = _ctrlCfg.a[i];
    }
    xSemaphoreGive(_mutex);

    // Shift error history: move e[k-1]->e[k-2], etc.
    for (int i = MAX_DIFF_ORDER; i > 0; i--) {
        _errorHist[i] = _errorHist[i-1];
    }
    _errorHist[0] = error;
    
    float u = 0.0f;
    for (int i = 0; i <= MAX_DIFF_ORDER; i++) {
        u += b[i] * _errorHist[i];
    }
    for (int j = 0; j < MAX_DIFF_ORDER; j++) {
        u -= a[j] * _outputHist[j];
    }
    
    // Shift output history
    for (int i = MAX_DIFF_ORDER - 1; i > 0; i--) {
        _outputHist[i] = _outputHist[i-1];
    }
    _outputHist[0] = u;
    
    return u;
}

float ControlEngine::_clampOutput(float value) {
    float outMin, outMax;
    xSemaphoreTake(_mutex, portMAX_DELAY);
    outMin = _ctrlCfg.outputMin;
    outMax = _ctrlCfg.outputMax;
    xSemaphoreGive(_mutex);

    if (value > outMax) return outMax;
    if (value < outMin) return outMin;
    return value;
}
