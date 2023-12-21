
// #include "Config.h"
#include "core0.h"
#include "core1.h"

#include <EEPROM.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const TickType_t x_Web = 2000 / portTICK_PERIOD_MS; // Delay Bluetooth

// ModbusIP mb;

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

// ---------------Ai Prediction Model

#include <Arduino.h>

#pragma once
namespace Eloquent {
    namespace ML {
        namespace Port {
            class LogisticRegression {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        float votes[8] = { 0.0f };
                        votes[0] = dot(x,   -0.252061898245  , 0.109712447171  , 0.145782761034 );
                        votes[1] = dot(x,   -0.11161064993  , -0.004441849411  , 0.160572589063 );
                        votes[2] = dot(x,   0.253408759936  , -0.111743349183  , -0.090885443051 );
                        votes[3] = dot(x,   0.221314316886  , 0.282195264686  , -0.495864031042 );
                        votes[4] = dot(x,   -0.245372314017  , -0.114081723784  , 0.34736126184 );
                        votes[5] = dot(x,   0.148349014425  , -0.316607735215  , 0.200251309322 );
                        votes[6] = dot(x,   -0.418247938405  , 0.297198736661  , 0.033778510179 );
                        votes[7] = dot(x,   0.40422070935  , -0.142231790925  , -0.300996957344 );
                        // return argmax of votes
                        uint8_t classIdx = 0;
                        float maxVotes = votes[0];

                        for (uint8_t i = 1; i < 8; i++) {
                            if (votes[i] > maxVotes) {
                                classIdx = i;
                                maxVotes = votes[i];
                            }
                        }

                        return classIdx;
                    }

                protected:
                    /**
                    * Compute dot product
                    */
                    float dot(float *x, ...) {
                        va_list w;
                        va_start(w, 3);
                        float dot = 0.0;

                        for (uint16_t i = 0; i < 3; i++) {
                            const float wi = va_arg(w, double);
                            dot += x[i] * wi;
                        }

                        return dot;
                    }
                };
            }
        }
    }


//  Color ==========================
Eloquent::ML::Port::LogisticRegression clf;

// TCS230 or TCS3200 pins wiring to Arduino
#define S0 5  // d1  
#define S1 4  // d2
#define S2 2  // d4
#define S3 14  // d5
#define sensorOut 12  // d6

#define Num_Arry 3

int Calculate_Color(float out);

// Stores frequency read by the photodiodes
int   red = 0;
int   green = 0;
int   blue = 0;
int   red_cal;
int   green_cal;
int   blue_cal;
int   Static_Color;
unsigned int minR ,minG ,minB;
unsigned int R_G_B_min ;
unsigned int R[15],G[15],B[15] ;
unsigned int C_R;

float R_G_B_Mean[3];

int * Ra = new int[10] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int * Ga = new int[10] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int * Ba = new int[10] {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

void Set_Pin_Color(){
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,HIGH);
}
void Read_rgb() {
  //delayMicroseconds(100);
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  //red = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
  red = pulseIn(sensorOut, LOW);
  delayMicroseconds(10);
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  green = pulseIn(sensorOut, LOW);
  delayMicroseconds(10);
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  delayMicroseconds(10);
  blue = pulseIn(sensorOut, LOW);
}
float getMean(int * val, int arrayCount ,int Index) {
  long total = 0;
  for (int i = 0; i < arrayCount; i++) {
    total = total + val[i];
  }
  float avg = total/(float)arrayCount;
  R_G_B_Mean[Index] = avg ;
  return avg;
}

/*
 * Get the standard deviation from an array of ints
 */
float getStdDev(int * val, int arrayCount , int Index) {
  float avg = getMean(val, arrayCount,Index);
  long total = 0;
  for (int i = 0; i < arrayCount; i++) {
    total = total + (val[i] - avg) * (val[i] - avg);
  }

  float variance = total/(float)arrayCount;
  float stdDev = sqrt(variance);
  return stdDev;
}
String Color_out[8] = {"Unknow","Iron","Black","White","Yellow","Green","Red","Blue" };
bool  Contrast = 0;
bool  Update_Out = 0;

float RGB_Mean(){
float out = 0;
Read_rgb();
C_R++;if ( C_R >= Num_Arry ){ C_R = 0; };
Ra[C_R] = red ;
Ga[C_R] = green ;
Ba[C_R] = blue ;

int arrayCount = Num_Arry; // number of elements
float Rstd  = getStdDev(Ra, arrayCount,0);
float Gstd  = getStdDev(Ga, arrayCount,1);
float Bstd  = getStdDev(Ba, arrayCount,2);
  
unsigned int R_G_B_sum ;
R_G_B_sum = Rstd + Gstd + Bstd ;

if ( red > 500 || green > 500 || blue > 500  ){
   if (  R_G_B_sum < 100 ) {
        if ( Contrast == 1) 
        { 
           out = Calculate_Color(out); 
           Static_Color = out;
           Update_Out = 1;
        }
        Contrast = 0;
        R_G_B_min = 10000;
   }     
}
else{
  
  if ( R_G_B_min > (R_G_B_sum ) )
  { 
       R_G_B_min = R_G_B_sum ;
       red_cal = R_G_B_Mean[0] ;
       green_cal = R_G_B_Mean[1] ;
       blue_cal = R_G_B_Mean[2] ;
//       Serial.print(red_cal);Serial.print( "," );
//       Serial.print(green_cal);Serial.print( "," );
//       Serial.print(blue_cal); Serial.print("  ");
//       Serial.print(R_G_B_min); Serial.print("  ");
       Contrast = 1; 
  }
  
}
  return (out);
}
int Calculate_Color(float out){
unsigned int OUT  ;
        Serial.print(red_cal);Serial.print( "," );
        Serial.print(green_cal);Serial.print( "," );
        Serial.print(blue_cal); Serial.print("  ");
        Serial.print(R_G_B_min); Serial.print("  ");
        if ( red_cal < 50 && green_cal < 50 && blue_cal < 50 ){
            OUT = 3;            
        }
        else if ( red_cal > 250 && green_cal > 250 && blue_cal > 250 ){
            OUT = 2;   
        }
        else {
            float Out1[] = {float(red_cal),float(green_cal),float(blue_cal)};
            OUT = clf.predict(Out1);           
        }
        Contrast = 1;
        Serial.println(Color_out[OUT]);
        return(OUT);     
}
void Test_RGB(){
  while(1){
      Read_rgb();
      Serial.print(red);Serial.print(",");
      Serial.print(green);Serial.print(",");
      Serial.println(blue);
      delay(100);
  }
}
void Check_c(){ 
bool check=0;
long r1=0,g1=0,b1=0;int tim = 1;
/*
     for ( int i = 1 ; i < 13 ; i ++){
           Read_rgb();
           R[i] = red ;G[i] = green ;B[i] = blue ;   
     }
     int i;

     for (  i = 0 ; i < 13 ; i ++){
           r1 += R[i]; g1 += G[i]; b1 += B[i]; 
     }
       
     red =    (r1) / (i); 
     green =  (g1) / (i);
     blue =   (b1) / (i);
     */
     Read_rgb();
     

}

// ------------------  END of Ai predect Model----------------------------------------------------

// ==================  START Color  ==================



// ==================  END of color =================


// Function to write data to EEPROM
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

// ==============

const int ssidAddr = 0;  // Address to store SSID in EEPROM
const int passAddr = 32; // Address to store password in EEPROM
String storedSSID;
String storedPass;
String NewUsername;
String NewPassword;
String UsernameFromEEPROM;
String passwordFromEEPROM;
int stepChangeWIFI;

// ====== Coding HTML + CSS + javascripe  //

// --------------------------------------------------- Start Void Coding ----------------------

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

// ========================================================================
