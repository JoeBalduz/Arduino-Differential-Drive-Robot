#include <Servo.h>
#include <EEPROM.h>
#include "MyEncoders.h"

Servo LServo;
Servo RServo;

//Arrays of RPS
int RPSMapLeft[161];
int RPSMapRight[161];

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
//Maximum is 80(0.8), minimum is -80(-0.8).
//Anything above 0 moves the robot forward while anything below zero moves the robot backwards
void setSpeedsRPS(float rpsLeft, float rpsRight){
  int rpsL, rpsR;

  //Gets the correct microseconds from the RPS array
  if(rpsLeft < 0 && rpsRight < 0){
    rpsL = -rpsLeft;
    rpsR = -rpsRight;
    setSpeeds(RPSMapLeft[80 - rpsL], RPSMapRight[80 - rpsR]);
  }
  else if(rpsLeft < 0){
    rpsL = -rpsLeft;
    rpsR = rpsRight;
    setSpeeds(RPSMapLeft[80 - rpsL], RPSMapRight[80 + rpsR]);
  }
  else if(rpsRight < 0){
    rpsL = rpsLeft;
    rpsR = -rpsRight;
    setSpeeds(RPSMapLeft[80 + rpsL], RPSMapRight[80 - rpsR]);
  }
  else{
    rpsL = rpsLeft;
    rpsR = rpsRight;
    setSpeeds(RPSMapLeft[80 + rpsL], RPSMapRight[80 + rpsR]);
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
  for(i = 1;i < 161; i++){
    RPSMapLeft[i] = -200;
    RPSMapRight[i] = -200;
  }

  /*Goes from 150 microseconds to -149 microseconds in incerments of 3. This number gets sent to setSpeeds*/
  for(j = 150; j >= -149; j -= 3){
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
        RPSMapLeft[80 + locationLeft] = j;
        RPSMapRight[80 + locationRight] = j;
      }
      else{
        RPSMapLeft[80 - locationLeft] = j;
        RPSMapRight[80 - locationRight] = j;
      }

  }
  setSpeeds(0, 0);

  //Initializes some values in the array
  RPSMapLeft[0] = -150;
  RPSMapRight[0] = -150;
  RPSMapLeft[80] = 0;
  RPSMapRight[80] = 0;
  

  /*Fills in any missing values with the average of the next non-missing value
    and the value directly under it*/
  for(i = 1; i < 161; i++){
    if(RPSMapLeft[i] == -200){
      for(j = i; j < 161; j++){
        if(RPSMapLeft[j] != -200){
          RPSMapLeft[i] = (RPSMapLeft[i - 1] + RPSMapLeft[j]) / 2;
          break;
        }
        if(j == 160)
          RPSMapLeft[i] = RPSMapLeft[i - 1];
      }
    }
    if(RPSMapRight[i] == -200){
      for(j = i; j < 161; j++){
        if(RPSMapRight[j] != -200){
          RPSMapRight[i] = (RPSMapRight[i - 1] + RPSMapRight[j]) / 2;
          break;
        }
        if(j == 160)
          RPSMapRight[i] = RPSMapRight[i - 1];
      }
    }
  }

//Places the RPS arrays into memory
int eeAddress = 0;
EEPROM.put(eeAddress, RPSMapLeft);
eeAddress += sizeof(int) *161;
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

  setSpeedsIPS(velocityLeft, velocityRight);
}



#endif 
