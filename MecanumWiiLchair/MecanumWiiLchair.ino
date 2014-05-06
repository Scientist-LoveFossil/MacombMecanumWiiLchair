

// Simple Robot controlled with Wii Remote
// ForkRobotics
//
#include <NewPing.h> 
#include <Servo.h>
#include <Wire.h>
#include "nunchuck_funcs.h"

//#define con1 650 // for Parallax Servos. Comment out for Sabertooth controller
//#define con2 850

#define con1 1000  // For Sabertooth controller. Uncomment for use with wheelchair motors
#define con2 2000   

#define BrakeReleaseLF 30  // These pins will go to MOSfets to disable the brakes on the motors
#define BrakeReleaseRF 31  // Change as necessary.
#define BrakeReleaseLB 32
#define BrakeReleaseRB 33

//Ultrasonic sensor definitions and variables- this will be in place to avoid any collisions (work in progress)
#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on ping sensor.
#define ECHO_PIN     11 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer; 

//Create Servo Object for mecanum Wiils
Servo LeftWheelF; // front mecanum pair
Servo RightWheelF;

Servo LeftWheelB; // Back mecanum pair
Servo RightWheelB;

 
int loop_cnt=0;
byte accx,accy,accz,zbut,cbut,joyx,joyy,joyxMid;     // hold Chuck readings
int speed,direction,LeftRotateF,RightRotateF;
int LeftRotateB,RightRotateB;
int distance;           // for ultrasonic sensor
int drag = 10;          // lower = faster speed response
int stallSpeed = 20;    // speed needed to start train movement
int brakeDrag = 15;     // lower = slows faster, must be >0
int rollDrag = 100;     // higher is lower
int cruise = 0; 

void setup()
{
   
  Serial.begin(19200);
  //pingTimer = millis();
  LeftWheelF.attach(10, con1, con2); 
  RightWheelF.attach(9, con1, con2); //goes to back left on pin 6
  LeftWheelB.attach(6, con1, con2); 
  RightWheelB.attach(5, con1, con2);  
  // initilization for the Wiichuck
  nunchuck_setpowerpins();
  nunchuck_init();
  
}
 
void loop()
{
  
// if( loop_cnt > 100 ) { // every 100 msecs get new data
    loop_cnt = 0;
    nunchuck_get_data();
    zbut = nunchuck_zbutton();
    cbut = nunchuck_cbutton(); 
   // if (accx != 0 ){ // Chuk plugged in
    
    digitalWrite(13, LOW);         
    speed  = nunchuck_joyy(); // reads joystick y axis (range of 30-211)
    direction  = nunchuck_joyx(); // reads joystick x axis (range of 34-228)
 
    // Converts the joystick input to the servo output range
    speed = map(speed, 30, 212,-90,90);
    //speed = map(speed, 30, 212,0,180);
    
    //direction = map(direction, 34, 230, 0, 180);
    direction = map(direction, 34, 230, -90, 90);
 
    // The joystick floats a little, this reduces jitters when it's near center
    if (speed >= -5 && speed <= 5) speed = 0;
    if (direction >= -5 && direction <= 5) direction = 0; 
    
    LeftRotateF=90+speed+direction;
    RightRotateF=90-speed+direction;
    LeftRotateB=90+speed-direction;
    RightRotateB=90-speed-direction;
    // Writes the rotation speed to the servos
    
   if (cbut && zbut) {
    LeftWheelB.write(0);
    RightWheelB.write(0);  // Left of the marble
    debugNote();
   }
   
   
   if (zbut  && !cbut) {
    BrakeRelease();
    LeftWheelF.write(LeftRotateF);
    RightWheelF.write(RightRotateF);
    LeftWheelB.write(LeftRotateB);
    RightWheelB.write(RightRotateB);
    debugNote();
    }
   if (cbut && !zbut) { // press Z button to set rotate wheel mapping
      BrakeRelease();
      rotate();
      digitalWrite(13, HIGH);
      LeftWheelF.write(LeftRotateF);
      RightWheelF.write(RightRotateF);
      LeftWheelB.write(LeftRotateB);
      RightWheelB.write(RightRotateB);
      debugNote();
    }
   if (!cbut && !zbut) {
    BrakeEnable();
    LeftWheelF.write(90);
    RightWheelF.write(90);
    LeftWheelB.write(90);
    RightWheelB.write(90);
    debugNote();
   }
  //}
  //loop_cnt++;
  //delay(1);
// }
}

//  void readChuk(){ // check Chuk readings
//  nunchuck_get_data(); // from nunchuck_funcs.h
//  delay(1); // not sure why needed, but keeps first data set clean
//  accx  = nunchuck_accelx(); // 70 - 125 - 182
//  accy  = nunchuck_accely(); // ranges from approx 65 - 112 - 173
//  accz  = nunchuck_accelz(); // ranges from approx ??  ??
//  joyx = nunchuck_joyx();  //34 - 137 - 234
//  joyy = nunchuck_joyy();  //34 - 129 - 217
//  zbut = nunchuck_zbutton();
//  cbut = nunchuck_cbutton(); 
//  }

void rotate(){
    RightRotateF=90+direction-speed;
    LeftRotateF=90+direction;
    
    RightRotateB=90+direction-speed;
    LeftRotateB=90+direction;
}

void debugNote() {
 Serial.print("RF: ");
 Serial.print(RightRotateF);
 Serial.print("\t");
 Serial.print("LF:");
 Serial.print(LeftRotateF);
 Serial.print("\t");
 Serial.print("RB");
 Serial.print(RightRotateB);
 Serial.print("\t");
 Serial.print("LB");
 Serial.print(LeftRotateB);
 Serial.print("\t");
 if (cbut) {
   Serial.print("+C ");
   Serial.print("\t");
 }
 if (zbut) {
   Serial.print("+Z");
   Serial.print("\t");
 }
 Serial.println();
}

void BrakeRelease(){
  
 digitalWrite,(BrakeReleaseLF, HIGH);
 //digitalWrite,(BrakeReleaseRF, HIGH);
 //digitalWrite,(BrakeReleaseLB, HIGH);
 //digitalWrite,(BrakeReleaseRB, HIGH);
 //delay(500); // this delay may cause problems with getting nunchuk data in loop above.
}

void BrakeEnable(){
  
 digitalWrite,(BrakeReleaseLF, LOW);
 //digitalWrite,(BrakeReleaseRF, LOW);
 //digitalWrite,(BrakeReleaseLB, LOW);
 //digitalWrite,(BrakeReleaseRB, LOW);
// delay(500);// this delay may cause problems with getting nunchuk data in loop above.
}
