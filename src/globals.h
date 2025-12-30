#ifndef GLOBALS_H
#define GLOBALS_H

#include <ESP8266WiFi.h>
#include <NTPClient.h> 
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 1024
#endif
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
// Can be overridden by build flags in platformio.ini
#ifndef LED_COUNT
#define LED_COUNT 121
#endif

// Matrix size (11x11 for standard, 8x8 for mini)
#ifndef MATRIX_SIZE
#define MATRIX_SIZE 11
#endif

// Version type (0=deutsch, 1=bayrisch, 2=mini)
#ifndef VERSION_TYPE
#define VERSION_TYPE 0
#endif

// NeoPixel strip object
extern Adafruit_NeoPixel strip;

// Webserver
extern ESP8266WebServer server;

// Device identification
extern String DEVICE_ID;
extern String DEVICE_NAME;
extern String CONFIG_URL;
#define DEVICE_VENDOR   "ZeitlichtT"
#define FW_VERSION "4.2.4"


// Size of firmware version string stored in EEPROM
#define VERSION_STR_MAX 16

// NTP configuration
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"

// MQTT configuration
extern String DEVICE_MODEL;
extern String topicOnState;
extern String topicOnCmd;
extern String topicEfxState;
extern String topicEfxCmd;
extern const char* effectOptions[12];
extern String topicAniState;
extern String topicAniCmd;
extern const char* aniOptions[6];
extern String topicV1State;
extern String topicV1Cmd;
extern String topicV2State;
extern String topicV2Cmd;
extern String topicH1State;
extern String topicH1Cmd;
extern String topicH2State;
extern String topicH2Cmd;
extern String topicVsState;
extern String topicVsCmd;
extern String topicHsState;
extern String topicHsCmd;
extern const char* farbschemaOptions[6];
extern String topicEfxTimeState;
extern String topicEfxTimeCmd;
extern const char* effecttimeOptions[3];
extern String topicAniTimeState;
extern String topicAniTimeCmd;
extern String topicAniDepthState;
extern String topicAniDepthCmd;
extern const char* effectdepthOptions[3];

extern WiFiClient espClient;
extern PubSubClient client;

extern String dns_name;
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
  char mqtt_prefix[20];
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
extern const uint8_t farben[][3] PROGMEM;
// Read palette color from PROGMEM into int array[3]
void getPaletteColor(uint8_t idx, int out[3]);

// Build MQTT topics with prefix
void buildMqttTopics();

extern String htmlfarben[14];
extern String htmlfarbschema[6];
extern String htmlefx[12];
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
extern int vordergrund[MATRIX_SIZE][MATRIX_SIZE][3];
extern int hintergrundschema;
extern int hintergrund[MATRIX_SIZE][MATRIX_SIZE][3];
extern int anzeige[MATRIX_SIZE][MATRIX_SIZE][3];
extern int anzeigealt[MATRIX_SIZE][MATRIX_SIZE][3];
extern int matrix[MATRIX_SIZE][MATRIX_SIZE];
extern int matrixanzeige[MATRIX_SIZE][MATRIX_SIZE];

// LED index mappings (built at startup)
#define MAX_WORD_LEDS 15
extern int16_t matrixmin_leds[12][MAX_WORD_LEDS];
extern uint8_t matrixmin_count[12];
extern int16_t matrixstunden_leds[12][MAX_WORD_LEDS];
extern uint8_t matrixstunden_count[12];

// inverse mapping: led index -> row/col in matrix
extern int16_t ledRow[LED_COUNT];
extern int16_t ledCol[LED_COUNT];

// build led mapping after any change to the textual maps
void buildLedMappings();
// validate runtime led lists (checks bounds)
bool validateLedLists();
// helper getters
uint8_t getWordLedCount(uint8_t minuteOrHourIndex, bool isHour);
int16_t getWordLed(uint8_t minuteOrHourIndex, bool isHour, uint8_t idx);
extern int geburtstage[5][3];

// Whether Home Assistant discovery publish should run on next MQTT connect
extern bool discoveryNeeded;
extern bool haDiscoveryEnabled;

// Animation arrays (only for 11x11 matrix)
#if MATRIX_SIZE == 11
extern int t1[11];
extern int t2[28];
extern int t3[36];
extern int t4[26];
extern int t5[12];
extern int startcolors[13][3];
#endif

// HTML content
extern const char htmlhead[] PROGMEM;
extern const char htmlinfo[] PROGMEM;

#endif // GLOBALS_H
extern const char htmlrecht[] PROGMEM;

// Save current firmware version to EEPROM (implementation in main.cpp)
void saveFirmwareVersion();

