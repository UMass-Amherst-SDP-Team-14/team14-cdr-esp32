#include <LoRa.h>
#include <ArduinoJson.h>
#include <SPI.h>

#define LORA_FREQ 915E6

String device_address = "";
void onReceive(int packetSize)
{
    String input = "";
    while (LoRa.available() > 0)
    {
        input = input + (char)LoRa.read();
    }

    Serial.println(input);
    //Serial.print("Packet RSSI: ");
    //Serial.println(LoRa.packetRssi());

    StaticJsonDocument<200> incoming_packet;
    DeserializationError error = deserializeJson(incoming_packet, input);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    String to_addr = incoming_packet["to"];
    Serial.println(to_addr);
    if (!to_addr.compareTo(device_address))
    {
        Serial.println("This packet is for me");
    }
    else
    {
        Serial.println("This packet is not for me");
    }
}

void initLora(SPIClass lora_spi, int sspin, int rstpin, int dio0pin, String unique_id)
{
    device_address = unique_id;

    LoRa.setPins(sspin, rstpin, dio0pin);
    LoRa.setSPI(lora_spi);

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

void sendMessage(String message, String destination) {

}