#include "data.h"

  void Data::create(float gpsLongitude, float gpsLatitude, float gpsSpeed, int gpsUv, time_t t){
      this->longitude = gpsLongitude;
      this->latitude = gpsLatitude;
      this->speed = gpsSpeed;
      this->uv = gpsUv;
      this->unixDate = t;
  }
