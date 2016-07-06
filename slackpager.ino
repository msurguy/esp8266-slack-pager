/**
   ESP8266 Arduino Real-Time Slack Pager with SSD1306 OLED display.

   Copyright (C) 2016, Maks Surguy.

   Licensed under the MIT License
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <WebSocketsClient.h>
#include <Hash.h>

#include <Wire.h>
#include "SSD1306.h" // Using https://github.com/squix78/esp8266-oled-ssd1306 library

#include <ArduinoJson.h>

#define WIFI_SSID       "******"
#define WIFI_PASSWORD   "******"
#define SLACK_TOKEN     "your-bot-token-here" // Follow https://my.slack.com/services/new/bot to create a new bot
#define SLACK_SSL_FINGERPRINT "AB F0 5B A9 1A E0 AE 5F CE 32 2E 7C 66 67 49 EC DD 6D 6A 38" // If Slack changes their SSL fingerprint, you would need to update this

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

SSD1306  display(0x3c, 4, 5);

#define Slack_Logo_width 90
#define Slack_Logo_height 26
const char Slack_Logo_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00,
  0x00, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00,
  0x00, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00,
  0x00, 0xf0, 0x01, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00,
  0x78, 0xf0, 0x3f, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00,
  0x78, 0xe0, 0x3f, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00,
  0xf8, 0xf8, 0x3f, 0x00, 0xfc, 0xf0, 0xf0, 0x03, 0xf0, 0xe1, 0xc1, 0x00,
  0xf8, 0xff, 0x3f, 0x00, 0xff, 0xf3, 0xfc, 0x0f, 0xfc, 0xe7, 0xe1, 0x01,
  0xf0, 0xff, 0x0f, 0x00, 0xff, 0xf3, 0xfc, 0x1f, 0xfe, 0xe7, 0xf1, 0x01,
  0xfc, 0xff, 0x03, 0x80, 0x87, 0xf1, 0x1c, 0x1e, 0x1f, 0xe2, 0xf9, 0x00,
  0xff, 0xff, 0x07, 0x80, 0x07, 0xf0, 0x00, 0x1c, 0x0f, 0xe0, 0x3d, 0x00,
  0xff, 0xc3, 0x07, 0x80, 0x0f, 0xf0, 0x00, 0xbc, 0x07, 0xe0, 0x1f, 0x00,
  0xff, 0x83, 0x0f, 0x00, 0x7f, 0xf0, 0xf0, 0xbf, 0x07, 0xe0, 0x1f, 0x00,
  0xee, 0x83, 0xff, 0x00, 0xfe, 0xf1, 0xfc, 0xbf, 0x07, 0xe0, 0x3f, 0x00,
  0xc0, 0x83, 0xff, 0x01, 0xf0, 0xf3, 0x3c, 0xbe, 0x07, 0xe0, 0x7f, 0x00,
  0xc0, 0xc7, 0xff, 0x01, 0xc0, 0xf3, 0x1e, 0xbc, 0x07, 0xe0, 0x79, 0x00,
  0xc0, 0xff, 0xff, 0x00, 0x80, 0xf3, 0x0e, 0x3c, 0x0f, 0xe0, 0xf1, 0x00,
  0x80, 0xff, 0x3f, 0x00, 0xcf, 0xf3, 0x1e, 0x3f, 0xbf, 0xe7, 0xe1, 0x01,
  0xe0, 0xff, 0x1f, 0x80, 0xff, 0xf3, 0xfc, 0x3f, 0xfe, 0xe7, 0xe1, 0x03,
  0xf8, 0xff, 0x3e, 0x00, 0xff, 0xf1, 0xf8, 0x3d, 0xfc, 0xe7, 0xc1, 0x03,
  0xf8, 0x1f, 0x3e, 0x00, 0x30, 0x00, 0x60, 0x00, 0xe0, 0x00, 0x80, 0x00,
  0xf8, 0x0f, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x70, 0x1f, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

long nextCmdId = 1;
bool connected = false;

/**
  Sends a ping message to Slack. Call this function immediately after establishing
  the WebSocket connection, and then every 5 seconds to keep the connection alive.
*/
void sendPing() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "ping";
  root["id"] = nextCmdId++;
  String json;
  root.printTo(json);
  webSocket.sendTXT(json);
}

void processSlackMessage(char *payload) {
  Serial.printf("Payload: %s\n", payload);

  StaticJsonBuffer<600> JSONBuffer;

  JsonObject& rootJSON = JSONBuffer.parseObject(payload);
  //Serial.printf("Type of message: %s\n", msgType);
  const char* msgType = rootJSON["type"];

  if (strcmp(msgType,"message")==0){
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 0, 128, rootJSON["text"].asString());
    display.display();
  }
}

/**
  Called on each web socket event. Handles disconnection, and also
  incoming messages from slack.
*/
void webSocketEvent(WStype_t type, uint8_t *payload, size_t len) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WebSocket] Disconnected :-( \n");
      connected = false;
      break;

    case WStype_CONNECTED:
      Serial.printf("[WebSocket] Connected to: %s\n", payload);
      sendPing();
      break;

    case WStype_TEXT:
      Serial.printf("[WebSocket] Message: %s\n", payload);
      processSlackMessage((char*)payload);
      break;
  }
}

/**
  Establishes a bot connection to Slack:
  1. Performs a REST call to get the WebSocket URL
  2. Connects the WebSocket
  Returns true if the connection was established successfully.
*/
bool connectToSlack() {
  // Step 1: Find WebSocket address via RTM API (https://api.slack.com/methods/rtm.start)
  HTTPClient http;

  Serial.println("Start request");
  http.begin("https://slack.com/api/rtm.start?token=" + SLACK_TOKEN, SLACK_SSL_FINGERPRINT);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
   Serial.printf("HTTP GET failed with code %d\n", httpCode);
   display.clear();
   display.setTextAlignment(TEXT_ALIGN_LEFT);
   display.drawString(0, 0, "Initial request failed." );
   display.display();
   return false;
  }

  // Grab the URL to websocket 
  WiFiClient *client = http.getStreamPtr();
  client->find("wss:\\/\\/");
  String host = client->readStringUntil('\\');
  String path = client->readStringUntil('"');
  path.replace("\\/", "/");

  http.end();

  // Step 2: Open WebSocket connection and register event handler
  Serial.println("WebSocket Host=" + host + " Path= " + path);
  webSocket.beginSSL(host, 443, path, "", "");
  webSocket.onEvent(webSocketEvent);
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128, "Connected to Slack!" );
  display.display();
  
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // Initialize display

  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
  }

  configTime(-7 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // Show Slack Logo
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawXbm(15, 14, Slack_Logo_width, Slack_Logo_height, Slack_Logo_bits);
  display.display();
}

unsigned long lastPing = 0;

/**
  Sends a ping every 5 seconds, and handles reconnections
*/
void loop() {
  webSocket.loop();

  if (connected) {
    // Send ping every 5 seconds, to keep the connection alive
    if (millis() - lastPing > 5000) {
      sendPing();
      lastPing = millis();
    }
  } else {
    // Try to connect / reconnect to slack
    connected = connectToSlack();
    if (!connected) {
      delay(500);
    }
  }
}
