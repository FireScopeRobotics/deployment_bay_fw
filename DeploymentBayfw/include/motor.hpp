#ifndef MOTOR
#define MOTOR
#include <Arduino.h>

//motors related
#define MOTORPIN1 12
#define MOTORPIN2 13
#define DOOR_UP_TIME_MS 8000
#define DOOR_DOWN_TIME_MS 8000



void init_motor();
void run_motor();
bool request_motor(bool is_request_open);
void set_id_motor(String id);



#endif