#include "defs.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"

WebServer server(80);

// SPIClass spi_main(VSPI);

void initSDCard()
{
  // spi_main.begin(VSPI_CLK, VSPI_MISO, VSPI_MOSI);
  if (!SD.begin(SD_CSPIN))
  {
    Serial.println("SD card failed to initialize");
    return;
  }
  Serial.println("SD card initialized");
}

String getContentType(String filename)
{
  if (server.hasArg("download"))
  { // check if the parameter "download" exists
    return "application/octet-stream";
  }
  else if (filename.endsWith(".htm"))
  { // check if the string filename ends with ".htm"
    return "text/html";
  }
  else if (filename.endsWith(".html"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".css"))
  {
    return "text/css";
  }
  else if (filename.endsWith(".js"))
  {
    return "application/javascript";
  }
  else if (filename.endsWith(".png"))
  {
    return "image/png";
  }
  else if (filename.endsWith(".gif"))
  {
    return "image/gif";
  }
  else if (filename.endsWith(".jpg"))
  {
    return "image/jpeg";
  }
  else if (filename.endsWith(".ico"))
  {
    return "image/x-icon";
  }
  else if (filename.endsWith(".xml"))
  {
    return "text/xml";
  }
  else if (filename.endsWith(".pdf"))
  {
    return "application/x-pdf";
  }
  else if (filename.endsWith(".zip"))
  {
    return "application/x-zip";
  }
  else if (filename.endsWith(".gz"))
  {
    return "application/x-gzip";
  }
  return "text/plain";
}

void handleRoot()
{
  fs::FS &fs = SD;
  String path = server.uri();

  if (path.endsWith("/"))
  {
    path += "index.html";
  }

  String contentType = getContentType(path);
  File file = fs.open(path, "r");
  size_t sent = server.streamFile(file, contentType);
  file.close();
}

void initWeb()
{
  initSDCard();

  // Connect to Wi-Fi network with SSID and password
  Serial.println("Initializing Access Point...");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(WEB_SSID);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.onNotFound(handleRoot);
  server.begin();
}

void handleWebClients()
{
  server.handleClient();
}

void updateLocations(int *worker_id_list, double *worker_lat_list, double *worker_lon_list, int *base_id_list, double *base_lat_list, double *base_lon_list)
{
  StaticJsonDocument<1000> data_json;
  JsonObject root = data_json.to<JsonObject>();

  // worker nodes
  JsonArray worker_id = root.createNestedArray("worker_id");
  JsonArray worker_lat = root.createNestedArray("worker_lat");
  JsonArray worker_lon = root.createNestedArray("worker_lon");

  for (int i = 0; i < sizeof(worker_id_list); i++)
  {
    if (worker_id_list[i] != 0)
    {
      worker_id.add(worker_id_list[i]);
      worker_lat.add(worker_lat_list[i]);
      worker_lon.add(worker_lon_list[i]);
    }
  }

  // base nodes
  JsonArray base_id = root.createNestedArray("base_id");
  JsonArray base_lat = root.createNestedArray("base_lat");
  JsonArray base_lon = root.createNestedArray("base_lon");

  for (int i = 0; i < sizeof(base_id_list); i++)
  {
    if (base_id_list[i] != 0)
    {
      base_id.add(base_id_list[i]);
      base_lat.add(base_lat_list[i]);
      base_lon.add(base_lon_list[i]);
    }
  }

  String data_json_content = "";
  serializeJson(data_json, data_json_content);

  File file = SD.open("/data.json", FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open data.json for writing");
    return;
  }

  if (!file.print(data_json_content))
  {
    Serial.println("Failed to write to data.json");
  }
  file.close();
}