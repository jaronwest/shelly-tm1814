#include <Arduino_JSON.h>

// for api to GET information and POST changes to this device
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// for OTA updates
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// for controlling TM1814 LED strip (12v AWWRGB)
#include <NeoPixelBus.h>

#include "arduino_secrets.h"

/* we are using a Shelly Relay with ESP8266 chip */
const uint8_t pixelPin = 3;       // this is ignored for ESP8266 and always uses RX pin
const uint8_t powerPin = 4;      // pin that powers the strip on/off via relay

// Your WiFi credentials in secrets
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

/*##
  use an array of colors with 1, 2, or more colors
  also include an optional point change, otherwise assumed even distribution
  0-100 as a percent of the whole strip
  [0, 50, 100] would be three even colors
  [0, 25, 100] would switch from color 1 to 2 in only 25% of the strip then 75% to color 3
*/

/* this is the number of pixels (string of 3 LEDs = 1 pixel)
   making it smaller will cause a failure */
const uint16_t pixelCount = 6;
int ww = 90;
// rainbow size: 1.0 is full rainbow, 0.5 is half rainbow, 2.0 is two full rainbows
float rainbowSize = 0.25;
// rainbow speed from 1-50 (slow to fast)
int rainbowSpeed = 40;
// rainbow brightness from 1-255 (dim to bright)
int rainbowBrightness = 255;
char mdns[] = "esp8266-test"; 

int r = 0;
int g = 0;
int b = 0;
int ww2 = ww;
int r2 = r;
int g2 = g;
int b2 = b;
int wwDelta = 0;
int rDelta = 0;
int gDelta = 0;
int bDelta = 0;

int mode = 2;
bool power = 1;


// location of rainbow from 0-255 of the color wheel
int rainbowPos = 0;
//track the clock for rainbow speed changes in position
unsigned long clockMs = millis();

NeoPixelBus<NeoWrgbTm1814Feature, NeoTm1814Method> strip(pixelCount, pixelPin);

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.println("");

  // setup LED strip first with a warm white fade up

  strip.Begin();
  
  strip.SetPixelSettings(NeoTm1814Settings(160,160,160,160));
  strip.Show();

  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  unsigned long lastBrightening = millis();
  int warmup = 10;
  while (warmup < 255) {
    for (int i = 0; i < pixelCount; i++) {
      strip.SetPixelColor(i, RgbwColor(warmup, 0, 0, 0));
    }
    strip.Show();
    if (millis() - lastBrightening > 30) {
      warmup++;
    }
  }
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    for (int i = 0; i < pixelCount; i++) {
      strip.SetPixelColor(i, RgbwColor(255, 0, 0, 0));
    }
    strip.Show();
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin(mdns)) {
    Serial.println("MDNS responder started");
  }
  ArduinoOTA.setHostname(mdns);
  
  // OTA code
  ArduinoOTA.onStart([]() {
    digitalWrite(powerPin, LOW);
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

// Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  ArduinoOTA.handle();
  
  handleStrip();
  server.handleClient();
}