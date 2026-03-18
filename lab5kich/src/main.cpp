#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>

using namespace httpsserver;

const char* ssid = "TP-LINK2020";
const char* password = "maks1234";

SSLCert cert = SSLCert(); 
HTTPSServer secureServer = HTTPSServer(&cert);

void handleSecureRoot(HTTPRequest * req, HTTPResponse * res) {
    // Basic Auth (admin:password)
    if (req->getHeader("Authorization") != "Basic YWRtaW46cGFzc3dvcmQ=") {
        res->setStatusCode(401);
        res->setHeader("WWW-Authenticate", "Basic realm=\"Secure\"");
        res->print("Unauthorized");
        return;
    }

    // ВІДКРИВАЄМО ТВІЙ ОКРЕМИЙ ФАЙЛ
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        res->setStatusCode(404);
        res->print("HTML File Missing in LittleFS");
        return;
    }

    res->setHeader("Content-Type", "text/html");

    // Читаємо файл малим буфером (128 байт), щоб не забити RAM
    uint8_t buffer[128];
    while (file.available()) {
        size_t readBytes = file.read(buffer, sizeof(buffer));
        res->write(buffer, readBytes);
        // Невеликий delay дозволяє системі обробляти TLS-пакети паралельно
        delay(1); 
    }
    file.close();
}

void setup() {
    Serial.begin(115200);
    
    // Обов'язково монтуємо LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println("FS Error");
        return;
    }

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println("\nIP: " + WiFi.localIP().toString());

    // ГЕНЕРУЄМО СЕРТИФІКАТ (Тільки 1024 біт для стабільності!)
    Serial.println("Generating RSA-1024...");
    createSelfSignedCert(cert, KEYSIZE_1024, "CN=esp32.local,O=Obsidian,C=UA");

    ResourceNode * nodeRoot = new ResourceNode("/", "GET", &handleSecureRoot);
    secureServer.registerNode(nodeRoot);

    secureServer.start();
    Serial.println("HTTPS Server Ready");
}

void loop() {
    secureServer.loop();
    delay(1); 
}