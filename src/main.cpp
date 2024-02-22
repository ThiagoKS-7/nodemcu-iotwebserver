#include <Arduino.h>
/*
  BSD 3-Clause License

  Copyright (c) 2024, Thiago Kasper de Souza

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
const int relay = 5;
bool relayState = false;

ESP8266WebServer server(80);

void configRoutes() {
  server.on("/", HTTPMethod::HTTP_GET, []() {
    JsonDocument doc;
    char data[300];

    doc["data"] = "hello world";
    serializeJson(doc,data);

    server.send(200, "application/json", data);
  });
  server.on("/toggle", HTTPMethod::HTTP_GET, []() {
    relayState = !relayState;
    
    JsonDocument doc;
    char data[300];

    doc["relayState"] = relayState;
    serializeJson(doc,data);

    server.send(200, "application/json", data);
  });
}


void configServer() {
  // Configures static IP address
  Serial.print("Configuring station...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  configRoutes();
  server.begin();
  Serial.println("HTTP server started");
}


void setupRelay() {
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.flush();

  setupRelay();
  configServer();
}

void loop() {
  server.handleClient();
  if(!relayState) {
    digitalWrite(relay, LOW);
  }else {
    digitalWrite(relay, HIGH);
  }
}
