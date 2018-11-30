#include "activity.h"

// Assign a unique id to each activity forever!
activity::activity(){
  int id = 0;
  EEPROM.get(0, id);
  this->activityId = id + 1;
  EEPROM.put(0, this->activityId);
}
