/**
 * main.cpp — System Bootstrap and Orchestration
 * 
 * ESP32 Universal Control & DAQ System
 * 
 * Boot sequence:
 *   1. Serial init
 *   2. StorageManager: load saved configs from NVS
 *   3. WiFi: start AP + optional STA connection
 *   4. ControlEngine: init HAL and start control task on Core 1
 *   5. WebManager: start async web server on Core 0
 *   6. Main loop: push telemetry + periodic status updates
 */

#include <Arduino.h>
#include <WiFi.h>

#include "Config.h"
#include "StorageManager.h"
#include "ControlEngine.h"
#include "WebManager.h"

// ============================================================
//  Global Instances
// ============================================================

StorageManager storage;
ControlEngine  engine;
WebManager     web;

// ============================================================
//  WiFi Setup
// ============================================================

void setupWiFi() {
    // Always start the Access Point (open network)
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID);

    Serial.println("--------------------------------------");
    Serial.println(" ESP32 Universal Controller");
    Serial.println("--------------------------------------");
    Serial.printf(" AP SSID : %s (open)\n", AP_SSID);
    Serial.printf(" AP IP   : %s\n", WiFi.softAPIP().toString().c_str());

    // Attempt STA connection if credentials are stored
    WiFiConfig wifiCfg = storage.loadWiFiConfig();
    if (wifiCfg.configured && strlen(wifiCfg.ssid) > 0) {
        Serial.printf(" STA     : Connecting to '%s'...\n", wifiCfg.ssid);
        WiFi.begin(wifiCfg.ssid, strlen(wifiCfg.password) > 0 ? wifiCfg.password : nullptr);

        // Wait up to 10 seconds for connection
        unsigned long start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
            delay(250);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("\n STA IP  : %s\n", WiFi.localIP().toString().c_str());
            Serial.printf(" RSSI    : %d dBm\n", WiFi.RSSI());
        } else {
            Serial.println("\n STA     : Connection failed (AP still active)");
        }
    } else {
        Serial.println(" STA     : No credentials stored");
    }
    Serial.println("--------------------------------------");
}

// ============================================================
//  Arduino Setup
// ============================================================

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n[Boot] Initializing...");

    // 1. Storage: load persisted configuration
    storage.init();
    Serial.println("[Boot] Storage initialized");

    // 2. WiFi: AP + STA
    setupWiFi();

    // 3. Control Engine: load config and start
    if (storage.hasStoredConfig()) {
        IOConfig ioCfg       = storage.loadIOConfig();
        ControlConfig ctrlCfg = storage.loadControlConfig();
        engine.setIOConfig(ioCfg);
        engine.setControlConfig(ctrlCfg);
        Serial.println("[Boot] Loaded config from NVS");
    } else {
        Serial.println("[Boot] No stored config, using defaults");
    }

    // Start the control engine (creates FreeRTOS task on Core 1)
    engine.start();
    Serial.println("[Boot] ControlEngine started (Core 1, 100 Hz)");

    // 4. Web Manager: start async web server
    web.start(&engine, &storage);
    Serial.println("[Boot] WebManager started (port 80)");

    Serial.println("[Boot] System ready!");
    Serial.printf("[Boot] Free heap: %u bytes\n", ESP.getFreeHeap());
}

// ============================================================
//  Arduino Main Loop (Core 0)
// ============================================================

void loop() {
    // Push telemetry over WebSocket (non-blocking)
    web.loop();

    // Small yield to avoid WDT on Core 0
    delay(1);
}