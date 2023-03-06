// #include <Servo.h>
// // int button1 = 4; //button pin, connect to ground to move servo
// int press1 = 0;
// Servo servo1;

// void setup()
// {
//   servo1.attach(4);
// }

// void loop()
// {
//   servo1.write(90);
// }

#include <Arduino.h>

int motor1pin1 = 12;
int motor1pin2 = 13;
#define LED 2 //Define blinking LED pin

int enA = 15;

int spdL = 256;
int spdR = 256;

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(LED, OUTPUT);

  pinMode(enA, OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:   

  //Controlling speed (0 = off and 255 = max speed):
  analogWrite(enA, spdR); //ENA pin

  digitalWrite(LED, HIGH); // Turn the LED on (Note that LOW is the voltage level)

  //Controlling spin direction of motors:
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  delay(1000);

  digitalWrite(LED, LOW); // Turn the LED on (Note that LOW is the voltage level)
  //Controlling spin direction of motors:
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  delay(1000);

}