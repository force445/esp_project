#include "Logistic.h"
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
