// Simple Robot controlled with Wii Remote
// ForkRobotics
//
 
#include <Servo.h>
#include <Wire.h>
#include "nunchuck_funcs.h"
#define con1 650 // for Parallax Servos. Comment out for Sabertooth controller
#define con2 850
//#define con1 1000  // For Sabertooth controller. Uncomment for use with wheelchair motors
//#define con2 2000   
//Create Servo Object for Left Wheel
Servo RightWheel;
Servo LeftWheel;
 
int loop_cnt=0;
int speed,direction,LeftRotate,RightRotate;
 
void setup()
{
  RightWheel.attach(9, con1, con2);  // attaches the Right Wheel to pin 5
  LeftWheel.attach(10, con1, con2);  // attaches the Left Wheel to pin 6
 
  // initilization for the Wiichuck
  nunchuck_setpowerpins();
  nunchuck_init();
}
 
void loop()
{
  if( loop_cnt > 100 ) { // every 100 msecs get new data
    loop_cnt = 0;
 
    nunchuck_get_data();
    speed  = nunchuck_joyy(); // reads joystick y axis (range of 38-232)
    direction  = nunchuck_joyx(); // reads joystick x axis (range of 25-223)
 
    // Converts the joystick input to the servo output range
    speed = map(speed, 38, 232,0,180);
    direction = map(direction, 25, 223,-90,90);
 
    // The joystick floats a little, this reduces jitters when it's near center
    if (speed >= 87 && speed <= 93) speed = 90;
    if (direction >= 87 && direction <= 93) direction = 0;
 
    // Translates the joystick reading to the rotation speed for each wheel
    RightRotate=speed-direction;
    LeftRotate=(180-speed)-direction;
 
    // Writes the rotation speed to the servos
    RightWheel.write(RightRotate);
    LeftWheel.write(LeftRotate);
  }
  loop_cnt++;
  delay(1);
}
