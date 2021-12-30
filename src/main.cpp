#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// LED - https://microcontrollerslab.com/led-blinking-using-esp8266-nodemcu/
// Button - https://www.instructables.com/NodeMCU-ProjectButton-Control-LED/
// WIFI - https://techtutorialsx.com/2016/07/17/esp8266-http-get-requests/

#define LED D0
#define BUTTON D2

int buttonState = 0;
int oldButtonState = 0;
int lightsOn = 0;
unsigned long previousMillis = 0; // will store last time of the check
const long interval = 10000;      // check for new value

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT);
  WiFi.begin("AP", "Password");
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // TODO Check WIFI state - blink LED if no connection - if (WiFi.status() == WL_CONNECTED)
  WiFiClient client;
  HTTPClient http;

  // Send regular HTTP GET request to server
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    
    http.begin(client, "http://danielweisser.de/lampe/");
    if (http.GET() == HTTP_CODE_OK)
    {
      String payload = http.getString();
      Serial.println("Got state: " + payload);
      lightsOn = payload.toInt();
    }
    http.end();
  }

  buttonState = digitalRead(BUTTON);
  if (buttonState != oldButtonState) // catch change
  {
    oldButtonState = buttonState;
    if (buttonState == HIGH)
    {
      lightsOn = !lightsOn;

      http.begin(client, "http://danielweisser.de/lampe/");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpCode = http.POST("state=" + String(lightsOn));
      if (httpCode == HTTP_CODE_OK)
      {
        Serial.println("Sent state: " + String(lightsOn));
      }
      http.end();
    }
  }
  digitalWrite(LED, lightsOn ? HIGH : LOW);
}