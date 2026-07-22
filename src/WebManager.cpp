/**
 * WebManager.cpp — Async Web Server & WebSocket Implementation
 *
 * Serves the embedded UI, handles HTTP config APIs,
 * and pushes real-time telemetry over WebSocket at ~20 Hz.
 */

#include "WebManager.h"
#include "WebContent.h"
#include "ControlEngine.h"
#include "StorageManager.h"
#include <WiFi.h>

// ============================================================
//  Constructor
// ============================================================

WebManager::WebManager()
    : _server(80), _ws("/ws") {}

// ============================================================
//  Public Methods
// ============================================================

void WebManager::start(ControlEngine* engine, StorageManager* storage) {
    _engine  = engine;
    _storage = storage;

    // WebSocket event handler
    _ws.onEvent([this](AsyncWebSocket* ws, AsyncWebSocketClient* client,
                       AwsEventType type, void* arg, uint8_t* data, size_t len) {
        _onWsEvent(ws, client, type, arg, data, len);
    });
    _server.addHandler(&_ws);

    _setupRoutes();
    _server.begin();
    Serial.println("[WebManager] Server started on port 80");
}

void WebManager::loop() {
    // Push telemetry if available and enough time has passed
    if (_engine && _engine->hasTelemetry()) {
        TelemetryPacket pkt = _engine->getTelemetry();

        // Build compact JSON telemetry message
        JsonDocument doc;
        doc["type"] = "tel";
        doc["t"]    = pkt.timestamp;
        
        // Use 2 decimal precision natively, avoiding String allocations
        doc["in"]   = round(pkt.input * 100.0) / 100.0;
        doc["out"]  = round(pkt.output * 100.0) / 100.0;
        doc["sp"]   = round(pkt.setpoint * 100.0) / 100.0;
        doc["err"]  = round(pkt.error * 100.0) / 100.0;

        // Only serialize and send if there are connected clients
        if (_ws.count() > 0) {
            String msg;
            serializeJson(doc, msg);
            _ws.textAll(msg);
        }
    }

    // Periodic cleanup of disconnected clients
    _ws.cleanupClients();
}

// ============================================================
//  HTTP Routes Setup
// ============================================================

void WebManager::_setupRoutes() {
    // --- Serve the embedded index page ---
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _serveIndex(req);
    });

    // --- GET current configuration ---
    _server.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* req) {
        _handleGetConfig(req);
    });

    // --- POST I/O configuration ---
    _server.on("/api/io-config", HTTP_POST,
        // Request handler (called after body is received)
        [](AsyncWebServerRequest* req) {},
        // Upload handler (not used)
        nullptr,
        // Body handler
        [this](AsyncWebServerRequest* req, uint8_t* data, size_t len,
               size_t index, size_t total) {
            if (index == 0) {
                // First chunk — store temporarily
                req->_tempObject = malloc(total + 1);
                if (req->_tempObject) {
                    memset(req->_tempObject, 0, total + 1);
                }
            }
            if (req->_tempObject) {
                memcpy((uint8_t*)req->_tempObject + index, data, len);
                if (index + len == total) {
                    _handlePostIOConfig(req, (uint8_t*)req->_tempObject, total);
                    free(req->_tempObject);
                    req->_tempObject = nullptr;
                }
            }
        }
    );

    // --- POST Control configuration ---
    _server.on("/api/control-config", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        nullptr,
        [this](AsyncWebServerRequest* req, uint8_t* data, size_t len,
               size_t index, size_t total) {
            if (index == 0) {
                req->_tempObject = malloc(total + 1);
                if (req->_tempObject) memset(req->_tempObject, 0, total + 1);
            }
            if (req->_tempObject) {
                memcpy((uint8_t*)req->_tempObject + index, data, len);
                if (index + len == total) {
                    _handlePostControlConfig(req, (uint8_t*)req->_tempObject, total);
                    free(req->_tempObject);
                    req->_tempObject = nullptr;
                }
            }
        }
    );

    // --- POST Save to NVS ---
    _server.on("/api/save", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handlePostSave(req);
    });

    // --- POST Restore defaults ---
    _server.on("/api/defaults", HTTP_POST, [this](AsyncWebServerRequest* req) {
        _handlePostDefaults(req);
    });

    // --- POST WiFi configuration ---
    _server.on("/api/wifi", HTTP_POST,
        [](AsyncWebServerRequest* req) {},
        nullptr,
        [this](AsyncWebServerRequest* req, uint8_t* data, size_t len,
               size_t index, size_t total) {
            if (index == 0) {
                req->_tempObject = malloc(total + 1);
                if (req->_tempObject) memset(req->_tempObject, 0, total + 1);
            }
            if (req->_tempObject) {
                memcpy((uint8_t*)req->_tempObject + index, data, len);
                if (index + len == total) {
                    _handlePostWiFi(req, (uint8_t*)req->_tempObject, total);
                    free(req->_tempObject);
                    req->_tempObject = nullptr;
                }
            }
        }
    );
}

// ============================================================
//  Route Handlers
// ============================================================

void WebManager::_serveIndex(AsyncWebServerRequest* req) {
    req->send(200, "text/html", INDEX_HTML);
}

void WebManager::_handleGetConfig(AsyncWebServerRequest* req) {
    if (!_engine) {
        req->send(500, "application/json", "{\"error\":\"Engine not ready\"}");
        return;
    }

    JsonDocument doc;
    doc["type"] = "config";

    // I/O Config
    IOConfig io = _engine->getIOConfig();
    JsonObject ioObj = doc["io"].to<JsonObject>();
    ioObj["inputPin"]        = io.inputPin;
    ioObj["inputMode"]       = (uint8_t)io.inputMode;
    ioObj["outputPin"]       = io.outputPin;
    ioObj["outputMode"]      = (uint8_t)io.outputMode;
    ioObj["inputGain"]       = io.inputGain;
    ioObj["inputOffset"]     = io.inputOffset;
    ioObj["movingAvgSamples"]= io.movingAvgSamples;
    ioObj["pwmFrequency"]    = io.pwmFrequency;
    ioObj["setpointPin"]     = io.setpointPin;
    ioObj["setpointSource"]  = (uint8_t)io.setpointSource;

    // Control Config
    ControlConfig ctrl = _engine->getControlConfig();
    JsonObject ctrlObj = doc["ctrl"].to<JsonObject>();
    ctrlObj["strategy"]   = (uint8_t)ctrl.strategy;
    ctrlObj["kp"]         = ctrl.kp;
    ctrlObj["ki"]         = ctrl.ki;
    ctrlObj["kd"]         = ctrl.kd;
    ctrlObj["bangHigh"]   = ctrl.bangHigh;
    ctrlObj["bangLow"]    = ctrl.bangLow;
    JsonArray bArr = ctrlObj["b"].to<JsonArray>();
    for (int i = 0; i <= MAX_DIFF_ORDER; i++) bArr.add(ctrl.b[i]);
    JsonArray aArr = ctrlObj["a"].to<JsonArray>();
    for (int i = 0; i < MAX_DIFF_ORDER; i++) aArr.add(ctrl.a[i]);
    ctrlObj["outputMin"]  = ctrl.outputMin;
    ctrlObj["outputMax"]  = ctrl.outputMax;
    ctrlObj["setpoint"]   = ctrl.setpoint;
    ctrlObj["running"]    = ctrl.running;

    String response;
    serializeJson(doc, response);
    req->send(200, "application/json", response);
}

void WebManager::_handlePostIOConfig(AsyncWebServerRequest* req, uint8_t* data, size_t len) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    IOConfig io = _engine->getIOConfig();

    if (doc["inputPin"].is<JsonVariant>())        io.inputPin        = doc["inputPin"].as<int8_t>();
    if (doc["inputMode"].is<JsonVariant>())       io.inputMode       = (PinMode_t)doc["inputMode"].as<uint8_t>();
    if (doc["outputPin"].is<JsonVariant>())       io.outputPin       = doc["outputPin"].as<int8_t>();
    if (doc["outputMode"].is<JsonVariant>())      io.outputMode      = (PinMode_t)doc["outputMode"].as<uint8_t>();
    if (doc["inputGain"].is<JsonVariant>())       io.inputGain       = doc["inputGain"].as<float>();
    if (doc["inputOffset"].is<JsonVariant>())     io.inputOffset     = doc["inputOffset"].as<float>();
    if (doc["movingAvgSamples"].is<JsonVariant>())io.movingAvgSamples= doc["movingAvgSamples"].as<uint8_t>();
    if (doc["pwmFrequency"].is<JsonVariant>())    io.pwmFrequency    = doc["pwmFrequency"].as<uint32_t>();
    if (doc["setpointPin"].is<JsonVariant>())     io.setpointPin     = doc["setpointPin"].as<int8_t>();
    if (doc["setpointSource"].is<JsonVariant>())  io.setpointSource  = (SetpointSrc_t)doc["setpointSource"].as<uint8_t>();

    _engine->setIOConfig(io);
    Serial.println("[WebManager] I/O config updated");
    req->send(200, "application/json", "{\"ok\":true}");
}

void WebManager::_handlePostControlConfig(AsyncWebServerRequest* req, uint8_t* data, size_t len) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    ControlConfig ctrl = _engine->getControlConfig();

    if (doc["strategy"].is<JsonVariant>())   ctrl.strategy  = (Strategy_t)doc["strategy"].as<uint8_t>();
    if (doc["kp"].is<JsonVariant>())         ctrl.kp        = doc["kp"].as<float>();
    if (doc["ki"].is<JsonVariant>())         ctrl.ki        = doc["ki"].as<float>();
    if (doc["kd"].is<JsonVariant>())         ctrl.kd        = doc["kd"].as<float>();
    if (doc["bangHigh"].is<JsonVariant>())   ctrl.bangHigh  = doc["bangHigh"].as<float>();
    if (doc["bangLow"].is<JsonVariant>())    ctrl.bangLow   = doc["bangLow"].as<float>();
    if (doc["outputMin"].is<JsonVariant>())  ctrl.outputMin = doc["outputMin"].as<float>();
    if (doc["outputMax"].is<JsonVariant>())  ctrl.outputMax = doc["outputMax"].as<float>();
    if (doc["setpoint"].is<JsonVariant>())   ctrl.setpoint  = doc["setpoint"].as<float>();

    // Difference equation coefficients
    if (doc["b"].is<JsonVariant>()) {
        JsonArray bArr = doc["b"].as<JsonArray>();
        for (size_t i = 0; i < bArr.size() && i <= MAX_DIFF_ORDER; i++) {
            ctrl.b[i] = bArr[i].as<float>();
        }
    }
    if (doc["a"].is<JsonVariant>()) {
        JsonArray aArr = doc["a"].as<JsonArray>();
        for (size_t i = 0; i < aArr.size() && i < MAX_DIFF_ORDER; i++) {
            ctrl.a[i] = aArr[i].as<float>();
        }
    }

    _engine->setControlConfig(ctrl);
    Serial.println("[WebManager] Control config updated");
    req->send(200, "application/json", "{\"ok\":true}");
}

void WebManager::_handlePostSave(AsyncWebServerRequest* req) {
    if (!_engine || !_storage) {
        req->send(500, "application/json", "{\"error\":\"Not ready\"}");
        return;
    }
    _storage->saveIOConfig(_engine->getIOConfig());
    _storage->saveControlConfig(_engine->getControlConfig());
    Serial.println("[WebManager] Config saved to NVS");
    req->send(200, "application/json", "{\"ok\":true}");
}

void WebManager::_handlePostDefaults(AsyncWebServerRequest* req) {
    if (!_storage || !_engine) {
        req->send(500, "application/json", "{\"error\":\"Not ready\"}");
        return;
    }
    _storage->clearAll();
    _engine->setIOConfig(IOConfig());
    _engine->setControlConfig(ControlConfig());
    _engine->resetState();
    Serial.println("[WebManager] Defaults restored, NVS cleared");
    req->send(200, "application/json", "{\"ok\":true}");
}

void WebManager::_handlePostWiFi(AsyncWebServerRequest* req, uint8_t* data, size_t len) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) {
        req->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    WiFiConfig wifiCfg;
    if (doc["ssid"].is<JsonVariant>()) {
        strncpy(wifiCfg.ssid, doc["ssid"].as<const char*>(), sizeof(wifiCfg.ssid) - 1);
    }
    if (doc["password"].is<JsonVariant>()) {
        strncpy(wifiCfg.password, doc["password"].as<const char*>(), sizeof(wifiCfg.password) - 1);
    }
    wifiCfg.configured = true;

    _storage->saveWiFiConfig(wifiCfg);
    Serial.printf("[WebManager] WiFi config saved: SSID=%s\n", wifiCfg.ssid);

    // Attempt to connect to the new WiFi
    WiFi.begin(wifiCfg.ssid, strlen(wifiCfg.password) > 0 ? wifiCfg.password : nullptr);
    req->send(200, "application/json", "{\"ok\":true,\"msg\":\"Connecting...\"}");
}

// ============================================================
//  WebSocket Event Handler
// ============================================================

void WebManager::_onWsEvent(AsyncWebSocket* ws, AsyncWebSocketClient* client,
                            AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("[WebSocket] Client #%u connected from %s\n",
                          client->id(), client->remoteIP().toString().c_str());
            break;

        case WS_EVT_DISCONNECT:
            Serial.printf("[WebSocket] Client #%u disconnected\n", client->id());
            break;

        case WS_EVT_DATA:
            _handleWsMessage(client, data, len);
            break;

        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void WebManager::_handleWsMessage(AsyncWebSocketClient* client, uint8_t* data, size_t len) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data, len);
    if (err) return;

    const char* cmd = doc["cmd"] | "";

    if (strcmp(cmd, "get_config") == 0) {
        _sendConfigToClient(client);
        _sendStatusToClient(client);
    }
    else if (strcmp(cmd, "setpoint") == 0 && doc["value"].is<JsonVariant>()) {
        float sp = doc["value"].as<float>();
        _engine->setSetpoint(sp);
    }
    else if (strcmp(cmd, "start") == 0) {
        ControlConfig cfg = _engine->getControlConfig();
        cfg.running = true;
        _engine->setControlConfig(cfg);
        _engine->start();
        Serial.println("[WebSocket] Control started");
    }
    else if (strcmp(cmd, "stop") == 0) {
        ControlConfig cfg = _engine->getControlConfig();
        cfg.running = false;
        _engine->setControlConfig(cfg);
        Serial.println("[WebSocket] Control stopped");
    }
}

void WebManager::_sendConfigToClient(AsyncWebSocketClient* client) {
    if (!_engine) return;

    JsonDocument doc;
    doc["type"] = "config";

    // I/O Config
    IOConfig io = _engine->getIOConfig();
    JsonObject ioObj = doc["io"].to<JsonObject>();
    ioObj["inputPin"]        = io.inputPin;
    ioObj["inputMode"]       = (uint8_t)io.inputMode;
    ioObj["outputPin"]       = io.outputPin;
    ioObj["outputMode"]      = (uint8_t)io.outputMode;
    ioObj["inputGain"]       = io.inputGain;
    ioObj["inputOffset"]     = io.inputOffset;
    ioObj["movingAvgSamples"]= io.movingAvgSamples;
    ioObj["pwmFrequency"]    = io.pwmFrequency;
    ioObj["setpointPin"]     = io.setpointPin;
    ioObj["setpointSource"]  = (uint8_t)io.setpointSource;

    // Control Config
    ControlConfig ctrl = _engine->getControlConfig();
    JsonObject ctrlObj = doc["ctrl"].to<JsonObject>();
    ctrlObj["strategy"]   = (uint8_t)ctrl.strategy;
    ctrlObj["kp"]         = ctrl.kp;
    ctrlObj["ki"]         = ctrl.ki;
    ctrlObj["kd"]         = ctrl.kd;
    ctrlObj["bangHigh"]   = ctrl.bangHigh;
    ctrlObj["bangLow"]    = ctrl.bangLow;
    JsonArray bArr = ctrlObj["b"].to<JsonArray>();
    for (int i = 0; i <= MAX_DIFF_ORDER; i++) bArr.add(ctrl.b[i]);
    JsonArray aArr = ctrlObj["a"].to<JsonArray>();
    for (int i = 0; i < MAX_DIFF_ORDER; i++) aArr.add(ctrl.a[i]);
    ctrlObj["outputMin"]  = ctrl.outputMin;
    ctrlObj["outputMax"]  = ctrl.outputMax;
    ctrlObj["setpoint"]   = ctrl.setpoint;
    ctrlObj["running"]    = ctrl.running;

    String msg;
    serializeJson(doc, msg);
    client->text(msg);
}

void WebManager::_sendStatusToClient(AsyncWebSocketClient* client) {
    String msg = _statusToJson();
    client->text(msg);
}

String WebManager::_statusToJson() {
    JsonDocument doc;
    doc["type"] = "status";

    // WiFi info
    if (WiFi.status() == WL_CONNECTED) {
        doc["ip"]   = WiFi.localIP().toString();
        doc["rssi"] = WiFi.RSSI();
        doc["ssid"] = WiFi.SSID();
    } else {
        doc["ip"]   = WiFi.softAPIP().toString();
        doc["rssi"] = 0;
        doc["ssid"] = WiFi.softAPSSID();
    }

    // System info
    doc["uptime"]   = millis() / 1000;
    doc["heap"]     = ESP.getFreeHeap();
    doc["hostname"] = "controle-esp.local";
    
    // Control Status
    if (_engine) {
        doc["running"] = _engine->getControlConfig().running;
    }

    String msg;
    serializeJson(doc, msg);
    return msg;
}
