#include "MyEncoders.h"
#include "robot.h"

String complete = "XXXXXXXXXXXXXXXX";   //Will be compared to the string visitedLocations
//The below four are used to make the robot enter the correct loops
bool first = true;
bool second = true;
bool third = true;
bool fourth = true;

bool broke = false; //If the robot makes a mistake while mapping, pressing the select button will make this true
bool load = true;   //Used to decide whether a map gets loaded in or the robot uses the map uses from mapping

//Functions for mapping and path planning
bool DFS();
bool BFS();
//Functions to setup location and orientation
void initDFS(); 
void initBFS();

//The below is all used for Chooser. It allows you to select things that are displayed on the LCD
unsigned int numPrograms = 2;
bool (*programList[])() = {&DFS, &BFS};
void (*setupProgramList[])() = {&initDFS,&initBFS};
const char * programNames[] {"Mapping", "Path Planning"};

byte startPoint = 1; //Robot starting point
byte endingPoint = 1; //Robot ending point
int popValue = 0;     //Popped value from stack

//Gives each cell their correct number
void fillCells()
{
  int i, j;
  byte cellNumber = 1;

  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      cells[i][j].cellNumber = cellNumber;
      cellNumber++;
    }
  }
}

void setup() {
  Serial.begin(115200);

  //The below connects the pins used in the color sensor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  //Sets frequency scaling of the color sensor
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  //Initializes the LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  initEncoders();
  LServo.attach(2);
  RServo.attach(3);

  //Loads in values to be able to set speeds correctly
  EEPROM.get(0, RPSMapLeft);
  EEPROM.get(sizeof(int) * 105, RPSMapRight);

  fillCells();

  initChooser(&lcd);
}

void loop() {
  execProgram();
}

//Mapping function
//Program will loop until true is returned
bool DFS(){
  getColor();
  int i, j;
  uint8_t buttons = lcd.readButtons();

  //Displays information on the screen the first time
  if(fourth){
    setSpeedsRPS(0, 0);
    updateDisplay();
    fourth = false;
  }

  //Starts the DFS
  if(first && second)
  {
    first = false;
    push(myRobot.cellLocation);   //Pushes the starting cell onto the stack
    updateCurrentCell();
    
    //DFS loop
    while(true){
      //This is only done at the first grid cell. It is done to check if there is a wall behind it since it has not sensor on the back
      if(!checkAllSide()){
        turnRobot();
        updateCurrentCell();
        updateDisplay();
      }

      //Every cell has been visited
      if(complete == visitedLocations)
      {
        setSpeedsIPS(0,0);
        clearScreen();
        lcd.setCursor(6,0);
        lcd.print("DONE");
        break;
      }

      //The below section is repeated four times, once for each orientation. I'm only commenting on this one since the rest are the same just a different orientation.
      //Checks if there is a wall in front of it and if it has been visited
      resetCounts();  //Tick counts are reset since doOne() will be called in the loop
      //Checks if there is a wall in front of the robot or if it is not a visited cell. If it's neither, the robot will move to that cell
      if(myRobot.orientation == 1 && cells[myRobot.x][myRobot.y].northWall == 0 && cells[myRobot.x - 1][myRobot.y].visited == false){
        while(true){
          //The robot will move forward until a blue or red is detected
          setSpeedsIPS(4, 4); 
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          //If the select button is pressed, the robot will stop what it's doing and display mapping information on the screen.
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          //Color has been detected
          if(getColor() == 2){
            myRobot.x = myRobot.x - 1;  //Change position of robot
            cells[myRobot.x][myRobot.y].southWall = 0;  //Since it came from the south, there can't be a wall behind it
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;  //Updates the robot's cell
            push(myRobot.cellLocation);   //Pushes the current cell onto the stack
            updateDisplay();  //Updates LCD
            //adjustment();
            travelToMid();    //Travels to middle of cell
            updateCurrentCell();    //Updates the cell's walls
            break;  //Breaks out of while loop and will repeat
          } 
          delay(60);   //Delay to get the sensors to work correctly
          doOne();    //Make an adjustment        
        } 
      }
      else if(myRobot.orientation == 2 && cells[myRobot.x][myRobot.y].eastWall == 0 && cells[myRobot.x][myRobot.y + 1].visited == false){
        while(true){
          setSpeedsIPS(4, 4);
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.y = myRobot.y + 1;
            cells[myRobot.x][myRobot.y].westWall = 0;
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
            push(myRobot.cellLocation);
            updateDisplay();
            //adjustment();
            travelToMid();
            updateCurrentCell(); 
            break;
          }
          delay(60);
          doOne();
        }
      }
      else if(myRobot.orientation == 3 && cells[myRobot.x][myRobot.y].southWall == 0 && cells[myRobot.x + 1][myRobot.y].visited == false){
        while(true){
          setSpeedsIPS(4, 4);
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.x = myRobot.x + 1;
            cells[myRobot.x][myRobot.y].northWall = 0;
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
            push(myRobot.cellLocation);
            updateDisplay();
            //adjustment();
            travelToMid();
            updateCurrentCell(); 
            break;
          }
          delay(60);
          doOne();
        }
      }
      else if(myRobot.orientation == 0 && cells[myRobot.x][myRobot.y].westWall == 0 && cells[myRobot.x][myRobot.y - 1].visited == false){
        while(true){
          setSpeedsIPS(4, 4);
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.y = myRobot.y - 1;
            cells[myRobot.x][myRobot.y].eastWall = 0;
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
            push(myRobot.cellLocation);
            updateDisplay();
            //adjustment();
            travelToMid();
            updateCurrentCell(); 
            break;
          }
          delay(60);
          doOne();
        }
      }
     //Every thing around the current cell is a wall or has been visited. This means backtracking must be done
      else if(allWallsOrVisited()){
        pop();  //Pops current cell
        while(true){
          if(broke)
            break;
          //Like previously, I will only comment on the first one of these since the other 3 are the same just for different orientatitons. It is also
          //similar to previous part as well, so not everything will be commented.
          //Checks if the cell in front of the robot is what is on top of the stack. If it is, travel to it
          if(myRobot.orientation == 1 && cells[myRobot.x][myRobot.y].northWall == 0 && cells[myRobot.x - 1][myRobot.y].cellNumber == top()){
            while(true){
              //Moves the robot forward until red or blue is detected
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              //If the select button is pressed, the robot will stop what it's doing and display mapping information on the screen.   
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              }                                     
              if(getColor() == 2){
                myRobot.x = myRobot.x - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                travelToMid();
                delay(100); //Done to just make the robot stop for a while once it makes it to the next cell. This isn't really needed
                break;
              }
              delay(60);  //Done to get the sensors to work correctly
              doOne();           
            }
            break; 
          }
          else if(myRobot.orientation == 2 && cells[myRobot.x][myRobot.y].eastWall == 0 && cells[myRobot.x][myRobot.y + 1].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              } 
              if(getColor() == 2){
                myRobot.y = myRobot.y + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                travelToMid(); 
                delay(100);
                break;
              }
              delay(60);
              doOne();
            }
            break;
          }
          else if(myRobot.orientation == 3 && cells[myRobot.x][myRobot.y].southWall == 0 && cells[myRobot.x + 1][myRobot.y].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              } 
              if(getColor() == 2){
                myRobot.x = myRobot.x + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                travelToMid();
                delay(100);
                break;
              }
              delay(60);
              doOne();
            }
            break;
          }
          else if(myRobot.orientation == 0 && cells[myRobot.x][myRobot.y].westWall == 0 && cells[myRobot.x][myRobot.y - 1].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              } 
              if(getColor() == 2){
                myRobot.y = myRobot.y - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                travelToMid();
                delay(100);
                break;
              }
              delay(60);
              doOne();
            }
            break;
          }
          //The cell in front is not on top of the stack
          else{
            turnRobot();
            updateDisplay();
          }
        }
      }
      else{
        //Since there is a wall or the cell in front is visited, the robot needs to turn
        turnRobot();
        updateDisplay();
      }
      uint8_t buttons = lcd.readButtons();
      //Ends the program if select is prerssed. Mapping information will displayed on the LCD
      if(buttons & BUTTON_SELECT || broke)
        break;
    }
  }
  //Mapping has been completed or the select button was previously pressed
  else if(buttons & BUTTON_SELECT && !first && second){
        clearScreen();
        lcd.setCursor(0,0);
        second = false;
        
        //Prints out if there is a wall or not for the first half of the cells
        //W for a wall, o for no wall
        for(i = 0; i < 2; i++)
        {
          for(j = 0; j < 4; j++)
          {
            if(cells[i][j].westWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].northWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].eastWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].southWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
          }
          lcd.setCursor(0,1);
        }
        delay(500); //Done so that the robot doesn't read multiple select button presses
  }
  //Since not all the mapping information can fit on the LCD at one, the select button must be pressed
  //a second time to show all of the information
  else if(buttons & BUTTON_SELECT && !first && !second){
    clearScreen();
    lcd.setCursor(0,0);
    for(i = 2; i < 4; i++)
        {
          for(j = 0; j < 4; j++)
          {
            if(cells[i][j].westWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].northWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].eastWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].southWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
          }
          lcd.setCursor(0,1);
        }
        first = true;
  }
  //End of the program. Returns back to the selection screen
  else if(buttons & BUTTON_SELECT && first && !second){
    clearScreen();
    return true;
  }
  delay(300);

  return false;
}

void initDFS(){
  load = false;
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Location");

  //Allows you to choose the starting location of the robot
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    lcd.print(myRobot.cellLocation);
    if(buttons & BUTTON_RIGHT){
      myRobot.cellLocation++;
      if(myRobot.cellLocation > 16)
        myRobot.cellLocation = 16;
    }
    if(buttons & BUTTON_LEFT){
      myRobot.cellLocation--;
      if(myRobot.cellLocation < 1)
        myRobot.cellLocation = 1;
    }
    if(buttons & BUTTON_SELECT)
      break;
  }
  setRobotCoordinates();
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Orientation");

  //Allows you to set the orientation of the robot
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    printOrientation();
    if(buttons & BUTTON_RIGHT){
      myRobot.orientation++;
      if(myRobot.orientation > 3)
        myRobot.orientation = 3;
    }
    if(buttons & BUTTON_LEFT){
      myRobot.orientation--;
      if(myRobot.orientation < 0)
        myRobot.orientation = 0;
    }
    if(buttons & BUTTON_SELECT)
      break;    
  }
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Exit");
  //Begins the mapping program once the select button is pressed
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    if(buttons & BUTTON_SELECT)
      break;
  }
}

//Path planning function
//Program will loop until true is returned
bool BFS(){
  getColor();
  emptyStack(); //Stack must be emptied so that the array used for the stack can be used here 
  fillBFS();
  uint8_t buttons = lcd.readButtons();

  while(true)
  {
  int current = startPoint;   //Makes the current location the starting point
  bfs[current].vistedBFS = true;
  bfs[current].prev = 0;    //Since the first location has no previous, it is set to 0
  queue(current);

  //This loop gives the shortest path
  //It takes the previous of the current point and adds it to the queue. At the end,
  //The shortest path will be what is in the queue. This loop happens once current is the end point
  while(current!=0 && third)
  {
    current = front();
    dequeue();
    
    if(current==endingPoint)
    {
      emptyStack();
      while(current != 0)
      {
        queue(current);
        current = bfs[current].prev;
      }
      break;
    }

    //The below code checks all of the cells around the current cell. If it there is no wall and it hasn't been visited,
    //it will be added to the queue
    if(bfs[current].southWall == 0 && bfs[current+4].vistedBFS==false)
    {
      queue(current+4);
      bfs[current+4].prev = current;
      bfs[current+4].vistedBFS = true;
    }
    if(bfs[current].westWall == 0 && bfs[current-1].vistedBFS==false)
    {
      queue(current-1);
      bfs[current-1].prev = current;
      bfs[current-1].vistedBFS=true;
    }
    if(bfs[current].northWall == 0 && bfs[current-4].vistedBFS==false)
    {
      queue(current-4);
      bfs[current-4].prev = current;
      bfs[current-4].vistedBFS=true;
    }
    if(bfs[current].eastWall == 0 && bfs[current+1].vistedBFS==false)
    {
      queue(current+1);
      bfs[current+1].prev = current;
      bfs[current+1].vistedBFS=true;
    }
  }
  third = false;
  
  myRobot.cellLocation = startPoint;  //Updates the robot's location
  setRobotCoordinates();

  setStackTop();
  popValue = pop();
  current = top();

  //This loop makes the robot follow the path that was previously found
  while(popValue!=-1)
  {
    //Robot has made it to the endpoint
    if(myRobot.cellLocation==endingPoint)
    {
      setSpeedsIPS(0,0);
      return true;
    }
    //The code below makes the robot check its surrounding cells until it finds the cell that is on top of the stack. It will
    //then move to it. Only the first one of these is commented since they are all the same just for different orientations
    if(myRobot.orientation == 1 && cells[myRobot.x - 1][myRobot.y].cellNumber == current){
            while(true){
              //Makes the robot move forward until red or blue is detected
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              if(getColor() == 2){
                myRobot.x = myRobot.x - 1;  //Updates the robot's coordinates
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;  //Updates the robot's location
                updateDisplay();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);  //Needed to get the sensors to work correctly
              doOne();    //An adjustment       
            } 
          }
          else if(myRobot.orientation == 2 && cells[myRobot.x][myRobot.y + 1].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
             
              if(getColor() == 2){
                myRobot.y = myRobot.y + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();
            }
          }
          else if(myRobot.orientation == 3 && cells[myRobot.x + 1][myRobot.y].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
             
              if(getColor() == 2){
                myRobot.x = myRobot.x + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();
            }
          }
          else if(myRobot.orientation == 0 && cells[myRobot.x][myRobot.y - 1].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
             
              if(getColor() == 2){
                myRobot.y = myRobot.y - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();
            }
          }
          //Robot must turn since the cell in front of it is not the one that is on top of the stack
          else{
            turnRobot();
            updateDisplay();
          }    
    }
  }
  setSpeedsIPS(0, 0);
  return true;
}

void initBFS(){
  third = true;

  //If mapping was not done before hand, a map will be loaded in
  if(load){
    loadMap();
    Serial.println("Loaded");
  }
  
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Starting Location");

  //Sets the starting location of the robot
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    lcd.print(startPoint);
    if(buttons & BUTTON_RIGHT){
      startPoint++;
      if(startPoint > 16)
        startPoint = 16;
    }
    if(buttons & BUTTON_LEFT){
      startPoint--;
      if(startPoint < 1)
        startPoint = 1;
    }
    if(buttons & BUTTON_SELECT)
      break;
  }

  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Ending Location");

  //Sets the ending location of the robot
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    lcd.print(endingPoint);
    if(buttons & BUTTON_RIGHT){
      endingPoint++;
      if(endingPoint > 16)
        endingPoint = 16;
    }
    if(buttons & BUTTON_LEFT){
      endingPoint--;
      if(endingPoint < 1)
        endingPoint = 1;
    }
    if(buttons & BUTTON_SELECT)
      break;
  }
  
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Orientation");

  //Sets the orientation of the robot
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    printOrientation();
    if(buttons & BUTTON_RIGHT){
      myRobot.orientation++;
      if(myRobot.orientation > 3)
        myRobot.orientation = 3;
    }
    if(buttons & BUTTON_LEFT){
      myRobot.orientation--;
      if(myRobot.orientation < 0)
        myRobot.orientation = 0;
    }
    
    if(buttons & BUTTON_SELECT)
      break;    
  }
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Exit");
  //Once select is pressed, the program will begin
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    if(buttons & BUTTON_SELECT)
      break;
  }
}

