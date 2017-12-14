#include "MyEncoders.h"
#include "MyServos.h"
#include "MySharpSensor.h"

float K = 0.6;  //Constant for PID control
float desiredDistance = 5.0;   //Desired distance to goal
bool first = true;

void setup() {
  Serial.begin(115200);
  initEncoders();
  LServo.attach(2);
  RServo.attach(3);
}

void loop() {
  delay(60);    //Delay of 60 so that the sensors work correctly

  /*If calibrate isn't commented out, it will calibrate normally. Otherwise, it will load in the RPS values from memory*/
  if(first){
    first = false;
    //calibrate();
    EEPROM.get(0, RPSMapLeft);
    EEPROM.get(sizeof(int) * 161, RPSMapRight);

  }
  
  takeNewMeasurement(SSFRONT);
  takeNewMeasurement(SRIGHT);
 
  float shortSensor = getCombinedDistance(SSFRONT);
  float rightSensor = getCombinedDistance(SRIGHT);

  float distanceErrorFront = shortSensor - desiredDistance;
  float distanceErrorRight = desiredDistance - rightSensor;

  /*Front short sensor is too close to the wall so it must turn until it is no longer so close*/
  if(shortSensor <= 5.4){
    while(shortSensor < 14){  //Turns until the short distance detects that the wall is greater than 14 inches away
      setSpeedsvw(0, 1);
      takeNewMeasurement(SSFRONT);
      shortSensor = getCombinedDistance(SSFRONT);
      delay(60);
    }
  }

  //The below are control signals that will correspond to the robot's velocity
  float uFront = K * distanceErrorFront;
  float uRight = K * distanceErrorRight;

  //uFront must be < 4 or > -4 for the robot to follow walls properly.
  //The below is a saturation function
  if(uFront > 4)
  {
    uFront = 4;
  }
  else if(uFront < -4)
  {
    uFront = -4;
  }
  
  //setSpeedsvw will be used. This means that uRight must not be zero since it will be the denominator.
  //uRight being so small means that the robot will be moving in a straight line
  if(uRight == 0)
    uRight = 0.0001;

  //Used to figure out if the robot is going to turn too hard where it shouldn't.
  //Otherwise, the robot could potentially spin forever
  float radius = uFront / uRight;

  //Another saturation function
  //Keeps the robot from turning too hard if it is close enough to a wall
  //The radius limit can be changed to allow the robot to turn harder or softer. Through testing,
  //9 and -9 allowed the robot to follow walls very well
  if(rightSensor <= 12 && radius < 9 && radius > -9){
    if(radius > 0)
      uRight = uFront / 9.0;
    else
      uRight = uFront / -9.0;
  }
  //Otherwise, it there is no wall that is close, it begins to turn.
  //This happens when it gets to a corner of a wall
  else if(rightSensor > 13){
    if(radius > 0)
      uRight = uFront / 2.0;
    else
      uRight = uFront / -2.0;
  }

  setSpeedsvw(uFront, uRight);
}
