/* Simple Robot controlled with Wii Remote
Derived and mutilated from
// ForkRobotics
// by K. Cole and A. Distel
This is the code for our mecanum Wiilchair of the Winter 2014 semester PRDE 2430 Capstone.
*/
#include <NewPing.h> 
#include <Servo.h>
#include <Wire.h>
#include "nunchuck_funcs.h"
//#define con1 650 // for Parallax Servos. Comment out for Sabertooth controller
//#define con2 850  

#define con1 1000  // For Sabertooth controller. Uncomment for use with wheelchair motors
#define con2 2000   

const int8_t incr =  1;
const int8_t decr = -1;
const int accelGap = 10;

#define BrakeRelease_PIN  12  // These pins will go to MOSfets to disable the brakes on the motors
//#define BrakeReleaseRF 31  // Change as necessary.
//#define BrakeReleaseLB 32
//#define BrakeReleaseRB 33

//Ultrasonic sensor definitions and variables- this will be in place to avoid any collisions (work in progress)
//#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on ping sensor.
//#define ECHO_PIN     11 // Arduino pin tied to echo pin on ping sensor.
#define MAX_DISTANCE 200
//NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned int pingSpeed = 50; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer; 

//Create Servo Object for mecanum Wiils
Servo FL_Wheel; // front mecanum pair
Servo FR_Wheel;

Servo BL_Wheel;// Back mecanum pair
Servo BR_Wheel;

 
byte zbut,cbut,joyx,joyy;     // hold Chuck readings
int speed, direction;
int currentFL = 90;
int currentFR = 90;
int currentBL = 90;
int currentBR = 90;
int targetFL, targetFR, targetBL, targetBR;
int FLM, FRM, BLM, BRM;
//int distance;           // for ultrasonic sensor
int drag = 10;          // lower = faster speed response
int stallSpeed = 20;    // speed needed to start train movement
int brakeDrag = 15;     // lower = slows faster, must be >0
int rollDrag = 100;     // higher is lower
int time = millis();

int8_t count = 0;

void setup()
{
   
  Serial.begin(115200);
  digitalWrite(13, LOW);
  pinMode( BrakeRelease_PIN, OUTPUT);
  //pingTimer = millis();
  FL_Wheel.attach(10, con1, con2); 
  FR_Wheel.attach(9, con1, con2); //goes to back left on pin 6
  BL_Wheel.attach(6, con1, con2); 
  BR_Wheel.attach(5, con1, con2); 
   FL_Wheel.write(90);
   FR_Wheel.write(90); // Here we are trying to kill all movement to the wheels once the arduino is powered on.
   BL_Wheel.write(90); //This should hopefully help against runaway brownouts.
   BR_Wheel.write(90);
   BrakeEnable();   
  // initilization for the Wiichuck
  nunchuck_setpowerpins();
  nunchuck_init();
  
}
 

int8_t constrainX; 
int8_t constrainY;

void loop()
{
    //constrainY = 0;  
    //constrainX = 0;
    nunchuck_get_data();         // Getting nunchuck data from the controller
    zbut = nunchuck_zbutton();   // Here we are assigning variables for the c and z button_states
    cbut = nunchuck_cbutton(); 
    // Default target of "do not move"
    targetFL=90;
    targetFR=90;
    targetBL=90;
    targetBR=90;

    // We intend to move somewhere
    if (cbut | zbut) {           // If C or Z are pressed, send a high signal to the brake pin to release the solenoid brakes 
      digitalWrite(BrakeRelease_PIN, HIGH); // on the wheelchair motors through the sparkfun mosfet breakout (inside battery box)
      speed  = nunchuck_joyy(); // reads joystick y axis (range of 30-211)
      direction  = nunchuck_joyx(); // reads joystick x axis (range of 34-228)
      // Converts the joystick input to the servo output range
      speed = map(speed, 20, 211, -90, 90);
      direction = map(direction, 34, 228, -90, 90);
      
      if (speed > 90 || speed < -90) { constrainY = 1; speed = constrain(speed, -90, 90);}      // constraining mapped values to remain in mapped zone (debugging)
      if (direction > 90 || direction < -90) { constrainX = 1; direction = constrain(direction, -90, 90);}
 
      // The joystick floats a little, this reduces jitters when it's near center
      if (speed >= -8 && speed <= 8) speed = 0;
      if (direction >= -8 && direction <= 8) direction = 0; 
 
      //printBasic();
 
      // Sets target speed   
      targetFL=90+speed+direction;
      targetFR=90-speed+direction;
      targetBL=90+speed-direction;
      targetBR=90-speed-direction;
}
  
   
   if (cbut && zbut) {
    BL_Wheel.write(0);
    BR_Wheel.write(0);  // if both buttons are pressed, we check if the controller works by moving back wheels only
    debugNote();
   }
   
   
   if (zbut  && !cbut) {   // if Z button is pressed, go to ramping routine and write Wheel Mappings for strafing wheels
    //if (currMS < 50) {currMS++;}  // Max at 50
    ramping();
 
    FL_Wheel.write(currentFL);
    FR_Wheel.write(currentFR);
    BL_Wheel.write(currentBL);
    BR_Wheel.write(currentBR);
    if (count > 20) {debugNote(); count = 0;}
    }
   if (cbut && !zbut) { // press Z button to set rotate wheel mapping
     rotate();
     ramping();
   
    //if (currMS < 50) {currMS++;}  // Max at 50 
     FL_Wheel.write(currentFL);
     FR_Wheel.write(currentFR);
     BL_Wheel.write(currentBL);
     BR_Wheel.write(currentBR);
    if (count > 20) {debugNote(); count = 0;}
    }
   if (!cbut && !zbut) {
   ramping();
   //if (currMS > 0) {currMS--;}       // Do not go negative
 
    FL_Wheel.write(currentFL);
    FR_Wheel.write(currentFR);
    BL_Wheel.write(currentBL);
    BR_Wheel.write(currentBR);
    if (count > 20) {debugNote(); count = 0;}
   }
   checkBrake();
   count++;
}

void rotate(){   // This mapps the direction and speed values backwards on the back wheels to enable tank-drive 
    targetFR=90+direction-speed;
    targetFL=90+direction;
    
    targetBR=90+direction-speed;
    targetBL=90+direction;
}

void debugNote() {
 Serial.print("RF: ");  Serial.print(currentFR); Serial.print("/"); Serial.print(targetFR); tab();
 Serial.print("LF: ");  Serial.print(currentFR); Serial.print("/"); Serial.print(targetFL); tab();
 Serial.print("RB: ");  Serial.print(currentBR); Serial.print("/"); Serial.print(targetBR); tab();
 Serial.print("LB: ");  Serial.print(currentBL); Serial.print("/"); Serial.print(targetBL); tab();
 if (cbut) {
   Serial.print("+C ");   tab();
 }
 if (zbut) {
   Serial.print("+Z");    tab();
 }
 if (constrainX) {
   Serial.print("ConX");  tab();
 }
 if (constrainY) {
   Serial.print("ConY");  tab();
 }
 // Drop to the next line
 Serial.println();
}

void tab() {Serial.print("\t");}

void printBasic() {Serial.print("Speed: "); Serial.print(speed); Serial.print("\t Direction:"); Serial.print(direction); Serial.println();} 

void ramping() {
  time = millis();
  if (time % accelGap == 0) {
    //  Front Right Wheel
    if (targetFR > currentFR) { FRM = incr;} 
    else if (targetFR < currentFR) { FRM = decr;}
    else {FRM = 0;}
    
    //  Front Left Wheel
    if (targetFL > currentFL) { FLM = incr;} 
    else if (targetFL < currentFL) { FLM = decr;}
    else {FLM = 0;}
  
    // Back Right Wheel
    if (targetBR > currentBR) { BRM = incr;} 
    else if (targetBR < currentBR) { BRM = decr;}
    else {BRM = 0;} 
  
    // Back Left Wheel
    if (targetBL > currentBL) { BLM = incr;} 
    else if (targetBL < currentBL) { BLM = decr;}
    else {BLM = 0;}

    currentFR += FRM;
    currentFL += FLM;
    currentBR += BRM;
    currentBL += BLM;
  }
}  
  
  
void checkBrake() {
  // Joystick released, begin slowing the system down
  if ( (currentFR==90) && (currentFL==90) && (currentBR==90) && (currentBL==90) && !zbut && !cbut) { 
   digitalWrite( BrakeRelease_PIN,LOW);  // Enable Brakes
  }
}
