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

//Create Servo Object for mecanum Wiils
Servo LeftWheelF; // front mecanum pair
Servo RightWheelF;

Servo LeftWheelB; // Back mecanum pair
Servo RightWheelB;

 
int loop_cnt=0;
byte accx,accy,zbut,cbut;
int speed,direction,LeftRotateF,RightRotateF;
int LeftRotateB,RightRotateB;
 
void setup()
{
   
  Serial.begin(19200);
  LeftWheelF.attach(5, con1, con2); 
  RightWheelF.attach(6, con1, con2);
  LeftWheelB.attach(9, con1, con2); 
  RightWheelB.attach(10, con1, con2);  
  // initilization for the Wiichuck
  nunchuck_setpowerpins();
  nunchuck_init();
}
 
void loop()
{
  if( loop_cnt > 100 ) { // every 100 msecs get new data
    loop_cnt = 0;
 
    nunchuck_get_data();
    cbut = nunchuck_cbutton(); 
            
    speed  = nunchuck_joyy(); // reads joystick y axis (range of 38-232)
    direction  = nunchuck_joyx(); // reads joystick x axis (range of 25-223)
 
    // Converts the joystick input to the servo output range
    speed = map(speed, 21, 212,0,180);
    direction = map(direction, 34, 230,0,180);
 
    // The joystick floats a little, this reduces jitters when it's near center
    if (speed >= 87 && speed <= 93) speed = 90;
    if (direction >= 87 && direction <= 93) direction = 90;
 
    // Translates the joystick reading to the rotation speed for each wheel
  if (cbut = 1){
    Serial.print (speed);
    Serial.print ("\t");
    Serial.print (direction);
    Serial.print ("\t");
    Serial.println (); 
    Serial.print ("Rotate");
    RightRotateF=speed-direction;
    LeftRotateF=(90-speed)-direction;
    
    RightRotateB=speed-direction;
    LeftRotateB=(90-speed)-direction;
    
   } else {
    Serial.print ("Strafe");
    RightRotateF=speed-direction;
    LeftRotateF=(90-speed)-direction;
    
    LeftRotateB=speed-direction;
    RightRotateB=(90-speed)-direction;
    // Writes the rotation speed to the servos
    
    LeftWheelF.write(LeftRotateF);
    RightWheelF.write(RightRotateF);
    LeftWheelB.write(LeftRotateB);
    RightWheelB.write(RightRotateB);
    
   }
 }
  loop_cnt++;
  delay(1);
}
