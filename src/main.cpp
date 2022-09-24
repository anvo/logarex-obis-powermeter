/*
 * Copyright (c) 2022 anvo
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <sstream>
#include <iomanip>

#include <WiFiManager.h> 

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

#include "obis.h"

SoftwareSerial inputSerial;
const int INPUT_PIN = 14;

const char* HOSTNAME = "logarex-obis-meter";

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer updateServer;

std::stringstream inputBuffer;
double total = 0;

void setup() {
  Serial.begin(9600);

  // Sensor Input
  Serial.println("Starting sensor");
  pinMode(INPUT_PIN, INPUT);
  inputSerial.begin(9600, SWSERIAL_8N1, INPUT_PIN, -1, false, 0, 95);
  inputSerial.enableRx(true);
  inputSerial.enableTx(false);  

  // Wifi
  Serial.println("Starting WiFi");
  
  WiFiManager wifiManager;
  if(!wifiManager.autoConnect(HOSTNAME)) {
    Serial.println("Failed to connect to WiFi");
  }
  WiFi.hostname(HOSTNAME);
  
  // MDNS
  Serial.print("Starting mDNS:");
  Serial.println(HOSTNAME);
  if(!MDNS.begin(HOSTNAME)) {
    Serial.println("Failed to start mDNS");
  }
  MDNS.addService("http", "tcp", 80);
  
  // Webserver
  Serial.println("Starting WebServer");
  server.on("/",[](){
    std::stringstream json;
    json << "{";
    if(total > 0) {
      json << "\"total\":" << std::setprecision(12) << total;
    }
    json << "}";
    server.send(200, "application/json,", json.str().c_str());
  });
  updateServer.setup(&server, "/update");
  server.begin();
  Serial.print("http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (inputSerial.available()) {      
    unsigned char c = inputSerial.read();
    inputBuffer << c;

    if (c == '\n') {
      std::vector<obis::data> data = obis::parse_lines(inputBuffer.str());

      for(obis::data& element: data) {
        Serial.print(element.code.c_str());
        Serial.print('=');
        Serial.println(element.value.c_str());

        if("1.8.0" == element.code) {
          total = obis::parse_kwh(element.value);
        }
      }
      inputBuffer.str("");
    }
  }
  server.handleClient();
  MDNS.update();
}