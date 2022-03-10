// State Definitions (change these depending on type of node)
#define BASE_NODE
//#define WORKER_NODE
//#define DEVICE_ADDRESS 88  // worker 2
//#define DEVICE_ADDRESS 79  // worker 1
#define DEVICE_ADDRESS 00  // base 0

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
#define GPS_RXPIN 42
#define GPS_TXPIN 17  // unused, set to whatever
#define GPS_CTRL_PIN 26  // power control pin (MOSFET) for the GPS

#define LORA_SSPIN 34
#define LORA_RSTPIN 39
#define LORA_DIO0PIN 40
// SCK = 36, MISO = 35, MOSI = 37 for FSPI on ESP32-S2
//#define LORA_SPICLASS FSPI

#define LED_PIN 19  // state LED pin
#define BTN_PIN 26  // User input button pin