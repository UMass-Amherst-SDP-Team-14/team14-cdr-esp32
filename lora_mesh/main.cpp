#include "main.h"

#define LED_BUILTIN 15

/** Timer for the LED control */
Ticker ledOffTick;

/** Structure for outgoing data */
dataMsg outData;
/** Route to the selected receiver node */
nodesList routeToNode;
/** Node ID of the selected receiver node */
uint32_t nodeId[48];
/** First hop ID of the selected receiver node */
uint32_t firstHop[48];
/** Number of hops to the selected receiver node */
uint8_t numHops[48];
/** Number of nodes in the map */
uint8_t numElements;

/** Flag if the Mesh map has changed */
boolean nodesListChanged = false;

/** Timer to send data frequently to random nodes */
time_t sendRandom;

/** 
 * Switch off the LED
 * Triggered by a timer
 */
void ledOff(void)
{
	digitalWrite(LED_BUILTIN, HIGH);
}

/**
 * Arduino setup
 */
void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Start Serial
	Serial.begin(115200);

	// Create node ID
	uint8_t deviceMac[8];

	BoardGetUniqueId(deviceMac);

	deviceID += (uint32_t)deviceMac[2] << 24;
	deviceID += (uint32_t)deviceMac[3] << 16;
	deviceID += (uint32_t)deviceMac[4] << 8;
	deviceID += (uint32_t)deviceMac[5];

	Serial.print("Mesh NodeId = ");
	Serial.println(deviceID);

	// Initialize the LoRa
	if (!initLoRa())
	{
		Serial.println("LoRa initialization failed!");
	}

	// Initialize timer for random data sending
	sendRandom = millis();
}

/**
 * Arduino loop
 */
void loop()
{
	delay(100);

	if ((millis() - sendRandom) >= 30000)
	{
		// Time to send a package
		sendRandom = millis();

		// if (random(0, 10) > 5)
		if (random(0, 10) > 5)
		{
			// Send a broadcast
			// Prepare data
			outData.mark1 = 'L';
			outData.mark2 = 'o';
			outData.mark3 = 'R';
			getNextBroadcastID();

			outData.dest = getNextBroadcastID();
			outData.from = deviceID;
			outData.type = LORA_BROADCAST;
			Serial.printf("Queuing broadcast with id %08X\n", outData.dest);

			int dataLen = MAP_HEADER_SIZE + sprintf((char *)outData.data, ">>BR from %08X<<", deviceID);
			// Add package to send queue
			if (!addSendRequest(&outData, dataLen))
			{
				Serial.println("Sending package failed");
			}
		}
		else
		{
			if (xSemaphoreTake(accessNodeList, (TickType_t)1000) == pdTRUE)
			{
				numElements = numOfNodes();
				if (numOfNodes() >= 2)
				{
					// Select random node to send a package
					getRoute(nodeId[random(0, numElements)], &routeToNode);
					// Release access to nodes list
					xSemaphoreGive(accessNodeList);
					// Prepare data
					outData.mark1 = 'L';
					outData.mark2 = 'o';
					outData.mark3 = 'R';
					if (routeToNode.firstHop != 0)
					{
						outData.dest = routeToNode.firstHop;
						outData.from = routeToNode.nodeId;
						outData.type = LORA_FORWARD;
						Serial.printf("Queuing msg to hop to %08X over %08X\n", outData.from, outData.dest);
					}
					else
					{
						outData.dest = routeToNode.nodeId;
						outData.from = deviceID;
						outData.type = LORA_DIRECT;
						Serial.printf("Queuing msg direct to %08X\n", outData.dest);
					}
					int dataLen = MAP_HEADER_SIZE + sprintf((char *)outData.data, ">>%08X<<", deviceID);
					// Add package to send queue
					if (!addSendRequest(&outData, dataLen))
					{
						Serial.println("Sending package failed");
					}
				}
				else
				{
					// Release access to nodes list
					xSemaphoreGive(accessNodeList);
					Serial.println("Not enough nodes in the list");
				}
			}
			else
			{
				Serial.println("Could not access the nodes list");
			}
		}
	}

	if (nodesListChanged)
	{
		// Nodes list changed, update display and report it
		nodesListChanged = false;
		Serial.println("---------------------------------------------");
		if (xSemaphoreTake(accessNodeList, (TickType_t)1000) == pdTRUE)
		{
			numElements = numOfNodes();

			for (int idx = 0; idx < numElements; idx++)
			{
				getNode(idx, nodeId[idx], firstHop[idx], numHops[idx]);
			}
			// Release access to nodes list
			xSemaphoreGive(accessNodeList);
			// Display the nodes
			Serial.printf("%d nodes in the map\n", numElements + 1);
			Serial.printf("Node #01 id: %08X\n", deviceID);
			for (int idx = 0; idx < numElements; idx++)
			{
				if (firstHop[idx] == 0)
				{
					Serial.printf("Node #%02d id: %08X direct\n", idx + 2, nodeId[idx]);
				}
				else
				{
					Serial.printf("Node #%02d id: %08X first hop %08X #hops %d\n", idx + 2, nodeId[idx], firstHop[idx], numHops[idx]);
				}
			}
		}
		else
		{
			Serial.println("Could not access the nodes list");
		}

		Serial.println("---------------------------------------------");
	}
}

/**
 * Callback after a LoRa package was received
 * @param rxPayload
 * 			Pointer to the received data
 * @param rxSize
 * 			Length of the received package
 * @param rxRssi
 * 			Signal strength while the package was received
 * @param rxSnr
 * 			Signal to noise ratio while the package was received
 */
void OnLoraData(uint32_t fromID, uint8_t *rxPayload, uint16_t rxSize, int16_t rxRssi, int8_t rxSnr)
{
	Serial.println("-------------------------------------");
	Serial.printf("Got data from node %08X\n", fromID);
	for (int idx = 0; idx < rxSize; idx++)
	{
		Serial.printf("%02X ", rxPayload[idx]);
	}
	Serial.printf("\n\n%s\n", rxPayload);
	Serial.println("-------------------------------------");

	digitalWrite(LED_BUILTIN, LOW);
	ledOffTick.detach();
	ledOffTick.once(1, ledOff);
}

/**
 * Callback after the nodes list changed
 */
void onNodesListChange(void)
{
	nodesListChanged = true;
}