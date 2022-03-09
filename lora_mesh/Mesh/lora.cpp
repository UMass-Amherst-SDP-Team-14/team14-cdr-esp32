#include "main.h"

/** HW configuration structure for the LoRa library */
extern hw_config _hwConfig;

int PIN_LORA_RESET = 39;
int PIN_LORA_DIO_1 = 40;
int PIN_LORA_BUSY = -1;  // what is this?
int PIN_LORA_NSS = 34;
int PIN_LORA_SCLK = 36;
int PIN_LORA_MISO = 37;
int PIN_LORA_MOSI = 35;
int RADIO_TXEN = -1;
int RADIO_RXEN = -1;

#define LED_BUILTIN 15

/** Callback if data was received over LoRa SX1262 */
void OnLoraData(uint32_t fromID, uint8_t *rxPayload, uint16_t rxSize, int16_t rxRssi, int8_t rxSnr);
/** Callback if Mesh map changed */
void onNodesListChange(void);

/** Structure with Mesh event callbacks */
static MeshEvents_t MeshEvents;

/**
 * Initialize the LoRa HW
 * @return bool
 * 		True if initialization was successfull, false if not
 */
bool initLoRa(void)
{
	bool initResult = true;

	// Define the HW configuration between MCU and SX126x
	_hwConfig.CHIP_TYPE = SX1262_CHIP;		   // eByte E22 module with an SX1262
	_hwConfig.PIN_LORA_RESET = PIN_LORA_RESET; // LORA RESET
	_hwConfig.PIN_LORA_NSS = PIN_LORA_NSS;	 // LORA SPI CS
	_hwConfig.PIN_LORA_SCLK = PIN_LORA_SCLK;   // LORA SPI CLK
	_hwConfig.PIN_LORA_MISO = PIN_LORA_MISO;   // LORA SPI MISO
	_hwConfig.PIN_LORA_DIO_1 = PIN_LORA_DIO_1; // LORA DIO_1
	_hwConfig.PIN_LORA_BUSY = PIN_LORA_BUSY;   // LORA SPI BUSY
	_hwConfig.PIN_LORA_MOSI = PIN_LORA_MOSI;   // LORA SPI MOSI
	_hwConfig.RADIO_TXEN = RADIO_TXEN;		   // LORA ANTENNA TX ENABLE
	_hwConfig.RADIO_RXEN = RADIO_RXEN;		   // LORA ANTENNA RX ENABLE
	_hwConfig.USE_DIO2_ANT_SWITCH = false;	  // Example uses an eByte E22 module which uses RXEN and TXEN pins as antenna control
	_hwConfig.USE_DIO3_TCXO = false;			   // Example uses an eByte E22 module which uses DIO3 to control oscillator voltage
	_hwConfig.USE_DIO3_ANT_SWITCH = false;	 // Only Insight ISP4520 module uses DIO3 as antenna control

	if (lora_hardware_init(_hwConfig) != 0)
	{
		Serial.println("Error in hardware init");
	}

	uint16_t readSyncWord = 0;
	SX126xReadRegisters(REG_LR_SYNCWORD, (uint8_t *)&readSyncWord, 2);
	
	Serial.print("Got syncword ");
	Serial.println(readSyncWord);

	MeshEvents.DataAvailable = OnLoraData;
	MeshEvents.NodesListChanged = onNodesListChange;

	// Initialize the LoRa Mesh
	// * events, number of nodes, frequency, TX power
	initMesh(&MeshEvents, 48);

	return initResult;
}