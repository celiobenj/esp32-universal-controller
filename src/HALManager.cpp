#include "HALManager.h"
#include <Arduino.h>

void HALManager::init(const IOConfig& config) {
    _config = config;
    _avgIndex = 0;
    _avgSum = 0.0f;
    _avgCount = 0;
    for (uint8_t i = 0; i < MAX_AVG_SAMPLES; i++) {
        _avgBuffer[i] = 0.0f;
    }
    _configureInput();
    _configureOutput();
}

void HALManager::updateConfig(const IOConfig& config) {
    _teardown();
    init(config);
}

void HALManager::_configureInput() {
    // Ensure 12-bit ADC resolution for consistent 0-4095 range
    analogReadResolution(12);
    
    if (_config.inputPin >= 0) {
        pinMode(_config.inputPin, INPUT);
    }
    if (_config.setpointPin >= 0 && _config.setpointSource == SetpointSrc_t::EXTERNAL_GPIO) {
        pinMode(_config.setpointPin, INPUT);
    }
}

void HALManager::_configureOutput() {
    if (_config.outputPin >= 0) {
        if (_config.outputMode == PinMode_t::PWM_OUTPUT) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
            // Arduino Core v3.x LEDC API
            ledcAttach(_config.outputPin, _config.pwmFrequency, PWM_RESOLUTION_BITS);
#else
            // Arduino Core v2.x LEDC API
            ledcSetup(LEDC_CHANNEL, _config.pwmFrequency, PWM_RESOLUTION_BITS);
            ledcAttachPin(_config.outputPin, LEDC_CHANNEL);
#endif
        } else if (_config.outputMode == PinMode_t::DIGITAL_OUTPUT) {
            pinMode(_config.outputPin, OUTPUT);
            digitalWrite(_config.outputPin, LOW);
        }
    }
}

void HALManager::_teardown() {
    if (_config.outputPin >= 0) {
        if (_config.outputMode == PinMode_t::PWM_OUTPUT) {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
            ledcDetach(_config.outputPin);
#else
            ledcDetachPin(_config.outputPin);
#endif
        }
        pinMode(_config.outputPin, INPUT);
    }
    if (_config.inputPin >= 0) {
        pinMode(_config.inputPin, INPUT);
    }
    if (_config.setpointPin >= 0) {
        pinMode(_config.setpointPin, INPUT);
    }
}

float HALManager::_applyMovingAverage(float raw) {
    uint8_t samples = _config.movingAvgSamples;
    if (samples == 0) return raw;
    if (samples > MAX_AVG_SAMPLES) samples = MAX_AVG_SAMPLES;

    if (_avgCount < samples) {
        _avgBuffer[_avgCount] = raw;
        _avgSum += raw;
        _avgCount++;
        return _avgSum / _avgCount;
    }

    _avgSum -= _avgBuffer[_avgIndex];
    _avgBuffer[_avgIndex] = raw;
    _avgSum += raw;
    
    _avgIndex++;
    if (_avgIndex >= samples) {
        _avgIndex = 0;
    }
    
    return _avgSum / samples;
}

float HALManager::readInput() {
    if (_config.inputPin < 0) {
        return 0.0f;
    }
    if (_config.inputMode == PinMode_t::ANALOG_INPUT) {
        float raw = (float)analogRead(_config.inputPin);
        float filtered = _applyMovingAverage(raw);
        return filtered * _config.inputGain + _config.inputOffset;
    } else if (_config.inputMode == PinMode_t::DIGITAL_INPUT) {
        return (float)digitalRead(_config.inputPin);
    }
    return 0.0f;
}

float HALManager::readSetpointExternal() {
    if (_config.setpointPin >= 0 && _config.setpointSource == SetpointSrc_t::EXTERNAL_GPIO) {
        float raw = (float)analogRead(_config.setpointPin);
        // Scale to 0-PWM_MAX_DUTY range
        return (raw / 4095.0f) * PWM_MAX_DUTY;
    }
    return 0.0f;
}

void HALManager::writeOutput(float value) {
    if (_config.outputPin < 0) return;
    
    if (_config.outputMode == PinMode_t::DIGITAL_OUTPUT) {
        digitalWrite(_config.outputPin, value >= 0.5f ? HIGH : LOW);
    } else if (_config.outputMode == PinMode_t::PWM_OUTPUT) {
        if (value < 0.0f) value = 0.0f;
        if (value > PWM_MAX_DUTY) value = PWM_MAX_DUTY;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        ledcWrite(_config.outputPin, (uint32_t)value);
#else
        ledcWrite(LEDC_CHANNEL, (uint32_t)value);
#endif
    }
}
