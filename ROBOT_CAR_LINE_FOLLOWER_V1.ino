#include "IrController.h"
#include "LedMatrix.h"
#include "PixelBand.h"
#include "CarController.h"

//IR MODULE
extern struct IRFLAGS IRF;
int REVC_PIN = 2;
IRrecv irrecv(REVC_PIN);
decode_results results;

//Other variables
extern bool extStarted;

/*************************************************************************
/* Function Name: setup
**************************************************************************
* Summary: 
* All Controllers Setup functions are called here 
***************************************************************************/
void setup() {

carControllerSetup();

ledMatrixSetup();

pixelsBandSetup();

irrecv.enableIRIn(); //enable infrared module

}

/*************************************************************************
/* Function Name: loop
**************************************************************************
* Summary: 
* Listen for IR command
* decode IR commands using the pattern implemented in decode(...) function from"IrController.h"
* we can have two types patterns: a setup pattern that begins with (#) and
* a route pattern that begin with(*). All paterns end with (OK) command.
* Setups include: Calibration (1), Testing motors (2), Stop with reset parameters (3)
* Route functions include: ABC (1), ACB(2), BCA(3), Pause (9), Stop and reset parameters (0) 
* PIDController(...) function is called after route selection. This function controls the car to
* keep the route.
* directionControlFunction(...) has its scope to set the speed for each motor
***************************************************************************/
void loop() {

if (IrReceiver.decode()) {

decode(IrReceiver.decodedIRData.command);
IrReceiver.resume(); // Enable receiving of the next value
  
} // end if reciever decode()

if(IRF.command==0xD){ //setups
      switch(IRF.func){
        case 0x45: // calibration on 1
                  startCalibrationMessage();
                  lfCalibration();
                  resetIrStruct();
        break;

        case 0x46: //testing motors on 2
                  testingMotorsMessage();
                  testMotors();
        break;
      
        case 0x19: //Stop and Reset parameters
                  directionControl(0,0);
                  resetIrStruct();
        break;
        
    } //end switch

} //end if decode setups

else if(IRF.command==0x16){ // * road functions
      switch(IRF.func){
        case 0x45:
                  if(extStarted==0){ //only first entry
                  displayStartMessage(1); // 1 = abc
                  pixelsBandColor('A');
                  extStarted = 1;
                  }
                  
                  PIDController('1');
        break;

        case 0x46:
                  if(extStarted==0){ //only first entry
                  displayStartMessage(2); // 2 = acb
                  pixelsBandColor('A');
                  extStarted = 1;
                  }
                  
                  PIDController('2');
        break;

        case 0x47:
                  if(extStarted==0){
                  displayStartMessage(3); // 3 = bca
                  pixelsBandColor('C'); //green
                  extStarted = 1;
                  }

                  PIDController('3');
        break;
        
        case 0x9: 
                  directionControl(0,0); //pause
                  IRF.lockIR = 0; //unlock receiving commands

        break;

        case 0x19: //stop and reset parameters
                  endOfCircuit(0); //end with no message
        break;
    } //end switch

} //end if decode functions


} //end loop()

