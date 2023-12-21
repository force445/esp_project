

#include "core0.h"
#include "core1.h"
#include "Config.h"



// // node-red
#include <Arduino.h>
#include <SoftwareSerial.h>
// #include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Modbus RTU
#include <ModbusMaster.h>


const TickType_t x_RunCore1 = 10000 / portTICK_PERIOD_MS; // Delay Bluetooth

// ** ReadMe ** 
// Kai jid dek try dek try bon park Oong
// =====================================================================================================


void RunCore1_Task(void *p) // void Loop
{  
  while(true)
  {  
    Serial.println("RunCore1_Task");
    vTaskDelay( x_RunCore1 );
  }  
}

void RunCore1_setup() 
{    
    xTaskCreatePinnedToCore(RunCore1_Task, "RunCore1_Task",  256* 8, NULL, 1, NULL, 0); 
}