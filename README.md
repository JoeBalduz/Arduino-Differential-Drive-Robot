# Arduino-Differential-Drive-Robot

This Arduino Differential Drive Robot has two motorized wheels and one castor wheel. The motorized wheels have encoders so
that it is possible to find out how many rotations a wheel has done. There are three short distance sensors and one long distance sensor.
The short distance sensors are placed facing forward, to the left, and to the right of the robot. The long distance sensor is placed
facing forward. There is also a color sensor placed underneath the robot. Finally, the robot also has an LCD display on top of it so that
information can be displayed. Below you'll find information about the different folders. Before any of the programs are run, the robot 
must calibrated. The calibrate function allows the robot to be sent an RPS that it should go at instead of sending it a length of the 
pulse width. StraightLine and SShape will both run the calibrate() function as long as that function is not commented out in their
respective .ino files. It only needs to be run once since the values the robot got from calibrate() will be stored in the robot's memory
afterwards.

# StraightLine
The files in this folder are used to make the robot move in a straight line. There are two parameters, PARAM_X and PARAM_Y, which are
defined in StraightLine.ino. Those values can be changed to make the robot travel a given distance while also being given a time limit
to complete the distance given. The robot can only move at a maximum of 0.80 rotations per second. If the distance and time combination
require the robot to go faster than possible, it will display a message on the LCD display that the movement is impossible. The robot will
wait until the select button is pressed to begin its movement.

# SShape
The files in this folder are used to make the robot do an S shape. There are three parameters that are used to make the robot complete
its S shape movement. PARAM_R1 and PARAM_R2 are radii. The first radius is used for the first half of the S while the second radius is
used for the second half of the movement. The third parameter is PARAM_Y which is the time limit to complete the given movement. These
parameters are definied in SShape.ino. If the radius and time combination given to the robot requires the robot to go faster than 0.80 
rotations per second, the robot will display a message on the LCD display that the movement is impossible. The robot will wait until
the select button is pressed to begin its movement. Once it makes half of the S shape, it will stop and wait for another select press.
When it is pressed the second time, the robot will complete the S shape. The entire S shape will be completed in the time specified by
PARAM_Y.

# WallDistance
The files in this folder are used to make the robot move a given distance away from a wall. The parameter desiredDistance can be changed 
to anything from 2-10 inches since only the front short distance sensor is being used. When the program starts, the robot will either move 
away or towards a wall to position itself whatever distance was given away from the wall. If the robot overshoots the desired distance, it 
will correct itself thanks to PID control. For this program to work correctly, the robot must have fully charged batteries, otherwise the
sensors will return incorrect readings.
