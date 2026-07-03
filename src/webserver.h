#include <ESP8266WebServer.h>
#include <Updater.h>
#include "globals.h"
#include "eeprom_layout.h"
#include "secure_storage.h"
#include "webserver_html.h"
#include <EEPROM.h>
#include "show.h"
#include "color.h"


void handlePortal();
void handleInfo();
void handleInfoText();
void handledatenschutz();
void handleWifi();
void handlebirthday();
void handleHAConfig();
void handleHADiscover();
void handleFactoryReset();
#ifdef USE_RCT
  void handlesettime();
#endif
void handlecolorPath();
void handledesignPath();
void handleUpload();
void handleUpdate();
void handleUploading();
