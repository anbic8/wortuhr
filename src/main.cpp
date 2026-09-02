#include <Arduino.h>
#define MQTT_MAX_PACKET_SIZE 1024

#include <ESP8266WiFi.h> //Aufbau des Wlans
#include <Adafruit_NeoPixel.h>
#include "OneButton.h"
#include <Arduino.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// RCT or NTP mode - controlled by USE_RCT build flag
#ifdef USE_RCT
  #include <Wire.h>
  #define DS1307_ADDRESS 0x68 // I²C Address
#else
  #include <NTPClient.h> 
  #include <WiFiUdp.h>
#endif


#include <time.h>   
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include <Updater.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "buttons.h"
#include "globals.h"
#include "eeprom_layout.h"
#include "secure_storage.h"
#include "webserver.h"
#include "show.h"
#include "color.h"
#include "animation.h"
#include "mqtt.h"
#include "mqtt-callback.h"
#include "mqtt-ha.h"
#include "birthday.h"
#include "effects.h"
#include "lighteffects.h"
#include "pomodoro.h"
#ifdef USE_RCT
  #include "rct.h"
#endif

 
void clearEEPROM() {
  EepromLayout::eraseAll();
  LOGLN("EEPROM gelöscht");
  delay(500);
}

// Shared timeout for both the initial (loop()-polled) NTP sync and the
// hourly resync below, so the two no longer drift apart (previously 30s vs 15s).
#define NTP_SYNC_TIMEOUT_MS 15000

// Set once configTime() is kicked off in setup(); cleared by loop() once
// the clock has synced or the timeout above elapses. Always false for
// USE_RCT builds (RTC time is available immediately, no wait needed).
static bool ntpInitialSyncPending = false;
static unsigned long ntpSyncStartMs = 0;

void setup() {

  //clearEEPROM(); //Nur zum Löschen des EEPROMs, danach auskommentieren

  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Warte auf Serial Monitor

  // Strip, buttons and the boot animation have no EEPROM/WiFi/NTP
  // dependency, so they run first: the matrix shows something and the
  // buttons respond immediately, instead of only after WiFi + NTP finish
  // (which can otherwise take up to ~50 seconds combined).
  strip.begin();
  //Button1
  bt1.attachClick(bt1click);
  bt1.attachLongPressStart(bt1longs);
  bt1.attachDoubleClick(bt1double);
  //Button2
  bt2.attachClick(bt2click);
  bt2.attachLongPressStart(bt2longs);
  bt2.attachDoubleClick(bt2double);
  //Button3 alter Button1
  bt3.attachClick(bt1click);
  bt3.attachLongPressStart(bt1longs);
  bt3.attachDoubleClick(bt1double);
  startup();

  EepromLayout::beginAll();
  EEPROM.get(EepromLayout::SETTINGS_OFFSET, user_connect);
  // A freshly flashed/erased EEPROM reads back as 0xFF. Without this check
  // that raw 0xFF ends up in user_connect.ssid/password and gets rendered
  // straight into the WLAN-Setup form's value='' attributes, which shows up
  // as garbled/invalid characters in the browser instead of empty fields.
  bool credentialsErased = ((uint8_t)user_connect.ssid[0] == 0xFF);
  if (credentialsErased) {
    memset(&user_connect, 0, sizeof(user_connect));
  }
  // Credentials are only encrypted once this exact layout version has been
  // written (see webserver.cpp handleWifi()). A device still on legacy
  // (unversioned, reads as 0xFF) or a different future layout keeps its
  // fields in plaintext here - decrypting those would corrupt them.
  if (!credentialsErased && EepromLayout::readLayoutVersion() == EepromLayout::CURRENT_LAYOUT_VERSION) {
    SecureStorage::cryptFields(user_connect);
  }

  // Derive dns_name (and DEVICE_ID/DEVICE_NAME/CONFIG_URL/MQTT topics) from
  // the persisted mqtt_prefix now, before MDNS.begin() below - otherwise
  // mDNS always registers under the default "wortuhr" name because
  // buildMqttTopics() used to only run later, gated on WiFi+MQTT being up.
  buildMqttTopics();

      // Timezone für Europa einstellen (z.B. CET/CEST)
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();  // Timezone aktualisieren - Auto-DST!


  // read stored firmware version (fixed-size string at end of used area)
  int countdownOffset = EepromLayout::COUNTDOWN_OFFSET;
  int verOffset = EepromLayout::VERSION_STR_OFFSET;
  char stored_fw[VERSION_STR_MAX + 1];
  for (int i = 0; i < VERSION_STR_MAX; ++i) {
    uint8_t b = EEPROM.read(verOffset + i);
    stored_fw[i] = (b == 0xFF) ? '\0' : (char)b;
  }
  stored_fw[VERSION_STR_MAX] = '\0';
  if (stored_fw[0] == '\0') {
    LOGLN("Keine gespeicherte Firmware-Version gefunden");
    discoveryNeeded = true;
  } else {
    LOG("Gespeicherte Firmware-Version: ");
    LOGLN(stored_fw);
    if (String(stored_fw) != String(FW_VERSION)) {
      LOG("Update erkannt: ");
      LOG(stored_fw);
      LOG(" -> ");
      LOGLN(FW_VERSION);
      discoveryNeeded = true;
    } else {
      discoveryNeeded = false;
      LOGLN("Firmware unverändert — Discovery übersprungen");
    }
  }
  // read HA discovery enabled flag (single byte after version slot)
  int haFlagOffset = EepromLayout::HA_FLAG_OFFSET;
  uint8_t haFlag = EEPROM.read(haFlagOffset);
  if (haFlag == 0xFF) {
    haDiscoveryEnabled = false; // default disabled
  } else {
    haDiscoveryEnabled = (haFlag != 0);
  }
  LOG("HomeAssistant Discovery enabled: "); LOGLN(haDiscoveryEnabled);
  if (haDiscoveryEnabled) {
    // Always send discovery on each boot when enabled.
    discoveryNeeded = true;
  } else {
    discoveryNeeded = false;
  }
  // NOTE: do NOT overwrite stored firmware version here.
  // The current firmware version will be written after successful discovery publishes.
  // Prepare verBuf (can be used later by saveFirmwareVersion)
  char verBuf[VERSION_STR_MAX];
  memset(verBuf, 0, sizeof(verBuf));
  strncpy(verBuf, FW_VERSION, VERSION_STR_MAX - 1);
  // read stored countdown timestamp (if present)
  EEPROM.get(countdownOffset, countdown_ts);
  if (countdown_ts == 0xFFFFFFFFUL) countdown_ts = 0; // treat erased as disabled

  // read Effekte-Modus state (erased/0xFF -> disabled, effect 0 - safe defaults)
  uint8_t fxEnabledByte = EEPROM.read(EepromLayout::LIGHT_EFFECTS_ENABLED_OFFSET);
  effectsModeActive = (fxEnabledByte == 1);
  uint8_t fxIdxByte = EEPROM.read(EepromLayout::LIGHT_EFFECT_INDEX_OFFSET);
  selectedLightEffect = (fxIdxByte < LIGHT_EFFECT_OPTIONS_COUNT) ? fxIdxByte : 0;

  uint8_t fxSpeedByte = EEPROM.read(EepromLayout::LIGHT_EFFECT_SPEED_OFFSET);
  lightEffectSpeedIdx = (fxSpeedByte <= 2) ? fxSpeedByte : 1; // erased/ungueltig -> mittel

  // "Zufällig aus Liste" Pool für den Übergangseffekt der Uhr-Anzeige
  EEPROM.get(EepromLayout::EFFECT_RANDOM_POOL_MASK_OFFSET, effectRandomPoolMask);
  if (effectRandomPoolMask == 0xFFFFU) effectRandomPoolMask = 0; // erased -> leere Liste

  // OTA-Update-Passwort (erased -> otaPasswordSet bleibt false -> Update offen)
  uint8_t otaPwSetByte = EEPROM.read(EepromLayout::OTA_PASSWORD_SET_OFFSET);
  otaPasswordSet = (otaPwSetByte == 1);
  if (otaPasswordSet) {
    EEPROM.get(EepromLayout::OTA_PASSWORD_OFFSET, otaPassword);
    SecureStorage::cryptBuffer(otaPassword, sizeof(otaPassword), 4);
  }

  // Minutengenaue Anzeige (nur 8x8 Mini)
  uint8_t minuteDotsByte = EEPROM.read(EepromLayout::MINUTE_DOTS_ENABLED_OFFSET);
  minuteDotsEnabled = (minuteDotsByte == 1);
  uint8_t minuteDotsColorByte = EEPROM.read(EepromLayout::MINUTE_DOTS_COLOR_OFFSET);
  minuteDotsColorIdx = (minuteDotsColorByte < 14) ? minuteDotsColorByte : 1; // erased/ungueltig -> rot

  // Pomodoro-Einstellungen (Laufzeitstatus pomodoroModeActive bleibt immer
  // false nach dem Boot, siehe eeprom_layout.h)
  uint8_t pomActivityByte = EEPROM.read(EepromLayout::POMODORO_ACTIVITY_MIN_OFFSET);
  pomodoroActivityMinutes = (pomActivityByte >= 1 && pomActivityByte <= 90) ? pomActivityByte : 25;
  uint8_t pomPauseByte = EEPROM.read(EepromLayout::POMODORO_PAUSE_MIN_OFFSET);
  pomodoroPauseMinutes = (pomPauseByte >= 1 && pomPauseByte <= 30) ? pomPauseByte : 5;
  uint8_t pomSchemeByte = EEPROM.read(EepromLayout::POMODORO_SCHEME_OFFSET);
  pomodoroScheme = (pomSchemeByte < FARBSCHEMA_OPTIONS_COUNT) ? pomSchemeByte : 0;
  uint8_t pomAnimationByte = EEPROM.read(EepromLayout::POMODORO_ANIMATION_OFFSET);
  pomodoroAnimationIdx = (pomAnimationByte < POMODORO_ANIMATION_OPTIONS_COUNT) ? pomAnimationByte : 0;
  uint8_t pomActC1Byte = EEPROM.read(EepromLayout::POMODORO_ACTIVITY_COLOR1_OFFSET);
  pomodoroActivityColor1Idx = (pomActC1Byte < 14) ? pomActC1Byte : 12;
  uint8_t pomActC2Byte = EEPROM.read(EepromLayout::POMODORO_ACTIVITY_COLOR2_OFFSET);
  pomodoroActivityColor2Idx = (pomActC2Byte < 14) ? pomActC2Byte : 1;
  uint8_t pomPauseC1Byte = EEPROM.read(EepromLayout::POMODORO_PAUSE_COLOR1_OFFSET);
  pomodoroPauseColor1Idx = (pomPauseC1Byte < 14) ? pomPauseC1Byte : 5;
  uint8_t pomPauseC2Byte = EEPROM.read(EepromLayout::POMODORO_PAUSE_COLOR2_OFFSET);
  pomodoroPauseColor2Idx = (pomPauseC2Byte < 14) ? pomPauseC2Byte : 6;

  // Konfigurierbare Tasten-Funktionen (nur auf NTP-Builds editierbar/genutzt)
  uint8_t b1cByte = EEPROM.read(EepromLayout::BTN1_CLICK_FUNCTION_OFFSET);
  btn1ClickFunction = (b1cByte < BUTTON_FUNCTION_OPTIONS_COUNT) ? b1cByte : 1;
  uint8_t b1lByte = EEPROM.read(EepromLayout::BTN1_LONG_FUNCTION_OFFSET);
  btn1LongFunction = (b1lByte < BUTTON_FUNCTION_OPTIONS_COUNT) ? b1lByte : 2;
  uint8_t b2cByte = EEPROM.read(EepromLayout::BTN2_CLICK_FUNCTION_OFFSET);
  btn2ClickFunction = (b2cByte < BUTTON_FUNCTION_OPTIONS_COUNT) ? b2cByte : 5;
  uint8_t b2lByte = EEPROM.read(EepromLayout::BTN2_LONG_FUNCTION_OFFSET);
  btn2LongFunction = (b2lByte < BUTTON_FUNCTION_OPTIONS_COUNT) ? b2lByte : 3;

  // EEPROM Debug: zeige gelesene Werte (vorsichtig, kann leer/garbage sein)
  LOGLN("EEPROM: gelesene Einstellungen:");
  LOG(" SSID: '"); LOG(user_connect.ssid); LOGLN("'");
  LOG(" MQTT Server: '"); LOG(user_connect.mqtt_server); LOGLN("'");
  LOG(" MQTT Port: "); LOGLN(user_connect.mqtt_port);
  
  

    
  

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_connect.ssid, user_connect.password);
  // Modem sleep periodically powers down the WiFi radio between DTIM
  // beacon intervals to save power - on a mains-powered device like this
  // one, that tradeoff isn't worth it: incoming connections (e.g. browser
  // requests to the web UI) can be missed or delayed while the radio is
  // asleep, causing intermittent "sometimes reachable, sometimes not"
  // behavior. Keep the radio fully awake instead.
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // Keep buttons and the watchdog serviced during this blocking wait.
    bt1.tick();
    bt2.tick();
    bt3.tick();
    ESP.wdtFeed();
    if (tries++ > 20) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      break;
    }
  }
  // Log Verbindung / AP Status
  if (WiFi.status() == WL_CONNECTED) {
    LOG("WLAN verbunden, IP: ");
    LOGLN(WiFi.localIP());
  } else {
    LOGLN("WLAN nicht verbunden, Access Point gestartet");
    LOG("AP SSID: ");
    LOGLN(ssid);
    LOG("AP IP: ");
    LOGLN(WiFi.softAPIP());
  }
  if (MDNS.begin(dns_name.c_str())) {
    LOGLN("DNS gestartet, erreichbar unter: ");
    LOGLN("http://" + String(dns_name) + ".local/");
  }

  server.on("/",  handlePortal);
  server.on("/color", handlecolorPath);
  server.on("/setting", handledesignPath);
  server.on("/info", handleInfo);
  server.on("/infotext", handleInfoText);
  server.on("/wifi", handleWifi);
  server.on("/birthday", handlebirthday);
  #ifdef USE_RCT
    server.on("/settime", handlesettime);
  #endif
  server.on("/datenschutz", handledatenschutz);
  server.on("/update", handleUpload);
  server.on("/upload", HTTP_POST, handleUpdate, handleUploading);
  server.on("/ha", handleHAConfig);
  server.on("/ha/discover", handleHADiscover);
  server.on("/factory-reset", handleFactoryReset);
  server.on("/api/effectsmode", handleEffectsModeApi);
  server.on("/pomodoro", handlePomodoroPath);
  server.on("/api/pomodoro", handlePomodoroApi);
  server.begin();
  LOGLN("Webserver gestartet");

#ifndef USE_RCT
  // Kick off NTP sync without blocking setup(): loop() polls for
  // completion below (see ntpInitialSyncPending), so the webserver,
  // buttons and mDNS become responsive immediately instead of after
  // up to NTP_SYNC_TIMEOUT_MS of waiting here.
  configTime(MY_TZ, MY_NTP_SERVER);
  ntpInitialSyncPending = true;
  ntpSyncStartMs = millis();
  LOGLN("NTP-Zeitsynchronisation gestartet (nicht-blockierend)...");
#endif

  // Compute New Year countdown in RAM (always next Jan 1 00:00)
  time(&now);
  localtime_r(&now, &tm);
  int year_now = tm.tm_year + 1900;
  struct tm tnew;
  tnew.tm_year = year_now + 1 - 1900; // next year
  tnew.tm_mon = 0; // January
  tnew.tm_mday = 1;
  tnew.tm_hour = 0;
  tnew.tm_min = 0;
  tnew.tm_sec = 0;
  tnew.tm_isdst = -1;
  time_t newyear_ts = mktime(&tnew);
  if (newyear_ts > 0) {
    newyear_countdown_ts = (unsigned long)newyear_ts;
    LOG("Computed New Year countdown (RAM): "); LOGLN(newyear_countdown_ts);
  }

  LOG("Matrixmodulomap: ");
for (int i=0; i<4; i++) {
  LOG(matrixminmodulomap[i]);
  LOG(" ");
}
LOGLN();


    EEPROM.get(EepromLayout::DESIGN_OFFSET, user_design);

    // Gate every field read from user_design on db>-1 (see comment on the
    // erased-EEPROM check below) - mqttenable used to be read unconditionally
    // right after EEPROM.get(), so a fresh/erased EEPROM's raw 0xFF byte was
    // misread as a truthy bool and MQTT came up enabled on first boot.
    if(user_design.db>-1){
  dbv= user_design.db;
  dvv= user_design.dv;
  uvv =user_design.uv;
  an = user_design.an;
  aus = user_design.aus;
  nacht = user_design.nacht;
  sommerzeit = user_design.sommerzeit;
  dimm = user_design.dimm;
  mqttenable = user_design.mqttenable;
    }else{
      design customDesign = {
    dbv,
    dvv,
    uvv,
    an,
    aus,
    nacht,
    sommerzeit,
    dimm,
    mqttenable
  };

  EEPROM.put(EepromLayout::DESIGN_OFFSET, customDesign);
  EEPROM.commit();
    }

    LOG("MQTT ENABLE: ");
    LOGLN(mqttenable);

  // MQTT-Client initialisieren (nur wenn Konfiguration plausibel)
  if (WiFi.status() == WL_CONNECTED && mqttenable == true) {
    bool mqttServerValid = true;
    if ((uint8_t)user_connect.mqtt_server[0] == 0xFF || user_connect.mqtt_server[0] == '\0') mqttServerValid = false;
    if (user_connect.mqtt_port <= 0 || user_connect.mqtt_port > 65535) mqttServerValid = false;
    if (mqttServerValid) {
      // Build MQTT topics with configured prefix
      buildMqttTopics();

      client.setServer(user_connect.mqtt_server, user_connect.mqtt_port);
      client.setCallback(mqttCallback);
      client.setBufferSize(1024);
    } else {
      LOGLN("MQTT-Konfiguration ungültig, MQTT deaktiviert");
      mqttenable = false;
    }
  }

  LOG("user_design.db = ");
  LOGLN(user_design.db);
  
  // Device model set by build version
  #if VERSION_TYPE == 1
    DEVICE_MODEL = "bayrische Wortuhr";
    LOGLN("Bayrische Build-Version");
  #else
    DEVICE_MODEL = "deutsche Wortuhr";
    LOGLN("Deutsche Build-Version");
  #endif
  
  // iist and nexthour are now set at compile time via VERSION_TYPE
  // dvv (viertel vor) handled by buildLedMappings using alternate static list
 
 

 
 
  // rebuild LED index mappings (after possible bay/viertel substitutions)
  buildLedMappings();
  
  #ifdef USE_RCT
    Wire.begin(5, 13); // Initialize I2C for RTC
  #endif

  neuefarbe();



  EEPROM.get(EepromLayout::BIRTHDAY_OFFSET, geburtstage);

  
}

void loop() {

#ifndef USE_RCT
  if (ntpInitialSyncPending) {
    if (time(nullptr) > 100000) {
      LOGLN("NTP-Sync erfolgreich!");
      lastNtpSync = millis();
      ntpInitialSyncPending = false;
      threshold = 0; // force an immediate render now that we have real time
    } else if (millis() - ntpSyncStartMs > NTP_SYNC_TIMEOUT_MS) {
      LOGLN("NTP-Sync Timeout! Verwende lokale Zeit.");
      ntpInitialSyncPending = false;
    }
  }
#endif

if (pomodoroModeActive) {
  renderPomodoro();
} else if (effectsModeActive) {
  renderLightEffects();
} else if(mode==1 && !ntpInitialSyncPending){

  milliaktuell = millis();

  static long sleft = 0;
  static bool secondMode = false;

  // minute-based update
  if (milliaktuell > threshold) {
    LOGLN("====================================");
    readTime();
    letzterstand = milliaktuell;
    
    if (countdown_ts > 0) {
      sleft = (long)countdown_ts - (long)now;
      LOG("Countdown TS:");
      LOG(countdown_ts);
      LOG(" Aktuelle Zeit (now): ");              LOGLN(now);
      LOG("Countdown Sekunden left: ");
      LOGLN(sleft);
      secondMode = (sleft >= 0 && sleft <= 120);
    } else {
      LOGLN("Kein Countdown aktiv");
      secondMode = false;
    }

    if(secondMode) {
      warten = 1000; 
    } else {
      warten = 60000 - (seconds * 1000) + 1000; // adjust wait to align with next minute
    }
    threshold = letzterstand + warten;
    showClock();
    LOGLN("Uhr aktualisiert");
    LOG("secondMode: ");LOGLN(secondMode);
    LOG("Nächste Aktualisierung in ms: "); LOGLN(warten);
  }

  if (aniMode > 0) {
    animationen();
  }

}
  //Listen to Buttons
  bt1.tick();
  bt2.tick();
  bt3.tick(); // war bisher nie eingebunden - Taste 1 reagierte dadurch
              // nicht, wenn sie physisch am aelteren bt3Pin (GPIO12)
              // statt am neueren bt1Pin (GPIO4) angeschlossen ist

  // Watchdog feed to prevent resets
  ESP.wdtFeed();
  
  server.handleClient();
  MDNS.update();
  
  // Heap monitoring - log warning if memory is low
  static unsigned long lastHeapCheck = 0;
  if (millis() - lastHeapCheck > 30000) { // Check every 30 seconds
    lastHeapCheck = millis();
    uint32_t freeHeap = ESP.getFreeHeap();
    if (freeHeap < 8192) {
      LOG("WARNING: Low heap memory: ");
      LOG(freeHeap);
      LOGLN(" bytes");
    }
  }
  
  #ifndef USE_RCT
  // Synchronize NTP time every hour
  static unsigned long lastNtpResync = 0;
  if (WiFi.status() == WL_CONNECTED && (millis() - lastNtpResync > 3600000)) { // 1 hour = 3600000 ms
    lastNtpResync = millis();
    LOGLN("NTP-Zeitsynchronisierung starten...");
    configTime(MY_TZ, MY_NTP_SERVER);
    delay(100);
    time_t ntpTimeout = millis();
    while (time(nullptr) < 100000 && (millis() - ntpTimeout < NTP_SYNC_TIMEOUT_MS)) {
      delay(100);
    }
    if (time(nullptr) > 100000) {
      LOGLN("NTP-Sync erfolgreich!");
      lastNtpSync = millis();
    } else {
      LOGLN("NTP-Sync fehlgeschlagen, verwende lokale Zeit");
    }
  }
  #endif
  
  //mqtt
  if (WiFi.status() == WL_CONNECTED && mqttenable== true){
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();
  // periodic MQTT connection health log
  static unsigned long lastMqttLog = 0;
  if (millis() - lastMqttLog > 10000) {
    lastMqttLog = millis();
    LOG("MQTT connected: "); LOG(client.connected());
    LOG(" state="); LOGLN(client.state());
  }
  // Publish sensor states every 60 seconds
  static unsigned long lastSensorPublish = 0;
  if (millis() - lastSensorPublish > 60000) {
    lastSensorPublish = millis();
    if (client.connected()) {
      publishSensorStates();
      LOGLN("Sensor states published");
    }
  }
  // Pomodoro-Status/Restzeit alle 5 Sekunden, nur waehrend aktiver Sitzung
  static unsigned long lastPomodoroPublish = 0;
  if (pomodoroModeActive && millis() - lastPomodoroPublish > 5000) {
    lastPomodoroPublish = millis();
    if (client.connected()) {
      publishPomodoroPhaseState();
      publishPomodoroRemainingState();
    }
  }
  }
  
}

// write current FW_VERSION into EEPROM (used after successful discovery publishes)
void saveFirmwareVersion() {
  char verBuf[VERSION_STR_MAX];
  memset(verBuf, 0, sizeof(verBuf));
  strncpy(verBuf, FW_VERSION, VERSION_STR_MAX - 1);
  EepromLayout::beginAll();
  EEPROM.put(EepromLayout::VERSION_STR_OFFSET, verBuf);
  EEPROM.commit();
  LOG("Firmware-Version gespeichert: ");
  LOGLN(verBuf);
}