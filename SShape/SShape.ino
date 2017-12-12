#include <Adafruit_RGBLCDShield.h>
#include "MyEncoders.h"
#include "MyServos.h"

#define PARAM_R1  5  //First radius in inches
#define PARAM_R2  5 //Second radius in inches
#define PARAM_Y   7.64  //Seconds

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
bool second = true; //Used to calibrate or load in values at the start
unsigned long counts[2];
bool first = true;
float wheel_distance = 2.03;
float wheel_circumference = 8.23;
float wheel_radius = 1.31;

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  initEncoders();
  LServo.attach(2);
  RServo.attach(3);

}

//In the code below, 1 refers to the first radius while 2 refers to the second radius
void loop() {
  delay(200);   //Done so that the button doesn't get pressed multiple times in a row
  uint8_t buttons = lcd.readButtons();

  /*If calibrate isn't commented out, it will calibrate normally. Otherwise, it will load in the RPS values from memory*/
  if(second){
    second = false;
    //calibrate();
    EEPROM.get(0, RPSMapLeft);
    EEPROM.get(sizeof(int) * 161, RPSMapRight);
  }

  /*Select button is pressed*/
  if(buttons & BUTTON_SELECT){
    /*Arc lengths are found*/
    float arcLength1 = PARAM_R1 * 3.14;
    float arcLength2 = PARAM_R2 * 3.14;
    float arcLength1Left = (PARAM_R1 + wheel_distance) * 3.14;
    float arcLength1Right = (PARAM_R1 - wheel_distance) * 3.14;
    float arcLength2Left = (PARAM_R2 - wheel_distance) * 3.14;
    float arcLength2Right = (PARAM_R2 + wheel_distance) * 3.14;

    /*Number of rotations for each wheel is found*/
    float rotations1Left = arcLength1Left / (2 * 3.14 * wheel_radius);
    float rotations1Right = arcLength1Right / (2 * 3.14 * wheel_radius);
    float rotations2Left = arcLength2Left / (2 * 3.14 * wheel_radius);
    float rotations2Right = arcLength2Right / (2 * 3.14 * wheel_radius);

    /*Number of ticks for each wheel is found*/
    unsigned long ticks1Left = rotations1Left * 32;
    unsigned long ticks1Right = rotations1Right * 32;
    unsigned long ticks2Left = rotations2Left * 32;
    unsigned long ticks2Right = rotations2Right * 32;

    /*Distance, velocity, and angular velocity for the different radii are found*/
    float distance = arcLength1 + arcLength2;
    float velocity = distance / PARAM_Y;
    float angular1 = velocity / PARAM_R1;
    float angular2 = velocity / PARAM_R2;

    /*Velocity for each wheel is found*/
    float velocityLeft1 = angular1 * (PARAM_R1 + wheel_distance);
    float velocityRight1 = angular1 * (PARAM_R1 - wheel_distance);
    float velocityLeft2 = angular2 * (PARAM_R2 - wheel_distance);
    float velocityRight2 = angular2 * (PARAM_R2 + wheel_distance);

    /*RPS for each wheel is found*/
    float rpsLeft1 = (velocityLeft1 / wheel_circumference) * 100;
    float rpsRight1 = (velocityRight1 / wheel_circumference) * 100;
    float rpsLeft2 = (velocityLeft2 / wheel_circumference) * 100;
    float rpsRight2 = (velocityRight2 / wheel_circumference) * 100;

    /*Checks to see if the motion can be completed*/
    if(rpsLeft1 > 80 || rpsLeft2 > 80 || rpsRight1 > 80 || rpsRight2 > 80){
      lcd.print("Motion cannot");
      lcd.setCursor(0, 1);
      lcd.print("be completed");
    }
    /*Select was pressed for the first time*/
    else if(first){
      first = false;
      setSpeedsRPS(rpsLeft1,rpsRight1);
      /*Continues to move at its speed until it makes enough rotations to reach its destination*/
      while(true){
        getCounts(counts);
        //The below print statements are required to make the loop work correctly
        Serial.println();
        Serial.println();
        if(counts[0] >= ticks1Left || counts[1] >= ticks1Right){
          setSpeeds(0, 0);
          resetCounts();
          break;
        }
      }
    }
    /*Select was pressed for the second time*/
    else{
      first = true;
      setSpeedsRPS(rpsLeft2, rpsRight2);
      /*Continues to move at its speed until it makes enough rotations to reach its destination*/
      while(true){
        getCounts(counts);
        //The below print statements are required to get the loop to work correctly
        Serial.println();
        Serial.println();
        if(counts[0] >= ticks2Left || counts[1] >= ticks2Right){
          setSpeeds(0, 0);
          resetCounts();
          break;
        }
      }
    }
    
  }

}
