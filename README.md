# Arduino-Differential-Drive-Robot

This Arduino Differential Drive Robot has two motorized wheels and one castor wheel. The motorized wheels have encoders so
that it is possible to find out how many rotations a wheel has done. There are three short distance sensors and one long distance 
sensor. The short distance sensors are placed facing forward, to the left, and to the right of the robot. The long distance sensor is 
placed facing forward. There is also a color sensor placed underneath the robot. Finally, the robot also has an LCD on top of it 
so that information can be displayed. Below you'll find information about the different folders. Before any of the programs are run, the 
robot  must calibrated. The calibrate function allows the robot to be sent an RPS that it should go at instead of sending it a length of 
the pulse width. StraightLine and SShape will both run the calibrate() function as long as that function is not commented out in their
respective .ino files. It only needs to be run once since the values the robot got from calibrate() will be stored in the robot's memory
afterwards.

# Maze
The files in this folder are used to make the robot map a maze and then plan the shortest path between a given starting point and an
ending point. The maze used is a 4x4 maze. Each cell in the maze has a width of about 16 inches. Each cell is also separated by red tape
horizontally and blue tape vertically. Before running this program, the robot must be calibrated. Since memory became an issue in this 
program, if the robot has been calibrated using any of the other programs, it must be recalibrated. When the program starts, you are 
able to choose either mapping or path planning. 

If mapping is chosen, you will be able to enter in the starting location and oriention into the robot using the buttons on the LCD. Once
exit has been selected, the robot will begin to the map the maze by doing a DFS. The robot uses all of its sensors to detect if there
are walls in a cell that it is in. It also uses its sensor to ensure that it stays as centered as possible in the cell that it is in.
The robot knows if it has entered a new cell by using its color sensor. If the robot crosses either blue or red tape, it means that it
has entered a new cell. Once every cell has been visited, mapping will be complete. The robot will then display "DONE!" on the LCD. The
select button can then be pressed to verify that the robot mapped the maze correctly. The robot will display whether there is a wall "W"
or whether there is no wall "o" on the LCD for each cell. Each 4 sections of the LCD is one cell. The order of the sides of the cell
goes west, north, east, and south. Only the first 8 grid cells will be displayed. The select button will have to pressed again to show
the last 8 cells. This is because the LCD is not big enough to show all the cells at once. When the select button is then pressed again,
the program will exit and you will be brought back to the selection screen. If anytime during the mapping the robot messes up, the 
select button can be pressed to exit the program.

If path planning is chosen, you will be able to enter in the starting location, ending location, and orientation into the robot using
the buttons on the LCD. Once exit has been selected, the robot will begin to find the shorest path between the two given locations using
a BFS. If mapping was not selected beforehand, the robot will perform the BFS on a map that is already loaded into the robot. To load a
map into the robot, edit the function loadMap() in robot.h. There is information there that tells how to load in a map correctly.
Once the BFS is completed, the robot will begin to follow the shortest path that was found. When the robot has made it to the ending
location, you will be brought back to the selection screen.

# StraightLine
The files in this folder are used to make the robot move in a straight line. There are two parameters, PARAM_X and PARAM_Y, which are
defined in StraightLine.ino. Those values can be changed to make the robot travel a given distance while also being given a time limit
to complete the distance given. The robot can only move at a maximum of 0.80 rotations per second. If the distance and time combination
require the robot to go faster than possible, it will display a message on the LCD that the movement is impossible. The robot 
will wait until the select button is pressed to begin its movement.

# SShape
The files in this folder are used to make the robot do an S shape. There are three parameters that are used to make the robot complete
its S shape movement. PARAM_R1 and PARAM_R2 are radii. The first radius is used for the first half of the S while the second radius is
used for the second half of the movement. The third parameter is PARAM_Y which is the time limit to complete the given movement. These
parameters are definied in SShape.ino. If the radius and time combination given to the robot requires the robot to go faster than 0.80 
rotations per second, the robot will display a message on the LCD that the movement is impossible. The robot will wait until
the select button is pressed to begin its movement. Once it makes half of the S shape, it will stop and wait for another select press.
When it is pressed the second time, the robot will complete the S shape. The entire S shape will be completed in the time specified by
PARAM_Y.

# WallDistance
The files in this folder are used to make the robot move a given distance away from a wall. The parameter desiredDistance can be changed 
to anything from 2-10 inches since only the front short distance sensor is being used. When the program starts, the robot will either 
move away or towards a wall to position itself whatever distance was given away from the wall. If the robot overshoots the desired 
distance, it will correct itself thanks to PID control. For this program to work correctly, the robot must have fully charged batteries, 
otherwise the sensors will return incorrect readings.

# WallFollowing
The files in this folder are used to make the robot follow walls. The parameter desiredDistance can be changed to anything from 3-8 
inches since the sensors on the robot are short distance sensors. The code only allows the robot to follow walls on its right side. PID
control is used to make the robot follow walls. For this program to work correctly, the robot must have fully charged batteries,
otherwise the sensors will return incorrect readings.
