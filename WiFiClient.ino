/*
 *  This sketch gets data from api.weatherapi.com
 *
 *  You need to get an api key for api.weatherapi.com
 *  
 *  Start with copying settings_default.h to settings.h and add your ssid, password and api key for api.weatherapi.com
 *
 */

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

  Serial.print("Setup");

  display.setBrightness(0x01);
  display.showNumberDec(1234);

  setup_wifi();
}

// Display float:
void display_float(float f) {
  Serial.println(f);

  // We have 4 digits: A B C D
  // We will use the point so it will be: A B C . D
  float f2 = f+0.05;
  Serial.println(f2);
  float f3 = f2*100;
  Serial.println(f3);

  int int_display = (int)(((f+0.05)*100)/10);

  Serial.print("int display: ");
  Serial.println(int_display);

  display.clear();
  display.showNumberDecEx(int_display, 0b01000000, false, 3);
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
    goto end;
  }

  Serial.print("Temp (C): ");
  Serial.println(temp_c);
  display_float(temp_c);

  // Sleep 5 mins = 300 s
  //delay(300000);

end:

  delay(5000);
}
