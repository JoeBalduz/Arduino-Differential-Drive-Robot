# Arduino-Differential-Drive-Robot

This Arduino Differential Drive Robot has two motorized wheels and one castor wheel. The motorized wheels have encoders so
that it is possible to find out how many rotations a wheel has done. There are three short distance sensors and one long distance sensor.
The short distance sensors are placed facing forward, to the left, and to the right of the robot. The long distance sensor is placed
facing forward. There is also a color sensor placed underneath the robot. Finally, the robot also has an LCD display on top of it so that
information can be displayed. Below you'll find information about the different folders.


# StraightLine
The files in this folder are used to make the robot move in a straight line. There are two parameters, PARAM_X and PARAM_Y, which are
defined in StraightLine.ino. Those values can be changed to make the robot travel a given distance while also being given a time limit
to complete the distance given. The robot can only move at a maximum of 0.80 rotations per second. If the distance and time combination
require the robot to go faster than possible, it will display a message on the LCD display that the movement is impossible. Before running
StraightLine.ino, the robot will have to be calibrated. There is an if statement in StraightLine.ino that has a function called 
calibrate() commented out. The calibrate() function must be run once. After that, the values that calibrate got will be stored into the 
robot's memory so the function can be commented out afterwards.
