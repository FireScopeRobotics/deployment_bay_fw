#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <TaskScheduler.h>

//path to the server, change depending on the ip address of the server
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
String serverName = "http://192.168.39.113:42069";

//pinout definitions
#define LED 2

//wifi passwords
const char* ssid = "TP-LINK_783E72";
const char* password = "24842488";
const char* ssid2 = "Frank_Mesh";
const char* password2 = "Llin2014";

bool LED_STATUS;

//tasks
void led_blink(); //test/stayalive task
Task led_blink_t(1000, TASK_FOREVER, &led_blink);
void heartbeat(); //heartbeat generater request receiver
Task heartbeat_t(250, TASK_FOREVER, &heartbeat);

//fw variables
String serial_code = "";

//multitasking variables:
Scheduler runner;

//helper functions
String init_req(String cmd);
void add_param_req(String& cmd, String param, String val);



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

  //once the wifi has been connected, query for a serial id from the server
  while (serial_code.length() == 0){
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      String request = init_req("request-init");
        
      // Your Domain name with URL path or IP address with path
      http.begin(client, request);

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode>0) {
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        serial_code = payload;
      }
      else{
        Serial.println(httpResponseCode);
        Serial.println("No or failed response from server\n retrying....");
      }
            // Free resources
      http.end();
      delay(1000);
    }
  }

  //add tasks to scheduler
  runner.addTask(led_blink_t);
  runner.addTask(heartbeat_t);
  led_blink_t.enable();
  heartbeat_t.enable();

  LED_STATUS = HIGH;
}

void loop() {
    runner.execute();
}


void led_blink(){
  LED_STATUS = !LED_STATUS;
  digitalWrite(LED_BUILTIN, LED_STATUS);
}

void heartbeat(){
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
}





//helpers
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