#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <Wire.h>
#include <BMP180MI.h>

#include "secrets.h"

#define BMP180_I2C_ADDR 0x77
#define INVALID_TEMPERATURE -500
BMP180I2C bmp180(BMP180_I2C_ADDR);

/***********************************/

#define DEVICE_NAME "urlencoded%20device%20name"

/***********************************/

void setup() {
  Serial.begin(115200);
  Wire.begin(D3,D4);

  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WIFI: Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("WIFI: IP address: ");
  Serial.println(WiFi.localIP());

  // BMP180 setup
  if (!bmp180.begin()) {
    Serial.println("BMP180: begin() failed. check your BMP180 Interface and I2C Address.");
    while (1);
  }
  bmp180.resetToDefaults();
  bmp180.setSamplingMode(BMP180MI::MODE_UHR);
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client;
  HTTPClient http;

  if (http.begin(client, "http://" SERVER_ADDRESS ":8080/log")) {
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    //start connection and send HTTP header
    char body[100];
    sprintf(body, "temp=%.2f&name=%s", getTemperature(), DEVICE_NAME);
    int code = http.POST(body);
    if(code > 0) {
      Serial.printf("HTTP response %d\n", code);
    }
    else {
      Serial.printf("HTTP error: %s\n", http.errorToString(code).c_str());
    }
    http.end();
  }
  else {
    Serial.println("Unable to connect");
  }
  delay(10000);
}

float getTemperature() {
  if (!bmp180.measureTemperature())
  {
    Serial.println("BMP180: could not start temperature measurement, is a measurement already running?");
    return INVALID_TEMPERATURE;
  }
  do {
    delay(100);
  } while(!bmp180.hasValue());
  return bmp180.getTemperature();
}
