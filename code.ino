#include <DFRobot_DHT11.h>
#include <TFT.h>  
#include <SPI.h>
#include <Wire.h>
#define cs   10
#define dc   9
#define rst  8
#define DHTPIN 7
#define led 4
#define buz 6

#define TdsSensorPin A0
#define VREF 5.0              // analog reference voltage(Volt) of the ADC
#define SCOUNT  30    

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
DFRobot_DHT11 DHT;
float humidity,temperature = 16;
String temp,hum,soil_moist;
int soil_moisture,SM;
char T[5],H[5],moist[3];

int analogBuffer[SCOUNT];     // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

float averageVoltage = 0;
float tdsValue = 0;

// median filtering algorithm
int getMedianNum(int bArray[], int iFilterLen){
  int bTab[iFilterLen];
  for (byte i = 0; i<iFilterLen; i++)
  bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0){
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void display_data_serial()
 {
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" deg C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %RH");

 }
void setup()
{
  TFTscreen.begin();    
  TFTscreen.background(0, 0, 0);
  Serial.begin(9600);
  Serial.println("temperature and purity monitoring program");
  TFTscreen.setTextSize(2);  
  TFTscreen.stroke(255,0,255);  
  TFTscreen.text("Tem: ", 0, 10);
  TFTscreen.text(" *C", 115, 15);
  TFTscreen.stroke(0,255,255);  
  TFTscreen.text("Moist: ", 0,100);
  TFTscreen.text("%", 110, 95);
  TFTscreen.setTextSize(3);
  pinMode(TdsSensorPin,INPUT);  
  pinMode(led,OUTPUT);
  pinMode(buz,OUTPUT);
}

void loop()
{

   static unsigned long analogSampleTimepoint = millis();
  if(millis()-analogSampleTimepoint > 40U){     //every 40 milliseconds,read the analog value from the ADC
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if(analogBufferIndex == SCOUNT){
      analogBufferIndex = 0;
    }
  } 

   static unsigned long printTimepoint = millis();
  if(millis()-printTimepoint > 800U){
    printTimepoint = millis();
    for(copyIndex=0; copyIndex<SCOUNT; copyIndex++){
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
     
      // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0;
     
      //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationCoefficient = 1.0+0.02*(temperature-25.0);
      //temperature compensation
      float compensationVoltage=averageVoltage/compensationCoefficient;
     
      //convert voltage value to tds value
      tdsValue=(133.42*compensationVoltage*compensationVoltage*compensationVoltage - 255.86*compensationVoltage*compensationVoltage + 857.39*compensationVoltage)*0.5;
     
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
    }
  }
  
  DHT.read(DHTPIN);
  humidity = DHT.humidity;
  temperature = DHT.temperature;
  digitalWrite(led,1);
  digitalWrite(buz,1);
  display_data_serial();
  hum = String(humidity);
  temp = String(temperature);
  hum.toCharArray(H,5);
  temp.toCharArray(T,5);
  TFTscreen.stroke(0,0,200);  
  TFTscreen.text(T,50,5);
  TFTscreen.stroke(0,200,0);  
  TFTscreen.text(H,50,55);
  TFTscreen.stroke(200,0,0);  
  TFTscreen.text(moist,70,95);
  delay(200);
  digitalWrite(led,0);
  digitalWrite(buz,0);
  delay(1800);
  TFTscreen.stroke(0,0,0);
  TFTscreen.text(T,50,5);
  TFTscreen.text(H,50,55);
  TFTscreen.text(moist,70,95);
  delay(200);
}
