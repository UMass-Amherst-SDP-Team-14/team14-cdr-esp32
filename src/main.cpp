#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
#include <string.h>
#include "gps/gps.h"
#include "lora/lora.h"
#include "web/web.h"

#define DEVICE_ADDRESS "83"
#define BASE_ADDRESS "00"

#define TIMEOUT_SEND 5000     // in ms
#define LOC_TOLERANCE 0.00005 // in coordinate system
#define LED_DELAY 1000
#define LED_ACTIVE_DELAY 100
#define LED_BLINK_DURATION 100

#define GPS_RXPIN 42
#define GPS_TXPIN 17 // unused
#define SERIAL_BAUD 115200
#define GPS_BAUD 9600

#define LORA_SSPIN 34
#define LORA_RSTPIN 39
#define LORA_DIO0PIN 40
// SCK = 36, MISO = 35, MOSI = 37

#define LORA_FREQ 915E6

#define LED_PIN 19 // onboard RGB pin
#define BTN_PIN 26
#define GPS_CTRL_PIN 26

#define TESTING_MODE 0 // activate if working inside with no GPS signal
#define VERBOSE_MODE 1 // activate for more serial output

#define WEB_SSID "gotcha-base0"

// The serial connection to the GPS device
SoftwareSerial ss(GPS_RXPIN, GPS_TXPIN);

// SPI channel for use with lora
SPIClass spi_lora(SPI);

// gps encoder/decoder object
TinyGPSPlus gps;

void setup()
{
    // serial
    Serial.begin(SERIAL_BAUD); // device serial
    ss.begin(GPS_BAUD);        // gps serial

    // setup i/o pins
    pinMode(BTN_PIN, INPUT_PULLUP);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    pinMode(GPS_CTRL_PIN, OUTPUT);
    digitalWrite(GPS_CTRL_PIN, LOW);

    // initialize lora
    initLora(spi_lora, LORA_SSPIN, LORA_RSTPIN, LORA_DIO0PIN, DEVICE_ADDRESS);

    // initialize web server
    initWebServer(WEB_SSID);
}

unsigned long lastSent = 0;
unsigned long lastBlink = 0;
double currentLat = 0;
double currentLng = 0;
double lastLat = 0;
double lastLng = 0;
uint32_t currentTime = 0;
uint32_t currentDate = 0;

int button_pressed = 0;
int executed_button = 0;
int isActive = 1;  // set to 1 to ignore button
int isInit = 0;

double tempLats[] = {10, 20};
double tempLons[] = {10, 20};

void loop()
{
    handleClient(tempLats, tempLons);  // handle web server clients

    /*
    // is the button being pressed?
    if (digitalRead(BTN_PIN) == HIGH)
    {
        // button is not being pressed
        if (button_pressed)
        {
            Serial.println("Button is released");
        }
        button_pressed = 0;
        executed_button = 0;
    }
    else
    {
        // button is being pressed
        if (!button_pressed)
        {
            Serial.println("Button is pressed");
        }
        button_pressed = 1;
    }

    if (button_pressed && !executed_button)
    {
        // need to do button action here
        isActive = 1;
    }

    // Do we need to blink the led?
    long nextime = 0;
    if (isActive)
    {
        nextime = lastBlink + LED_ACTIVE_DELAY;
    }
    else
    {
        nextime = lastBlink + LED_DELAY;
    }

    if (millis() > nextime)
    {
        digitalWrite(LED_PIN, HIGH);
        delay(LED_BLINK_DURATION);
        digitalWrite(LED_PIN, LOW);
        lastBlink = millis();
    }

    if (isActive)
    {
        if (!isInit)
        {
            gpsInit(GPS_CTRL_PIN);
            Serial.println("GPS Init");
            isInit = 1;

            while (ss.available() == 0)
                ; // wait for something from the GPS
        }

        // Do we need to send anything? Check if lat/lon has changed or if the timeout is exceeded
        float diffLat = abs(currentLat - lastLat);
        float diffLon = abs(currentLng - lastLng);
        if (diffLat > LOC_TOLERANCE || diffLon > LOC_TOLERANCE || millis() > lastSent + TIMEOUT_SEND)
        {
            // need to send packet here
            LoRa.beginPacket();

            LoRa.print("{");

            LoRa.print("\"from\":");
            LoRa.print("\"");
            LoRa.print(DEVICE_ADDRESS);
            LoRa.print("\"");

            LoRa.print(",");

            LoRa.print("\"to\":");
            LoRa.print("\"");
            LoRa.print(BASE_ADDRESS);
            LoRa.print("\"");

            LoRa.print(",");

            LoRa.print("\"location\":");
            LoRa.print("[");
            LoRa.print(currentLat, 6);
            LoRa.print(",");
            LoRa.print(currentLng, 6);

            LoRa.print("]");

            LoRa.print(",");

            LoRa.print("\"time\":");
            LoRa.print("\"");
            LoRa.print(currentTime);
            LoRa.print("\"");

            LoRa.print(",");

            LoRa.print("\"date\":");
            LoRa.print("\"");
            LoRa.print(currentDate);
            LoRa.print("\"");

            LoRa.print("}");

            LoRa.endPacket();
            LoRa.receive(); // go back to receive mode

            Serial.println("Sent LoRa Packet");

            lastSent = millis();
        }

        // check if GPS has something to report
        while (ss.available() > 0)
        {
            if (gps.encode(ss.read()))
            {
                float recordedLat = getCurrentLat(gps);
                float recordedLng = getCurrentLng(gps);

                lastLng = currentLng;
                lastLat = currentLat;
                currentLat = recordedLat;
                currentLng = recordedLng;
                currentTime = getCurrentTime(gps);
                currentDate = getCurrentDate(gps);
            }
        }
    }*/

    //delay(10);
}
