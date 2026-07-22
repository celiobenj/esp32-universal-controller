/**
 * WebManager.h — Async Web Server & WebSocket Manager
 * 
 * Handles HTTP API routes, serves the embedded web UI,
 * and manages WebSocket connections for real-time telemetry.
 * Runs on Core 0 alongside WiFi stack.
 */

#pragma once

#include "Config.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Forward declarations
class ControlEngine;
class StorageManager;

class WebManager {
public:
    WebManager();

    /**
     * Initialize and start the web server.
     * @param engine  Pointer to the ControlEngine for config access and telemetry.
     * @param storage Pointer to the StorageManager for persistence operations.
     */
    void start(ControlEngine* engine, StorageManager* storage);

    /**
     * Called periodically from main loop to push telemetry via WebSocket.
     * Should be called frequently (e.g., every loop iteration).
     */
    void loop();

private:
    AsyncWebServer _server;
    AsyncWebSocket _ws;

    ControlEngine*  _engine  = nullptr;
    StorageManager* _storage = nullptr;

    // --- HTTP Route Handlers ---
    void _setupRoutes();
    void _serveIndex(AsyncWebServerRequest* req);
    void _handleGetConfig(AsyncWebServerRequest* req);
    void _handlePostIOConfig(AsyncWebServerRequest* req, uint8_t* data, size_t len);
    void _handlePostControlConfig(AsyncWebServerRequest* req, uint8_t* data, size_t len);
    void _handlePostSave(AsyncWebServerRequest* req);
    void _handlePostDefaults(AsyncWebServerRequest* req);
    void _handlePostWiFi(AsyncWebServerRequest* req, uint8_t* data, size_t len);

    // --- WebSocket ---
    void _onWsEvent(AsyncWebSocket* ws, AsyncWebSocketClient* client,
                    AwsEventType type, void* arg, uint8_t* data, size_t len);
    void _handleWsMessage(AsyncWebSocketClient* client, uint8_t* data, size_t len);
    void _sendConfigToClient(AsyncWebSocketClient* client);
    void _sendStatusToClient(AsyncWebSocketClient* client);

    // --- Telemetry ---
    unsigned long _lastTelemetryMs = 0;

    // --- Helpers ---
    String _ioConfigToJson(const IOConfig& io);
    String _controlConfigToJson(const ControlConfig& ctrl);
    String _statusToJson();
};
