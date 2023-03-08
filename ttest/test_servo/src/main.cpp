#include <Arduino.h>

int motor1pin1 = 12;
int motor1pin2 = 13;

#define SPDL 1024
#define ENABLE 14


void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);

  pinMode(ENABLE, OUTPUT);
}

void loop(){
  // put your main code here, to run repeatedly:   
  Serial.print("RUN");

  //Controlling speed (0 = off and 255 = max speed):
  analogWrite(ENABLE, SPDL); //ENB pin
  

  //Controlling spin direction of motors:
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  delay(1000);
    //Controlling spin direction of motors:
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  delay(1000);
}