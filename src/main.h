#ifndef MAIN_INCLUDE_H

#define MAIN_INCLUDE_H

#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson


#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <PubSubClient.h> // mqtt

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


extern "C" {
  #include "user_interface.h"
}

// for strcat, strcpy
#include <stdio.h>
#include <string.h>


// saved parameters
char mqtt_server[40];
char mqtt_port[6] = "1883"; // not used yet
char device_role[10] = ""; // FAN or SWITCH
char mqtt_nickname[20]; // example "Fan02";


char ch_pinLED[3] = "2";
char ch_pinButton[3] = "0";

char ch_pinRed[3] = "12";
char ch_pinGrn[3] = "13";
char ch_pinBlu[3] = "14";

// end saved parameters

int mqtt_port_int = 1883;

// Pins
int pinLED = 2; // output

int pinButton = 0; // input

int pinRed = 12; // input
int pinGrn = 13; // input
int pinBlu = 14; // input

//flag for saving data
bool shouldSaveConfig = false;


// long/short button press
long buttonTimer = 0;
long longPressTime = 10000; // 10 seconds
bool buttonActive = false;
bool longPressActive = false;

int currentRed = 0;
int currentBlu = 0;
int currentGrn = 0;

int currentPower = 0;

// broker topics will be the nickname dash suffix
// if Fan02 is the nickname
// one of the topics would be
// Fan02-Power

#define RED_FEED_SUFFIX "Red"
#define GRN_FEED_SUFFIX "Green"
#define BLU_FEED_SUFFIX "Blue"

#define HUE_FEED_SUFFIX "Hue"
#define SAT_FEED_SUFFIX "Saturation"
#define VAL_FEED_SUFFIX "Value"

#define MAX_RGB_VALUE 1023
// might be 255

#define POWER_FEED_SUFFIX "Power"

// the following are defined in Setup()
char RED_FEED[100]; // sub
char GRN_FEED[100]; // sub
char BLU_FEED[100]; // sub

char HUE_FEED[100]; // sub
char SAT_FEED[100]; // sub
char VAL_FEED[100]; // sub

char POWER_FEED[100]; // sub

int enableSerial = 1; // 0 none, 1 errors & startup, 2 verbose

// end of normally-replaceable constants

const int LEDOn = 0;
int LED_status = 1 - LEDOn;
int networkSafeMode = 0;

#include <Ticker.h>
Ticker ticker_control;

// forward declarations

boolean my_publish(const char* topic, char* value);
void setFeaturesFromRole();

// for main_include_h
#endif
