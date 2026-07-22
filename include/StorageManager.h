#pragma once

#include "Config.h"
#include <Preferences.h>

class StorageManager {
public:
    void init();
    
    void saveIOConfig(const IOConfig& config);
    void saveControlConfig(const ControlConfig& config);
    void saveWiFiConfig(const WiFiConfig& config);
    
    IOConfig loadIOConfig();
    ControlConfig loadControlConfig();
    WiFiConfig loadWiFiConfig();
    
    bool hasStoredConfig();
    void clearAll();

private:
    static constexpr const char* NS_IO = "io";
    static constexpr const char* NS_CTRL = "ctrl";
    static constexpr const char* NS_WIFI = "wifi";
};
