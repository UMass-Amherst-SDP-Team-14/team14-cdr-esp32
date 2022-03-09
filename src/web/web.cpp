#include <WiFi.h>

#define WEB_PORT 80
#define WEB_TIMEOUTTIME 2000 // in ms

WiFiServer http_server(WEB_PORT);

void initWebServer(char* web_ssid)
{
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Initializing Access Point...");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(web_ssid);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // start the web server
    http_server.begin();
}

void handleClient(double lats[], double lons[])
{
    WiFiClient client = http_server.available();
    if (client)
    {
        Serial.println("Serving web server client...");
        unsigned long web_previousTime = millis();
        String http_header = "";
        String currentLine = ""; // make a String to hold incoming data from the client

        while (client.connected())
        {
            if (millis() - web_previousTime <= WEB_TIMEOUTTIME)
            {
                if (client.available())
                {
                    char c = client.read(); // read a byte, then
                    Serial.write(c);        // print it out the serial monitor
                    http_header += c;
                    if (c == '\n')
                    {
                        if (currentLine.length() == 0)
                        {
                            client.println("HTTP/1.1 200 OK");        // start with a http response code
                            client.println("Content-type:text/html"); // indicate content type
                            client.println("Connection: close");
                            client.println();

                            // Display the HTML web page
                            client.println("<!DOCTYPE html><html>");
                            client.println("<head>");
                            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"); // for mobile viewing

                            // add javascript API
                            client.println("<script src=\"https://openlayers.org/en/v4.6.5/build/ol.js\" type=\"text/javascript\"></script>");

                            client.println("</head>");
                            client.println("<body>");
                            //client.println("<div id=\"map\" style=\"width: 100vw; height: 100vh;\"></div>");

                            size_t count = sizeof(lats);
                            if (sizeof(lons) == count) {
                                for (int i = 0; i < count; i++) {
                                    // add point on map
                                    client.print("<h2>ID: ");
                                    client.print(i);
                                    client.println("</h2>");
                                    client.print("<h3>Longitude: ");
                                    client.print(lons[i]);
                                    client.print("</h3>");
                                    client.print("<h3>Latitude: ");
                                    client.print(lats[i]);
                                    client.print("</h3>");
                                }
                            }

                            // mapping code
                            client.println("<script>");

                            client.println("var map;");
                            client.println("var mapLat = 42.411622;");
                            client.println("var mapLon = -72.535245;");
                            client.println("var mapDefaultZoom = 10");

                            client.println("map = new ol.Map({");

                            client.println("target: \"map\",");
                            client.println("layers: [ new ol.layer.Tile({ source: new ol.source.OSM({");
                            client.println("url: \"https://a.tile.openstreetmap.org/{z}/{x}/{y}.png\"");
                            client.println("}) }) ],");

                            client.println("view: new ol.View({");
                            client.println("center: ol.proj.fromLonLat([mapLng, mapLat]),");
                            client.println("zoom: mapDefaultZoom");
                            client.println("})");

                            client.println("});");

                            // LOOP STARTS HERE
                            //size_t count = sizeof(lats);
                            if (sizeof(lons) == count) {
                                for (int i = 0; i < count; i++) {
                                    // add point on map
                                    client.println("map.addLayer(");
                                    client.println("new ol.layer.Vector({");
                                    client.println("source: new ol.source.Vector({ features: [new ol.Feature({");
                                    client.print("geometry: new ol.geom.Point(ol.proj.transform([parseFloat(");
                                    client.print(lons[i]);
                                    client.print("), parseFloat(");
                                    client.print(lats[i]);
                                    client.println(")], 'EPSG:4326', 'EPSG:3857')),");
                                    client.println("})] }),");
                                    client.println("style: new ol.style.Style({ image: new ol.style.Icon({");
                                    client.println("anchor: [0.5, 0.5],");
                                    client.println("anchorXUnits: \"fraction\",");
                                    client.println("anchorYUnits: \"fraction\",");
                                    client.println("src: \"https://upload.wikimedia.org/wikipedia/commons/e/ec/RedDot.svg\"");
                                    client.println("}) })");
                                    client.println("});");
                                    client.println(");");
                                }
                            }

                            client.println("</script>");

                            client.println("</body>");
                            client.println("</html>");

                            client.println(); // The HTTP response ends with another blank line
                            // Break out of the while loop
                            break;
                        }
                        else
                        { // if you got a newline, then clear currentLine
                            currentLine = "";
                        }
                    }
                    else if (c != '\r')
                    {                     // if you got anything else but a carriage return character,
                        currentLine += c; // add it to the end of the currentLine
                    }
                }
            }
            else
            {
                Serial.println("Client timeout reached");
                break;
            }
        }

        // Close the connection
        client.stop();
        Serial.println("Web client disconnected");
    }
}