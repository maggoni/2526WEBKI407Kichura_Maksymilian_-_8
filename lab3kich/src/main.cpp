#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
const char* ssid = "TP-LINK2020";
const char* password = "maks1234";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void setup() {
    Serial.begin(115200);
    LittleFS.begin(true);
    Serial.println("[SYSTEM] TCS34725 Simulation Active");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println("[CONNECTED] IP: " + WiFi.localIP().toString());

    server.addHandler(&ws);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *r){
        r->send(LittleFS, "/index.html", "text/html");
    });
    server.begin();
}

void loop() {
    ws.cleanupClients();
    static unsigned long lastUpdate = 0;
    static float hue = 0;

    if (millis() - lastUpdate > 500) {
        lastUpdate = millis();
        
        // Симуляція кольору
        hue += 0.2;
        int r8 = (sin(hue) * 127) + 128;
        int g8 = (sin(hue + 2.0) * 127) + 128;
        int b8 = (sin(hue + 4.0) * 127) + 128;

        // ОДНАКОВИЙ ВИВІД У SERIAL
        Serial.printf("COLOR DATA >> R: %d | G: %d | B: %d\n", r8, g8, b8);

        JsonDocument doc;
        doc["r"] = r8;
        doc["g"] = g8;
        doc["b"] = b8;
        String json;
        serializeJson(doc, json);
        ws.textAll(json);
    }
}