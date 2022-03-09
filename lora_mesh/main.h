#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <Ticker.h>

extern uint32_t deviceID;

/** LoRa package types */
#define LORA_INVALID 0
#define LORA_DIRECT 1
#define LORA_FORWARD 2
#define LORA_BROADCAST 3
#define LORA_NODEMAP 4

// LoRa & Mesh
#include <SX126x-Arduino.h>
#include <SPI.h>
#include <Mesh/mesh.h>
bool initLoRa(void);