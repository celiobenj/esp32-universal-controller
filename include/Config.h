/**
 * Config.h — Shared Configuration Types and Constants
 * 
 * Central definitions for the Universal Embedded Control & DAQ System.
 * All modules include this header for consistent type usage.
 * 
 * Target: ESP32-WROOM-32 (DevKitC)
 */

#pragma once
#include <Arduino.h>

// ============================================================
//  GPIO Validation Lists — ESP32-WROOM-32
//  Excluded: GPIO 0 (boot), 1 (TX), 3 (RX), 6-11 (flash SPI),
//            12 (boot strapping)
// ============================================================

// ADC1 pins only — ADC2 does not work when WiFi is active
constexpr int8_t VALID_ADC_PINS[]    = {32, 33, 34, 35, 36, 39};
constexpr uint8_t NUM_ADC_PINS       = sizeof(VALID_ADC_PINS) / sizeof(VALID_ADC_PINS[0]);

// General-purpose digital input pins
constexpr int8_t VALID_DIN_PINS[]    = {4, 5, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39};
constexpr uint8_t NUM_DIN_PINS       = sizeof(VALID_DIN_PINS) / sizeof(VALID_DIN_PINS[0]);

// Digital/PWM output pins — excludes input-only GPIOs 34-39
constexpr int8_t VALID_DOUT_PINS[]   = {2, 4, 5, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
constexpr uint8_t NUM_DOUT_PINS      = sizeof(VALID_DOUT_PINS) / sizeof(VALID_DOUT_PINS[0]);

// ============================================================
//  Enumerations
// ============================================================

enum class PinMode_t : uint8_t {
    NONE = 0,
    DIGITAL_INPUT,
    DIGITAL_OUTPUT,
    ANALOG_INPUT,
    PWM_OUTPUT
};

enum class Strategy_t : uint8_t {
    DIRECT = 0,
    BANG_BANG,
    PID,
    DIFF_EQUATION
};

enum class SetpointSrc_t : uint8_t {
    WEB_UI = 0,
    EXTERNAL_GPIO
};

// ============================================================
//  System Constants
// ============================================================

// Difference equation maximum order
constexpr uint8_t  MAX_DIFF_ORDER      = 3;

// Moving average filter
constexpr uint8_t  MAX_AVG_SAMPLES     = 16;

// PWM (LEDC) defaults
constexpr uint8_t  PWM_RESOLUTION_BITS = 10;
constexpr uint32_t PWM_MAX_DUTY        = (1 << PWM_RESOLUTION_BITS) - 1;  // 1023
constexpr uint32_t PWM_DEFAULT_FREQ    = 5000;                            // 5 kHz

// LEDC channel assignment (Arduino Core v2.x)
constexpr uint8_t  LEDC_CHANNEL        = 0;

// Control loop timing
constexpr uint32_t CONTROL_FREQ_HZ     = 100;
constexpr uint32_t CONTROL_PERIOD_MS   = 1000 / CONTROL_FREQ_HZ;          // 10 ms
constexpr float    CONTROL_DT          = 1.0f / (float)CONTROL_FREQ_HZ;   // 0.01 s

// Telemetry (WebSocket) timing
constexpr uint32_t TELEMETRY_FREQ_HZ   = 20;
constexpr uint8_t  TELEMETRY_DECIMATION = CONTROL_FREQ_HZ / TELEMETRY_FREQ_HZ;  // 5

// Plotter buffer (browser-side)
constexpr uint16_t PLOTTER_BUFFER_SIZE = 500;

// WiFi Access Point — open network (no password)
#define AP_SSID "ESP32-Controller"

// ============================================================
//  Data Structures
// ============================================================

/**
 * I/O channel configuration.
 * Defines which GPIOs are used and how analog signals are scaled.
 */
struct IOConfig {
    // Input channel
    int8_t      inputPin          = -1;
    PinMode_t   inputMode         = PinMode_t::NONE;

    // Output channel
    int8_t      outputPin         = -1;
    PinMode_t   outputMode        = PinMode_t::NONE;

    // ADC linear scaling: value_eng = raw * gain + offset
    float       inputGain         = 1.0f;
    float       inputOffset       = 0.0f;
    uint8_t     movingAvgSamples  = 8;

    // PWM frequency (Hz)
    uint32_t    pwmFrequency      = PWM_DEFAULT_FREQ;

    // External setpoint source
    int8_t      setpointPin       = -1;
    SetpointSrc_t setpointSource  = SetpointSrc_t::WEB_UI;
};

/**
 * Control algorithm configuration.
 * Holds parameters for all 4 strategies; only the active one is used.
 */
struct ControlConfig {
    Strategy_t  strategy          = Strategy_t::DIRECT;

    // PID gains
    float       kp                = 1.0f;
    float       ki                = 0.0f;
    float       kd                = 0.0f;

    // Bang-Bang thresholds
    float       bangHigh          = 3000.0f;
    float       bangLow           = 1000.0f;

    // Difference equation coefficients
    //   u[k] = sum_{i=0}^{N} b[i]*e[k-i] - sum_{j=1}^{N} a[j]*u[k-j]
    float       b[MAX_DIFF_ORDER + 1] = {1.0f, 0.0f, 0.0f, 0.0f};   // b0 .. b3
    float       a[MAX_DIFF_ORDER]     = {0.0f, 0.0f, 0.0f};          // a1 .. a3

    // Output clamping limits
    float       outputMin         = 0.0f;
    float       outputMax         = (float)PWM_MAX_DUTY;              // 1023.0

    // Setpoint value (when source = WEB_UI)
    float       setpoint          = 0.0f;

    // Control loop running state
    bool        running           = false;
};

/**
 * Telemetry data packet — sent via WebSocket at TELEMETRY_FREQ_HZ.
 */
struct TelemetryPacket {
    uint32_t    timestamp;    // millis()
    float       input;        // Scaled input value
    float       output;       // Control output value
    float       setpoint;     // Active setpoint
    float       error;        // setpoint - input
};

/**
 * WiFi station credentials for STA mode.
 */
struct WiFiConfig {
    char        ssid[33]      = {0};
    char        password[65]  = {0};
    bool        configured    = false;
};
