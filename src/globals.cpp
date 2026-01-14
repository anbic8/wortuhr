#include "globals.h"
#include <pgmspace.h>

//Tasten
const int bt1Pin = 4; //version alte Platine pins 12, 14
const int bt2Pin = 14; // version neue Platine pins 4, 14
const int bt3Pin = 12; // Btn 1 in alter Version

OneButton bt1(bt1Pin, true); //button ist aktiv wenn Low
OneButton bt2(bt2Pin, true);
OneButton bt3(bt3Pin, true);

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Wlan und MQTT

ESP8266WebServer    server(80);

// MQTT-Konfiguration
#if VERSION_TYPE == 0
String DEVICE_MODEL = "deutsche Wortuhr"; 
#elif VERSION_TYPE == 1
String DEVICE_MODEL = "bayrische Wortuhr";
#elif VERSION_TYPE == 2
String DEVICE_MODEL = "Mini Wortuhr";
#else
String DEVICE_MODEL = "Wortuhr";
#endif 
// MQTT Topics - werden durch buildMqttTopics() initialisiert
String topicOnState;
String topicOnCmd;
String topicEfxState;
String topicEfxCmd;
const char* effectOptions[13] = {
  "kein Effekt",
  "zufällig",
  "Fade",
  "Running",
  "Schlange",
  "Zeilen",
  "Scrollen",
  "Slide in",
  "Diagonal",
  "Rain",
  "Spirale",
  "Schlangenfresser",
  "Raute"
};
String topicAniState;
String topicAniCmd;
const char* aniOptions[6] = {
  "keine Animation", "Blinken", "Vordergrundblinken", "Pulsieren", "Verlauf","Fliegen"
};
// --- Topics für Licht v1 ---
String topicV1State;
String topicV1Cmd;
// --- Topics für Licht v2 ---
String topicV2State;
String topicV2Cmd;

// --- Topics für Licht h1 ---
String topicH1State;
String topicH1Cmd;
// --- Topics für Licht h2 ---
String topicH2State;
String topicH2Cmd;
// --- Topics für Lichtschema vs ---
String topicVsState;
String topicVsCmd;
// --- Topics für Lichtschema hs ---
String topicHsState;
String topicHsCmd;

const char* farbschemaOptions[6] = {
  "einfarbig",
  "Schachbrett",
  "Spalten",
  "Zeilen",
  "Verlauf",
  "Zufällig"
};
// --- Topics für efx time  ---
String topicEfxTimeState;
String topicEfxTimeCmd;
const char* effecttimeOptions[3] = {
  "langsam",
  "mittel",
  "schnell"
};
// --- Topics für ani time  ---
String topicAniTimeState;
String topicAniTimeCmd;
// --- Topics für ani depth  ---
String topicAniDepthState;
String topicAniDepthCmd;
const char* effectdepthOptions[3] = {
  "schwach",
  "mittel",
  "stark"
};

WiFiClient espClient;
PubSubClient client(espClient);


// dns_name is now initialized as String variable above

// Replace with your network credentials
const char *ssid     = "wortuhr";
const char *password = "123456789";

 bool mqttenable = false;

settings user_connect = {};

// Whether Home Assistant discovery publish should run on next MQTT connect
bool discoveryNeeded = true;
// Home Assistant discovery enabled (persisted in EEPROM)
bool haDiscoveryEnabled = true;

/* Globals */
time_t now;                         // this are the seconds since Epoch (1970) - UTC
struct tm tm;                              // the structure tm holds time information in a more convenient way

int mode=1;   //mode 0 for debugging mode 1 normaler modus
int settimemode=0; //mode für RCT Mode
int zeit; //
int hours=0, minutes=0, stunden=0, seconds=0, year=0, month=0, day=0;
int h=0, m=0, mb=0;
unsigned long letzterstand=0;
unsigned long letzterani=0;
unsigned long milliaktuell=0;
unsigned long warten=60000; //warten für die Uhr 10s.
unsigned long threshold=0, zeitneu2=0;
unsigned long lastNtpSync=0; // Last NTP sync timestamp
int on = 1;  // Nachtmodus 0 für Nachtmodus 1 für Uhrzeit normal
int gebstat=0; 

//Globals für die Anzeige
design user_design={};

int dbv = 0; //Frontversion 0 für deutsch 1 für bayrisch
int dvv = 0; // Anzeige von xx:45 0 für dreiviertel 1 für viertel vor
int uvv = 0; // Anzeige der Stunde 0 für immer 1 für zur vollen Stunde 2 für nie
int aus = 0; // minuten nach Mitternacht an dem der Nachtmodus anfängt
int an = 0;  // minuten nach Mitternacht an dem der Nachtmodus aufhört
int nacht = 0; // Art Nachtmodus 0 für aus 1 für gedimmt
int sommerzeit; // Sommerzeit für RCT
int dimm = 250; // Dimm-Faktor


MyColor user_color={};

int v1=0;
int v2=13;
int h1=13;
int h2=13;


// alles auf uint8_t umstellen

int vf1[3]={128,128,128};
int vf2[3]={128,128,0};
int hf1[3]={0,0,0};
int hf2[3]={0,0,0};
int effectMode=0; 
int efxtime=100;

int aniMode=0;
unsigned long anitime=1000;
int anidepth=3;
int anistate=0;
int mqtton=1;
int mqttonset=0;


int const anzahlfarben=14;
// palette in PROGMEM
const uint8_t farben[][3] PROGMEM = {
    {255,255,255},{255,0,0},{255,0,128},{255,0,255},{128,0,255},{0,0,255},{0,128,255},{0,255,255},{0,255,128},{0,255,0},{128,255,0},{255,255,0},{255,128,0},{0,0,0}
};

void getPaletteColor(uint8_t idx, int out[3]){
    if(idx >= (uint8_t)anzahlfarben){
        out[0]=0; out[1]=0; out[2]=0; return;
    }
    out[0] = (int)pgm_read_byte(&farben[idx][0]);
    out[1] = (int)pgm_read_byte(&farben[idx][1]);
    out[2] = (int)pgm_read_byte(&farben[idx][2]);
}

String htmlfarben[anzahlfarben]={"weiß","rot","rosa","magenta","violet","blau","azure","turkis","hellgrün","grün","gelbgrün","gelb","orange","aus"};
String htmlfarbschema[6]={"eine Farbe","zwei Farben Schachbrett", "zwei Farben Reihen","zwei Farben Zeilen", "zwei Farben Verlauf", "zufällige Farben"};
String htmlefx[13]={"kein Effekt","zufällig", "Fade", "Running", "Schlange", "Zeilen", "Scrollen", "Slide in", "Diagonal", "Rain", "Spirale", "Schlangenfresser", "Raute"};
String htmlefxtime[3]={"langsam", "mittel", "schnell"};
int htmlefxtimeint[3]={150,100,50};
int efxtimeint=0;

String htmlani[6]={"keine Animation", "Blinken", "Vordergrundblinken", "Pulsieren", "Verlauf","Fliegen"};
String htmlanitime[3]={"langsam", "mittel", "schnell"};
int htmlanitimeint[5][3]={
  {0,0,0},
  {1500, 1000, 500},
  {1500, 1000, 500},
  {400, 250, 125},
  {400, 250, 125}
  };
int anitimeint=0;
String htmlanidepth[3]={"schwach", "mittel", "stark"};
int flypos[3][2]={
  {5,5},
  {5,5},
  {5,5}
};



const int matrixminmodulomap[4]={2, 4, 6, 8};

int ste=62;

// ES IST/IS length - set by build version
#if VERSION_TYPE == 1
  int iist = 5;  // Bayrisch: ES IS
#else
  int iist = 6;  // Deutsch: ES IST
#endif

// Device identification (initialized from mqtt_prefix)
String DEVICE_ID = "wortuhr";
String DEVICE_NAME = "Wortuhr";
String dns_name = "wortuhr";
String CONFIG_URL = "http://wortuhr.local";

// Next hour threshold - set by build version
#if VERSION_TYPE == 1
  int nexthour = 4;  // Bayrisch: ab 20 min (10 vor halb)
#else
  int nexthour = 5;  // Deutsch: ab 25 min (halb)
#endif

int vordergrundschema=0;
#if MATRIX_SIZE == 11
int vordergrund[11][11][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#elif MATRIX_SIZE == 8
int vordergrund[8][8][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#endif

int hintergrundschema=0;
#if MATRIX_SIZE == 11
int hintergrund[11][11][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#elif MATRIX_SIZE == 8
int hintergrund[8][8][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#endif

#if MATRIX_SIZE == 11
int anzeige[11][11][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#elif MATRIX_SIZE == 8
int anzeige[8][8][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#endif
#if MATRIX_SIZE == 11
int anzeigealt[11][11][3]={
   {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#elif MATRIX_SIZE == 8
int anzeigealt[8][8][3]={
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
  {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
};
#endif

#if MATRIX_SIZE == 11
int matrix[11][11]={
  {110,111,112,113,114,115,116,117,118,119,120},
  {109,108,107,106,105,104,103,102,101,100,99},
  {88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98},
  {87, 86, 85, 84, 83, 82, 81, 80, 79, 78,77},
  {66,67,68,69,70,71,72,73,74,75,76},
  {65,64,63,62,61,60,59,58,57,56,55},
  {44,45,46,47,48,49,50,51,52,53,54},
  {43,42,41,40,39,38,37,36,35,34,33},
  {22,23,24,25,26,27,28,29,30,31,32},
  {21,20,19,18,17,16,15,14,13,12,11},
  {0,1,2,3,4,5,6,7,8,9,10}
};
#elif MATRIX_SIZE == 8
int matrix[8][8]={
  {0,1,2,3,4,5,6,7},
  {8,9,10,11,12,13,14,15},
  {16,17,18,19,20,21,22,23},
  {24,25,26,27,28,29,30,31},
  {32,33,34,35,36,37,38,39},
  {40,41,42,43,44,45,46,47},
  {48,49,50,51,52,53,54,55},
  {56,57,58,59,60,61,62,63}
};
#endif

#if MATRIX_SIZE == 11
int matrixanzeige[11][11]={
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0}
};
#elif MATRIX_SIZE == 8
int matrixanzeige[8][8]={
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
#endif

// inverse mapping (filled by buildLedMappings)
int16_t ledRow[LED_COUNT];
int16_t ledCol[LED_COUNT];

// LED lists for minute/hour words (runtime)
int16_t matrixmin_leds[12][MAX_WORD_LEDS];
uint8_t matrixmin_count[12];
int16_t matrixstunden_leds[12][MAX_WORD_LEDS];
uint8_t matrixstunden_count[12];

// Build LED-index mappings from the range-based maps (matrixminmap/matrixstundenmap)
#include "ledmap.h"

// initialize inverse map and copy static lists from flash into runtime arrays
void buildLedMappings() {
    // init inverse maps to -1
    for (int i = 0; i < LED_COUNT; ++i) {
        ledRow[i] = -1;
        ledCol[i] = -1;
    }
    // build inverse map from matrix[row][col] -> (row,col)
    for (int r = 0; r < MATRIX_SIZE; ++r) {
        for (int c = 0; c < MATRIX_SIZE; ++c) {
            int idx = matrix[r][c];
            if (idx >= 0 && idx < LED_COUNT) {
                ledRow[idx] = r;
                ledCol[idx] = c;
            }
        }
    }

    // static lists live in ledmap.h (PROGMEM)

    // copy selected static lists from PROGMEM into runtime arrays
    for (int i = 0; i < 12; ++i) {
        matrixmin_count[i] = 0;
        for (int k = 0; k < MAX_WORD_LEDS; ++k) {
            uint8_t raw;
            #if VERSION_TYPE == 0
                // Deutsche Version - aber Runtime-Umschaltung auf Bayrisch möglich
                if (dbv == 1) {
                    raw = (uint8_t)pgm_read_byte(&bay_min_lists[i][k]);
                } else {
                    raw = (uint8_t)pgm_read_byte(&std_min_lists[i][k]);
                }
            #elif VERSION_TYPE == 1
                // Bayrische Version
                raw = (uint8_t)pgm_read_byte(&bay_min_lists[i][k]);
            #elif VERSION_TYPE == 2
                // Mini Version
                raw = (uint8_t)pgm_read_byte(&mini_min_lists[i][k]);
            #else
                // Fallback: Deutsche Version
                if (dbv == 1) {
                    raw = (uint8_t)pgm_read_byte(&bay_min_lists[i][k]);
                } else {
                    raw = (uint8_t)pgm_read_byte(&std_min_lists[i][k]);
                }
            #endif
            
            // dvv override (nur für deutsche/bayrische Version)
            #if VERSION_TYPE != 2
            if (i == 9 && dvv == 1 && k < MAX_WORD_LEDS) {
                raw = (uint8_t)pgm_read_byte(&dvv9_list[k]);
            }
            #endif
            
            int16_t v = (raw == 255) ? -1 : (int16_t)raw;
            matrixmin_leds[i][k] = v;
            if (v >= 0) matrixmin_count[i]++;
        }
    }

    for (int i = 0; i < 12; ++i) {
        matrixstunden_count[i] = 0;
        for (int k = 0; k < MAX_WORD_LEDS; ++k) {
            uint8_t raw;
            #if VERSION_TYPE == 0
                // Deutsche Version - aber Runtime-Umschaltung auf Bayrisch möglich
                if (dbv == 1) {
                    raw = (uint8_t)pgm_read_byte(&bay_hour_lists[i][k]);
                } else {
                    raw = (uint8_t)pgm_read_byte(&std_hour_lists[i][k]);
                }
            #elif VERSION_TYPE == 1
                // Bayrische Version
                raw = (uint8_t)pgm_read_byte(&bay_hour_lists[i][k]);
            #elif VERSION_TYPE == 2
                // Mini Version
                raw = (uint8_t)pgm_read_byte(&mini_hour_lists[i][k]);
            #else
                // Fallback: Deutsche Version
                if (dbv == 1) {
                    raw = (uint8_t)pgm_read_byte(&bay_hour_lists[i][k]);
                } else {
                    raw = (uint8_t)pgm_read_byte(&std_hour_lists[i][k]);
                }
            #endif
            
            int16_t v = (raw == 255) ? -1 : (int16_t)raw;
            matrixstunden_leds[i][k] = v;
            if (v >= 0) matrixstunden_count[i]++;
        }
    }
    // validate lists
    validateLedLists();
}

bool validateLedLists() {
    bool ok = true;
    for (int w = 0; w < 12; ++w) {
        for (int k = 0; k < matrixmin_count[w]; ++k) {
            int16_t v = matrixmin_leds[w][k];
            if (v < 0 || v >= LED_COUNT) {
                Serial.print("Invalid minute led index: "); Serial.println(v);
                ok = false;
            }
        }
    }
    for (int w = 0; w < 12; ++w) {
        for (int k = 0; k < matrixstunden_count[w]; ++k) {
            int16_t v = matrixstunden_leds[w][k];
            if (v < 0 || v >= LED_COUNT) {
                Serial.print("Invalid hour led index: "); Serial.println(v);
                ok = false;
            }
        }
    }
    return ok;
}

uint8_t getWordLedCount(uint8_t minuteOrHourIndex, bool isHour) {
    return isHour ? matrixstunden_count[minuteOrHourIndex] : matrixmin_count[minuteOrHourIndex];
}

int16_t getWordLed(uint8_t minuteOrHourIndex, bool isHour, uint8_t idx) {
    return isHour ? matrixstunden_leds[minuteOrHourIndex][idx] : matrixmin_leds[minuteOrHourIndex][idx];
}

int geburtstage[5][3];

//Für Einschalt animation (nur für 11x11 Matrix)
#if MATRIX_SIZE == 11
int t1[11]={27,38,49,60,71,82,91,92,93,94,95};
int t2[28]={15,16,17,26,39,48,61,70,83,84,85,90,107,106,105,104,103,102,101,96,79,80,81,72,59,50,37,28};
int t3[36]={3,4,5,6,7,14,29,36,51,58,73,74,75,78,97,100,119,118,117,116,115,114,113,112,111,108,89,86,67,68,69,62,47,40,25,18};
int t4[26]={2,19,24,41,46,63,64,65,66,87,88,109,110,8,13,30,35,52,57,56,55,76,77,98,99,120};
int t5[12]={1,20,23,42,45,44,9,12,31,34,53,54};

int startcolors[13][3]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,255},{0,64,127},{0,127,127},{0,127,64},{64,64,64},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
#endif

const char htmlhead[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wortuhr</title>
     <style>
        @import url('https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap');

        html, body {
            margin: 0;
            padding: 0;
            height: 100%;
        }

        body {
            font-family: 'Roboto', Arial, sans-serif;
            background: linear-gradient(135deg, #1c2833, #34495e);
            background-attachment: fixed;
    background-size: cover;
            color: #fff;
        }
        a {
            text-decoration: none;
            color: #fff;
        }

        .menu {
            display: flex;
            flex-direction: column;
            width: 250px;
            position: fixed;
            height: 100%;
            top: 0;
            left: 0;
            box-shadow: 2px 0 5px rgba(0, 0, 0, 0.3);
        }

        .menu a {
            text-decoration: none;
            color: #fff;
            padding: 15px 20px;
            display: flex;
            align-items: center;
            gap: 10px;
            border-bottom: 1px solid #34495e;
        }

        .menu a:hover {
            background-color: #34495e;
        }

        .menu a i {
            font-size: 1.5em;
        }

        .content {
            margin-left: 260px;
            padding: 20px;
        }

        h1 {
            text-align: center;
            margin-bottom: 20px;
            font-size: 2em;
        }

        form {
            max-width: 600px;
            margin: 0 auto;
            background: rgba(255, 255, 255, 0.1);
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
        }

        .form-floating {
            margin-bottom: 15px;
        }

        label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
        }

        select {
            width: 100%;
            padding: 10px;
            font-size: 1em;
            border: 1px solid #ccc;
            border-radius: 5px;
            background: #fff;
            color: #333;
        }

        button {
            width: 100%;
            padding: 10px;
            font-size: 1.2em;
            color: #fff;
            background: linear-gradient(135deg, #4caf50, #2e7d32);
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background 0.3s;
        }

        button:hover {
            background: linear-gradient(135deg, #66bb6a, #388e3c);
        }

        .menu-button {
            display: none;
            background-color: #333;
            color: #fff;
            border: none;
            padding: 10px;
            cursor: pointer;
            width: 100%;
        }
        

        @media screen and (max-width: 768px) {
            .menu {
                display: none;
                width: 100%;
                border-right: none;
                border-bottom: 1px solid #ddd;
            }
            .menu-button {
                display: block;
            }
            .content {
                margin-left: 0;
                padding: 10px;
            }

            .form {
                width: 100%;
            }
            .menu-button {
                display: block;
            }
            .menu.open {
                display: flex;
                position: static;
                width: 100%;
            }
        }
    </style>
    <script src='https://kit.fontawesome.com/a3508372ae.js' crossorigin="anonymous"></script> 
    <script>
        function toggleMenu() {
            const menu = document.getElementById('menu');
            if (menu.classList.contains('open')) {
                menu.classList.remove('open');
            } else {
                menu.classList.add('open');
            }
        }

        
    </script>
</head>
<body>
    <button class="menu-button" onclick="toggleMenu()">☰ Menu</button>
    <nav class="menu" id="menu">
        <a href="/wifi"><i class="fas fa-wifi"></i>Verbindung</a>
        <a href="/setting"><i class="fas fa-cog"></i>Einstellungen</a>
        <a href="/color"><i class="fas fa-palette"></i>Farben</a>
        <a href="/birthday"><i class="fa-solid fa-cake-candles"></i>Geburtstage</a>
        <a href="/info"><i class="fas fa-info-circle"></i>Info</a>
        <a href="/update"><i class="fa-solid fa-upload"></i>Update</a>
        
    </nav>
    <div class="content">
           

)rawliteral";


const char htmlinfo[] PROGMEM = R"rawliteral(
<main style="max-width: 800px; margin: 0 auto;">
    <h1><i class="fas fa-clock"></i> Willkommen bei deiner Wortuhr!</h1>
    
    <div style="background: rgba(76, 175, 80, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 20px; border-left: 4px solid #4caf50;">
        <p style="margin: 0; font-size: 1.1em;">
            <i class="fas fa-heart"></i> Schön, dass du da bist! Diese handgefertigte Wortuhr zeigt dir die Zeit auf eine ganz besondere Art – 
            in Worten statt in Ziffern. Lass uns gemeinsam schauen, wie du deine Uhr ganz nach deinen Wünschen einrichten kannst.
        </p>
    </div>

    <h2><i class="fas fa-rocket"></i> Erste Schritte – Los geht's!</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <ol style="line-height: 1.8;">
            <li><strong><a href="/wifi" style="color: #66bb6a;"><i class="fas fa-wifi"></i> Verbindung einrichten</a></strong><br>
                Zuerst verbindest du deine Uhr mit deinem WLAN. Gib einfach deine Netzwerkdaten ein – fertig! 
                Optional kannst du auch MQTT für Smarthome-Integration aktivieren (z.B. für Home Assistant).</li>
            
            <li><strong><a href="/color" style="color: #66bb6a;"><i class="fas fa-palette"></i> Farben wählen</a></strong><br>
                Jetzt wird's bunt! Wähle deine Lieblingsfarben für Vorder- und Hintergrund. Du kannst zwischen verschiedenen 
                Farbschemata wählen – von einfarbig bis Regenbogen-Verlauf. Die Helligkeit regelst du mit dem Schieberegler.</li>
            
            <li><strong><a href="/setting" style="color: #66bb6a;"><i class="fas fa-cog"></i> Einstellungen anpassen</a></strong><br>
                Hier legst du fest, wie die Zeit angezeigt wird: Möchtest du "viertel vor" oder "dreiviertel"? 
                Soll das Wort "Uhr" immer leuchten? Und wann soll die Uhr nachts in den Nachtmodus gehen?</li>
            
            <li><strong><a href="/birthday" style="color: #66bb6a;"><i class="fa-solid fa-cake-candles"></i> Geburtstage eintragen</a></strong> (optional)<br>
                Trage wichtige Geburtstage ein – deine Uhr erinnert dich daran mit einer speziellen Anzeige!</li>
        </ol>
    </div>

    <h2><i class="fas fa-book-open"></i> Detaillierte Bedienungsanleitung</h2>
    
    <h3><i class="fas fa-palette"></i> Farben und Effekte</h3>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 15px;">
        <p>Im <a href="/color" style="color: #66bb6a;">Farben-Menü</a> hast du die volle kreative Kontrolle:</p>
        <ul style="line-height: 1.6;">
            <li><strong>Vordergrundfarben:</strong> Die Farbe der leuchtenden Buchstaben (die die Zeit anzeigen)</li>
            <li><strong>Hintergrundfarben:</strong> Die Farbe der nicht aktiven Buchstaben</li>
            <li><strong>Farbschemata:</strong> Wähle zwischen einfarbig, Schachbrett, Spalten, Zeilen, Verlauf oder Regenbogen</li>
            <li><strong>Helligkeit:</strong> Passe die Leuchtintensität an (von gedimmt bis hell)</li>
            <li><strong>Effekte & Animationen:</strong> Aktiviere sanfte Übergänge oder dynamische Animationen</li>
        </ul>
        <p><em>Tipp: Die Live-Vorschau zeigt dir sofort, wie deine Einstellungen aussehen werden!</em></p>
    </div>

    <h3><i class="fas fa-cog"></i> Einstellungen und Anzeigeoptionen</h3>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 15px;">
        <p>In den <a href="/setting" style="color: #66bb6a;">Einstellungen</a> bestimmst du das Verhalten deiner Uhr:</p>
        <ul style="line-height: 1.6;">
            <li><strong>Zeitanzeige bei xx:45:</strong> "dreiviertel" (z.B. dreiviertel vier) oder "viertel vor" (z.B. viertel vor vier)</li>
            <li><strong>Wort "Uhr":</strong> Wann soll es leuchten? Immer, zur vollen Stunde oder nie</li>
            <li><strong>Nachtmodus:</strong> Lege eine Zeit fest, wann die Uhr automatisch dimmt oder ausgeht (z.B. 23:00 bis 6:00)</li>
            <li><strong>Nachtmodus-Verhalten:</strong> LEDs komplett ausschalten oder nur stark dimmen</li>
        </ul>
    </div>

    <h3><i class="fas fa-wifi"></i> WLAN und MQTT</h3>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 15px;">
        <p>Über <a href="/wifi" style="color: #66bb6a;">Verbindung</a> richtest du die Netzwerkanbindung ein:</p>
        <ul style="line-height: 1.6;">
            <li><strong>WLAN-Zugangsdaten:</strong> Deine Uhr benötigt Internet für die Zeitsynchronisation</li>
            <li><strong>MQTT (optional):</strong> Für Smarthome-Fans! Steuere deine Uhr über Home Assistant, ioBroker oder andere Systeme</li>
            <li><strong>MQTT-Präfix:</strong> Gib deiner Uhr einen eindeutigen Namen, falls du mehrere hast (z.B. "wohnzimmer" oder "schlafzimmer")</li>
            <li><strong>Home Assistant:</strong> Aktiviere die automatische Erkennung für nahtlose Integration</li>
        </ul>
        <p><em>Hinweis: Nach dem Speichern startet die Uhr neu – das ist ganz normal!</em></p>
    </div>

    <h3><i class="fa-solid fa-upload"></i> Firmware-Update</h3>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 15px;">
        <p>Über das <a href="/update" style="color: #66bb6a;">Update-Menü</a> kannst du neue Funktionen und Verbesserungen installieren:</p>
        <ul style="line-height: 1.6;">
            <li>Lade die neueste Firmware-Datei (.bin) herunter</li>
            <li>Wähle die Datei im Update-Menü aus und starte das Update</li>
            <li>Warte, bis der Upload abgeschlossen ist – die Uhr startet automatisch neu</li>
        </ul>
        <p><em>Wichtig: Trenne während des Updates nicht die Stromversorgung!</em></p>
    </div>

    <h2><i class="fas fa-tools"></i> Technische Details</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 15px;">
        <ul style="line-height: 1.6;">
            <li><strong>LEDs:</strong> Adressierbarer WS2812s LED-Strip für brillante Farbdarstellung</li>
            <li><strong>Prozessor:</strong> ESP8266 Mikrocontroller mit WLAN</li>
            <li><strong>Zeitsynchronisation:</strong> Automatisch über NTP-Server (Internet erforderlich)</li>
            <li><strong>Stromversorgung:</strong> USB-Netzteil mit mindestens 2A (5V) empfohlen</li>
            <li><strong>Webinterface:</strong> Erreichbar unter <strong>wortuhr.local</strong> (oder deinem gewählten MQTT-Präfix)</li>
        </ul>
    </div>

    <h2><i class="fas fa-heart"></i> Pflege und Wartung</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <ul style="line-height: 1.6;">
            <li><strong>Reinigung:</strong> Verwende nur ein weiches, trockenes oder leicht angefeuchtetes Tuch</li>
            <li><strong>Standort:</strong> Vermeide direkte Sonneneinstrahlung und Feuchtigkeit</li>
            <li><strong>Stromversorgung:</strong> Nutze ein qualitativ hochwertiges USB-Netzteil (5V, mind. 2A)</li>
            <li><strong>Transport:</strong> Behandle die Uhr vorsichtig – sie ist ein handgefertigtes Unikat</li>
        </ul>
    </div>

    <h2><i class="fas fa-question-circle"></i> Hilfe und Support</h2>
    <div style="background: rgba(33, 150, 243, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 20px; border-left: 4px solid #2196f3;">
        <p style="margin: 0;">
            Falls du Fragen hast oder Unterstützung benötigst, bin ich gerne für dich da! 
            Schreib mir einfach eine E-Mail an 
            <a href="mailto:bichlmaier.andreas0+wortuhr@gmail.de" style="color: #64b5f6;">bichlmaier.andreas0@gmail.de</a>
        </p>
    </div>

    <div style="background: rgba(255, 255, 255, 0.05); padding: 20px; border-radius: 10px; margin-bottom: 20px; text-align: center;">
        <h3><i class="fa-solid fa-scale-balanced"></i> Rechtliche Hinweise</h3>
        <p>
            <a href="/datenschutz" style="color: #66bb6a; font-size: 1.1em;">
                <i class="fas fa-shield-alt"></i> Datenschutz und Gewährleistung anzeigen
            </a>
        </p>
        <p style="font-size: 0.9em; color: rgba(255,255,255,0.7); margin-top: 10px;">
            Diese Uhr wird von mir mit viel Liebe zum Detail handgefertigt. Alle wichtigen rechtlichen 
            Informationen zu Datenschutz, Gewährleistung und verwendeten Open-Source-Komponenten findest du im verlinkten Bereich.
        </p>
    </div>

    <div style="text-align: center; padding: 20px; margin-top: 30px; border-top: 1px solid rgba(255,255,255,0.1);">
        <p style="color: rgba(255,255,255,0.5);">
            <i class="fas fa-code"></i> Mit <i class="fas fa-heart" style="color: #e74c3c;"></i> handgefertigt von Andy B
        </p>
    </div>
</main>
</body></html>

        )rawliteral";

const char htmlrecht[] PROGMEM = R"rawliteral(
<main style="max-width: 800px; margin: 0 auto;">
    <h1><i class="fa-solid fa-scale-balanced"></i> Rechtliche Hinweise</h1>
    
    <div style="background: rgba(33, 150, 243, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 30px; border-left: 4px solid #2196f3;">
        <p style="margin: 0; font-size: 1.05em;">
            <i class="fas fa-info-circle"></i> Transparenz ist mir wichtig! Hier findest du alle Informationen zu Datenschutz, 
            Gewährleistung und den verwendeten Open-Source-Komponenten deiner Wortuhr.
        </p>
    </div>

    <h2><i class="fas fa-shield-alt"></i> Datenschutzhinweise</h2>
    <div style="background: rgba(76, 175, 80, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 20px; border-left: 4px solid #4caf50;">
        <p><strong>Kurz und klar: Deine Daten bleiben bei dir!</strong></p>
        <p>Dieses Webinterface läuft ausschließlich lokal in deinem Heimnetzwerk. Es werden keine personenbezogenen Daten gespeichert, 
        verarbeitet oder an Dritte weitergegeben. Die Uhr kommuniziert nicht mit externen Servern (außer für die Zeitsynchronisation).</p>
    </div>

    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <h3><i class="fas fa-database"></i> Welche Daten werden wo gespeichert?</h3>
        <ul style="line-height: 1.8;">
            <li><strong>WLAN-Zugangsdaten & Einstellungen:</strong><br>
                Werden ausschließlich lokal auf dem ESP8266-Chip deiner Uhr gespeichert. Kein externer Zugriff, keine Cloud.</li>
            
            <li><strong>Zeitsynchronisation (NTP):</strong><br>
                Die Uhr synchronisiert die aktuelle Uhrzeit über das Internet von einem öffentlichen NTP-Server (z.B. pool.ntp.org). 
                Dabei wird technisch bedingt die IP-Adresse deiner Uhr übermittelt, aber es erfolgt keine Speicherung oder 
                Weitergabe deiner Daten. Dies ist vergleichbar mit jedem normalen Internetzugriff.</li>
            
            <li><strong>MQTT (optional):</strong><br>
                Wenn du MQTT aktivierst, kommuniziert die Uhr mit deinem lokalen MQTT-Broker (z.B. Mosquitto). 
                Auch hier bleiben alle Daten in deinem Netzwerk – es sei denn, du hast bewusst einen externen Cloud-Broker konfiguriert.</li>
        </ul>
    </div>

    <h2><i class="fas fa-handshake"></i> Produkthaftung und Gewährleistung</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <p>Diese Wortuhr ist ein handgefertigtes Einzelstück, das ich mit viel Liebe zum Detail für dich erstellt habe. 
        Da es sich um einen <strong>Privatverkauf nach § 13 BGB</strong> handelt, gelten folgende rechtliche Rahmenbedingungen:</p>
        
        <h3><i class="fas fa-gavel"></i> Gewährleistungsausschluss</h3>
        <ul style="line-height: 1.8;">
            <li>Als Privatverkäufer kann ich leider keine gesetzliche Gewährleistung oder Garantie anbieten.</li>
            <li>Die Beschreibung der Uhr und ihrer Funktionen erfolgt nach bestem Wissen und Gewissen.</li>
            <li>Trotzdem bin ich natürlich bei Fragen oder Problemen gerne für dich da und helfe, wo ich kann!</li>
        </ul>
        
        <h3><i class="fas fa-exclamation-triangle"></i> Nutzung und Sicherheit</h3>
        <ul style="line-height: 1.8;">
            <li><strong>Eigenverantwortung:</strong> Die Nutzung der Uhr erfolgt auf eigene Verantwortung. 
                Bitte gehe sorgsam mit der Elektronik um und beachte die Hinweise zur Stromversorgung.</li>
            
            <li><strong>Stromversorgung:</strong> Verwende unbedingt ein hochwertiges USB-Netzteil mit mindestens 2 Ampere (5V). 
                Minderwertige oder ungeeignete Netzteile können zu Fehlfunktionen führen.</li>
            
            <li><strong>Keine Modifikationen:</strong> Öffne die Uhr nicht und nimm keine eigenmächtigen Veränderungen an der 
                Hardware vor. Bei Schäden durch unsachgemäße Nutzung oder Modifikationen kann ich leider keine Haftung übernehmen.</li>
            
            <li><strong>Aufstellort:</strong> Vermeide direkte Sonneneinstrahlung, Feuchtigkeit und extreme Temperaturen.</li>
        </ul>
    </div>

    <h2><i class="fas fa-code-branch"></i> Open-Source & Verwendete Bibliotheken</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <p>Diese Wortuhr steht auf den Schultern von Giganten! Folgende großartige Open-Source-Bibliotheken machen 
        das Projekt erst möglich. Ein herzliches Dankeschön an alle Entwickler:</p>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-microchip"></i> Hardware & Netzwerk</h3>
            <ul style="line-height: 1.6;">
                <li><strong>ESP8266WiFi:</strong> WLAN-Konnektivität für den ESP8266</li>
                <li><strong>ESP8266WebServer:</strong> Webserver für das Interface, das du gerade nutzt</li>
                <li><strong>ESP8266mDNS:</strong> Ermöglicht den Zugriff per "wortuhr.local" statt IP-Adresse</li>
                <li><strong>EEPROM:</strong> Dauerhafte Speicherung deiner Einstellungen</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-clock"></i> Zeit & Synchronisation</h3>
            <ul style="line-height: 1.6;">
                <li><strong>NTPClient:</strong> Zeitsynchronisation über NTP-Server</li>
                <li><strong>WiFiUdp:</strong> UDP-Kommunikation für NTP</li>
                <li><strong>Wire:</strong> I²C-Kommunikation mit dem optionalen RTC-Modul (DS1307)</li>
                <li><strong>time.h:</strong> Zeitfunktionen und -berechnungen</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-lightbulb"></i> LEDs & Steuerung</h3>
            <ul style="line-height: 1.6;">
                <li><strong>Adafruit_NeoPixel:</strong> Ansteuerung des WS2812s LED-Strips für brillante Farben</li>
                <li><strong>OneButton:</strong> Intelligente Verarbeitung von Tasteneingaben</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-network-wired"></i> Smarthome-Integration</h3>
            <ul style="line-height: 1.6;">
                <li><strong>PubSubClient:</strong> MQTT-Unterstützung für Home Assistant & Co.</li>
                <li><strong>ArduinoJson:</strong> JSON-Verarbeitung für strukturierte Datenübertragung</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-cogs"></i> Basis-Framework</h3>
            <ul style="line-height: 1.6;">
                <li><strong>Arduino:</strong> Grundlegende Funktionen und Framework für den ESP8266</li>
            </ul>
        </div>
        
        <p style="margin-top: 20px; padding: 15px; background: rgba(76, 175, 80, 0.2); border-radius: 5px; border-left: 3px solid #4caf50;">
            <i class="fas fa-heart"></i> <strong>Open-Source-Gedanke:</strong> Alle diese Bibliotheken werden von der 
            Community kostenlos bereitgestellt und weiterentwickelt. Wenn dir das Projekt gefällt, denk doch mal darüber nach, 
            die Entwickler mit einer kleinen Spende zu unterstützen!
        </p>
    </div>

    <h2><i class="fas fa-envelope"></i> Kontakt & Unterstützung</h2>
    <div style="background: rgba(33, 150, 243, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 30px; border-left: 4px solid #2196f3;">
        <p style="line-height: 1.8;">
            Hast du Fragen, Anregungen oder benötigst Hilfe? Ich bin gerne für dich da!<br>
            Schreib mir einfach eine E-Mail an 
            <a href="mailto:bichlmaier.andreas0+wortuhr@gmail.de" style="color: #64b5f6; font-weight: bold;">
                bichlmaier.andreas0@gmail.de
            </a>
        </p>
        <p style="margin: 15px 0 0 0; color: rgba(255,255,255,0.8);">
            <i class="fas fa-clock"></i> <em>Ich antworte in der Regel innerhalb von 24-48 Stunden.</em>
        </p>
    </div>

    <div style="text-align: center; padding: 20px;">
        <a href="/info" style="display: inline-block; padding: 12px 30px; background: linear-gradient(135deg, #4caf50, #2e7d32); 
           color: white; border-radius: 5px; text-decoration: none; font-weight: bold;">
            <i class="fas fa-arrow-left"></i> Zurück zur Startseite
        </a>
    </div>

    <div style="text-align: center; padding: 20px; margin-top: 20px; border-top: 1px solid rgba(255,255,255,0.1);">
        <p style="color: rgba(255,255,255,0.5);">
            <i class="fas fa-code"></i> Mit <i class="fas fa-heart" style="color: #e74c3c;"></i> handgefertigt von Andy B
        </p>
    </div>
</main>
</body></html>
)rawliteral";

// Build MQTT topics based on mqtt_prefix from settings
void buildMqttTopics() {
  String prefix = String(user_connect.mqtt_prefix);
  
  // Default to "wortuhr" if prefix is empty or invalid
  if (prefix.length() == 0 || (uint8_t)user_connect.mqtt_prefix[0] == 0xFF) {
    prefix = "wortuhr";
  }
  
  // Update DEVICE_ID, DEVICE_NAME, dns_name and CONFIG_URL from mqtt_prefix
  // Remove trailing slash for device ID
  String cleanPrefix = prefix;
  if (cleanPrefix.endsWith("/")) {
    cleanPrefix = cleanPrefix.substring(0, cleanPrefix.length() - 1);
  }
  DEVICE_ID = cleanPrefix;
  DEVICE_NAME = "Wortuhr " + cleanPrefix;
  dns_name = cleanPrefix;
  CONFIG_URL = "http://" + cleanPrefix + ".local";
  
  // Ensure prefix ends with "/"
  if (!prefix.endsWith("/")) {
    prefix += "/";
  }
  
  // Build all topics with the prefix
  topicOnState = prefix + "on";
  topicOnCmd = prefix + "on/set";
  
  topicEfxState = prefix + "efx";
  topicEfxCmd = prefix + "efx/set";
  
  topicAniState = prefix + "ani";
  topicAniCmd = prefix + "ani/set";
  
  topicV1State = prefix + "v1";
  topicV1Cmd = prefix + "v1/set";
  
  topicV2State = prefix + "v2";
  topicV2Cmd = prefix + "v2/set";
  
  topicH1State = prefix + "h1";
  topicH1Cmd = prefix + "h1/set";
  
  topicH2State = prefix + "h2";
  topicH2Cmd = prefix + "h2/set";
  
  topicVsState = prefix + "vs";
  topicVsCmd = prefix + "vs/set";
  
  topicHsState = prefix + "hs";
  topicHsCmd = prefix + "hs/set";
  
  topicEfxTimeState = prefix + "efxtime";
  topicEfxTimeCmd = prefix + "efxtime/set";
  
  topicAniTimeState = prefix + "anitime";
  topicAniTimeCmd = prefix + "anitime/set";
  
  topicAniDepthState = prefix + "anidepth";
  topicAniDepthCmd = prefix + "anidepth/set";
  
  Serial.println("MQTT Topics initialized with prefix: " + prefix);
}