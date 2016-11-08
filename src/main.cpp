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

#include "OLEDDisplayUi.h"
#include "images.h"

SSD1306 display(0x3c, 5, 4);
OLEDDisplayUi ui(&display);

const char* ssid = "Headquarter";
const char* password = "commandcenter";
const char* mqtt_server = "m13.cloudmqtt.com";

String message = "No new message...";
bool msgRead = true;

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    msg = msg + (char)payload[i];
  }
  message = msg;
  msgRead = false;
  Serial.print(msg);
  Serial.println();
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

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void drawStatusBar(OLEDDisplay *display, OLEDDisplayUiState* state) {
  if (WiFi.status() == WL_CONNECTED) {
    display->drawXbm(2, 2, wifi_width, wifi_height, wifi_bits);
  }
  if (client.connected() == true) {
    display->drawXbm(20, 2, mqtt_width, mqtt_height, mqtt_bits);
  }
  if (! msgRead) {
    display->drawXbm(110, 2, newMsg_width, newMsg_height, newMsg_bits);
  }
}

void drawDaysTogether(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_24);
  display->drawString(x + 64, y + 13, "76");
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + 64, y + 36, "Days Together");
}

void drawMessage(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(x + 64, y + 13, message);
  msgRead = true;
}

void nextBtnInterrupt() {
  ui.nextFrame();
}

void prevBtnInterrupt() {
  ui.previousFrame();
}

FrameCallback frames[] = { drawDaysTogether, drawMessage };
int frameCount = 2;

OverlayCallback overlays[] = { drawStatusBar };
int overlaysCount = 1;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Hello!!!");
  Serial.println();

  attachInterrupt(12, prevBtnInterrupt, FALLING);
  attachInterrupt(13, nextBtnInterrupt, FALLING);
  // attachInterrupt(0, testInterrupt, CHANGE);

  ui.setTargetFPS(60);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.disableAutoTransition();
  ui.setFrames(frames, frameCount);
  ui.setOverlays(overlays, overlaysCount);
  ui.init();
  display.flipScreenVertically();

  ui.update(); // Draws the initial taskbar

  setup_wifi();

  if (client.connect("poyu_dev", "poyu-device", "b_0b>>w|S//i")) {
    client.subscribe("poyu");
  }

}

void loop() {
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    client.loop();
  }
}
