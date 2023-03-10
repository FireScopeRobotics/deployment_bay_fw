#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#ifndef WIFI
#define WIFI
static String serverName = "http://192.168.23.7:8000/dock";
//fw variable
#endif