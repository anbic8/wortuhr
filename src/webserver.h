#include <ESP8266WebServer.h>
#include <Updater.h>
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
#ifdef USE_RCT
  void handlesettime();
#endif
void handlecolorPath();
void handledesignPath();
void handleUpload();
void handleUpdate();
void handleUploading();
