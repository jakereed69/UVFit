#ifndef ACTIVITY_H
#define ACTIVITY_H
#include "data.h"
#include "Particle.h"
#include <queue>

class activity{
    
private:
 int activityId = 0;
 int pauseCount = 0;
 std::vector<Data> data ;
 
public:
  activity();
  int getId(){ return this->activityId; };
  void addData(Data *newData){ this->data.push_back(*newData); };
  std::vector<Data>* getData(){ return &this->data; };
  int getPauseCount(){ return this->pauseCount; };
  void incPauseCount(){ this->pauseCount++; };
  void resetPauseCount(){ this->pauseCount = 0; };
};

#endif