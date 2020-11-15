/*
 *  This sketch gets data from api.weatherapi.com
 *
 *  You need to get an api key for api.weatherapi.com
 *  
 *  Start with copying settings_default.h to settings.h and add your ssid, password and api key for api.weatherapi.com
 *
 */

#include <math.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <TM1637Display.h>
#include "settings.h"

#define LED_ONBOARD 2

#define CLK 19
#define DIO 18

TM1637Display display(CLK, DIO);

void setup_wifi()
{
  Serial.print("Connecting to ");
  Serial.println(settings_ssid);

  WiFi.begin(settings_ssid, settings_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display_error_sign();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(9600);
  delay(10);

  Serial.println("Setup");

  display.setBrightness(0x01);
  display.clear();
  // Show an "underscore" at the right most digit
  uint8_t segments[] = { 0b00001000 };
  display.setSegments(segments, 1, 3);

  setup_wifi();
}

int error_shown = 0;

void display_int(int i)
{
  error_shown = 0;
  display.clear();
  display.showNumberDec(i);
}

// This will display a "rotating" error sign
void display_error_sign()
{
  uint8_t segments[] = { 0 };
  int mod = error_shown % 4;
  if (mod == 0) {
    segments[0] = 0b00000001;
  } else if (mod == 1) {
    segments[0] = 0b00000010;
  } else if (mod == 2) {
    segments[0] = 0b01000000;
  } else {
    segments[0] = 0b00100000;
  }
  display.setSegments(segments, 1, 0);
  error_shown++;
}

// Returns true if all went well
bool get_weather_data(float *temp)
{
  const char* host = "api.openweathermap.org";

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  // We now create a URI for the request
  // Here one can see the info for Haninge: https://openweathermap.org/city/2707953
  String url = "/data/2.5/weather?q=Haninge&units=metric&appid="+settings_api_key;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  String json;
  while(client.available()) {
    String line = client.readStringUntil('\r');
    line.trim();
    if (line.startsWith("{")) {
      json = line;
    }
  }
  Serial.println("--- JSON ---");
  Serial.println(json);
  Serial.println("---------------");

  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(23) + 520;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, json);
  JsonObject current = doc["main"];
  float current_temp_c = current["temp"];
  *temp = current_temp_c;

  return true;
}

void loop()
{
  float temp_c;
  bool success = get_weather_data(&temp_c);
  if (!success) {
    Serial.println("Failed getting weather data");
    display_error_sign();
    // Sleep 5 seconds, then try again
    delay(5000);
    return ;
  }

  Serial.print("Temp (C): ");
  Serial.println(temp_c);
  int temp_c_rounded = round(temp_c);
  display_int(temp_c_rounded);


  // Wait 5 minutes
  delay(300000);
}
