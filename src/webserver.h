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
// Leichter AJAX-Endpunkt: aktiviert Effekte-Modus-Aenderungen sofort, ohne
// dass die ganze /color-Seite mit "Speichern" abgeschickt werden muss.
void handleEffectsModeApi();
void handlePomodoroPath();
// Leichter AJAX-Endpunkt: startet/stoppt Pomodoro sofort, ohne die ganze
// /pomodoro-Seite mit "Speichern" abzuschicken.
void handlePomodoroApi();
void handleUpload();
void handleUpdate();
void handleUploading();
