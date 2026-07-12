#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <Arduino.h>

// Size of firmware version string stored in EEPROM
#define VERSION_STR_MAX 16
#define FW_VERSION "4.5.0"

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

// Save current firmware version to EEPROM (implementation in main.cpp)
void saveFirmwareVersion();

#endif // PERSISTENCE_H
