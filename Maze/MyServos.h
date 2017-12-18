#include <Servo.h>
#include <EEPROM.h>
#include "MyEncoders.h"

Servo LServo;
Servo RServo;

//Arrays of RPS
int RPSMapLeft[105];
int RPSMapRight[105];

#ifndef __MY_SERVOS__
#define __MY_SERVOS__

//Sets the speed of the wheels using microseconds.
//Maximum is 200, minimum is -200.
//Anything above 0 moves the robot forward while anything below zero moves the robot backwards
void setSpeeds(int microsLeft, int microsRight){
  int speedLeft = 1500 + microsLeft;
  int speedRight = 1500 - microsRight;
  
  LServo.writeMicroseconds(speedLeft);
  RServo.writeMicroseconds(speedRight);
}

//Sets the spped of the wheels in RPS.
//Maximum is 52(0.52), minimum is -52(-0.52).
//Anything above 0 moves the robot forward while anything below zero moves the robot backwards
void setSpeedsRPS(float rpsLeft, float rpsRight){
  int rpsL, rpsR;

  //Gets the correct microseconds from the RPS array
  if(rpsLeft < 0 && rpsRight < 0){
    rpsL = -rpsLeft;
    rpsR = -rpsRight;
    setSpeeds(RPSMapLeft[52 - rpsL], RPSMapRight[52 - rpsR]);
  }
  else if(rpsLeft < 0){
    rpsL = -rpsLeft;
    rpsR = rpsRight;
    setSpeeds(RPSMapLeft[52 - rpsL], RPSMapRight[52 + rpsR]);
  }
  else if(rpsRight < 0){
    rpsL = rpsLeft;
    rpsR = -rpsRight;
    setSpeeds(RPSMapLeft[52 + rpsL], RPSMapRight[52 - rpsR]);
  }
  else{
    rpsL = rpsLeft;
    rpsR = rpsRight;
    setSpeeds(RPSMapLeft[52 + rpsL], RPSMapRight[52 + rpsR]);
  }
}

//Sets the speed of the wheels  in IPS.
//Anything above 0 moves the robot forward while anything below zero moves the robot backwards
void setSpeedsIPS(float ipsLeft, float ipsRight){
  float circumference = 2.61 * 3.14;
  int rpsLeft, rpsRight;

  //Calculates the RPS of each wheel
  rpsLeft = (ipsLeft / circumference) * 100;
  rpsRight = (ipsRight / circumference) * 100;
  

  setSpeedsRPS(rpsLeft, rpsRight);
}


//Maps the microseconds sent to the motors to an RPS
void calibrate(){
  int i, j;
  int locationLeft, locationRight;
  unsigned long past, current;
  float speeds[2];
  float sumLeft, sumRight, averageLeft, averageRight, count;
  int bottomLeft, bottomRight, topLeft, topRight;

  /*Sets all values in the array to -1*/
  for(i = 1;i < 105; i++){
    RPSMapLeft[i] = -200;
    RPSMapRight[i] = -200;
  }

  /*Goes from 60 microseconds to -59 microseconds in incerments of 3. This number gets sent to setSpeeds*/
  for(j = 60; j >= -59; j -= 3){
    count = 0;
    sumLeft = 0;
    sumRight = 0;
    setSpeeds(j, j);
    delay(500);
    past = millis();
    /*Gets the average speed over a period of time*/
    while(millis() - past < 1500){
      getSpeeds(speeds);
      sumLeft += speeds[0];
      sumRight += speeds[1];
      count++;
      if(count == 28)
        break;
      delay(30);
    }
      //Gets the average of both wheels and gets the correct location to place the microseconds into the array
      averageLeft = sumLeft / count;
      averageRight = sumRight / count;
      locationLeft = averageLeft * 100;
      locationRight = averageRight * 100;
      /*Places the microseconds into the arrays*/
      if(j > 0){
        RPSMapLeft[52 + locationLeft] = j;
        RPSMapRight[52 + locationRight] = j;
      }
      else{
        RPSMapLeft[52 - locationLeft] = j;
        RPSMapRight[52 - locationRight] = j;
      }

  }
  setSpeeds(0, 0);

  //Initializes some values in the array
  RPSMapLeft[0] = -60;
  RPSMapRight[0] = -160;
  RPSMapLeft[52] = 0;
  RPSMapRight[52] = 0;
  

  /*Fills in any missing values with the average of the next non-missing value
    and the value directly under it*/
  for(i = 1; i < 105; i++){
    if(RPSMapLeft[i] == -200){
      for(j = i; j < 105; j++){
        if(RPSMapLeft[j] != -200){
          RPSMapLeft[i] = (RPSMapLeft[i - 1] + RPSMapLeft[j]) / 2;
          break;
        }
        if(j == 104)
          RPSMapLeft[i] = RPSMapLeft[i - 1];
      }
    }
    if(RPSMapRight[i] == -200){
      for(j = i; j < 105; j++){
        if(RPSMapRight[j] != -200){
          RPSMapRight[i] = (RPSMapRight[i - 1] + RPSMapRight[j]) / 2;
          break;
        }
        if(j == 104)
          RPSMapRight[i] = RPSMapRight[i - 1];
      }
    }
  }

//Places the RPS arrays into memory
int eeAddress = 0;
EEPROM.put(eeAddress, RPSMapLeft);
eeAddress += sizeof(int) * 105;
EEPROM.put(eeAddress, RPSMapRight);
}

//Set speeds using a linear speed and angular velocity.
//A positive angular velocity will result in a counterclockwise spin while a negative
//angular velocity will result in a clockwise spin.
void setSpeedsvw(float v, float w){
  float radius;
  float velocityLeft, velocityRight;
  float wheel_distance = 2;

  //Calculates the velocity required for each wheel
  if(w < 0){
    w = -w;
    radius = v / w;
    velocityLeft = w * (radius + wheel_distance);
    velocityRight = w * (radius - wheel_distance);
  }
  else{
    radius = v / w;
    velocityLeft = w * (radius - wheel_distance);
    velocityRight = w * (radius + wheel_distance);
  }

  //The below is done since the robot's maximum IPS is 6
  //Checks if velocity is greater than 6  
  if(velocityLeft > 6)
    velocityLeft = 6;
  else if(velocityLeft < -6)
    velocityLeft = -6;
  if(velocityRight > 6)
    velocityRight = 6;
  else if(velocityRight < -6)
    velocityLeft = -6;

  setSpeedsIPS(velocityLeft, velocityRight);
}



#endif 
