#include <Adafruit_NeoPixel.h>

#define PIN 1 
#define NUMPIXELS 8 

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
unsigned char lastColor;

/*************************************************************************
/* Function Name: pixelsBandSetup
**************************************************************************
* Summary: 
* Setting up the pixels band object
* Clearing the pixels band
***************************************************************************/
void pixelsBandSetup() {

pixels.setBrightness(200); //0 to 255
pixels.begin(); // INITIALIZE NeoPixel strip object
pixels.clear();

}

/*************************************************************************
/* Function Name: setRgbPixelsBandColor
**************************************************************************
* Summary: 
* Set the strip neopixels color with given parameters
*
* Parameters:
* R = Red, G = green, B = Blue 
***************************************************************************/
void setRgbPixelsBandColor(uint8_t r, uint8_t g, uint8_t b){

    pixels.clear();
    for(int i=0; i<NUMPIXELS; i++) { 
    pixels.setPixelColor(i, r,g,b);  //blue
    pixels.show(); 
    }

}

/*************************************************************************
/* Function Name: pixelsBandColor
**************************************************************************
* Summary: 
* Call the setRgbPixelsBandColor function depending on letterToColor parameter 
* Save the color for ultrasonic interrupt
* Parameters:
* letterToColor: 'A' = Blue, 'B' = green, 'C' = yellow, 'F' = White 
***************************************************************************/
void pixelsBandColor(char letterToColor){

switch(letterToColor){

case 'A':
      setRgbPixelsBandColor(0, 0, 255); //blue
break;

case 'B':
      setRgbPixelsBandColor(0,128,0);  //green
break;

case 'C':
      setRgbPixelsBandColor(255,255,0);  //yellow
break;

case 'F': 
      setRgbPixelsBandColor(255,255,255);  //white
break;

}

lastColor = letterToColor;  

}

/*************************************************************************
/* Function Name: lastPixelBandColor
**************************************************************************
* Summary: 
* Return the lastColor saved in pixelsBandColor
*
***************************************************************************/

unsigned char lastPixelsBandColor(){

return lastColor;

  
}

/*************************************************************************
/* Function Name: pixelsBandAllOff
**************************************************************************
* Summary: 
* Turn off all the neopixels strip leds
*
***************************************************************************/
void pixelsBandAllOff() { 
  pixels.clear();
  for(int i=0; i<NUMPIXELS; i++){
   pixels.setPixelColor(i, 0,0,0);
   pixels.show();
}
}
