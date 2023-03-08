#include <main.hpp>



void setup_motors() {
  // put your setup code here, to run once:
  pinMode(MOTORPIN1, OUTPUT);
  pinMode(MOTORPIN2, OUTPUT);

  pinMode(ENABLE, OUTPUT);
}

void run_motors() {
  // put your main code here, to run repeatedly:   

  //Controlling speed (0 = off and 255 = max speed):
  analogWrite(ENABLE, SPDL); //ENB pin

  //Controlling spin direction of motors:
  digitalWrite(MOTORPIN1, LOW);
  digitalWrite(MOTORPIN2, HIGH);

  delay(1000);
}