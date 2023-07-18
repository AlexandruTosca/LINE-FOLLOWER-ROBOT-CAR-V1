#define TRIG_PIN 7
#define ECHO_PIN 6
#define BUZZ_PIN 3

struct ULTRA_SONIC{

  const uint8_t trigPin=7;
  const uint8_t echoPin=6;
  const uint8_t buzzPin=3;

}US;

/*************************************************************************
/* Function Name: ultraSonicSetup
**************************************************************************
* Summary: 
* Setting up echo and trigger pins for UltraSonic
* Setting up buzzer pin for Buzzer
* 
* Parameters:
* none
* 
* Returns:
*  none
***************************************************************************/
void ultraSonicSetup(){

  pinMode(US.buzzPin,OUTPUT);
  pinMode(US.trigPin,OUTPUT);
  pinMode(US.echoPin,INPUT);

  digitalWrite(US.buzzPin,HIGH);

}