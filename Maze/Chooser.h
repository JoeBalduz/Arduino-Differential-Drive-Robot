
#ifndef _CHOOSER_
#define _CHOOSER_

#include "Arduino.h"
#include <Adafruit_RGBLCDShield.h>


#define BLUE 0x4
#define RED 0x1
#define WHITE 0x7

//the following variables must be defined somewhere
extern unsigned int numPrograms;     // the number programs
extern bool (*programList[])();      // array of pointers to functions of length "numPrograms", must return true when done
extern void (*setupProgramList[])(); // array of setup functions for each program, if no setup assign 0
extern const char * programNames[];  // array of names of each function

void initChooser(Adafruit_RGBLCDShield* lcd); //this function must be called during the setup
void execProgram();

#endif


