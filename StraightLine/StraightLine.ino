#include <Adafruit_RGBLCDShield.h>
#include "MyEncoders.h"
#include "MyServos.h"

#define PARAM_X 10 //Inches
#define PARAM_Y 4  //Seconds

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();  //Initializes the LCD display
float wheel_circumference = 2.61 * 3.14;
bool first = true;
float counts[2];  //Holds tick count for each wheel

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  initEncoders();
  LServo.attach(2);
  RServo.attach(3);
}

void loop() {
  delay(200);   //Done so that the button doesn't get pressed multiple times in a row
  uint8_t buttons = lcd.readButtons();

  /*If calibrate isn't commented out, it will calibrate normally. Otherwise, it will load in the RPS values from memory*/
  if(first){
    first = false;
    //calibrate();
    EEPROM.get(0, RPSMapLeft);
    EEPROM.get(sizeof(int) * 161, RPSMapRight);
  }

  /*Select button is pressed*/
  if(buttons & BUTTON_SELECT){
    //Find the required IPS
    float IPS = PARAM_X/PARAM_Y;

    //Find how many wheel ticks are required
    float ticks = (PARAM_X/wheel_circumference)*32;


    //Converts IPS to RPS
    float maximum_check = IPS/wheel_circumference;

    //Check if the RPS is above the robot's maximum RPS
    //Displays a message on the LCD display if the speed is not possible for the robot
    if(maximum_check > 0.8)
    {
      lcd.print("Cannot complete");
      lcd.setCursor(0, 1);
      lcd.print("movement");
    }
    else
    {
      setSpeedsIPS(IPS, IPS);

      //Continues to move with the set IPS on until its traveled the required distance according to wheel ticks
      while(true)
      {
        /*The below print statements are required for the loop to run correctly*/
        Serial.println();
        Serial.println();

        
        if((counts[0] >= ticks)||(counts[1] >= ticks))
        {
          setSpeeds(0,0);
          resetCounts();
          break;
        }
      }
    }
  }
}
