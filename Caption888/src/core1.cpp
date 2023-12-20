
// #include "Config.h"
#include "core0.h"
#include "core1.h"
#include "color.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const TickType_t x_Web = 2000 / portTICK_PERIOD_MS; // Delay Bluetooth
const int ssidAddr = 0;  // Address to store SSID in EEPROM
const int passAddr = 32; // Address to store password in EEPROM

int stepChangeWIFI;

String storedSSID;
String storedPass;
String NewUsername;
String NewPassword;
String UsernameFromEEPROM;
String passwordFromEEPROM;

void Web_Task(void *p) // void Loop
{
  while (true)
  {
    Serial.println("Web_Task");
    vTaskDelay(x_Web);
  }
}

void Web_setup()
{
  xTaskCreatePinnedToCore(Web_Task, "Web_Task", 2048 * 8, NULL, 1, NULL, 1);
}

void writeEEPROM(int addr, const String &data)
{
  int len = data.length();
  for (int i = 0; i < len; ++i)
  {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + len, 0); // Null-terminate string
  EEPROM.commit();
}

// Function to read data from EEPROM
void readEEPROM(int addr, String &data)
{
  char character;
  data = "";
  for (int i = addr; i < addr + 32; ++i)
  {
    character = EEPROM.read(i);
    if (character == 0)
    {
      break;
    }
    data += character;
  }
}

void handleLoginPage(AsyncWebServerRequest *request)
{
  String html = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<style>";
  html += "html, body { height: 100%; margin: 0; display: flex; justify-content: center; align-items: center; font-family: Arial, sans-serif; background-color: #f2f2f2;}";
  html += "form { background-color: white; padding: 20px; border-radius: 5px; box-shadow: 0px 0px 10px 0px rgba(0,0,0,0.2); }";
  html += "h1 { text-align: center; }";
  html += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin-bottom: 10px; border: 1px solid #ccc; border-radius: 3px; }";
  html += "input[type='submit'] { background-color: #4CAF50; border: none; color: white; padding: 15px 30px; border-radius: 3px; cursor: pointer; display: block; margin: 0 auto; }"; // ปรับขนาดของปุ่ม
  html += "</style></head><body>";

  html += "<form action='/login' method='post'>";
  html += "<h1>Setting Wifi</h1>"; // เพิ่มหัวข้อ "Smartsite"
  // html += "<p>Your OTP: " + OTP_x + "</p>";

  //   html += "<p>Your OTP: " + String(www_password) + "</p>";

  html += "<input type='text' name='username' placeholder='Username'><br>";
  html += "<input type='password' name='password' placeholder='OTP password'><br>";
  html += "<input type='submit' value='Login'>";
  html += "</form>";

  html += "</body></html>";
  request->send(200, "text/html", html);
}

void saveWiFiCredentials(const char *ssid, const char *password)
{
  // Write Wi-Fi credentials to EEPROM
  for (int i = 0; i < strlen(ssid); ++i)
  {
    EEPROM.write(i, ssid[i]);
  }
  for (int i = 0; i < strlen(password); ++i)
  {
    EEPROM.write(32 + i, password[i]);
  }
  EEPROM.commit();
}

void ERROM_Wifi_Task(void *p) // void Loop
{
  while (true)
  {
    Serial.print("Step >> ");
    Serial.print(stepChangeWIFI);
    Serial.print(" IP address: ");
    Serial.println(WiFi.localIP());

    readEEPROM(0, UsernameFromEEPROM);  // Assuming the password is stored starting from address 32 in EEPROM
    readEEPROM(32, passwordFromEEPROM); // Assuming the password is stored starting from address 32 in EEPROM

    Serial.print("User : ");
    Serial.print(UsernameFromEEPROM);
    Serial.print("| Password : ");
    Serial.println(passwordFromEEPROM);

    if (stepChangeWIFI == 1)
    {
      WiFi.disconnect();
      vTaskDelay(500 / portTICK_PERIOD_MS); 
      stepChangeWIFI = 2;
    }
    else if (stepChangeWIFI == 2)
    {
      WiFi.begin(UsernameFromEEPROM.c_str(), passwordFromEEPROM.c_str());
      Serial.println("Connecting to WiFi...");

      while (WiFi.status() != WL_CONNECTED)
      {
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
        Serial.println("Connecting...");
      }

      Serial.println("Connected to WiFi!");
      
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      stepChangeWIFI = 0;
    }

    vTaskDelay(x_Web);
  }
}

void ERROM_Wifi_setup()
{

  EEPROM.begin(512); // Initialize EEPROM
  // Write initial WiFi credentials to EEPROM  >>>>>>> For Connect Frist time !!!

  writeEEPROM(ssidAddr, "kfc");
  writeEEPROM(passAddr, "12345678");

  // Read WiFi credentials from EEPROM

  readEEPROM(ssidAddr, storedSSID);
  readEEPROM(passAddr, storedPass);

  // Connect to WiFi using stored credentials
  WiFi.begin(storedSSID.c_str(), storedPass.c_str());

  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //  Setting ModbusTCP IP Address

  // Setting Server

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    // Serve your HTML form to input Wi-Fi credentials
    request->send(200, "text/html", "<form action='/save' method='post'>SSID: <input type='text' name='ssid'><br>Password: <input type='password' name='password'><br><input type='submit' value='Save'></form>"); });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request)
            {
    // Handle form submission and save credentials to EEPROM
    String Newssid = request->arg("ssid");
    String Newpassword = request->arg("password");
  

    Serial.print("New User : ");
    Serial.println(Newssid);
    Serial.print("New Password : ");
    Serial.print(Newpassword);

    readEEPROM(0, UsernameFromEEPROM);  // Assuming the password is stored starting from address 32 in EEPROM
    readEEPROM(32, passwordFromEEPROM); // Assuming the password is stored starting from address 32 in EEPROM

  if(((Newssid != UsernameFromEEPROM) || (Newpassword != passwordFromEEPROM)) && stepChangeWIFI == 0){
      stepChangeWIFI = 1 ;
  }

    writeEEPROM(ssidAddr, Newssid);
    writeEEPROM(passAddr, Newpassword);

    request->send(200, "text/plain", "Credentials saved."); });

  server.begin();

  xTaskCreatePinnedToCore(ERROM_Wifi_Task, "ERROM_Wifi_Task", 2048 * 8, NULL, 1, NULL, 1);
}
