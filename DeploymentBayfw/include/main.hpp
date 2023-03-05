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

