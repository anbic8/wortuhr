#include "globals.h"

//Tasten
const int bt1Pin = 4; //version alte Platine pins 12, 14
const int bt2Pin = 14; // version neue Platine pins 4, 14
const int bt3Pin = 12; // Btn 1 in alter Version

OneButton bt1(bt1Pin, true); //button ist aktiv wenn Low
OneButton bt2(bt2Pin, true);
OneButton bt3(bt3Pin, true);

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(121, 2, NEO_GRB + NEO_KHZ800);

//Wlan und MQTT

ESP8266WebServer    server(80);

// MQTT-Konfiguration
String DEVICE_MODEL = "deutsche Wortuhr"; 
// Power
const char* topicOnState  = "wortuhr/on";
const char* topicOnCmd    = "wortuhr/on/set";
// Übergangseffekt
const char* topicEfxState = "wortuhr/efx";
const char* topicEfxCmd   = "wortuhr/efx/set";
const char* effectOptions[10] = {
  "kein Effekt",
  "zufällig",
  "Fade",
  "Running",
  "Schlange",
  "Zeilen",
  "Scrollen",
  "Slide in",
  "Diagonal",
  "Rain"
};
const char* topicAniState = "wortuhr/ani";
const char* topicAniCmd   = "wortuhr/ani/set";
const char* aniOptions[6] = {
  "keine Animation", "Blinken", "Vordergrundblinken", "Pulsieren", "Verlauf","Fliegen"
};
// --- Topics für Licht v1 ---
const char* topicV1State = "wortuhr/v1";
const char* topicV1Cmd   = "wortuhr/v1/set";
// --- Topics für Licht v2 ---
const char* topicV2State = "wortuhr/v2";
const char* topicV2Cmd   = "wortuhr/v2/set";

// --- Topics für Licht h1 ---
const char* topicH1State = "wortuhr/h1";
const char* topicH1Cmd   = "wortuhr/h1/set";
// --- Topics für Licht v2 ---
const char* topicH2State = "wortuhr/h2";
const char* topicH2Cmd   = "wortuhr/h2/set";
// --- Topics für Lichtschema vs ---
const char* topicVsState = "wortuhr/vs";
const char* topicVsCmd   = "wortuhr/vs/set";
// --- Topics für Lichtschema hs ---
const char* topicHsState = "wortuhr/hs";
const char* topicHsCmd   = "wortuhr/hs/set";

const char* farbschemaOptions[6] = {
  "einfarbig",
  "Schachbrett",
  "Spalten",
  "Zeilen",
  "Verlauf",
  "Zufällig"
};
// --- Topics für efx time  ---
const char* topicEfxTimeState = "wortuhr/efxtime";
const char* topicEfxTimeCmd   = "wortuhr/efxtime/set";
const char* effecttimeOptions[3] = {
  "langsam",
  "mittel",
  "schnell"
};
// --- Topics für ani time  ---
const char* topicAniTimeState = "wortuhr/anitime";
const char* topicAniTimeCmd   = "wortuhr/anitime/set";
// --- Topics für ani depth  ---
const char* topicAniDepthState = "wortuhr/anidepth";
const char* topicAniDepthCmd   = "wortuhr/anidepth/set";
const char* effectdepthOptions[3] = {
  "schwach",
  "mittel",
  "stark"
};

WiFiClient espClient;
PubSubClient client(espClient);


const char* dns_name = "wortuhr"; //name im netzwerk

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
int farben[anzahlfarben][3]={{64,64,64},{127,0,0},{127,0,64},{127,0,127},{64,0,127},{0,0,255},{0,64,127},{0,127,127},{0,127,64},{0,127,0},{64,127,0},{127,127,0},{127,64,0},{0,0,0}};

String htmlfarben[anzahlfarben]={"weiß","rot","rosa","magenta","violet","blau","azure","turkis","hellgrün","grün","gelbgrün","gelb","orange","aus"};
String htmlfarbschema[6]={"eine Farbe","zwei Farben Schachbrett", "zwei Farben Reihen","zwei Farben Zeilen", "zwei Farben Verlauf", "zufällige Farben"};
String htmlefx[10]={"kein Effekt","zufällig", "Fade", "Running", "Schlange", "Zeilen", "Scrollen", "Slide in", "Diagonal", "Rain"};
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


int matrixminmap[12][9]={
  {0,0,0,0,0,0,0,0,0}, // 0
  {0, 7, 10, 3, 7, 10, 0,0,0}, //Fünf nach 
  {1, 0, 3, 3, 7, 10, 0,0,0}, //Zehn nach
  {2, 4, 10, 3, 7, 10, 0,0,0}, //viertel nach
  {1, 4, 10, 3, 7, 10, 0,0,0}, //zwanzig nach
  {0, 7, 10, 3, 0, 2, 4, 0, 3}, // fünf vor halb
  {4, 0, 3, 0,0,0,0,0,0}, //halb
  {0, 7, 10, 3, 7, 10, 4, 0, 3}, //fünf nach halb
  {1, 0, 3, 3, 7, 10, 4, 0, 3}, //zehn nach halb
  {2, 0, 10, 0,0,0, 0,0,0}, //dreiviertel
  {1, 0, 3, 3, 0, 2, 0,0,0}, //zehn vor
  {0, 7, 10, 3, 0, 2, 0,0,0} //fünf vor

};

const int matrixminmodulomap[4]={2, 4, 6, 8};

int ste=62;
int iist=6;

int matrixstundenmap[12][3]={
  {8, 6, 10}, //12 Uhr
  {5, 0, 3}, // 1 Uhr
  {5, 7, 10}, // 2 Uhr
  {6, 0, 3}, // 3 Uhr
  {6, 7, 10}, // 4 Uhr
  {4, 7, 10}, // 5 Uhr
  {7, 0, 4}, // 6 Uhr
  {8, 0, 5}, // 7 Uhr
  {7, 7, 10}, // 8 Uhr
  {9, 3, 6}, // 9 Uhr
  {9, 0, 3}, // 10 Uhr
  {4, 5, 7} // 11 Uhr
};

int nexthour=5;

int baymatrixminmap[12][9]={
  {0,0,0,0,0,0,0,0,0}, // 0
  {1, 0, 3, 3, 7, 10, 0,0,0}, //Fünf nach 
  {1, 6, 9, 3, 7, 10, 0,0,0}, //Zehn nach
  {2, 4, 10, 3, 7, 10, 0,0,0}, //viertel nach
  {1, 6, 9, 3, 0, 2, 4,0,4}, //zehn vor hoibe
  {1, 0, 3, 3, 0, 2, 4, 0, 4}, // fünf vor halb
  {4, 0, 4, 0,0,0,0,0,0}, //halb
  {1, 0, 3, 3, 7, 10, 4, 0, 4}, //fünf nach halb
  {1, 6, 9, 3, 7, 10, 4, 0, 4}, //zehn nach halb
  {2, 0, 10, 0,0,0, 0,0,0}, //dreiviertel
  {1, 6, 9, 3, 0, 2, 0,0,0}, //zehn vor
  {1, 0, 3, 3, 0, 2, 0,0,0} //fünf vor

};

int bayiist=5;
 
int baymatrixstundenmap[12][3]={
  {8, 0, 5}, //12 Uhr
  {8, 7, 10}, // 1 Uhr
  {6, 7, 10}, // 2 Uhr
  {4, 6, 9}, // 3 Uhr
  {5, 0, 4}, // 4 Uhr
  {5, 6, 10}, // 5 Uhr
  {6, 0, 5}, // 6 Uhr
  {7, 0, 4}, // 7 Uhr
  {7, 6, 10}, // 8 Uhr
  {9, 0, 4}, // 9 Uhr
  {9, 6, 10}, // 10 Uhr
  {8, 2, 5} // 11 Uhr
};

int baynexthour=4;

int vordergrundschema=0;
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

int hintergrundschema=0;
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

int geburtstage[5][3];

//Für Einschalt animation
int t1[11]={27,38,49,60,71,82,91,92,93,94,95};
int t2[28]={15,16,17,26,39,48,61,70,83,84,85,90,107,106,105,104,103,102,101,96,79,80,81,72,59,50,37,28};
int t3[36]={3,4,5,6,7,14,29,36,51,58,73,74,75,78,97,100,119,118,117,116,115,114,113,112,111,108,89,86,67,68,69,62,47,40,25,18};
int t4[26]={2,19,24,41,46,63,64,65,66,87,88,109,110,8,13,30,35,52,57,56,55,76,77,98,99,120};
int t5[12]={1,20,23,42,45,44,9,12,31,34,53,54};

int startcolors[13][3]={{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,255},{0,64,127},{0,127,127},{0,127,64},{64,64,64},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

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
<h1>Info zur Wortuhr</h1>
        <p>Diese handgefertigte Wortuhr zeigt die Zeit auf eine einzigartige Weise: in Worten. Die Steuerung und Anpassung erfolgen bequem über dieses Webinterface. Ob Farben, Anzeigeeinstellungen oder Helligkeit – die Uhr kann individuell angepasst werden, um perfekt zu deinem Stil und deinen Bedürfnissen zu passen.</p>
        <h2>Technische Details</h2>
        <ul>
            <li><strong>LEDs:</strong> Die Uhr basiert auf einem WS2812s LED-Strip, der präzise und farbenfrohe Lichtpunkte liefert.</li>
            <li><strong>Steuerung:</strong> Die LEDs und Funktionen werden von einem ESP8266 gesteuert.</li>
            <li><strong>Anpassung:</strong> Über das Webinterface lassen sich Farben, Anzeigeoptionen und Helligkeit einstellen.</li>
        </ul>
        <h2>Bedienung</h2>
        <h3>Farben</h3>
        <p>Im Menüpunkt <strong>Farben</strong> kannst du:</p>
        <ul>
            <li>Die Vordergrund- und Hintergrundfarben der Anzeige individuell festlegen.</li>
            <li>Zwischen verschiedenen Übergängen wählen, wie die Wörter eingeblendet werden.</li>
            <li>Animationen aktivieren, um die Anzeige lebendiger zu gestalten.</li>
        </ul>
        <h3>Einstellungen</h3>
        <p>Im Menüpunkt <strong>Einstellungen</strong> kannst du:</p>
        <ul>
            <li>Entscheiden, ob bei z. B. xx:45 viertel vor oder dreiviertel angezeigt wird.</li>
            <li>Zeiten festlegen, zu denen die Uhr gedimmt oder ausgeschaltet sein soll.</li>
            <li>Wählen, ob das Wort Uhr immer, zur vollen Stunde oder nie leuchten soll.</li>
        </ul>
        <h2>Pflege und Hinweise</h2>
        <ul>
            <li>Reinigung: Verwende ein weiches, trockenes Tuch, um die Oberfläche zu reinigen.</li>
            <li>Stromversorgung: Die Uhr benötigt ein USB-Netzteil mit mindestens 2 Ampere.</li>
        </ul>
        <p>Bei Fragen oder Problemen erreichst du mich unter <a href='mailto:bichlmaier.andreas0+wortuhr@gmail.de'>bichlmaier.andreas0@gmail.de</a>.</p>

        <p><h1><i class="fa-solid fa-scale-balanced"></i><a href="/datenschutz">Datenschutz und Gewährleistung </a></h1></p>
        
        </div></body></html>

        )rawliteral";

const char htmlrecht[] PROGMEM = R"rawliteral(
  <h1>Rechtliche Hinweise</h1>
        <h2>Datenschutzhinweise</h2>
        <p>Dieses Webinterface ist ausschließlich lokal zugänglich. Es speichert keine personenbezogenen Daten und gibt keine Daten an Dritte weiter.</p>
        <ul>
            <li><strong>Netzwerk- und Geräteeinstellungen:</strong> Diese werden ausschließlich lokal auf dem ESP8266 gespeichert und nicht extern übertragen.</li>
            <li><strong>Zeitdaten:</strong> Die Uhrzeit wird automatisch von einem NTP-Server synchronisiert. Dabei wird die IP-Adresse des Geräts für die Kommunikation verwendet. Es erfolgt keine Speicherung oder Weitergabe dieser Daten.</li>
        </ul>
        <h2>Produkthaftung und Gewährleistung</h2>
        <p>Diese Wortuhr wird als handgefertigtes Einzelstück von mir privat verkauft. Daher gelten folgende Bedingungen:</p>
        <ul>
            <li>Verkauf von Privat gemäß § 13 BGB. Keine Gewährleistung oder Garantie.</li>
            <li>Die Nutzung erfolgt auf eigene Verantwortung. Der Käufer ist für die sichere Verwendung, insbesondere der Stromversorgung, selbst verantwortlich.</li>
            <li>Es wird empfohlen, ein USB-Netzteil mit mindestens 2 Ampere zu verwenden. Schäden durch unsachgemäße Nutzung oder Modifikationen am Gerät sind ausgeschlossen.</li>
        </ul>
        <p>Für Rückfragen oder Unterstützung stehe ich unter <a href='mailto:bichlmaier.andreas0+wortuhr@gmail.de'>bichlmaier.andreas0+wortuhr@gmail.de</a> zur Verfügung.</p>
        <h2>Open-Source-Bibliotheken</h2>
        <p>Diese Wortuhr verwendet folgende Open-Source-Komponenten:</p>
        <ul>
            <li><strong>ESP8266WiFi:</strong> Ermöglicht die WLAN-Konnektivität.</li>
            <li><strong>NTPClient:</strong> Synchronisiert die Uhrzeit mit einem NTP-Server.</li>
            <li><strong>WiFiUdp:</strong> Bietet UDP-Funktionalität für die Kommunikation mit dem NTP-Server.</li>
            <li><strong>Adafruit_NeoPixel:</strong> Steuert den WS2812s LED-Strip.</li>
            <li><strong>OneButton:</strong> Verarbeitet Eingaben von Tastern.</li>
            <li><strong>Arduino:</strong> Bietet grundlegende Funktionen für den ESP8266.</li>
            <li><strong>Wire:</strong> Ermöglicht die Kommunikation mit dem I²C-RTC-Modul (DS1307).</li>
            <li><strong>time.h:</strong> Stellt Zeitfunktionen bereit.</li>
            <li><strong>ESP8266WebServer:</strong> Bietet die Basis für das Webinterface.</li>
            <li><strong>ESP8266mDNS:</strong> Ermöglicht den Zugriff auf die Uhr per Hostnamen.</li>
            <li><strong>EEPROM:</strong> Speichert Konfigurationsdaten dauerhaft.</li>
            <li><strong>PubSubClient:</strong> MQTT-Bibliothek für eventuelle Erweiterungen.</li>
            <li><strong>ArduinoJson:</strong> Verarbeitet JSON-Daten für Einstellungen und Konfigurationen.</li>
        </ul>

        </div></body></html>
)rawliteral";