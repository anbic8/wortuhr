#include "globals_network.h"
#include "globals.h" // LED_COUNT, VERSION_TYPE build-config macros

//Tasten
// bt1Pin kann per Build-Flag in platformio.ini ueberschrieben werden
// (z.B. kkg-makerspace: Pin 5)
#ifndef BT1_PIN
#define BT1_PIN 4
#endif
const int bt1Pin = BT1_PIN; //version alte Platine pins 12, 14
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

// Device identification (initialized from mqtt_prefix)
String DEVICE_ID = "wortuhr";
String DEVICE_NAME = "Wortuhr";
String dns_name = "wortuhr";
String CONFIG_URL = "http://wortuhr.local";

// Replace with your network credentials
const char *ssid     = "wortuhr";
const char *password = "123456789";

bool mqttenable = false;

bool otaPasswordSet = false;
char otaPassword[20] = {0};
