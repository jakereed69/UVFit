/*
    Firmware for the UVFit based off of the ECE 513 requirements
    Author: Jake Reed
    Date: 11/25/18
    
    
    EEPROM Memory Map
    0 -> (sizeof(int)-1)            = activity id (changes for each activity)
    sizeof(int) -> 2*sizeof(int)-1  = uv threshold (Changes when user updates on front end)
    2*sizeof(int) -> sizeof(apiKey) = api key string... not sure how big this will be so keep this at the end
    
*/


// TODO: record data and submit after activity is stopped even when we have wifi
//---------------------------------------------------------------------
SYSTEM_THREAD(ENABLED);         // Allows connection logic to run in background

#include "data.h"
#include "activity.h"
#include "stateMachineHandler.h"
#include <vector>
#include <queue>
#include <Adafruit_VEML6070.h>
#include "Adafruit_GPS.h"
#include <math.h>
#include <time.h>

#define mySerial Serial1
#define CHIRP_LENGTH 1000
#define STOPPED_THRESHOLD 1
//#define DEBUG
//---------------------------------------------------------------------
Adafruit_GPS GPS(&mySerial);
Adafruit_VEML6070 uvSensor;
uint16_t uv = 0;

stateHandler stateMachine;
Timer activityTimer(1000, getGPSData);
bool dataReady = false;
bool dataStored = false;

// For checking activity status
enum activities {wait, startActivity, recordActivity, endActivity};
int activityState = 0;
int activityNumber = -1;

// For keeping track of what were doing when we dont have wifi
std::vector<activity> allActivities;            // Might be useful to make this a vector of pointers to activities to save space
std::vector<Data>* currentData = new std::vector<Data>;

// I/O stuff
int button = D6;
int speaker = A1;
int debounce;

bool tooHot = false;        // sounds the alarm if it is too bright

// Pause functionality --ALSO needs to be adapted to no wifi--
int stoppedCount = 0;
bool paused = false;

// Stuff for unix time conversion
struct tm t;
time_t t_of_day;
time_t unixTime;

// EEPROM variable locations
int apiLocation = sizeof(int) + sizeof(int);
int uvLocation = sizeof(int);

// EEPROM variable buffers
int storedUvThreshold = 0;
const int STRING_BUF_SIZE = 33;
char stringBuf[STRING_BUF_SIZE];

//---------------------------------------------------------------------

void setup() {
  // Add LED control
  RGB.control(true);
  
  // Initialize UV sensor
  uvSensor.begin(VEML6070_1_T);

  // Initialize GPS ssensor and serial------------------
  GPS.begin(9600);
  mySerial.begin(9600);
  Serial.begin(9600);

  // Tell GPS to only send the RMC data
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  //-----------------------------------------------------
  
  // Delay while things are initialized
  delay(1000);
  debounce = millis();
  
  // Set up webhooks
  Particle.subscribe("hook-response/activity_2", myHandler, MY_DEVICES);
  bool success_api = Particle.function("apiUpdate", apiFunction);               // Function to recieve API key from server upon device registration
  bool success_uv = Particle.function("uvUpdate", uvFunction);                  // Function to recieve UV threshold from server

  // Setup button functionality
  pinMode(button, INPUT_PULLUP);
  pinMode(speaker, OUTPUT);
  attachInterrupt(button, buttonChange, FALLING);
  
  }

//---------------------------------------------------------------------

void loop() {
  // We want to read GPS as often as we can
  GPS.read();
  GPS.parse(GPS.lastNMEA());
  
  
  // Alerts user if the UV reading is above the UV threshold value
  if(tooHot){
     chirp();
     tooHot = false;
  }
  
  // Start activity (happens only once per activity)
  if(activityState == startActivity){
     activity newActivity;       // Create new activity
     allActivities.push_back(newActivity);
     activityNumber++;
     activityTimer.start();// Start activity timer
  }
  
  // Start recording/publishing data 
  else if(activityState == recordActivity){
     if (dataReady){
         
        // Implement pause logic
        if(GPS.speed < STOPPED_THRESHOLD){
            stoppedCount++;
            if(stoppedCount > 2){
                paused = true;
                RGB.color(0,255,0);
            }
        }
        else{
            stoppedCount = 0;
            paused = false;
        }
        
        // Get unix timestamp and stored api key and stored uv threshold
        unixTime = getUnixDate(GPS.year-1900, GPS.month-1, GPS.day, GPS.hour, GPS.minute, GPS.seconds, -1);
        EEPROM.get(uvLocation, storedUvThreshold);
        Serial.println(storedUvThreshold);
        EEPROM.get(apiLocation, stringBuf);
		stringBuf[sizeof(stringBuf) - 1] = 0; // make sure it's null terminated
		String str(stringBuf);
		Serial.printlnf("str=%s",str.c_str());

        // Publish data if were connected to cloud
        if (Particle.connected() && !paused){    
          /* To help save space in the string we use the following encoding:
             a) longitude
             b) latitude
             c) speed
             d) UV reading
             i) Activity ID
             t) Unix timestamp
             k) API key
             There should be no whitespace in the string 
          */
          String collectedData = "{\"a\":" + String(GPS.longitude) + ",\"b\":" + String(GPS.latitude) + ",\"c\":" + String(GPS.speed) + ",\"d\":" + String(uv) + ",\"i\":" + String(allActivities.at(activityNumber).getId()) +",\"t\":" + String(unixTime) + ",\"k\":\"" + String(stringBuf) + "\"}";
          Particle.publish("activity_2", collectedData, PRIVATE);
        }
        
        // Save data if we are not connected to wifi
        else if(!Particle.connected() && !paused){
          //--------For saving data---------------------
          Data* newData = new Data;
          newData->create(GPS.longitude, GPS.latitude, GPS.speed, uv, unixTime);
          allActivities.at(activityNumber).addData(newData);
          dataStored = true;
        }
        dataReady = false;
     }
  }
  
  // stop the activity (happens only once per activity)
  else if(activityState == endActivity){
      activityTimer.stop();        // Turn off the timer when were done with activity
      if(!dataStored && (allActivities.size() > 0)){
          allActivities.clear();        // Delete all stored activities if we have submitted stored data 
          activityNumber = -1;
      }
       // Reset pause counter
       paused = false;
       stoppedCount = 0;
  }
   
  // **********POSSIBLY REMOVE*********
  else{
 
  }
  // **********************************
  
  // Publish all stored data when we are connected again
  if((activityState == endActivity) && (dataStored) && (Particle.connected())){
     
      for (int actNum = 0; actNum < allActivities.size(); actNum++){
            currentData = allActivities.at(actNum).getData();

       for (int i = 0; i < currentData->size(); i++){
            String storedData = "{\"a\":" + String(currentData->at(i).lon()) + ",\"b\":" + String(currentData->at(i).lat()) + ",\"c\":" + String(currentData->at(i).spd()) + ",\"d\":" + String(currentData->at(i).uvr()) + ",\"i\":" + String(allActivities.at(actNum).getId()) + ",\"t\":" + String(currentData->at(i).t()) + ",\"k\":" + String(stringBuf) + "}";
            Serial.println(storedData);
            Particle.publish("activity_2", storedData, PRIVATE);
            delay(1000);
          }
      }
          allActivities.clear();
          activityNumber = -1;
          dataStored = false;
  }
  
  // Constantly check for changes in state machine
  stateMachine.checkButton();
  activityState = stateMachine.getActivityStatus();
  if(!paused){
    RGB.color(stateMachine.red,stateMachine.green,stateMachine.blue);
  }
}

//---------------------------------------------------------------------

// When we obtain response from the publish
void myHandler(const char *event, const char *data) {
  // Formatting output
  String output = String::format("Response from Post:\n  %s\n", data);
  // Log to serial console
  Serial.println(output);
}

//---------------------------------------------------------------------

// wait 1 second after initialization to use the button
void buttonChange(){
    if ((millis() - debounce) > 1000){
      stateMachine.buttonPressed();
    }
}
//---------------------------------------------------------------------

// Reads all relevent data
void getGPSData(){
  // Read the uv sensor
  uv = uvSensor.readUV();
  if(uv > storedUvThreshold){
      tooHot = true;
  }
  dataReady = true;
}
//---------------------------------------------------------------------
// Alerts user of high uv by chirping the speaker
void chirp(){
    for(int j = 0; j < CHIRP_LENGTH; j++){
    digitalWrite(speaker, HIGH);
     delay(1);
     digitalWrite(speaker, LOW);
     delay(1);
    }
}

//--------------------------------------------------------------------
// Create timestamp in unix time for easy sorting on server
time_t getUnixDate(int year, int month, int day, int hour, int min, int sec, int isdst){
t.tm_year = year;
t.tm_mon = month;           // Month, 0 = jan
t.tm_mday = day;          // Day of the month
t.tm_hour = hour;
t.tm_min = min;
t.tm_sec = sec;
t.tm_isdst = isdst;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
t_of_day = mktime(&t);
return t_of_day;
}

//--------------------------------------------------------------------
// Stores api key into EEPROM 
// We assume this will happen immediatly upon device registration
int apiFunction(String newApi){
    Serial.print("Server request new api key: ");
    Serial.println(newApi);
    const char *newApiKey[] = {newApi};
    String str(stringBuf);
	str = String(newApiKey[0 & 0xf]);
	str.getBytes((unsigned char *)stringBuf, sizeof(stringBuf));  
    EEPROM.put(apiLocation, stringBuf);
    return 1;
}

//--------------------------------------------------------------------
// Stores UV threshold into EEPROM
// We assume this will be initialized during device registration
int uvFunction(String newUv){
    int uvInt = newUv.toInt();
    Serial.print("Server request new UV threshold: ");
    Serial.println(uvInt);
    EEPROM.put(uvLocation, uvInt);
    return 1;
}
