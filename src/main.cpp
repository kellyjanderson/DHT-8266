#include "DHTesp.h"
#include <ESP8266WiFi.h>
#include <FS.h>
#include <LittleFS.h>
#include <WiFiClient.h>
#include <moustache.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <AsyncDelay.h>

DHTesp dht;

float h = 0.0;
float t = 0.0;

String templateText;

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

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount file system");
    }
    Serial.println("File system mounted");

    // Read the file
    File file = LittleFS.open("/index.html", "r");
    if (!file) {
        Serial.println("Failed to open file");
    }
    // Set up the mustache template
    templateText = file.readString();

    AsyncElegantOTA.begin(&server); // Start ElegantOTA

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { 
                float tf = (t * 9/5) + 32;

                static char TempC[128];
                static char TempF[128];
                static char Humidity[128];
                sprintf(TempF, "%3.2f", tf);
                sprintf(TempC, "%3.2f", t);
                sprintf(Humidity, "%3.2f", h);
                
                
                // Prepare the data for rendering
                moustache_variable_t data[] = {
                    {"tf", TempF},
                    {"tc", TempC},
                    {"h", Humidity}
                };
        
                //sprintf(TempCF, "<h1>Temp: %3.2f C, %3.2f F</h1><h2>%3.2f% H</h2>", t, tf, h);
                // Render the template
                String renderedText = moustache_render(templateText, data);
                request->send(200, "text/html", renderedText); 
            });

    
    server.begin();
    Serial.println("HTTP server started");
    
    Serial.println("listing files:");
    Dir dir = LittleFS.openDir("/");
    
    // List all files in the root directory
    while (dir.next()) {
        Serial.print(".\n..\n");
        Serial.println(dir.fileName());
    }


    // Close the file
    file.close();
    
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
    

    // Serial.print("{\"humidity\": ");
    // Serial.print(h);
    // Serial.print(", \"temp\": {\"c\": ");
    // Serial.print(t);
    // Serial.print(", \"f\": ");
    // float tf = (t * 9 / 5) + 32;
    // Serial.print(tf);
    // Serial.print("}");

    // Serial.print("}\n");

    delay(2000);
}