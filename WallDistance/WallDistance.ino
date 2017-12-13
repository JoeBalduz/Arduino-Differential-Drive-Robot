#include "MyEncoders.h"
#include "MyServos.h"
#include "MySharpSensor.h"
#include <EEPROM.h>

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
  delay(80);    //Delay of 80 so that the sensors work correctly

  /*If calibrate isn't commented out, it will calibrate normally. Otherwise, it will load in the RPS values from memory*/
  if(first){
    first = false;
    //calibrate();
    EEPROM.get(0, RPSMapLeft);
    EEPROM.get(sizeof(int) * 105, RPSMapRight);
  }
  
  takeNewMeasurement(SSFRONT);
 
  float shortSensor = getCombinedDistance(SSFRONT);

  float distanceError = shortSensor - desiredDistance;

  float u = K * distanceError;  //Control signal corresponding to robot's velocity

  
  //The below is done since the robot's maximum IPS is 6
  //Checks if velocity is greater than 6
  if(u > 6)
  {
    setSpeedsIPS(6, 6);
  }
  //Checks if velocity is less than 6
  else if(u < -6)
  {
    setSpeedsIPS(-6, -6);
  }
  else
  {
    setSpeedsIPS(u, u);
  }
}
