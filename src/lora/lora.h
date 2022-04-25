void sendMessage(double lat, double lon, uint32_t time, uint32_t date);
void sendRelay(String message);
void rxPackets();
void onReceive(int packetSize);
void processRelays();
void initLora();
int* getIDList();
double* getLatList();
double* getLonList();