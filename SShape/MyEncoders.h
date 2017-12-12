#include <PinChangeInt.h>

#define ENCODER_PORT_RIGHT 11
#define ENCODER_PORT_LEFT 10


#ifndef __MY_ENCODERS__
#define __MY_ENCODERS__

unsigned long rightTicks;
unsigned long leftTicks;
unsigned long rightTime;
unsigned long leftTime;
unsigned long oldRightTime;
unsigned long oldLeftTime;

//The two functions below increments the tick counts on each wheel. Interrupts are used

void risingRight(){
  rightTicks++;
  oldRightTime = rightTime;
  rightTime = millis();
}

void risingLeft(){
  leftTicks++;
  oldLeftTime = leftTime;
  leftTime = millis();
}

//Sets the tick counts to 0
void resetCounts(){
  noInterrupts();
  rightTicks = 0;
  leftTicks = 0;
  interrupts();
}

//Used to get ticks counts for each wheel
void getCounts(unsigned long counts[]){
  counts[0] = leftTicks;
  counts[1] = rightTicks;
}


//Used to get the speed of the wheels in IPS
void getSpeeds(float speeds[]){
  unsigned long current = millis();

  if(current - leftTime >= 1500)   //No ticks have been read in a while
    speeds[0] = 0;
  else
    speeds[0] = 1 / (((leftTime - oldLeftTime) * 0.001) * 32);

  
  if(current - rightTime >= 1500)   //No ticks have been read in a while
    speeds[1] = 0;
  else
    speeds[1] = 1 / (((rightTime - oldRightTime) * 0.001) * 32);
}

//Initializes the encoders and tick counts
void initEncoders(){
  rightTicks = 0;
  leftTicks = 0;
  leftTime = millis();
  rightTime = millis();
  
  //Code below initializes encoders and interrupts
  pinMode(ENCODER_PORT_RIGHT, INPUT_PULLUP);
  pinMode(ENCODER_PORT_LEFT, INPUT_PULLUP);
  PCintPort::attachInterrupt(ENCODER_PORT_RIGHT, &risingRight, RISING);
  PCintPort::attachInterrupt(ENCODER_PORT_LEFT, &risingLeft, RISING);
}


#endif
