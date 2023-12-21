
#include "core0.h"
#include "core1.h"
#include "esp_task_wdt.h"
#include "Config.h"
// ESP32 WROOM have two core

void setup()

{
  Serial.begin(115200);
  disableCore0WDT();

  // ++ CORE 0 ++


  RunCore1_setup();

  // ++ CORE 1 ++

  ERROM_Wifi_setup(); 
  // Web_setup(); // core 1  4096

  BT_setup();

  BT_Connect();
}

void loop()
{
  // Bluetooth_Connect();
  //   // ไม่ต้องทำอะไรใน loop() เนื่องจากเราใช้ Multitask
}

//     vTaskDelay(500 / portTICK_PERIOD_MS); // หน่วงเวลา 500 มิลลิวินาที
