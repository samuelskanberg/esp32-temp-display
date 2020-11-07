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
#include "settings.h"

void setup()
{
    //Serial.begin(115200);
    Serial.begin(9600);
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
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

void loop()
{
    const char* host = "api.weatherapi.com";

    Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/v1/current.json?key="+settings_api_key+"&q=Haninge";

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
            return;
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
    JsonObject current = doc["current"];
    int current_temp_c = current["temp_c"];
    Serial.print("Temp (C): ");
    Serial.println(current_temp_c);

    Serial.println();
    Serial.println("closing connection");

    delay(5000);
}
