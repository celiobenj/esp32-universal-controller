#pragma once

#include "Config.h"
#include "HALManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

class ControlEngine {
public:
    ControlEngine();
    ~ControlEngine();
    
    void start();      // Creates FreeRTOS task on Core 1, priority 10
    void stop();       // Deletes task, resets internal state
    
    void setControlConfig(const ControlConfig& config);  // Thread-safe
    ControlConfig getControlConfig();                      // Thread-safe
    
    void setIOConfig(const IOConfig& config);  // Thread-safe, updates HAL
    IOConfig getIOConfig();                     // Thread-safe
    
    void setSetpoint(float sp);   // Quick setpoint update (from WebSocket)
    
    void resetState();  // Clears PID integral, error history, etc.
    
    // Telemetry access (called from Core 0 by WebManager)
    bool hasTelemetry();
    TelemetryPacket getTelemetry();

private:
    static void _taskFunc(void* param);
    void _loop();  // Single iteration of the control loop
    
    // Control strategies
    float _computeDirect(float input);
    float _computeBangBang(float input, float setpoint);
    float _computePID(float error);
    float _computeDiffEquation(float error);
    float _clampOutput(float value);
    
    // FreeRTOS
    TaskHandle_t _taskHandle = nullptr;
    SemaphoreHandle_t _mutex = nullptr;
    
    // Owned HAL instance
    HALManager _hal;
    
    // Configuration (mutex-protected)
    ControlConfig _ctrlCfg;
    IOConfig _ioCfg;
    
    // PID state
    float _integral = 0.0f;
    float _prevError = 0.0f;
    
    // Bang-Bang state
    bool _bangState = false;  // Current ON/OFF state for hysteresis
    
    // Difference equation history buffers
    float _errorHist[MAX_DIFF_ORDER + 1] = {0};   // e[k], e[k-1], ...
    float _outputHist[MAX_DIFF_ORDER] = {0};       // u[k-1], u[k-2], ...
    
    // Telemetry
    volatile bool _telemetryReady = false;
    TelemetryPacket _telemetryPkt;
    uint8_t _decimCounter = 0;
};
