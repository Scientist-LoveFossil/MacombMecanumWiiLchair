
#include "Arduino.h"
#include "mecanumRobot.h"
#include "PlainProtocol.h"
#include "PID_v1.h"
#include "TimerOne.h"

PlainProtocol Wireless(Serial2,57600);
Mecanum myMecanum;

void setup(){
  Wireless.init();
  Serial.begin(57600);
  while (!Serial);
  myMecanum.init();
}


void loop(){
  static unsigned long wirelessReceivedTimerShort=millis();
  static unsigned long wirelessReceivedTimerLong=millis();
  
  static unsigned long encoderTestTimer=millis();
  
  if (millis() >= encoderTestTimer+250) {
    encoderTestTimer=millis();
    Serial.print("encoderTest:");
    for (int i=0; i<4; i++) {
      Serial.print(myMecanum.inputPID[i]);
      Serial.print(" ");
    };
    Serial.println();
    
    
  }
  
  if (millis() >= wirelessReceivedTimerShort+200) {
    myMecanum.setSpeed(0, 0, 0);
    wirelessReceivedTimerShort=millis();
  }
  
  if (millis() >= wirelessReceivedTimerLong+500) {
    myMecanum.powerOff();
    wirelessReceivedTimerLong=millis();
  }
  
  myMecanum.updateSpeed();
  
  if (Wireless.receiveFrame()) {
    if (Wireless.receivedCommand=="mecanum") {
      myMecanum.setSpeed(Wireless.receivedContent[0], Wireless.receivedContent[1], Wireless.receivedContent[2]);
      if (Wireless.receivedContent[3]) {
        myMecanum.powerOn();
      }
      else{
        myMecanum.powerOff();
      }
      
      wirelessReceivedTimerShort=millis();
      wirelessReceivedTimerLong=millis();
    }
    else if (Wireless.receivedCommand=="PID"){
      double PID_P=Wireless.receivedContent[0]/100.0;
      double PID_I=Wireless.receivedContent[1]/100.0;
      double PID_D=Wireless.receivedContent[2]/100.0;
      
      myMecanum.pidArray[LeftForward]->SetTunings(PID_P,PID_I,PID_D);
      myMecanum.pidArray[RightForward]->SetTunings(PID_P,PID_I,PID_D);
      myMecanum.pidArray[LeftBackward]->SetTunings(PID_P,PID_I,PID_D);
      myMecanum.pidArray[RightBackward]->SetTunings(PID_P,PID_I,PID_D);
    }
    else{
      Serial.println("command not available");
    }
  }
}

