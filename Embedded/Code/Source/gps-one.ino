// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_VEML6070.h>
#include <string>
// This #include statement was automatically added by the Spark IDE.
#include "Adafruit_GPS.h"
#include <math.h>

#define mySerial Serial1
Adafruit_GPS GPS(&mySerial);
  
   

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  true

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;

#define APP_VERSION 10

#define UPDATE_RATE 5000 // Update rate in milliseconds
Adafruit_VEML6070 uvSensor;

byte bufferSize = 64;
byte bufferIndex = 0;
char buffer[65];
char c;

uint32_t timer;

void setup() {
  // Initialize UV sensor
  uvSensor.begin(VEML6070_1_T);

  // Initialize GPS ssensor and serial    
  GPS.begin(9600);
  mySerial.begin(9600);
  Serial.begin(9600);

  // Tell GPS to only send the RMC data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  delay(1000);
 
  timer = millis();

  // Set up webhooks
  Particle.subscribe("hook-response/activity_2", myHandler, MY_DEVICES);

}

void loop() {

  // Read GPS data
  GPS.read();
  GPS.parse(GPS.lastNMEA());
  // Read the uv sensor
  uint16_t uv = 0;
  uv = uvSensor.readUV();


  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every X seconds or so, print out the current stats
  if (millis() - timer > UPDATE_RATE) {
    timer = millis(); // reset the timer

      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); 
      Serial.print(GPS.lat);
      Serial.print(", "); 
      
      Serial.print(GPS.longitude, 4); 
      Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("UV Index: "); Serial.println(uv);
      
    //  String data = "{\"long\": " + String(GPS.longitude) + ", \"lat\": " + String(GPS.latitude) + ", \"speed\": " + String(GPS.speed) + ", \"uv\": " + String(uv)\
    //  + ", \"info\": [ {\"time\": 6, \"test\": 7}, {\"time\": 8, \"test\": 9}, {\"time\": 8, \"test\": 9},{\"time\": 10, \"test\": 11},{\"time\": 12, \"test\": 13},{\"time\": 14, \"test\": 15},\
    //  {\"time\": 16, \"test\": 17}, {\"time\": 18, \"test\": 19} ] }";
      
      String data = "{ \"long\": " + String(GPS.longitude) + ", \"lat\": " + String(GPS.latitude) + ", \"speed\": " + String(GPS.speed) + ", \"uv\": " + String(uv) + "}";
      
      Particle.publish("activity_2", data, PRIVATE);

  }
}
 
 
 //http://arduinodev.woofex.net/2013/02/06/adafruit_gps_forma/
 double convertDegMinToDecDeg (float degMin) {
  double min = 0.0;
  double decDeg = 0.0;
 
  //get the minutes, fmod() requires double
  min = fmod((double)degMin, 100.0);
 
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( min / 60 );
 
  return decDeg;
}

 // When obtain response from the publish
void myHandler(const char *event, const char *data) {
  // Formatting output
  String output = String::format("Response from Post:\n  %s\n", data);
  // Log to serial console
  Serial.println(output);
}

 
 