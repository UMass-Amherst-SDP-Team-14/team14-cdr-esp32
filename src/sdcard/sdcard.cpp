#include "defs.h"
#include <mySD.h>

void initSDCard()
{
  if (!SD.begin(SD_CSPIN, VSPI_MOSI, VSPI_MISO, VSPI_CLK))
  {
    Serial.println("SD card failed!");
    return;
  }
  Serial.println("SD card initialized");
}