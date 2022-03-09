#include "defs.h"
#include <LoRa.h>
#include <ArduinoJson.h>
#include <SPI.h>

#ifdef BASE_NODE
#define MAX_NODES 100 // maximum number of nodes in the entire network
int num_disc_nodes = 0;
int node_idlist[MAX_NODES];
double node_latlist[MAX_NODES];
double node_lonlist[MAX_NODES];
#endif

#ifdef WORKER_NODE
void sendMessage(double lat, double lon, uint32_t time, uint32_t date)
{
    StaticJsonDocument<300> sending_data;

    sending_data["from"] = DEVICE_ADDRESS;
    sending_data["lat"] = lat;
    sending_data["lon"] = lon;
    sending_data["time"] = time;
    sending_data["date"] = date;

    String message = "";
    serializeJson(sending_data, message);

    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    LoRa.receive(); // go back to receive mode

    Serial.print("Sent LoRa Packet: ");
    Serial.println(message);
}
#endif

void relayMessage(String message) {
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    LoRa.receive(); // go back to receive mode

    Serial.print("Relayed LoRa message: ");
    Serial.println(message);
}

#define SENT_BUFFER_SIZE 200

int sentList[SENT_BUFFER_SIZE];
int listIndexIncrement = 0;
void onReceive(int packetSize)
{
    String input = "";
    while (LoRa.available() > 0)
    {
        input = input + (char)LoRa.read();
    }

    Serial.println(input); // print out incoming packet

    StaticJsonDocument<300> incoming_packet;
    DeserializationError error = deserializeJson(incoming_packet, input);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // Serial.print("Packet RSSI: ");
    // Serial.println(LoRa.packetRssi());

    bool sendRelay = true;
    for (int i = 0; i < SENT_BUFFER_SIZE; i++)
    {
        if (sentList[i] == (int)incoming_packet["id"])
        {
            sendRelay = false;
        }
    }

    if (sendRelay)
    {
        relayMessage(input); // relay the packet
        sentList[listIndexIncrement] = (int)incoming_packet["id"];
        listIndexIncrement++;
        if (listIndexIncrement >= SENT_BUFFER_SIZE)
        {
            listIndexIncrement = 0;
        }
        Serial.println("Relaying packet...");
    }
    else
    {
        Serial.println("Already relayed this packet, skipping");
    }

#ifdef BASE_NODE
    // pull coordinates out of packet
    // check if index already exists

    int node_index = -1;
    for (int i = 0; i < num_disc_nodes; i++)
    {
        if (node_idlist[i] == (int)incoming_packet["from"])
        {
            node_index = i;
        }
    }

    if (node_index < 0)
    {
        node_index = num_disc_nodes;
        num_disc_nodes++;
    }

    node_idlist[node_index] = incoming_packet["from"];
    node_latlist[node_index] = incoming_packet["lat"];
    node_lonlist[node_index] = incoming_packet["lon"];

    Serial.print("Received updated coordinates from node ID: ");
    Serial.println(node_idlist[node_index]);
#endif
}

void initLora()
{
    LoRa.setPins(LORA_SSPIN, LORA_RSTPIN, LORA_DIO0PIN);
    SPIClass spi_lora(LORA_SPICLASS);
    LoRa.setSPI(spi_lora);

    // LoRa.setTxPower(17);
    if (!LoRa.begin(LORA_FREQ))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    Serial.println("LoRa Initialized");

    LoRa.receive();
    LoRa.onReceive(onReceive);
}

#ifdef BASE_NODE
int *getIDList()
{
    return node_idlist;
}

double *getLatList()
{
    return node_latlist;
}

double *getLonList()
{
    return node_lonlist;
}
#endif