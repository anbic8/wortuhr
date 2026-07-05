#ifndef GLOBALS_NETWORK_H
#define GLOBALS_NETWORK_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include "OneButton.h"

//Tasten
extern const int bt1Pin; //version alte Platine pins 12, 14
extern const int bt2Pin; // version neue Platine pins 4, 14
extern const int bt3Pin; // Btn 1 in alter Version

extern OneButton bt1; //button ist aktiv wenn Low
extern OneButton bt2;
extern OneButton bt3;

// NeoPixel strip object
extern Adafruit_NeoPixel strip;

// Webserver
extern ESP8266WebServer server;

// Device identification
extern String DEVICE_ID;
extern String DEVICE_NAME;
extern String CONFIG_URL;
extern String DEVICE_MODEL;
#define DEVICE_VENDOR   "ZeitlichtT"

// NTP configuration
#define MY_NTP_SERVER "at.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"

extern String dns_name;
extern const char *ssid;
extern const char *password;
extern bool mqttenable;

// OTA-Update-Schutz (HTTP Basic Auth auf /update, /upload). Fester
// Benutzername, konfigurierbares Passwort. Solange otaPasswordSet false
// ist (Werkszustand / noch nicht konfiguriert), bleibt OTA offen wie
// bisher. otaPassword liegt hier immer als Klartext im RAM (fuer den
// Basic-Auth-Vergleich); nur die EEPROM-Kopie ist verschluesselt.
#define OTA_USERNAME "admin"
extern bool otaPasswordSet;
extern char otaPassword[20];

#endif // GLOBALS_NETWORK_H
