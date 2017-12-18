#include "stack.h"
#include "Chooser.h"

#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//Pins that are used for the color sensor
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

//Gets the color. The commented out statements could be used to add new colors
int getColor()
{

  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  int redFrequency = pulseIn(sensorOut, LOW);
/*
  Serial.print(redFrequency);
  Serial.print("  ");
*/
  //Code below is for green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  int greenFrequency = pulseIn(sensorOut, LOW);
/*
  Serial.print(greenFrequency);
  Serial.print("  ");
*/
  //Code below is for blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  int blueFrequency = pulseIn(sensorOut, LOW);

//  Serial.println(blueFrequency);

  lcd.setCursor(0, 0);

  //Returns 2 if a blue or red is detected, 0 if it's a different color. Knowing whether the color was red or blue is not important.
  //It also flashes the LCD blue or red
  if(redFrequency < 46 && greenFrequency > blueFrequency && greenFrequency > 75 && redFrequency < blueFrequency)
  {
    lcd.setBacklight(RED);
    return 2; //Red
  }
  else if(redFrequency < 130 && redFrequency > greenFrequency && greenFrequency > blueFrequency && blueFrequency < 40)
  {
    lcd.setBacklight(BLUE);
    return 2; //Blue
  }  
  else
  {
    lcd.setBacklight(WHITE);
    return 0; //Nothing
  }
}
