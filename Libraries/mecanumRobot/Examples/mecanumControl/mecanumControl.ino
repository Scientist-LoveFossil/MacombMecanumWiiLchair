

// Include application, user and local libraries
//#include "LocalLibrary.h"
#include "PlainProtocol.h"
#include "Arduino.h"

#define x 0
#define y 1
#define z 2

#define CenterError (50)
#define MaxError (400)
#define RealError (CenterError+MaxError)

#define LedPin 13

bool clickEnable=true;
bool powerOn=false;
#define ClickPin  4

PlainProtocol Wireless;

int JoystickCenter[3];
int absTarget[3];

long Axis[3]={0,0,0};
int AxisTimes=0;

void getData(){
  for(int i = 0; i < 3; i++){
    Axis[i] += analogRead(i);
    //    Serial1.println(Axis[i]);
  }
  AxisTimes ++;
}

void setup(){
  Serial.begin(57600);
  Wireless.init();
  
  pinMode(ClickPin, INPUT_PULLUP);
  pinMode(LedPin, OUTPUT);
  
  for (int i=0; i<100; i++) {
    getData();
  }
  
  JoystickCenter[x] = Axis[x]/AxisTimes; // remeber the center of the joystick
  JoystickCenter[y] = Axis[y]/AxisTimes;
  JoystickCenter[z] = Axis[z]/AxisTimes;
  
  for (int i=0; i<3; i++) {
    Axis[i]=0;
  }
  AxisTimes=0;
}

void alog2Vextor(){
  absTarget[x] = constrain(Axis[x] - JoystickCenter[x], -450,450);
  absTarget[y] = constrain(Axis[y] - JoystickCenter[y], -450,450);
  absTarget[z] = constrain(JoystickCenter[z] - Axis[z], -450,450);
  
//  Serial.println(absTarget[x]);
//  Serial.println(absTarget[y]);
//  Serial.println(absTarget[z]);
  
  if (absTarget[x]<-CenterError) {
    absTarget[x] += CenterError;
  }
  else if (absTarget[x] >= -CenterError && absTarget[x] < 0){
    absTarget[x] = 0;
  }
  else if(absTarget[x] >= 0 && absTarget[x] < CenterError ){
    absTarget[x] = 0;
  }
  else{
    absTarget[x] -= CenterError;
  }
  
  if (absTarget[y]<-CenterError) {
    absTarget[y] += CenterError;
  }
  else if (absTarget[y] >= -CenterError && absTarget[y] < 0){
    absTarget[y] = 0;
  }
  else if(absTarget[y] >= 0 && absTarget[y] < CenterError ){
    absTarget[y] = 0;
  }
  else{
    absTarget[y] -= CenterError;
  }
  
  if (absTarget[z]<-CenterError) {
    absTarget[z] += CenterError;
  }
  else if (absTarget[z] >= -CenterError && absTarget[z] < 0){
    absTarget[z] = 0;
  }
  else if(absTarget[z] >= 0 && absTarget[z] < CenterError ){
    absTarget[z] = 0;
  }
  else{
    absTarget[z] -= CenterError;
  }

  int maxSpeed = constrain(sqrt((long)absTarget[x]*(long)absTarget[x] + (long)absTarget[y]*(long)absTarget[y]),0,400);
  
  int Degree = degrees(atan2(absTarget[y],absTarget[x]));
  
  Wireless.sendFrame("mecanum", 4, maxSpeed , Degree, absTarget[z], powerOn);
}

void printData(){
  Serial.print("Analog: ");
  for(int i = 0; i < 3; i++){
    Serial.print(absTarget[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void loop(){
  static unsigned long lastTimer=millis();
  
  getData();

  if (millis()-lastTimer>50) {
    digitalWrite(LedPin, powerOn);
    lastTimer=millis();
    for (int i=0; i<3; i++) {
      Axis[i] = Axis[i]/AxisTimes;
    }
    alog2Vextor();
    for (int i=0; i<3; i++) {
      Axis[i]=0;
    }
    AxisTimes=0;
    
  }
  
  static unsigned long relayTimer;

  if (!digitalRead(ClickPin) && clickEnable) {
    clickEnable=false;
    relayTimer=millis()+500;
    powerOn=!powerOn;
  }
  
  if (!clickEnable) {
    if (millis()>relayTimer) {
      clickEnable=true;
    }
  }
  
  if (Serial.available()) {
    Serial1.write(Serial.read());
  }
  
}
