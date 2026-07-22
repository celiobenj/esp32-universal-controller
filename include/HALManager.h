#pragma once

#include "Config.h"

class HALManager {
public:
    void init(const IOConfig& config);
    void updateConfig(const IOConfig& config);  // Tears down old pins, configures new
    float readInput();           // Returns scaled+filtered value (analog) or 0/1 (digital)
    float readSetpointExternal(); // Reads analog setpoint from external GPIO
    void writeOutput(float value); // Writes to digital (0/1 threshold) or PWM (duty cycle)

private:
    IOConfig _config;
    float _avgBuffer[MAX_AVG_SAMPLES];
    uint8_t _avgIndex = 0;
    float _avgSum = 0.0f;
    uint8_t _avgCount = 0;
    
    void _configureInput();
    void _configureOutput();
    void _teardown();
    float _applyMovingAverage(float raw);
};
