#ifndef GLOBALS_H
#define GLOBALS_H

#include <ESP8266WiFi.h>
#include <NTPClient.h> 
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include "OneButton.h"
#include <Arduino.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <time.h>   
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Updater.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "buttons.h"

//Tasten
extern const int bt1Pin; //version alte Platine pins 12, 14
extern const int bt2Pin; // version neue Platine pins 4, 14
extern const int bt3Pin; // Btn 1 in alter Version

extern OneButton bt1; //button ist aktiv wenn Low
extern OneButton bt2;
extern OneButton bt3;

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 121

// NeoPixel strip object
extern Adafruit_NeoPixel strip;

// Webserver
extern ESP8266WebServer server;

// Device configuration constants
#define DEVICE_ID       "wortuhr_02"
#define DEVICE_NAME     "Wortuhr"
#define DEVICE_VENDOR   "ZeitlichtT"
#define FW_VERSION "3.0.3"
#define CONFIG_URL "http://wortuhr.local"

// NTP configuration
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"

// MQTT configuration
extern String DEVICE_MODEL;
extern const char* topicOnState;
extern const char* topicOnCmd;
extern const char* topicEfxState;
extern const char* topicEfxCmd;
extern const char* effectOptions[10];
extern const char* topicAniState;
extern const char* topicAniCmd;
extern const char* aniOptions[6];
extern const char* topicV1State;
extern const char* topicV1Cmd;
extern const char* topicV2State;
extern const char* topicV2Cmd;
extern const char* topicH1State;
extern const char* topicH1Cmd;
extern const char* topicH2State;
extern const char* topicH2Cmd;
extern const char* topicVsState;
extern const char* topicVsCmd;
extern const char* topicHsState;
extern const char* topicHsCmd;
extern const char* farbschemaOptions[6];
extern const char* topicEfxTimeState;
extern const char* topicEfxTimeCmd;
extern const char* effecttimeOptions[3];
extern const char* topicAniTimeState;
extern const char* topicAniTimeCmd;
extern const char* topicAniDepthState;
extern const char* topicAniDepthCmd;
extern const char* effectdepthOptions[3];

extern WiFiClient espClient;
extern PubSubClient client;

extern const char* dns_name;
extern const char *ssid;
extern const char *password;
extern bool mqttenable;

typedef struct {
  char ssid[30];
  char password[30];
  char mqtt_server[20];
  int mqtt_port;
  char mqtt_user[30];
  char mqtt_password[20];
} settings;

extern settings user_connect;

// Time variables
extern time_t now;
extern struct tm tm;

// Mode and state variables
extern int mode;
extern int settimemode;
extern int zeit;
extern int hours, minutes, stunden, seconds, year, month, day;
extern int h, m, mb;
extern unsigned long letzterstand;
extern unsigned long letzterani;
extern unsigned long milliaktuell;
extern unsigned long warten;
extern unsigned long threshold, zeitneu2;
extern int on;
extern int gebstat;

// Display configuration
typedef struct {
  int db;
  int dv;
  int uv;
  int an;
  int aus;
  int nacht;
  int sommerzeit;
  int dimm;
  bool mqttenable;
} design;

extern design user_design;

extern int dbv;
extern int dvv;
extern int uvv;
extern int aus;
extern int an;
extern int nacht;
extern int sommerzeit;
extern int dimm;

// Color configuration
typedef struct {
  int vf1;
  int vf2;
  int vs;
  int hf1;
  int hf2;
  int hs;
  int efx;
  int efxtime;
  int ani;
  int anitime;
  int anidepth;
} MyColor;

extern MyColor user_color;

extern int v1;
extern int v2;
extern int h1;
extern int h2;

extern int vf1[3];
extern int vf2[3];
extern int hf1[3];
extern int hf2[3];
extern int effectMode;
extern int efxtime;

extern int aniMode;
extern unsigned long anitime;
extern int anidepth;
extern int anistate;
extern int mqtton;
extern int mqttonset;

extern const int anzahlfarben;
extern int farben[14][3];

extern String htmlfarben[14];
extern String htmlfarbschema[6];
extern String htmlefx[10];
extern String htmlefxtime[3];
extern int htmlefxtimeint[3];
extern int efxtimeint;

extern String htmlani[6];
extern String htmlanitime[3];
extern int htmlanitimeint[5][3];
extern int anitimeint;
extern String htmlanidepth[3];
extern int flypos[3][2];

// Matrix mappings
extern int matrixminmap[12][9];
extern const int matrixminmodulomap[4];
extern int ste;
extern int iist;
extern int matrixstundenmap[12][3];
extern int nexthour;

extern int baymatrixminmap[12][9];
extern int bayiist;
extern int baymatrixstundenmap[12][3];
extern int baynexthour;

extern int vordergrundschema;
extern int vordergrund[11][11][3];
extern int hintergrundschema;
extern int hintergrund[11][11][3];
extern int anzeige[11][11][3];
extern int anzeigealt[11][11][3];
extern int matrix[11][11];
extern int matrixanzeige[11][11];
extern int geburtstage[5][3];

// Animation arrays
extern int t1[11];
extern int t2[28];
extern int t3[36];
extern int t4[26];
extern int t5[12];
extern int startcolors[13][3];

// HTML content
extern const char htmlhead[] PROGMEM;
extern const char htmlinfo[] PROGMEM;

#endif // GLOBALS_H
extern const char htmlrecht[] PROGMEM;

