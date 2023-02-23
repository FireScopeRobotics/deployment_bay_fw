#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
String serverName = "http://192.168.39.113:42069//update-sensor";

#define LED 2
const char* ssid = "TP-LINK_783E72";
const char* password = "24842488";

const char* ssid2 = "Frank_Mesh";
const char* password2 = "Llin2014";

uint8_t LED_STATUS;

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

String init_req(String cmd){
  String command = serverName + "/" +cmd;
  return command;
}

void add_param_req(String& cmd, String param, String val){
  cmd += "/";
  cmd += param;
  if (val.length() > 0){
    cmd += "-";
    cmd += val;
  }
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP(ssid2, password2);   // add Wi-Fi networks you want to connect to
  Serial.println("Connecting ...");

  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(200);
    Serial.print('.');
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  
  lastTime = 0;
  LED_STATUS = HIGH;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis()%1000 == 0) {
    if (LED_STATUS == HIGH){
      LED_STATUS = LOW;
    }
    else{
      LED_STATUS = HIGH;
    }
    digitalWrite(LED, LED_STATUS);
  }

    // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String request = init_req("heartbeat");
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, request);
  
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

}