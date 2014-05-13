//
// LocalLibrary.cpp 
// Library C++ code
// ----------------------------------
// Developed with embedXcode 
// http://embedXcode.weebly.com
//
// Project mecanumRobot
//
// Created by Angelo.qiao , 13-3-29 9:42


#include "Arduino.h"
#include "mecanumRobot.h"
#include "wiring_private.h"
#include "TimerOne.h"

volatile int encoderSpeed[4];
volatile int encoderSpeedLatch[4];

int hallAPin[4];
int hallBPin[4];
int interruptPin[4];

void speedLeftForward()
{
  int hallA,hallB;
  hallA=digitalRead(hallAPin[LeftForward]);
  hallB=digitalRead(hallBPin[LeftForward]);
  
  if (hallA==LOW) {
    if (hallB == LOW) {
      (encoderSpeed[LeftForward])++;
    }
    else{
      (encoderSpeed[LeftForward])--;
    }
  }
  else{
    if (hallB == HIGH) {
      (encoderSpeed[LeftForward])++;
    }
    else{
      (encoderSpeed[LeftForward])--;
    }
  }
}


void speedRightForward()
{
  int hallA,hallB;
  hallA=digitalRead(hallAPin[RightForward]);
  hallB=digitalRead(hallBPin[RightForward]);
  
  if (hallA==HIGH) {
    if (hallB == LOW) {
      (encoderSpeed[RightForward])++;
    }
    else{
      (encoderSpeed[RightForward])--;
    }
  }
  else{
    if (hallB == HIGH) {
      (encoderSpeed[RightForward])++;
    }
    else{
      (encoderSpeed[RightForward])--;
    }
  }
}


void speedLeftBackward()
{
  int hallA,hallB;
  hallA=digitalRead(hallAPin[LeftBackward]);
  hallB=digitalRead(hallBPin[LeftBackward]);
  
  if (hallA==LOW) {
    if (hallB == LOW) {
      (encoderSpeed[LeftBackward])++;
    }
    else{
      (encoderSpeed[LeftBackward])--;
    }
  }
  else{
    if (hallB == HIGH) {
      (encoderSpeed[LeftBackward])++;
    }
    else{
      (encoderSpeed[LeftBackward])--;
    }
  }
}

void speedRightBackward()
{
  int hallA,hallB;
  hallA=digitalRead(hallAPin[RightBackward]);
  hallB=digitalRead(hallBPin[RightBackward]);
  
  if (hallA==HIGH) {
    if (hallB == LOW) {
      (encoderSpeed[RightBackward])++;
    }
    else{
      (encoderSpeed[RightBackward])--;
    }
  }
  else{
    if (hallB == HIGH) {
      (encoderSpeed[RightBackward])++;
    }
    else{
      (encoderSpeed[RightBackward])--;
    }
  }
}

void getEncoderSpeedISR()
{
  for (int i=0; i<4; i++) {
    encoderSpeedLatch[i]=encoderSpeed[i];
    encoderSpeed[i]=0;
  }
}

Mecanum::Mecanum(int dirLeftForward,
                 int dirRightForward,
                 int dirLeftBackward,
                 int dirRightBackward,
                 
                 int pwmLeftForward,
                 int pwmRightForward,
                 int pwmLeftBackward,
                 int pwmRightBackward,
                 
                 int hallALeftForward,
                 int hallARightForward,
                 int hallALeftBackward,
                 int hallARightBackward,
                 
                 int hallBLeftForward,
                 int hallBRightForward,
                 int hallBLeftBackward,
                 int hallBRightBackward,
                 
                 void (*interruptLeftForward)(),
                 void (*interruptRightForward)(),
                 void (*interruptLeftBackward)(),
                 void (*interruptRightBackward)(),
                 
                 int sampleTimeDuration,
                 
                 int relayForward,
                 int relayBackward
                 )
{
  sampleTime=sampleTimeDuration;
  
  pidArray[LeftForward]=new PID(inputPID+LeftForward,
                     outputPID+LeftForward,
                     setpointPID+LeftForward,
                     KP, KI, KD,
                     DIRECT);
  pidArray[RightForward]=new PID(inputPID+RightForward,
                      outputPID+RightForward,
                      setpointPID+RightForward,
                      KP, KI, KD,
                      DIRECT);
  pidArray[LeftBackward]=new PID(inputPID+LeftBackward,
                      outputPID+LeftBackward,
                      setpointPID+LeftBackward,
                      KP, KI, KD,
                      DIRECT);
  pidArray[RightBackward]=new PID(inputPID+RightBackward,
                       outputPID+RightBackward,
                       setpointPID+RightBackward,
                       KP, KI, KD,
                       DIRECT);
  
  dirPin[LeftForward] = dirLeftForward;
  dirPin[RightForward] = dirRightForward;
  dirPin[LeftBackward] = dirLeftBackward;
  dirPin[RightBackward] = dirRightBackward;

  pwmPin[LeftForward] = pwmLeftForward;
  pwmPin[RightForward] = pwmRightForward;
  pwmPin[LeftBackward] = pwmLeftBackward;
  pwmPin[RightBackward] = pwmRightBackward;
  
  hallAPin[LeftForward] = hallALeftForward;
  hallAPin[RightForward] = hallARightForward;
  hallAPin[LeftBackward] = hallALeftBackward;
  hallAPin[RightBackward] = hallARightBackward;
  
  hallBPin[LeftForward] = hallBLeftForward;
  hallBPin[RightForward] = hallBRightForward;
  hallBPin[LeftBackward] = hallBLeftBackward;
  hallBPin[RightBackward] = hallBRightBackward;
  
  relayPin[Forward] = relayForward;
  relayPin[Backward] = relayBackward;
  
    
  interruptAgent[LeftForward] = interruptLeftForward;
  interruptAgent[RightForward] = interruptRightForward;
  interruptAgent[LeftBackward] = interruptLeftBackward;
  interruptAgent[RightBackward] = interruptRightBackward;
  
  memset(motorPwm, 0, sizeof(motorPwm));
  memset(inputPID, 0, sizeof(inputPID));
  memset(outputPID, 0, sizeof(outputPID));
  memset(setpointPID, 0, sizeof(setpointPID));
  
}

/*************************** Details *****************************/

//#define	OC0A				DIO13
//#define	OC0B				DIO4
//#define	OC1A				DIO11
//#define	OC1B				DIO12
//#define	OC2A				DIO10
//#define	OC2B				DIO9
//#define	OC3A				DIO5
//#define	OC3B				DIO2
//#define	OC3C				DIO3
//#define	OC4A				DIO6
//#define	OC4B				DIO7
//#define	OC4C				DIO8
//#define	OC5A				DIO46
//#define	OC5B				DIO45
//#define	OC5C				DIO44


void Mecanum::init()
{
  Timer1.initialize(20000);
  Timer1.attachInterrupt(getEncoderSpeedISR);
  
  for (int i=0; i<4; i++) {
    switch (hallAPin[i]) {
      case 2:
        interruptPin[i]=0;
        break;
      case 3:
        interruptPin[i]=1;
        break;
      case 21:
        interruptPin[i]=2;
        break;
      case 20:
        interruptPin[i]=3;
        break;
      case 19:
        interruptPin[i]=4;
        break;
      case 18:
        interruptPin[i]=5;
        break;
      default:
        break;
    }
  }

  for (int i=0; i<4; i++) {
    pinMode(dirPin[i], OUTPUT);
    pinMode(pwmPin[i], OUTPUT);
    pinMode(hallAPin[i], INPUT_PULLUP);
    pinMode(hallBPin[i], INPUT_PULLUP);
    
  }
  
  digitalWrite(relayPin[Forward], LOW);
  digitalWrite(relayPin[Backward], LOW);

  pinMode(relayPin[Forward], OUTPUT);
  pinMode(relayPin[Backward], OUTPUT);
  
  for (int i=0; i<4; i++) {
    attachInterrupt(interruptPin[i], interruptAgent[i], CHANGE);
  }
  
  pidArray[LeftForward]->SetMode(AUTOMATIC);
  pidArray[RightForward]->SetMode(AUTOMATIC);
  pidArray[LeftBackward]->SetMode(AUTOMATIC);
  pidArray[RightBackward]->SetMode(AUTOMATIC);
  
  pidArray[LeftForward]->SetSampleTime(sampleTime);
  pidArray[RightForward]->SetSampleTime(sampleTime);
  pidArray[LeftBackward]->SetSampleTime(sampleTime);
  pidArray[RightBackward]->SetSampleTime(sampleTime);
  
  pidArray[LeftForward]->SetOutputLimits(-700, 700);
  pidArray[RightForward]->SetOutputLimits(-700, 700);
  pidArray[LeftBackward]->SetOutputLimits(-700, 700);
  pidArray[RightBackward]->SetOutputLimits(-700, 700);

#ifdef __AVR_ATmega2560__
  
  TCCR4A=_BV(COM4A1) | _BV(COM4B1) |_BV(COM4C1) | _BV(WGM41);
  TCCR4B=_BV(WGM43) | _BV(WGM42) | _BV(CS40);
  
  TCCR3A=_BV(COM3A1) | _BV(WGM31);
  TCCR3B=_BV(WGM33) | _BV(WGM32) | _BV(CS30);
  uint8_t oldSREG = SREG;

  //16000000/2/1/20000
  cli();
  ICR4=800;
  ICR3=800;
  SREG=oldSREG;
#endif
}


double Mecanum::mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Mecanum::setSpeed(int speed, int degree, int direction)
{
  //
  
//  int motorMaxPwm=MotorPwmThreshold;
  double angle;

  magnitude=constrain(speed,0,MaxSpeed);
  angle=radians(degree);
  rotation=constrain(direction,-MaxSpeed,MaxSpeed);
  
  //  Serial.println(magnitude);
  //  Serial.println(angle);
  //  Serial.println(rotation);
  
  setpointPID[LeftForward] = magnitude*sin(PI/4.0 + angle) + rotation;
  setpointPID[RightForward] = magnitude*cos(PI/4.0 + angle) - rotation;
  setpointPID[LeftBackward] = magnitude*cos(PI/4.0 + angle) + rotation;
  setpointPID[RightBackward] = magnitude*sin(PI/4.0 + angle) - rotation;
  
//  Serial2.print(setpointPID[LeftForward]);
//  Serial2.print(" ");
//  Serial2.print(setpointPID[RightForward]);
//  Serial2.print(" ");
//  Serial2.print(setpointPID[LeftBackward]);
//  Serial2.print(" ");
//  Serial2.print(setpointPID[RightBackward]);
//  Serial2.print(" ");
//  for (int i = 0; i < 4; i++) {
//    //    Serial.print(motor.Vx[i]);
//    //    Serial.print(" ");
//    motorMaxPwm=max(abs(setpointPID[i]), motorMaxPwm);
//  }
  
  
  for(int i = 0; i < 4; i++){
    if (setpointPID[i] != 0.0) {
      setpointPID[i] = constrain(setpointPID[i], -MaxSpeed, MaxSpeed);
      setpointPID[i] = mapDouble(setpointPID[i], -MaxSpeed, MaxSpeed, -EncoderMaxSpeed, EncoderMaxSpeed) ;
    }
//        Serial.print(setpointPID[i]);
//        Serial.print(" ");
  }
//  Serial.println();
}

                    
void Mecanum::setMotor(uint8_t which)
{
  int motorPwmBuf;
  if(motorPwm[which] >= 0){
    digitalWrite(dirPin[which],LOW);
  }
  else {
    digitalWrite(dirPin[which],HIGH);
  }
  motorPwmBuf=abs(motorPwm[which]);
//  Serial.print(which);
//  Serial.print(":");
//  Serial.println(inputPID[which]);

//  analogWrite(pwmPin[which],abs(motorPwm[which]));
  switch (which) {
    case LeftForward:
      OCR4A=motorPwmBuf;
      break;
    case RightForward:
      OCR4B=motorPwmBuf;
      break;
    case LeftBackward:
      OCR4C=motorPwmBuf;
      break;
    case RightBackward:
      OCR3A=motorPwmBuf;
      break;
      
    default:
      break;
  }
}

void Mecanum::powerOn()
{
  digitalWrite(relayPin[Forward], HIGH);
  digitalWrite(relayPin[Backward], HIGH);
  for (int i=0; i<4; i++) {
    pidArray[i]->SetMode(AUTOMATIC);
  }
}

void Mecanum::powerOff()
{
  digitalWrite(relayPin[Forward], LOW);
  digitalWrite(relayPin[Backward], LOW);
  for (int i=0; i<4; i++) {
    pidArray[i]->SetMode(MANUAL);
    inputPID[i]=0.0;
    outputPID[i]=0.0;
    setpointPID[i]=0.0;
  }
}

void Mecanum::updateSpeed()
{  
  for (int i=0; i<4; i++) {
    if (pidArray[i]->Compute()) {
      
      if (/*fabs(inputPID[i])<=5.0 && */magnitude== 0 && rotation == 0) {
        pidArray[i]->SetMode(MANUAL);
        inputPID[i]=0.0;
        outputPID[i]=0.0;
        setpointPID[i]=0.0;
        pidArray[i]->SetMode(AUTOMATIC);
      }
      
      motorPwm[i]=(int)outputPID[i];
      
      setMotor(i);
      int encoderSpeedLatchBuf;
      encoderSpeedLatchBuf=encoderSpeedLatch[i];
      inputPID[i]=(double)encoderSpeedLatchBuf;
      
    }
  }
}