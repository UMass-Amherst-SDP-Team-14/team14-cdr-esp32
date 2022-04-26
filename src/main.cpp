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

int isInit = 0;
int isActive = 0; // set to 0 later

void setup()
{
    // serial
    Serial.begin(SERIAL_BAUD); // device serial
    ss.begin(GPS_BAUD);        // gps serial

#if (NODE_TYPE == 1)
    // setup i/o pins
    pinMode(BTN_PIN, INPUT_PULLUP);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
#endif

    // intialize GPS
    gpsInit();

#if (NODE_TYPE == 0)
    isActive = 1;

    // initialize web server
    //initWeb();
#endif

    // initialize lora
    initLora();
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

void loop()
{
    // we cannot execute lora functions when web server is reading SD card (same SPI bus)
    rxPackets();
    processRelays();

    // create base location arrays (for now we only support one base station)
#if (NODE_TYPE == 0)
    /*int base_idlist[] = {01};
    double base_latlist[] = {currentLat};
    double base_lonlist[] = {currentLng};

    updateLocations(getIDList(), getLatList(), getLonList(), base_idlist, base_latlist, base_lonlist);
    handleWebClients();*/
#endif

#if (NODE_TYPE == 1)
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
#endif

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
#if (NODE_TYPE == 1)
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
