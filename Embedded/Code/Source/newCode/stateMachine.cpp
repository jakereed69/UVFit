// This file defines functions used for handling the state machine and flow
// of logic throughout the uvFit firmware

#include "stateMachineHandler.h"
#include "Arduino.h"

//---------------------------------------------------------------------------
// Inturrupt calls this function we debounce the button here
void stateHandler::buttonPressed(){

 if(this->buttonState == notPressed){
        this->buttonState = debouncePressed;
    }
    else if(this->buttonState == pressed){
        this->buttonState = debounceNotPressed;
    }
}

//---------------------------------------------------------------------------
// To be called in the main loop so we can properly debounce the button
// We also change the led colors here
void stateHandler::checkButton(){
 switch(this->buttonState){
  case pressed:
      this->red = 0;
      this->green = 0;
      this->blue = 255;
      this->activityState = recordActivity;
      break;
      
  case notPressed:
      this->red = 255;
      this->green = 0;
      this->blue = 0;  
      break;
    
  // Start activity here    
  case debouncePressed:
      this->red = 255;
      this->green = 0;
      this->blue = 0;
      delay(200);
      this->buttonState = pressed;
      this->activityState = startActivity;
      break;
  
  // Stop activity here
  case debounceNotPressed:
      this->red = 0;
      this->green = 0;
      this->blue = 255;
      delay(200);
      this->buttonState = notPressed;
      this->activityState = endActivity;
      break;
  }
  
    
}
//---------------------------------------------------------------------------
