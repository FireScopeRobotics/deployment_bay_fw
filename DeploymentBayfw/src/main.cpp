#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <TaskScheduler.h>
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//path to the server, change depending on the ip address of the server
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
String serverName = "http://192.168.0.101:42069";

//pinout definitions
#define LED 2

typedef enum 
{ 
    IDLE, 
    UNDOCKING, 
    DOCKING,
    OPERATING,
    NETWORK_ERROR,
    ERROR,
} dock_status;

//wifi passwords
const char* ssid = "TP-LINK_783E72";
const char* password = "24842488";
const char* ssid2 = "Frank_Mesh";
const char* password2 = "Llin2014";

bool LED_STATUS;

//tasks
void led_blink(); //test/stayalive task
Task led_blink_t(400, TASK_FOREVER, &led_blink);
void heartbeat(); //heartbeat generater request receiver
Task heartbeat_t(500, TASK_FOREVER, &heartbeat);
void ledstrip();
#define led_period 25
Task ledstrip_t(led_period, TASK_FOREVER, &ledstrip);

//fw variables
String serial_code = "";

//multitasking variables:
Scheduler runner;

//helper functions
String init_req(String cmd);
void add_param_req(String& cmd, String param, String val);
void change_LED_status(dock_status status);
std::vector<std::string> spitString(std::string str, std::string key);

//colorstrip
#define CTRL_PIN1 4 //D2
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(60, CTRL_PIN1, NEO_GRB + NEO_KHZ800); //32 lights
#define num_pixels_strip1 32
// #define CTRL_PIN2 4
// Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(60, CTRL_PIN2, NEO_GRB + NEO_KHZ800);
struct led_status_s{
  dock_status status;
  int pixel_var;
  int8_t sign;
  int vars [num_pixels_strip1]; 
  int8_t signs [num_pixels_strip1];
};
led_status_s led_status;



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
      String request = serverName + "/" + "request-init";
        
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

  //setup led strips
  strip1.begin();
  strip1.setBrightness(64);
  strip1.show(); // Initialize all pixels to 'off'
  // change_LED_status(IDLE);
  change_LED_status(UNDOCKING);


  //add tasks to scheduler
  runner.addTask(led_blink_t);
  runner.addTask(heartbeat_t);
  runner.addTask(ledstrip_t);
  led_blink_t.enable();
  heartbeat_t.enable();
  ledstrip_t.enable();

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
        std::string payld = payload.c_str();
        Serial.println(payld.size());
        if (payld.size() > 1){
          std::vector tokens = spitString(payld, std::string("-"));
        
          if (led_status.status == NETWORK_ERROR){
            change_LED_status(IDLE);
          }

          if (tokens[0] == "OpenDoor"){
          }
          else if (tokens[0] == "SwtichLights"){
            Serial.println(tokens[1].size());
            Serial.println(tokens[1] == "IDLE");
            if (tokens[1] == "IDLE") {change_LED_status(IDLE); Serial.println("here");}
            else if (tokens[1] == "UNDOCKING") change_LED_status(UNDOCKING);
            else if (tokens[1] == "DOCKING") change_LED_status(DOCKING);
            else if (tokens[1] == "OPERATING") change_LED_status(OPERATING);
            else if (tokens[1] == "ERROR") change_LED_status(ERROR);
          }
        }

      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
        if (led_status.status != NETWORK_ERROR){
          change_LED_status(NETWORK_ERROR);
        }
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");

    }

    //ESP.restart()
}

void ledstrip(){

  switch (led_status.status){
    case (IDLE):
      for( int i=0; i < num_pixels_strip1; i++) {
        strip1.setPixelColor(i, strip1.Color(0, 0, led_status.pixel_var));
      }
      led_status.pixel_var += 1*led_status.sign;
      if (led_status.pixel_var >= 255){
        led_status.sign = -1;
      }
      else if (led_status.pixel_var <= 0){
        led_status.sign = 1;
      }
      break;
    case (UNDOCKING):
    case (DOCKING):
    {

      for( int i=0; i < num_pixels_strip1; i++) {
        strip1.setPixelColor(i,  strip1.Color(0, led_status.vars[i], 0));
        led_status.vars[i] += 255/num_pixels_strip1*led_status.signs[i]*2;
        if (led_status.vars[i] >= 255){
          led_status.signs[i] = -1;
          led_status.vars[i] = 255;
        }
        else if (led_status.vars[i] <= 0){
          led_status.signs[i] = 1;
          led_status.vars[i] = 0;
        }
      }

      break;

    }
    case (NETWORK_ERROR):
    case (ERROR):
      int ticks = int(500/led_period);
      if (led_status.pixel_var > ticks){
        for(int i=0; i< num_pixels_strip1; i++) {
          strip1.setPixelColor(i, strip1.Color(125 + led_status.sign*125 , 0, 0));
        }
        led_status.pixel_var = 0;
        if (led_status.sign == 1) led_status.sign = -1;
        else if (led_status.sign == -1) led_status.sign = 1;
      }
      led_status.pixel_var++;
      break;

  }
  strip1.show();
}

//helpers
std::vector<std::string> spitString(std::string str, std::string key)
{
    const std::string WHITESPACE = " \n\r\t\f\v";
    std::vector<std::string> tokens;

    size_t start = str.find_first_not_of(WHITESPACE);
    size_t end = str.find_last_not_of(WHITESPACE);
    std::string wrking = str.substr(start, end+1); //trim whitespace
    // std::string wrking = str;
    while (wrking.find(key) != std::string::npos){
        auto pos = wrking.find(key);
        
        tokens.push_back(wrking.substr(0, pos));
        wrking.erase(0, pos + key.length());
    }
    tokens.push_back(wrking);
    
    return tokens;
}


String init_req(String cmd){
  String command = serverName + "/" +serial_code + "/" + cmd;
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
void change_LED_status(dock_status status){
  led_status.status = status;
  switch (status){
    case (IDLE):
      led_status.pixel_var = 0;
      led_status.sign = 1;
      break;
    case (UNDOCKING):
    case (DOCKING):
      for (int i = 0; i < num_pixels_strip1; i++){
        if (i == 0){
          led_status.vars[i] = 0;
        }
        else{
          led_status.vars[i] = led_status.vars[i-1] + int(255/num_pixels_strip1);
        }
        if (status == UNDOCKING){
          led_status.signs[i] = -1;
        }
        else if (status == DOCKING){
          led_status.signs[i] = 1;
        }

      }
      break;
    case(NETWORK_ERROR):
    case (ERROR):
      led_status.pixel_var = 0;
      led_status.sign = 1;
      break;
    default:
      Serial.print("Received Unknown status");
      Serial.print(status);
      Serial.println("switching to error mode");
      change_LED_status(ERROR);
  }
}