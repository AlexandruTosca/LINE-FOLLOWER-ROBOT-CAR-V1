#include <QTRSensors.h>
#include "UltraSonicController.h"
#define NUM_SENSORS   6     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define LM1A 5 //left motor forward
#define LM1B 4 //left motor back
#define RM2A 9 //right motor forward
#define RM2B 8 //right motor back
#define LAST 5000
#define LED_PIN 13

QTRSensorsRC qtrrc((unsigned char[]) {A0, A1, A2, A3, A4, A5},NUM_SENSORS,TIMEOUT);
uint16_t sensorValues[NUM_SENSORS];

extern struct ULTRA_SONIC US;

/*************************************************************************
* PID control system variables 
*************************************************************************/
struct PID_VARIABLES{
float Kp = 0.09; 
float Ki = 0.0000; 
float Kd = 0.4; 
int16_t P;
int16_t I;
int16_t D;

int16_t lastError = 0;


}PIDV;


/*************************************************************************
* Motor speed variables (choose between 0 - no speed, and 255 - maximum speed)
*************************************************************************/
const uint8_t maxSpeedL = 180; 
const uint8_t maxSpeedR = 180; 
const uint8_t baseSpeedL = 160; 
const uint8_t baseSpeedR = 160; 

/*************************************************************************
* Global variables
*************************************************************************/
uint8_t lastIntersect=0;
bool extStarted = 0;
uint16_t decisionTime = 0;
bool finish = 0;
bool objectDetected = 0;
unsigned char lastBandColor = '\0';

void endOfCircuit(bool);

/*************************************************************************
/* Function Name: carControllerSetup
**************************************************************************
* Summary: 
* Setting up LED_PIN and motors pins. 
* ultraSonicSetup() is called here because we need it here, not in the loop. 
***************************************************************************/
void carControllerSetup(){

pinMode(LED_PIN,OUTPUT);

pinMode(LM1A,OUTPUT);
pinMode(LM1B,OUTPUT);  
pinMode(RM2A,OUTPUT);
pinMode(RM2B,OUTPUT);

ultraSonicSetup();

}


/*************************************************************************
* Function Name: directionControl
**************************************************************************
* Summary: Controls the speed of each other motor
*************************************************************************/
void directionControl(uint16_t leftMotorSpeed, uint16_t rightMotorSpeed) {
  //set the appropriate values for aphase and bphase so that the robot goes straight
  analogWrite(LM1A, leftMotorSpeed);
  analogWrite(RM2A, rightMotorSpeed);

  digitalWrite(LM1B, LOW);
  digitalWrite(RM2B, LOW);

}


/*************************************************************************
* Function Name: testMotors
**************************************************************************
* Summary: Read line follower sensors values and sets the speed of each motor
*************************************************************************/
void testMotors(){

uint16_t position = qtrrc.readLine(sensorValues);

if(sensorValues[4]>500 && sensorValues[5]>500){
  directionControl(maxSpeedL,0);
} 
else if (sensorValues[2]>500 && sensorValues[3]>500){
  directionControl(maxSpeedL,maxSpeedR);
}
else if (sensorValues[0]>500 && sensorValues[1]>500){
  directionControl(0,maxSpeedR);
}
else {
  directionControl(0,0);
}

} //end test motors

/*************************************************************************
/* Function Name: PIDController
**************************************************************************
* Summary: 
* correction to be applied of the speed of the motors after the PID algorithm(proportional, integral and derivate) 
* route decision (1, 2 or 3)
* intersection decision
* white led handle
* led matrix destinations graphics assignment 
* neopixels destination colors assignment
* obstacles avoiding handle 
* Input Parameters:
* sequence : 1 for ABC, 2 for ACB, 3 for BCA
***************************************************************************/
void PIDController(char sequence) {

  digitalWrite(LED_PIN,LOW);

  //ultra sonic trigger sequrence
  digitalWrite(US.trigPin,LOW);
  delayMicroseconds(5);  
  digitalWrite(US.trigPin,HIGH);
  uint16_t position = qtrrc.readLine(sensorValues); //read sensor line position
  digitalWrite(US.trigPin,LOW);

  long duration = pulseIn(US.echoPin,HIGH); //reading echo
  int cm = duration/2*0.0343;

  int16_t error = 2500 - position; //2500 is the center of the line

    PIDV.P = error;
    PIDV.I = PIDV.I + error;
    PIDV.D = error - PIDV.lastError;
    PIDV.lastError = error;
    int16_t correction = PIDV.P*PIDV.Kp + PIDV.I*PIDV.Ki + PIDV.D*PIDV.Kd; //calculate the correction
    
  int16_t motorSpeedL = baseSpeedL + correction;
  int16_t motorSpeedR = baseSpeedR - correction;

  if(sensorValues[1]>500 && sensorValues[2]>500 && sensorValues[3]>500&&sensorValues[4]>500){
    
  digitalWrite(LED_PIN,HIGH);

if(sequence=='1') //abc
    {
    uint8_t intersect = lastIntersect+1;
    lastIntersect = intersect;

      switch(intersect){
      
        case 1:  //turn right
        motorSpeedR = 0;
        motorSpeedL = maxSpeedL;
        decisionTime = 800;
        
        break;

         case 2: //forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        m.clear();
        printGraph('B');
        pixelsBandColor('B');
        break;

        case 3: //forward      
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        m.clear();
        printGraph('C'); 
        pixelsBandColor('C');
       
        break;

        case 4: //forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        m.clear();
        printGraph('F');
        pixelsBandColor('F');
      
        break;

        case 5: //turn right
        motorSpeedR = 0;      
        motorSpeedL = maxSpeedL;
        decisionTime = 800;
        break;

        case 6: //stop
        motorSpeedL = 0;
        motorSpeedR = 0;
        decisionTime = 0;
        finish = 1;

        break;

      }
  } //end if 'abc'

else if(sequence=='2') //acb
    {
    uint8_t intersect = lastIntersect+1;
    lastIntersect = intersect;

      switch(intersect){

        case 1:  //turn right
        motorSpeedR = 0;
        motorSpeedL = maxSpeedL;
        decisionTime = 800;
        break;

        case 2: //turn left
        motorSpeedL = 0;
        motorSpeedR = maxSpeedR;
        decisionTime= 1000;
        m.clear();
        printGraph('C');
        pixelsBandColor('C');
        
        break;

        case 3: //go forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        break;

        case 4: //turn right
        motorSpeedR = 0;      
        motorSpeedL = maxSpeedL;
        decisionTime = 800;
        m.clear();
        printGraph('B');
        pixelsBandColor('B');
        break;

        case 5: //turn right
        motorSpeedR = 0;      
        motorSpeedL = maxSpeedL;
        decisionTime = 700;
        m.clear();
        printGraph('F');
        pixelsBandColor('F');
        break;
    
        case 6: //go forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        break;

        case 7: //go forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        break;
         case 8: //stop
        motorSpeedL = 0;
        motorSpeedR = 0;
        decisionTime =0;
        finish = 1;
        break;

      }
    
  } //end if 'acb'

  else if(sequence=='3') //bca
    {
    uint8_t intersect = lastIntersect+1;
    lastIntersect = intersect;
      
      switch(intersect){

        case 1: //go forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        break;

        case 2: //go forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        break;

        case 3: 
        motorSpeedL = 0; //turn left 
        motorSpeedR = maxSpeedR;
        decisionTime= 1000;
        m.clear();
        printGraph('C');
        pixelsBandColor('C');
        break;

        case 4: 
        motorSpeedL = 0; //turn left 
        motorSpeedR = maxSpeedR;
        decisionTime= 1000;
        m.clear();
        printGraph('A');
        pixelsBandColor('A');
        break;

        case 5: //go forward
        motorSpeedL = maxSpeedL;
        motorSpeedR = maxSpeedR;
        decisionTime = 500;
        break;

        case 6: //turn right
        motorSpeedR = 0;      
        motorSpeedL = maxSpeedL;
        decisionTime = 800;
        m.clear();
        printGraph('F');
        pixelsBandColor('F');
        break;
    
        case 7: //turn left  
        motorSpeedL = 0;      
        motorSpeedR = maxSpeedR;
        decisionTime = 1000;
        break;

        case 8: //stop
        motorSpeedL = 0;
        motorSpeedR = 0;
        decisionTime =0;
        finish = 1;
        break;

      }
    
  } //end if bca

  }//end decision block

 
  if (motorSpeedL > maxSpeedL) {
    motorSpeedL = maxSpeedL;
  }
  if (motorSpeedR > maxSpeedR) {
    motorSpeedR = maxSpeedR;
  }
  if (motorSpeedL < 0) {
    motorSpeedL = 0;
  }
  if (motorSpeedR < 0) {
    motorSpeedR = 0;
  }


if(cm<=10 && cm>0){
if(!objectDetected){
  lastBandColor = lastPixelsBandColor();
  objectDetected = 1;
}
  digitalWrite(US.buzzPin,LOW);
  pixelsBandAllOff();
  delay(50);
  digitalWrite(US.buzzPin,HIGH);
  pixelsBandColor(lastBandColor);
  delay(50);
  directionControl(0, 0);
  
} else {

if(objectDetected){
pixelsBandColor(lastBandColor);
objectDetected = 0;
lastBandColor = '\0';
}
digitalWrite(US.buzzPin,HIGH);
directionControl(motorSpeedL, motorSpeedR);
delay(decisionTime);
decisionTime = 0;

if(finish==1){
         endOfCircuit(1);
} 


} //end else buzzpin 

} //end PIDController


/*************************************************************************
* Function Name: lfCalibration
**************************************************************************
* Summary: 
* Handle line follower calibration
* The led is on during the calibration
*************************************************************************/
void lfCalibration(){

delay(2000);
digitalWrite(LED_PIN, HIGH); 
uint16_t i;
    
  for (i = 0; i < 400; i++)  // make the calibration take about 10 seconds i=400
  {
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  digitalWrite(LED_PIN, LOW);     // turn off LED to indicate we are through with calibration
  delay(1000);


}

/*************************************************************************
* Function Name: endOfCircuit
**************************************************************************
* Summary: 
* Print the finish message
* Turn off led and neopixels strip
* reset flags
*************************************************************************/
void endOfCircuit(bool hasMessage){
          m.clear();
          if(hasMessage){
          printStringWithShift(MESSAGE.end, 60); //print end message
          m.clear();
          }
          digitalWrite(LED_PIN,LOW);
          pixelsBandAllOff();
          resetIrStruct();
          lastIntersect = 0;
          finish = 0;
          extStarted = 0;
}