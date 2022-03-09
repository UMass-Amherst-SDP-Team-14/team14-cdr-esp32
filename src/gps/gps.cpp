#include "defs.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

double getCurrentLat(TinyGPSPlus gps)
{
    if (gps.location.isValid())
    {
        return gps.location.lat();
    }
    else
    {
        return 0;
    }
}

double getCurrentLng(TinyGPSPlus gps)
{
    if (gps.location.isValid())
    {
        return gps.location.lng();
    }
    else
    {
        return 0;
    }
}

double getCurrentAltitude(TinyGPSPlus gps)
{
    // not yet implemented
    return 0;
}

uint32_t getCurrentDate(TinyGPSPlus gps)
{
    if (gps.date.isValid())
    {
        return gps.date.value();
    }
    else
    {
        return 0;
    }
}

uint32_t getCurrentTime(TinyGPSPlus gps)
{
    if (gps.date.isValid())
    {
        return gps.time.value();
    }
    else
    {
        return 0;
    }
}

void gpsInit()
{
    pinMode(GPS_CTRL_PIN, OUTPUT);
    digitalWrite(GPS_CTRL_PIN, LOW);
    Serial.println("GPS Init");
}

void gpsOn()
{
    digitalWrite(GPS_CTRL_PIN, HIGH);
    Serial.println("GPS On");
}

void gpsOff()
{
    digitalWrite(GPS_CTRL_PIN, HIGH);
    Serial.println("GPS Off");
}