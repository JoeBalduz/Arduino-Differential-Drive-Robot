//The code below implements a stack
#include <Adafruit_RGBLCDShield.h>

//To save memory, the stack also gets used as a queue
struct myStack{
  byte stackArr[16];  //Max size of 16 since there are only 16 grid cells
  int top = -1;
  byte front = 0;   //Location of front of array
};

struct myStack stack;
int i;

//Pushes number onto the stack
void push(int number)
{
  if(stack.top == 15)
  {
    Serial.println("Stack is full");
  }
  else
  {
    stack.top++;
    stack.stackArr[stack.top] = number;
  }
}

//Pops an element off of the stack
int pop()
{
  int number;
  
  if(stack.top == -1)
  {
    Serial.println("Stack is empty");
    return -1;
  }
  else
  {
    number = stack.stackArr[stack.top];
    stack.top--;
    return number;
  }
}

//Returns whether the stack is empty
bool empty()
{
  if(stack.top == -1)
    return true;
  else
    return false;
}

//Returns the top of the stack
int top()
{
  return stack.stackArr[stack.top];
}

//Returns the front of the array
int front()
{
  return stack.stackArr[0];
}

//Queues an element onto the array
void queue(int number)
{
  for(i = 0; i < 16; i++)
  {
    if(stack.stackArr[i]==0)
    {
      stack.stackArr[i] = number;
      break;
    }
  }
}

//Dequeues an element in the array
void dequeue()
{
  for(i = 1; i<16; i++)
  {
    stack.stackArr[i-1] = stack.stackArr[i];
  }
}

//Empties the entire stack
void emptyStack()
{
  for(i = 0; i<16; i++)
  {
    stack.stackArr[i]=0;
  }
}  

//Sets the top of the stack to the first non-zero element
void setStackTop()
{
  for(i=0; i<16; i++)
  {
   Serial.println("");  //This print statement is required to get the loop to work correctly
   if(stack.stackArr[i]==0)
   {   
      stack.top = i-1;
      break;
   }
  }
}




