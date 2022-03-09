void initLora(SPIClass lora_spi, int sspin, int rstpin, int dio0pin, String unique_id);
void onReceive(int packetSize);
void sendMessage(String message, String destination);