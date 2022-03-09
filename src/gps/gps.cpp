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

void gpsInit(int gps_ctrl_pin) {
    digitalWrite(gps_ctrl_pin, HIGH);
}