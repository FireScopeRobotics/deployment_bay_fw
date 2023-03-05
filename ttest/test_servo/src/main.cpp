#include <Servo.h>
// int button1 = 4; //button pin, connect to ground to move servo
int press1 = 0;
Servo servo1;

void setup()
{
  servo1.attach(4);
}

void loop()
{
  servo1.write(90);
}