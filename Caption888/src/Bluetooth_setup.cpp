#include <BluetoothSerial.h>
#include <Arduino.h>

const TickType_t x_BTSetup = 1000 / portTICK_PERIOD_MS; 
BluetoothSerial SerialBT;

void Bluetooth_setup(void *p){
    Serial.begin(115200);
    SerialBT.begin("PYAM_BT");
    vTaskDelay(x_BTSetup);
}

void BT_setup(){
    xTaskCreatePinnedToCore(Bluetooth_setup, "Bluetooth_setup", 1024 * 8, NULL, 1, NULL, 1);
}
