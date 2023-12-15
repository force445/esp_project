// #include <Arduino.h>
// #include <color.h>

#include <WiFi.h>
// Setting mode >> 

const bool connectWeatherStation = 1 ; //  | StanAlone ="0"  | Connect_Weather station = "1" |

// *** When StandAlone use Wifi and MQTT 

// time

const TickType_t x_readModbusRTU_Delay    = 5000     / portTICK_PERIOD_MS;   // Delay I2C
const TickType_t x_MQTT_Delay             = 8000     / portTICK_PERIOD_MS;   // Delay HeartBeat
const TickType_t x_saveSDcard             = 10000     / portTICK_PERIOD_MS;   // Delay Save SD card
const TickType_t x_SendToWeatherStation   = 3000     / portTICK_PERIOD_MS;   // Delay Send to weather station


