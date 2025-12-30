#include <ESP8266WebServer.h>
#include <Updater.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "globals.h"
#include <EEPROM.h>
#include "show.h"
#include "color.h"


void handlePortal();
void handleInfo();
void handledatenschutz();
void handleWifi();
void handlebirthday();
void handleHAConfig();
void handleHADiscover();
// void handlesettime();
void handlecolorPath();
void handledesignPath();
void handleUpload();
void handleUpdate();
void handleUploading();
void handleCheckUpdate();
void handleDoUpdate();
