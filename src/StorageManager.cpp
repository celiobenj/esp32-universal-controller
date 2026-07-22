#include "StorageManager.h"
#include <string.h>

void StorageManager::init() {
    // Preferences does not require global initialization.
}

void StorageManager::saveIOConfig(const IOConfig& config) {
    Preferences prefs;
    prefs.begin(NS_IO, false);
    prefs.putChar("inPin", config.inputPin);
    prefs.putUChar("inMode", (uint8_t)config.inputMode);
    prefs.putChar("outPin", config.outputPin);
    prefs.putUChar("outMode", (uint8_t)config.outputMode);
    prefs.putFloat("inGain", config.inputGain);
    prefs.putFloat("inOffset", config.inputOffset);
    prefs.putUChar("avgSamp", config.movingAvgSamples);
    prefs.putUInt("pwmFreq", config.pwmFrequency);
    prefs.putChar("spPin", config.setpointPin);
    prefs.putUChar("spSrc", (uint8_t)config.setpointSource);
    prefs.end();
}

void StorageManager::saveControlConfig(const ControlConfig& config) {
    Preferences prefs;
    prefs.begin(NS_CTRL, false);
    prefs.putUChar("strat", (uint8_t)config.strategy);
    prefs.putFloat("kp", config.kp);
    prefs.putFloat("ki", config.ki);
    prefs.putFloat("kd", config.kd);
    prefs.putFloat("bangH", config.bangHigh);
    prefs.putFloat("bangL", config.bangLow);
    prefs.putBytes("b", config.b, sizeof(config.b));
    prefs.putBytes("a", config.a, sizeof(config.a));
    prefs.putFloat("outMin", config.outputMin);
    prefs.putFloat("outMax", config.outputMax);
    prefs.putFloat("sp", config.setpoint);
    prefs.putBool("run", config.running);
    prefs.end();
}

void StorageManager::saveWiFiConfig(const WiFiConfig& config) {
    Preferences prefs;
    prefs.begin(NS_WIFI, false);
    prefs.putString("ssid", config.ssid);
    prefs.putString("pass", config.password);
    prefs.putBool("conf", config.configured);
    prefs.end();
}

IOConfig StorageManager::loadIOConfig() {
    IOConfig config;
    Preferences prefs;
    prefs.begin(NS_IO, true);
    if (prefs.isKey("inPin")) config.inputPin = prefs.getChar("inPin", config.inputPin);
    if (prefs.isKey("inMode")) config.inputMode = (PinMode_t)prefs.getUChar("inMode", (uint8_t)config.inputMode);
    if (prefs.isKey("outPin")) config.outputPin = prefs.getChar("outPin", config.outputPin);
    if (prefs.isKey("outMode")) config.outputMode = (PinMode_t)prefs.getUChar("outMode", (uint8_t)config.outputMode);
    if (prefs.isKey("inGain")) config.inputGain = prefs.getFloat("inGain", config.inputGain);
    if (prefs.isKey("inOffset")) config.inputOffset = prefs.getFloat("inOffset", config.inputOffset);
    if (prefs.isKey("avgSamp")) config.movingAvgSamples = prefs.getUChar("avgSamp", config.movingAvgSamples);
    if (prefs.isKey("pwmFreq")) config.pwmFrequency = prefs.getUInt("pwmFreq", config.pwmFrequency);
    if (prefs.isKey("spPin")) config.setpointPin = prefs.getChar("spPin", config.setpointPin);
    if (prefs.isKey("spSrc")) config.setpointSource = (SetpointSrc_t)prefs.getUChar("spSrc", (uint8_t)config.setpointSource);
    prefs.end();
    return config;
}

ControlConfig StorageManager::loadControlConfig() {
    ControlConfig config;
    Preferences prefs;
    prefs.begin(NS_CTRL, true);
    if (prefs.isKey("strat")) config.strategy = (Strategy_t)prefs.getUChar("strat", (uint8_t)config.strategy);
    if (prefs.isKey("kp")) config.kp = prefs.getFloat("kp", config.kp);
    if (prefs.isKey("ki")) config.ki = prefs.getFloat("ki", config.ki);
    if (prefs.isKey("kd")) config.kd = prefs.getFloat("kd", config.kd);
    if (prefs.isKey("bangH")) config.bangHigh = prefs.getFloat("bangH", config.bangHigh);
    if (prefs.isKey("bangL")) config.bangLow = prefs.getFloat("bangL", config.bangLow);
    if (prefs.isKey("b")) prefs.getBytes("b", config.b, sizeof(config.b));
    if (prefs.isKey("a")) prefs.getBytes("a", config.a, sizeof(config.a));
    if (prefs.isKey("outMin")) config.outputMin = prefs.getFloat("outMin", config.outputMin);
    if (prefs.isKey("outMax")) config.outputMax = prefs.getFloat("outMax", config.outputMax);
    if (prefs.isKey("sp")) config.setpoint = prefs.getFloat("sp", config.setpoint);
    if (prefs.isKey("run")) config.running = prefs.getBool("run", config.running);
    prefs.end();
    return config;
}

WiFiConfig StorageManager::loadWiFiConfig() {
    WiFiConfig config;
    Preferences prefs;
    prefs.begin(NS_WIFI, true);
    if (prefs.isKey("ssid")) {
        String ssid = prefs.getString("ssid", "");
        strncpy(config.ssid, ssid.c_str(), sizeof(config.ssid) - 1);
        config.ssid[sizeof(config.ssid) - 1] = '\0';
    }
    if (prefs.isKey("pass")) {
        String pass = prefs.getString("pass", "");
        strncpy(config.password, pass.c_str(), sizeof(config.password) - 1);
        config.password[sizeof(config.password) - 1] = '\0';
    }
    if (prefs.isKey("conf")) config.configured = prefs.getBool("conf", config.configured);
    prefs.end();
    return config;
}

bool StorageManager::hasStoredConfig() {
    Preferences prefs;
    prefs.begin(NS_IO, true);
    bool hasKey = prefs.isKey("inPin");
    prefs.end();
    return hasKey;
}

void StorageManager::clearAll() {
    Preferences prefs;
    prefs.begin(NS_IO, false);
    prefs.clear();
    prefs.end();
    
    prefs.begin(NS_CTRL, false);
    prefs.clear();
    prefs.end();
    
    prefs.begin(NS_WIFI, false);
    prefs.clear();
    prefs.end();
}
