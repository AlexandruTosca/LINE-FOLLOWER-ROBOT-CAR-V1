#include <MaxMatrix.h>
#include <avr/pgmspace.h>
#include "graphics.h"
            
#define CLK 12
#define CS  11
#define DIN 10
#define MAX_IN_USE 1

MaxMatrix m(DIN, CS, CLK, MAX_IN_USE);

extern PROGMEM const unsigned char CH[];
extern const unsigned char arrowA[];
extern const unsigned char arrowB[];
extern const unsigned char arrowC[];
extern struct MESSAGES MESSAGE;

byte buffer[10];
bool textOneTime = 0; //show the text one time only

/*************************************************************************
* Function Name: printChar
**************************************************************************
* Summary: 
* to print a char
*************************************************************************/
void printChar(char c, unsigned int ms) {
  if (c < 32) return;
  c -= 32;
  memcpy_P(buffer, CH + 7 * c, 7);
  m.writeSprite(2, 0, buffer);
  delay(ms);
  m.clear();

}


/*************************************************************************
* Function Name: printCharWithShift
**************************************************************************
* Summary: 
* Displays the extracted characters with scrolling
*************************************************************************/
void printCharWithShift(char c, unsigned int shift_speed) {
  if (c < 32) return;
  c -= 32;
  memcpy_P(buffer, CH + 7 * c, 7);
  m.writeSprite(32, 0, buffer);
  m.setColumn(32 + buffer[0], 0);

  for (int i = 0; i < buffer[0] + 1; i++)
  {
    delay(shift_speed);
    m.shiftLeft(false,false);
    
 
  }
}


/*************************************************************************
* Function Name: printStringWithShift
**************************************************************************
* Summary: 
* Extracts characters from the string
*************************************************************************/
void printStringWithShift(char* s, unsigned int shift_speed) {
  while (*s != 0) {
    printCharWithShift(*s, shift_speed);
    s++;
  }
 
}


/*************************************************************************
/* Function Name: ledMatrixSetup
**************************************************************************
* Summary: 
* Setting up the led matrix
* Welcome message display
***************************************************************************/
void ledMatrixSetup(){

  m.init(); 
  m.setIntensity(7); // dot matix intensity 0-15
  m.clear();

  delay(1000);
  printStringWithShift(MESSAGE.firstMess, 60); //welcome message
  m.clear();

}

/*************************************************************************
/* Function Name: printGraph
**************************************************************************
* Summary: 
* Handle custom graphics printing
***************************************************************************/
void printGraph(char letter){

switch(letter){

case 0x41: //hex A
          m.writeSprite(2, 0, arrowA);
          break;
case 0x42: //hex B
          m.writeSprite(2, 0, arrowB);
          break;
case 0x43: //hex C
          m.writeSprite(2, 0, arrowC);
          break;
case 0x46: //hex F
          m.writeSprite(1, 0, finishLine);

}
}

/*************************************************************************
/* Function Name: displayStartMessage
**************************************************************************
* Summary: 
* Start message display
***************************************************************************/
void displayStartMessage(uint8_t sequence){

switch(sequence){
case 1:
      printStringWithShift(MESSAGE.abc, 60);
      break;
case 2:
      printStringWithShift(MESSAGE.acb, 60);
      break;
case 3:
      printStringWithShift(MESSAGE.bca, 60);
      break;
}
m.clear();
printChar('3',700);
printChar('2',700);
printChar('1',700);
printStringWithShift(MESSAGE.go, 50);
m.clear();
if(sequence==3)
printGraph('B');
else
printGraph('A');


}

/*************************************************************************
/* Function Name: startCalibrationMessage
**************************************************************************
* Summary: 
* Calibration message
***************************************************************************/
void startCalibrationMessage(){
  printStringWithShift(MESSAGE.startCalib, 50);
  m.clear();
}

/*************************************************************************
/* Function Name: testingMotorMessage
**************************************************************************
* Summary: 
* Testing motor message
***************************************************************************/
void testingMotorsMessage(){
  if(!textOneTime){
  printStringWithShift(MESSAGE.textTestMotors, 50);
  m.clear();
  textOneTime = 1;
  }
}