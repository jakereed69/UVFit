#ifndef STATEMACHINEHANDLER_H
#define STATEMACHINEHANDLER_H

class stateHandler {
enum button {notPressed, pressed, debouncePressed, debounceNotPressed};
enum activity {wait, startActivity, recordActivity, endActivity};

private:
  
  button buttonState;
  activity activityState;

public: 
  // We initialize in a wait state with no led on
  stateHandler(){ this->buttonState = notPressed; red, green, blue = 0; activityState = wait; };
  button getButtonState(){ return this->buttonState; };
  int getActivityStatus(){ return this->activityState; };
  void buttonPressed();
  void checkButton();
  void endAct(){ this->buttonState = debounceNotPressed; };
  
  // LED colors public so its easy to access in main loop
  int red;
  int green;
  int blue;

};
#endif