#include "defs.h"
#include <LoRa.h>
#include <ArduinoJson.h>
#include <SPI.h>

#define MAX_NODES 100 // maximum number of nodes in the entire network
int num_disc_nodes = 0;
int node_idlist[MAX_NODES];
double node_latlist[MAX_NODES];
double node_lonlist[MAX_NODES];

#if(NODE_TYPE == 1)
int packetCounter = 0;
void sendMessage(double lat, double lon, uint32_t time, uint32_t date)
{
    StaticJsonDocument<200> sending_data;

    char delimiter = '/';
    String packet_id = NODE_ADD + (String)delimiter + (String)packetCounter;

    sending_data["id"] = packet_id;
    sending_data["from"] = NODE_ADD;
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

    packetCounter++;
}
#endif

void sendRelay(String message)
{
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();

    LoRa.receive(); // go back to receive mode

    Serial.print("Relayed LoRa Packet: ");
    Serial.println(message);
}

#define SENT_BUFFER_SIZE 200
String sentList[SENT_BUFFER_SIZE];

#define RELAY_QUEUE_SIZE 200
String relayQueue[RELAY_QUEUE_SIZE];
int relayQueueIndex = 0;

int listIndexIncrement = 0;

void rxPackets()
{
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String input = "";
        while (LoRa.available() > 0)
        {
            input = input + (char)LoRa.read();
        }

        Serial.print("Rx Packet: ");
        Serial.println(input); // print out incoming packet

        StaticJsonDocument<200> incoming_packet;

        if (deserializeJson(incoming_packet, input))
        {
            Serial.println("JSON Decoding Failed");
            return;
        }

        // Serial.print("Packet RSSI: ");
        // Serial.println(LoRa.packetRssi());

        String packet_id = incoming_packet["id"];
        bool shouldSendRelay = true;
        for (int i = 0; i < SENT_BUFFER_SIZE; i++)
        {
            if (sentList[i] == packet_id)
            {
                shouldSendRelay = false;
            }
        }

        if (incoming_packet["from"] == NODE_ADD)
        {
            shouldSendRelay = false;
        }

        if (shouldSendRelay)
        {
            sentList[listIndexIncrement] = packet_id;
            listIndexIncrement++;
            if (listIndexIncrement >= SENT_BUFFER_SIZE)
            {
                listIndexIncrement = 0;
            }

            relayQueue[relayQueueIndex] = input;
            relayQueueIndex++;
            if (relayQueueIndex >= RELAY_QUEUE_SIZE)
            {
                relayQueueIndex = 0;
            }
        }

#if(NODE_TYPE == 0)
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

        node_idlist[node_index] = (int)incoming_packet["from"];
        node_latlist[node_index] = (double)incoming_packet["lat"];
        node_lonlist[node_index] = (double)incoming_packet["lon"];

        // Serial.print("Received updated coordinates from node ID: ");
        // Serial.println(node_idlist[node_index]);
#endif
    }
}

void onReceive(int packetSize)
{
    String input = "";
    while (LoRa.available() > 0)
    {
        input = input + (char)LoRa.read();
    }

    Serial.print("Rx Packet: ");
    Serial.println(input); // print out incoming packet

    StaticJsonDocument<200> incoming_packet;

    if (deserializeJson(incoming_packet, input))
    {
        Serial.println("JSON Decoding Failed");
        return;
    }

    // Serial.print("Packet RSSI: ");
    // Serial.println(LoRa.packetRssi());

    String packet_id = incoming_packet["id"];
    bool shouldSendRelay = true;
    for (int i = 0; i < SENT_BUFFER_SIZE; i++)
    {
        if (sentList[i] == packet_id)
        {
            shouldSendRelay = false;
        }
    }

    if (incoming_packet["from"] == NODE_ADD)
    {
        shouldSendRelay = false;
    }

    if (shouldSendRelay)
    {
        sentList[listIndexIncrement] = packet_id;
        listIndexIncrement++;
        if (listIndexIncrement >= SENT_BUFFER_SIZE)
        {
            listIndexIncrement = 0;
        }

        relayQueue[relayQueueIndex] = input;
        relayQueueIndex++;
        if (relayQueueIndex >= RELAY_QUEUE_SIZE)
        {
            relayQueueIndex = 0;
        }
    }

#if(NODE_TYPE == 0)
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

    node_idlist[node_index] = (int)incoming_packet["from"];
    node_latlist[node_index] = (double)incoming_packet["lat"];
    node_lonlist[node_index] = (double)incoming_packet["lon"];

    // Serial.print("Received updated coordinates from node ID: ");
    // Serial.println(node_idlist[node_index]);
#endif
}

void processRelays()
{
    for (int i = 0; relayQueue[i] != ""; i++)
    {
        sendRelay(relayQueue[i]);
        relayQueue[i] = "";
    }

    relayQueueIndex = 0;
}

void initLora()
{
    LoRa.setPins(LORA_SSPIN, LORA_RSTPIN, LORA_DIO0PIN);
    // SPIClass spi_lora(LORA_SPICLASS);
    // LoRa.setSPI(spi_lora);
    LoRa.setSyncWord(LORA_SYNC);
    LoRa.setTxPower(LORA_TXPOWER);

    if (!LoRa.begin(LORA_FREQ))
    {
        Serial.println("LoRa initialization failed!");
        while (1)
            ;
    }

    Serial.println("LoRa Initialized");

    LoRa.receive();
}

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