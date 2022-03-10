#include "defs.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include "gps/gps.h"
#include "lora/lora.h"
#include "web/web.h"

// The serial connection to the GPS device
SoftwareSerial ss(GPS_RXPIN, GPS_TXPIN);

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

    // intialize GPS
    gpsInit();

    // initialize lora
    initLora();

#ifdef BASE_NODE
    // initialize web server
    initWebServer();
#endif
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

#ifdef BASE_NODE
int isActive = 1; // set to 1 to ignore button
#endif

#ifdef WORKER_NODE
int isActive = 1; // set to 0 later
#endif

int isInit = 0;

void loop()
{
    rxPackets();
    processRelays();

#ifdef BASE_NODE
    handleClient(currentLat, currentLng, getIDList(), getLatList(), getLonList()); // handle web server clients
#endif

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
            gpsOn();
            isInit = 1;

            while (ss.available() == 0)
                ; // wait for something from the GPS
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

// Do we need to send anything? Check if lat/lon has changed or if the timeout is exceeded
#ifdef WORKER_NODE
        float diffLat = abs(currentLat - lastLat);
        float diffLon = abs(currentLng - lastLng);
        if (diffLat > LOC_TOLERANCE || diffLon > LOC_TOLERANCE || millis() > lastSent + TIMEOUT_SEND)
        {
            Serial.println("Sending updated location...");
            sendMessage(currentLat, currentLng, currentTime, currentDate);

            lastSent = millis(); // update timestamp for the last message sent
        }
#endif
    }
}
