#include "defs.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

AsyncWebServer server(80);

void initWeb()
{
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Initializing Access Point...");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(WEB_SSID);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SD, "/index.html", "text/html"); });

    server.serveStatic("/", SD, "/");
    server.begin();
}