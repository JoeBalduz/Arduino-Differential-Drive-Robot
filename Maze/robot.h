#include "MyServos.h"
#include "MySharpSensor.h"
#include "color.h"

String visitedLocations = "OOOOOOOOOOOOOOOO";   //This gets display on the LCD. It tells which cells have been visited

struct robot{
  int orientation = 0;  //0(west), 1(north), 2(east), 3(south)
  byte cellLocation = 1;  //Current cell location
  //The below are current coordinates
  byte x;
  byte y;
};

struct cell{
  bool visited = false;   //Tells if a cell has been visited in the DFS search
  bool vistedBFS = false; //Tells if a cell was visited in the BFS search
  //2 for unknown, 0 for no wall, and 1 for wall
  byte westWall = 2;
  byte northWall = 2;       
  byte eastWall = 2;
  byte southWall = 2;
  byte cellNumber;  //Tells which cell number it is
  byte prev; //Tells which cell is before it in BFS
};

struct cell cells[4][4];  //4x4 maze
struct cell bfs[17];      //Used to make BFS easier
struct robot myRobot;

//Makes sure that everything around the robot has been checked. This only ever happens in the very first cell
//since it doesn't know what is behind it
bool checkAllSide()
{
  if(cells[myRobot.x][myRobot.y].westWall != 2 && cells[myRobot.x][myRobot.y].northWall != 2 && cells[myRobot.x][myRobot.y].eastWall != 2 && cells[myRobot.x][myRobot.y].southWall != 2)
    return true;
  else
    return false; 
}

//Turns the robot 90 degrees to the right
void turnRobot()
{
  myRobot.orientation++;    //Changes the orientation value
  if(myRobot.orientation > 3)
    myRobot.orientation = 0;

  resetCounts();
  while(leftTicks < 14)     //Turns almost 90 degrees
    setSpeedsvw(0, -1);
  setSpeedsIPS(0, 0);
}

//Updates all information about the current cell
void updateCurrentCell()
{
  int left = 1, front = 1, right = 1; //Initilize all grid cell walls to 1
  int i;

  //Marks the cell as visited
  cells[myRobot.x][myRobot.y].visited = true;

  //Gets wall measurements to be able to figure out if there is a wall
  for(i = 0; i < 5; i++){
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    takeNewMeasurement(SLONG);
    delay(60);    //Delay of 60 is to get the sensors to work correctly
  }
  
  //Checks if there is a wall
  if(getCombinedDistance(SLEFT) > 11)
    left = 0;
  if(getCombinedDistance(SLONG) > 18)
    front = 0;
  if(getCombinedDistance(SRIGHT) > 11)
    right = 0;


  //Places the walls in the correct place depending on orientation
  if(myRobot.orientation == 1){
    cells[myRobot.x][myRobot.y].westWall = left;
    cells[myRobot.x][myRobot.y].northWall = front;
    cells[myRobot.x][myRobot.y].eastWall = right;
  }
  else if(myRobot.orientation == 2){
    cells[myRobot.x][myRobot.y].northWall = left;
    cells[myRobot.x][myRobot.y].eastWall = front;
    cells[myRobot.x][myRobot.y].southWall = right;
  }
  else if(myRobot.orientation == 3){
    cells[myRobot.x][myRobot.y].eastWall = left;
    cells[myRobot.x][myRobot.y].southWall = front;
    cells[myRobot.x][myRobot.y].westWall = right;
  }
  else{
    cells[myRobot.x][myRobot.y].southWall = left;
    cells[myRobot.x][myRobot.y].westWall = front;
    cells[myRobot.x][myRobot.y].northWall = right;
  }
  
}

//Since the robot can't do perfect 90 degree turns, the robot must be adjusted whenever it gets too close to a wall.
//This makes the robot follow walls.
void adjustment()
{
  //The robot adjusts itself more if it is closer to the wall.
  //That's to get the robot away from the wall as fast as possible
  if(getCombinedDistance(SLEFT) < 2.5)
  {
    resetCounts();
    while(leftTicks < 2)
      setSpeedsvw(0, -1);
  }
  else if(getCombinedDistance(SLEFT) < 5.75)
  {
    resetCounts();
    while(leftTicks < 1)
      setSpeedsvw(0, -1);
  }
  if(getCombinedDistance(SRIGHT) < 2.5){
    resetCounts();
    while(leftTicks < 2){
      setSpeedsvw(0,1);
    }
  }
  else if(getCombinedDistance(SRIGHT) < 5.75)
  {
    resetCounts();
    while(leftTicks < 1)
      setSpeedsvw(0, 1);
  }
  setSpeedsIPS(0,0);
}

//Goes to the middle of a cell
void travelToMid()
{
  //To get to the middle of the cell, 36 ticks are required. It has been split up so that the robot
  //can do adjustments while traveling to the middle of the cell. Otherwise, the robot could end up
  //hitting a wall
  resetCounts();
  while(leftTicks < 14){
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    setSpeedsIPS(4,4);
    getColor();
  }
  adjustment();
  resetCounts();
  while(leftTicks < 10)
  {
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    setSpeedsIPS(4, 4);
    getColor();
  }
  adjustment();
  resetCounts();
  while(leftTicks < 12){
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    setSpeedsIPS(4, 4);
    getColor();
  }
  adjustment();
  setSpeedsIPS(0, 0);
}

//Updates the LCD
void updateDisplay()
{
  visitedLocations[myRobot.cellLocation - 1] = 'X';   //Marks the location as visited in the display
  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(3, 1);
  lcd.print("O"); //O = Orientation

  //Prints the correct orientation to the display
  if(myRobot.orientation == 0)
    lcd.print('W');
  else if(myRobot.orientation == 1)
    lcd.print('N');
  else if(myRobot.orientation == 2)
    lcd.print('E');
  else
    lcd.print('S');

  lcd.setCursor(0, 1);
  lcd.print("G");   //G = Grid cell number
  lcd.print(myRobot.cellLocation);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(visitedLocations);
  
}

//Checks if all borders of cell are visited or walls
bool allWallsOrVisited()
{
  if((cells[myRobot.x][myRobot.y].westWall == 1 || cells[myRobot.x][myRobot.y - 1].visited == true) && (cells[myRobot.x][myRobot.y].northWall == 1 || cells[myRobot.x - 1][myRobot.y].visited == true) &&
    (cells[myRobot.x][myRobot.y].eastWall == 1 || cells[myRobot.x][myRobot.y + 1].visited == true) && (cells[myRobot.x][myRobot.y].southWall == 1 || cells[myRobot.x + 1][myRobot.y].visited == true)){
      return true;
    }
  else
    return false;
     
}

//Clears the screen. Helpful for removing everything from the screen before printing something new out
void clearScreen()
{
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

//Sets the robot's coordinates
void setRobotCoordinates()
{
  int i, j;

  //Goes through every grid cell in the array until the cell number is equal to the robot's location
  for(i = 0; i < 4; i++)
  {
    for(j = 0; i < 4; j++)
    {
      if(cells[i][j].cellNumber == myRobot.cellLocation)
      {
        myRobot.x = i;
        myRobot.y = j;
        return;
      }
    }
  }
}

//This is used to do adjustments while the robot is traveling to another cell. It only adjusts if 12 ticks are counted
void doOne(){
  if(leftTicks > 12){
    resetCounts();
    adjustment();
  }
}

//Prints the robot's orientation
void printOrientation(){
  if(myRobot.orientation == 0)
    lcd.print('W');
  else if(myRobot.orientation == 1)
    lcd.print('N');
  else if(myRobot.orientation == 2)
    lcd.print('E');
  else
    lcd.print('S');
}

//Fills in the BFS array with the 4x4 array
void fillBFS()
{ 
  bfs[1] = cells[0][0];
  bfs[2] = cells[0][1];
  bfs[3] = cells[0][2];
  bfs[4] = cells[0][3];
  bfs[5] = cells[1][0];
  bfs[6] = cells[1][1];
  bfs[7] = cells[1][2];
  bfs[8] = cells[1][3];
  bfs[9] = cells[2][0];
  bfs[10] = cells[2][1];
  bfs[11] = cells[2][2];
  bfs[12] = cells[2][3];
  bfs[13] = cells[3][0];
  bfs[14] = cells[3][1];
  bfs[15] = cells[3][2];
  bfs[16] = cells[3][3];
}

//This is used to load in a map without doing SLAM. Useful for testing out BFS since it would take a long time
//to continuously to SLAM to test out BFS
void loadMap()
{
  //1
  cells[0][0].westWall = 1;
  cells[0][0].northWall = 1;
  cells[0][0].eastWall = 1;
  cells[0][0].southWall = 0;
  //2
  cells[0][1].westWall = 1;
  cells[0][1].northWall = 1;
  cells[0][1].eastWall = 0;
  cells[0][1].southWall = 1;
  //3
  cells[0][2].westWall = 0;
  cells[0][2].northWall = 1;
  cells[0][2].eastWall = 0;
  cells[0][2].southWall = 0;
  //4
  cells[0][3].westWall = 0;
  cells[0][3].northWall = 1;
  cells[0][3].eastWall = 1;
  cells[0][3].southWall = 0;
  //5
  cells[1][0].westWall = 1;
  cells[1][0].northWall = 0;
  cells[1][0].eastWall = 0;
  cells[1][0].southWall = 0;
  //6
  cells[1][1].westWall = 0;
  cells[1][1].northWall = 1;
  cells[1][1].eastWall = 0;
  cells[1][1].southWall = 1;
  //7
  cells[1][2].westWall = 0;
  cells[1][2].northWall = 0;
  cells[1][2].eastWall = 1;
  cells[1][2].southWall = 1;
  //8
  cells[1][3].westWall = 1;
  cells[1][3].northWall = 0;
  cells[1][3].eastWall = 1;
  cells[1][3].southWall = 0;
  //9
  cells[2][0].westWall = 1;
  cells[2][0].northWall = 0;
  cells[2][0].eastWall = 0;
  cells[2][0].southWall = 0;
  //10
  cells[2][1].westWall = 0;
  cells[2][1].northWall = 1;
  cells[2][1].eastWall = 0;
  cells[2][1].southWall = 1;
  //11
  cells[2][2].westWall = 0;
  cells[2][2].northWall = 1;
  cells[2][2].eastWall = 0;
  cells[2][2].southWall = 1;
  //12
  cells[2][3].westWall = 0;
  cells[2][3].northWall = 0;
  cells[2][3].eastWall = 1;
  cells[2][3].southWall = 0;
  //13
  cells[3][0].westWall = 1;
  cells[3][0].northWall = 0;
  cells[3][0].eastWall = 0;
  cells[3][0].southWall = 1;
  //14
  cells[3][1].westWall = 0;
  cells[3][1].northWall = 1;
  cells[3][1].eastWall = 0;
  cells[3][1].southWall = 1;
  //15
  cells[3][2].westWall = 0;
  cells[3][2].northWall = 1;
  cells[3][2].eastWall = 1;
  cells[3][2].southWall = 1;
  //16
  cells[3][3].westWall = 1;
  cells[3][3].northWall = 0;
  cells[3][3].eastWall = 1;
  cells[3][3].southWall = 1;
}

