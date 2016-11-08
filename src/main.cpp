/**
* The MIT License (MIT)
*
* Copyright (c) 2016 by Daniel Eichhorn
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SSD1306.h"

SSD1306  display(0x3c, 5, 4);

const char* ssid = "Headquarter";
const char* password = "commandcenter";
const char* mqtt_server = "m13.cloudmqtt.com";

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    msg = msg + (char)payload[i];
  }
  Serial.print(msg);
  Serial.println();

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, msg);
  display.display();

}

WiFiClient espClient;
PubSubClient client(mqtt_server, 12577, callback, espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void drawJson() {
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Hello World!");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Hello!!!");
  Serial.println();

  setup_wifi();

  if (client.connect("poyu_dev", "poyu-device", "b_0b>>w|S//i")) {
    client.subscribe("poyu");
  }

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);

  drawJson();
}

void loop() {
  // clear the display
  // display.clear();
  client.loop();
  // display.setTextAlignment(TEXT_ALIGN_RIGHT);
  // display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  // display.display();

  // counter++;
  delay(500);
}
