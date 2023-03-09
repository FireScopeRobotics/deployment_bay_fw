#include <motor.hpp>
#include <Arduino.h>

bool DOOR_OPEN;
bool DOOR_OPERATING;
bool OPERATION; //(OPEN IS TRUE, CLOSE IS FALSE)
unsigned long estimated_completion_time;

void init_motor(){
  // put your setup code here, to run once:
  pinMode(MOTORPIN1, OUTPUT);
  pinMode(MOTORPIN2, OUTPUT);

  DOOR_OPEN = false;
  DOOR_OPERATING = false;
  OPERATION = false;
  estimated_completion_time = 0;
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
    Serial.println("operating open case");
  }
  else if(OPERATION == false){//close case
    digitalWrite(MOTORPIN1, HIGH);
    digitalWrite(MOTORPIN2, LOW);
    Serial.println("operating close case");
  }
  //check if the door can stop
  if (DOOR_OPERATING && millis() > estimated_completion_time){
    digitalWrite(MOTORPIN1, LOW);
    digitalWrite(MOTORPIN2, LOW);
    DOOR_OPERATING = false; //stops the door
    DOOR_OPEN = OPERATION; //true for open false for close
    estimated_completion_time = 0; //reset to prevent false triggers
    Serial.println("reset");

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