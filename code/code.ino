#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <DHT.h>

#define TFT_CS 10
#define TFT_RST 9
#define TFT_DC 8
#define DHT_PIN 7

#define TDS_PIN A0

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

DHT dht(DHT_PIN, DHT11);

float tdsValue;

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  dht.begin();
  pinMode(TDS_PIN, INPUT);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  int sensorValue = analogRead(TDS_PIN);
  tdsValue = (float)(133.42 * pow((double)sensorValue / 1024.0, 3) - 255.86 * pow((double)sensorValue / 1024.0, 2) + 857.39 * (double)sensorValue / 1024.0) * 0.5;
  
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Temperature:");
  tft.setCursor(0, 20);
  tft.print(temperature);
  tft.print(" C");
  tft.setCursor(0, 40);
  tft.print("Humidity:");
  tft.setCursor(0, 60);
  tft.print(humidity);
  tft.print(" %");
  tft.setCursor(0, 80);
  tft.print("TDS:");
  tft.setCursor(0, 100);
  tft.print(tdsValue);
  tft.print(" ppm");
  
  delay(1000);
}
