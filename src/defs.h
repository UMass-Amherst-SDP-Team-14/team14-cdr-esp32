// General Settings
#define SERIAL_BAUD 115200  // ESP32 Baud rate
#define LED_DELAY 1000
#define LED_ACTIVE_DELAY 100
#define LED_BLINK_DURATION 100

// Web Settings
#define WEB_SSID "gotcha-base0"  // only relevant if "BASE_NODE" is defined
#define WEB_PORT 80
#define WEB_TIMEOUTTIME 5000 // in ms

// LORA Settings
#define LORA_FREQ 915E6
#define LORA_SYNC 0x17
#define LORA_TXPOWER 17

// GPS Settings
#define GPS_BAUD 9600  // GPS baud rate
#define TIMEOUT_SEND 5000     // in ms (time before sending an updated position anyway)
#define LOC_TOLERANCE 0.00005 // in coordinate system (before sending an updates packet)

// Physical Pins
#define GPS_RXPIN 34
#define GPS_TXPIN 12  // unused, set to whatever
#define GPS_CTRL_PIN 35  // power control pin (MOSFET) for the GPS

#define LORA_SSPIN 5
#define LORA_RSTPIN 14
#define LORA_DIO0PIN 2

// VSPI Bus
// SCK = 30, MISO = 31, MOSI = 37 for ESP32
#define VSPI_CLK 18
#define VSPI_MISO 19
#define VSPI_MOSI 23

#define LED_PIN 25  // state LED pin
#define BTN_PIN 32  // User input button pin

// SD Card
#define SD_CSPIN 4