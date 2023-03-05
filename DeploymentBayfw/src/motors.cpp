#include <main.hpp>

int motor1pin1 = 9;
int motor1pin2 = 10;

int enB = 13;

#define SPDL 128

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);

  pinMode(enB, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:   

  //Controlling speed (0 = off and 255 = max speed):
  analogWrite(enB, SPDL); //ENB pin

  //Controlling spin direction of motors:
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  delay(1000);
}