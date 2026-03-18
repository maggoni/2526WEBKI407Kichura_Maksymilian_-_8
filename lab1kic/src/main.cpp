#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

// --- Creds ---
const char* ssid = "TP-LINK2020";
const char* password = "maks1234";

WebServer server(80);

void setup() {
    Serial.begin(115200);
    
    // Mount Secure Storage
    if(!LittleFS.begin(true)){
        Serial.println("FS Mount Error.");
        return;
    }

    // Network Sync
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\n[NODE_CONNECTED]");
    Serial.print("Access point: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](){
        if(LittleFS.exists("/index.html")){
            File file = LittleFS.open("/index.html", "r");
            server.streamFile(file, "text/html");
            file.close();
        } else {
            server.send(404, "text/plain", "Error 404: Resource Missing");
        }
    });

    server.begin();
}

void loop() {
    server.handleClient();
}