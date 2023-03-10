#include <motor.hpp>
#include <wifi.hpp>
// #include <Arduino.h>


bool DOOR_OPEN;
bool DOOR_OPERATING;
bool OPERATION; //(OPEN IS TRUE, CLOSE IS FALSE)
unsigned long estimated_completion_time;

String id_cpy = "";

void init_motor(){
  // put your setup code here, to run once:
  pinMode(MOTORPIN1, OUTPUT);
  pinMode(MOTORPIN2, OUTPUT);

  DOOR_OPEN = false;
  DOOR_OPERATING = false;
  OPERATION = false;
  estimated_completion_time = 0;
}

void set_id_motor(String id){
  id_cpy = id;
}

void run_motor(){
  if (!DOOR_OPERATING){
    //Controlling spin direction of motors:
    digitalWrite(MOTORPIN1, LOW);
    digitalWrite(MOTORPIN2, LOW);
    // Serial.println("IDLE");
  }
  else if(OPERATION == true){ //open case
    //Controlling spin direction of motors:
    digitalWrite(MOTORPIN1, LOW);
    digitalWrite(MOTORPIN2, HIGH);
  }
  else if(OPERATION == false){//close case
    digitalWrite(MOTORPIN1, HIGH);
    digitalWrite(MOTORPIN2, LOW);
  }
  //check if the door can stop
  if (DOOR_OPERATING && millis() > estimated_completion_time){
    digitalWrite(MOTORPIN1, LOW);
    digitalWrite(MOTORPIN2, LOW);
    DOOR_OPERATING = false; //stops the door
    DOOR_OPEN = OPERATION; //true for open false for close
    estimated_completion_time = 0; //reset to prevent false triggers
    
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      String request;
      if (DOOR_OPEN)
        request = serverName + "/" + id_cpy + "/" + "status" + "/" + "1";
      else
        request = serverName + "/" + id_cpy + "/" + "status" + "/" + "0";

      http.begin(client, request);
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      if (httpResponseCode>0) {
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
    }

  }
}

bool request_motor(bool is_request_open){
  Serial.println("morot request");
  if (DOOR_OPERATING){
    //door currently operating
    return false;
  }
  if (is_request_open && DOOR_OPEN){
    //door already open
    return false;
  }
  else if (!is_request_open && !DOOR_OPEN){
    //door already closed
    return false;
  }
  //door can be operated
  DOOR_OPERATING = true; //initiates the door operation
  OPERATION = is_request_open; //determine direction (true is up, false is down)
  //determine ammount of time required
  if (is_request_open) estimated_completion_time = millis() + DOOR_UP_TIME_MS;
  else estimated_completion_time = millis() + DOOR_DOWN_TIME_MS;



  return true;
}