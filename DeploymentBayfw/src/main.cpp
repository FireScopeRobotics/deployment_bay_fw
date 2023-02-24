#include <Arduino.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <TaskScheduler.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//path to the server, change depending on the ip address of the server
ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
String serverName = "http://192.168.39.113:42069";

//pinout definitions
#define LED 2

typedef enum 
{ 
    IDLE, 
    UNDOCKING, 
    DOCKING 
} dock_status;

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
Task heartbeat_t(500, TASK_FOREVER, &heartbeat);
void ledstrip();
Task ledstrip_t(25, TASK_FOREVER, &ledstrip);

//fw variables
String serial_code = "";

//multitasking variables:
Scheduler runner;

//helper functions
String init_req(String cmd);
void add_param_req(String& cmd, String param, String val);
void change_LED_status(dock_status status);
uint32_t Wheel(byte WheelPos);

//colorstrip
#define CTRL_PIN1 4 //D2
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(60, CTRL_PIN1, NEO_GRB + NEO_KHZ800); //32 lights
#define num_pixels_strip1 32
// #define CTRL_PIN2 4
// Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(60, CTRL_PIN2, NEO_GRB + NEO_KHZ800);
struct led_status_s{
  dock_status status;
  int pixel_var;
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

  //setup led strips
  strip1.begin();
  strip1.setBrightness(64);
  strip1.show(); // Initialize all pixels to 'off'
  change_LED_status(IDLE);

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

void ledstrip(){
  // int i;
  // for (i = 0; i<32; i++)
  // {
  //    strip1.setPixelColor(i, strip1.Color(0, 150+i, 0));
  // }
  switch (led_status.status){
    case (IDLE):
      for( int i=0; i < num_pixels_strip1; i++) {
        strip1.setPixelColor(i, strip1.Color(0, 0, led_status.pixel_var));
      }
      led_status.pixel_var += 1;
      if (led_status.pixel_var > 255){
        led_status.pixel_var = led_status.pixel_var%255;
      }
      break;
    case (UNDOCKING):
      for(int i=0; i< num_pixels_strip1; i++) {
        strip1.setPixelColor(i, strip1.Color(0, 150+i, 0));
      }
      break;

  }
  strip1.show();
  // uint16_t i, j;
  // for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
  //   for(i=0; i< strip1.numPixels(); i++) {
  //     strip1.setPixelColor(i, Wheel(((i * 256 / strip1.numPixels()) + j) & 255));
  //   }
  //   strip1.show();
  // }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
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
void change_LED_status(dock_status status){
  led_status.status = IDLE;
  switch (status){
    case (IDLE):
      led_status.pixel_var = 0;
      break;
    case (UNDOCKING):
      led_status.pixel_var = 0;
  }
}