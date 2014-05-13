///
/// @file	LocalLibrary.h 
/// @brief	Library header
///
/// @details	<#details#>
/// @n	
/// @n @b	Project mecanumRobot
/// @n @a	Developed with [embedXcode](http://embedXcode.weebly.com)
/// 
/// @author	qiao
/// @author	qiao
/// @date	13-3-29 上午9:42
/// @version	<#version#>
/// 
/// @copyright	© qiao, 2013年
/// @copyright	CC = BY NC SA
///
/// @see	ReadMe.txt for references
///

#include "PID_v1.h"
#include "Arduino.h"  


#ifndef mecanumRobot_h
#define mecanumRobot_h


#define PI 3.14159265358979f

// Motor ID
#define LeftForward 0
#define RightForward 1
#define LeftBackward 2
#define RightBackward 3

#define Forward 0
#define Backward 1


#define MotorPwmThreshold 230

#define MaxSpeed  400
#define MaxDirection  400

#define KP 10.5
#define KI 30.0
#define KD 0.7


#define EncoderMaxSpeed 40

/****************** parameters *****************/



void speedLeftForward();
void speedRightForward();
void speedLeftBackward();
void speedRightBackward();

extern int hallAPin[4];
extern int hallBPin[4];
extern int interruptPin[4];

extern volatile int encoderSpeed[4];





class Mecanum {

  
public:
  //pin definition
  int dirPin[4];
  int pwmPin[4];
  int relayPin[2];
  int sampleTime;
  
  //PID pointer
  PID *pidArray[4];
  
  
  //parameter of algorithm
  int magnitude;
  int angle;
  int rotation;
  void (*interruptAgent[4])();
  
  int motorPwm[4];
  
  double inputPID[4];
  double outputPID[4];
  double setpointPID[4];
  
  
//  int lastMotorPwm[4];
  
  Mecanum(int dirLeftForward = 28,
          int dirRightForward = 29,
          int dirLeftBackward = 30,
          int dirRightBackward = 31,
          
          int pwmLeftForward = 6,
          int pwmRightForward = 7,
          int pwmLeftBackward = 8,
          int pwmRightBackward = 5,
          
          int hallALeftForward = 3,
          int hallARightForward = 2,
          int hallALeftBackward = 20,
          int hallARightBackward = 21,
          
          int hallBLeftForward = 22,
          int hallBRightForward = 23,
          int hallBLeftBackward = 24,
          int hallBRightBackward = 25,
          
          void (*interruptLeftForward)()=speedLeftForward,
          void (*interruptRightForward)()=speedRightForward,
          void (*interruptLeftBackward)()=speedLeftBackward,
          void (*interruptRightBackward)()=speedRightBackward,
          
          int sampleTimeDuration=20,
          
          int relayForward =14 ,
          int relayBackward = 15
          );
  void setSpeed(int magnitude, int angle, int rotation);
  void run(int magnitude, int angle, int rotation);
  void setMotor(uint8_t which);
  void init();
  
  void updateSpeed();
  double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);
  void powerOn();
  void powerOff();
//  void updateMaxSpeed();


};



/*************** Call functions ****************/

//void setupDriver();

//void updateMotorSpeed(int Speed,int Degree,int Direction);
//void runMotors();


#endif
