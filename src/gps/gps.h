#include <TinyGPS++.h>

double getCurrentLat(TinyGPSPlus gps);
double getCurrentLng(TinyGPSPlus gps);
double getCurrentAltitude(TinyGPSPlus gps);
double getCurrentDate(TinyGPSPlus gps);
double getCurrentTime(TinyGPSPlus gps);
void gpsInit(int gps_ctrl_pin);