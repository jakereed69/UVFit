#ifndef DATA_H
#define DATA_H

#include <time.h>

class Data{
    
private:
  float speed;
  float longitude;
  float latitude;
  int uv;
  time_t unixDate;
public:
  Data(){ speed, longitude, latitude, uv = 0; };
  void create(float gpsLongitude, float gpsLatitude, float gpsSpeed, int gpsUv, time_t t);
  float lon(){ return this->longitude; };
  float lat(){ return this->latitude; };
  float spd(){return this->speed; };
  float uvr(){ return this->uv; };
  time_t t(){ return this->unixDate; }
};

#endif