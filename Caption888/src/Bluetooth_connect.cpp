#include <BluetoothSerial.h>

const TickType_t x_BTCon = 2500 / portTICK_PERIOD_MS; 
BluetoothSerial SerialBTC;
void Bluetooth_Connect(void *p){
    while(true){
        if (SerialBTC.available()){
        char incomingmsg = SerialBTC.read();
        Serial.print("Received: ");
        Serial.println(incomingmsg);
        vTaskDelay(x_BTCon);
    }
    }
}

void BT_Connect(){
    xTaskCreatePinnedToCore(Bluetooth_Connect, "Bluetooth_Connect", 1024 * 8, NULL, 2, NULL, 1);
}