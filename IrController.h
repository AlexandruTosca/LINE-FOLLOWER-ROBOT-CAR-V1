#include <IRremote.h>
#define IRNUMRESTRICT 7
// restrict *,#,OK,arrows for the second command
char restrictions[IRNUMRESTRICT] = {0x16,0xD,0x1C,0x8,0x18,0x52,0x5A};

struct IRFLAGS{

char func='\0';
char command = '\0';
bool lockIR = 0;
bool restrictFlag = 0;
char lastCommand = '\0';
char lastFunc = '\0';

}IRF;


/*************************************************************************
/* Function Name: decode
**************************************************************************
* Summary: 
* implements a protocol for recieved commands
* Description: 
* Three consecutive commands are handled by the protocol  
* First command must be # for setups or * for routes
* Second command must be a digit
* Third command is a validation command and must be OK   
* If in a route (*) , the protocol is locked and can be unlock only by 
* the sequence (* 9 OK)
***************************************************************************/
void decode(char command){

        if (command == 0x16 || command == 0xD) { // * OR # (first command)
          IRF.lastCommand = command; //save the command as lastCommand
          IRF.lastFunc = '\0'; 
        }

        else if(IRF.lastCommand!='\0' && IRF.lastFunc == '\0'){ // second command
          IRF.restrictFlag = 0;

          for(int i=0;i<IRNUMRESTRICT;i++)
          if(command==restrictions[i]){
            IRF.restrictFlag = 1; //not a valid command 
            IRF.lastCommand = '\0';
          }

          if(!IRF.restrictFlag)
          IRF.lastFunc = command; //save the command as lastFunc 
          
        }

        else if((IRF.lastCommand==0x16 || IRF.lastCommand == 0xD) && command == 0x1C){ 
         //third command OK -> protocol validation
         if(IRF.lockIR == 0){ //if protocol is unlocked, assign commands
         IRF.func = IRF.lastFunc; 
         IRF.command = IRF.lastCommand;
         }
         if(IRF.lastCommand == 0x16 && IRF.lastFunc==0x9){ // * 9 ok combination unlocks the protocol
           IRF.lockIR = 0;
         }
         else if(IRF.lastCommand == 0x16){ //lock the protocol when in a route (*)
          IRF.lockIR = 1;
         }
         
        } //end protocol validation

} //end decode()


/*************************************************************************
* Function Name: resetIrStruct
**************************************************************************
* Summary: 
* Reset the flags
*************************************************************************/
void resetIrStruct(){
  IRF.lastCommand = '\0';
  IRF.lastFunc = '\0';
  IRF.command = IRF.lastCommand;
  IRF.func = IRF.lastFunc;
  IRF.lockIR = 0;
}
