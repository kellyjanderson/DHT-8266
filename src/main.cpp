#include "DHTesp.h"
#include <ESP8266WiFi.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <AsyncDelay.h>

DHTesp dht;

float h = 0.0;
float t = 0.0;

const char *ssid = "thecollective";
const char *password = "SouthOf44";

AsyncDelay samplingInterval;
AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    dht.setup(D4, DHTesp::DHT22);

    samplingInterval.start(200, AsyncDelay::MILLIS);    
    
    WiFi.hostname("DHT1");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    AsyncElegantOTA.begin(&server); // Start ElegantOTA

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
                static char TempCF[128];
                float tf = (t * 9/5) + 32;
                sprintf(TempCF, "<h1>Temp: %3.2f C, %3.2f F</h1><h2>%3.2f% H</h2>", t, tf, h);
                request->send(200, "text/html", TempCF); 
            });

    
    server.begin();
    Serial.println("HTTP server started");

    Serial.print("{----setup---- ");
}

void loop()
{
    if(samplingInterval.isExpired()){
        h = dht.getHumidity();
        t = dht.getTemperature();
        float h = dht.getHumidity();
        float t = dht.getTemperature();
    }
    

    Serial.print("{\"humidity\": ");
    Serial.print(h);
    Serial.print(", \"temp\": {\"c\": ");
    Serial.print(t);
    Serial.print(", \"f\": ");
    float tf = (t * 9 / 5) + 32;
    Serial.print(tf);
    Serial.print("}");

    Serial.print("}\n");

    delay(2000);
}