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
#include "webserver.h"
#include "show.h"
#include "color.h"
#include "animation.h"
#include "mqtt.h"
#include "mqtt-callback.h"
#include "mqtt-ha.h"
#include "birthday.h"
#include "effects.h"
#ifdef USE_RCT
  #include "rct.h"
#endif

 
void clearEEPROM() {
  const int size = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage) + VERSION_STR_MAX;
  EEPROM.begin(size);
  for (int i = 0; i < size; ++i) {
    EEPROM.write(i, 0xFF); // oder 0x00 wenn du Nullen willst
  }
  EEPROM.commit();
  EEPROM.end();
  Serial.println("EEPROM gelöscht");
  delay(500);
}


void setup() {

  //clearEEPROM(); //Nur zum Löschen des EEPROMs, danach auskommentieren

  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Warte auf Serial Monitor
  const int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + sizeof(unsigned long) + VERSION_STR_MAX + 1; // +1 for HA flag
  EEPROM.begin(eepromTotalSize );
  EEPROM.get( 0, user_connect );

      // Timezone für Europa einstellen (z.B. CET/CEST)
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();  // Timezone aktualisieren - Auto-DST!


  // read stored firmware version (fixed-size string at end of used area)
  int countdownOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
  int verOffset = countdownOffset + sizeof(unsigned long);
  char stored_fw[VERSION_STR_MAX + 1];
  for (int i = 0; i < VERSION_STR_MAX; ++i) {
    uint8_t b = EEPROM.read(verOffset + i);
    stored_fw[i] = (b == 0xFF) ? '\0' : (char)b;
  }
  stored_fw[VERSION_STR_MAX] = '\0';
  if (stored_fw[0] == '\0') {
    Serial.println("Keine gespeicherte Firmware-Version gefunden");
    discoveryNeeded = true;
  } else {
    Serial.print("Gespeicherte Firmware-Version: ");
    Serial.println(stored_fw);
    if (String(stored_fw) != String(FW_VERSION)) {
      Serial.print("Update erkannt: ");
      Serial.print(stored_fw);
      Serial.print(" -> ");
      Serial.println(FW_VERSION);
      discoveryNeeded = true;
    } else {
      discoveryNeeded = false;
      Serial.println("Firmware unverändert — Discovery übersprungen");
    }
  }
  // read HA discovery enabled flag (single byte after version slot)
  int haFlagOffset = verOffset + VERSION_STR_MAX;
  uint8_t haFlag = EEPROM.read(haFlagOffset);
  if (haFlag == 0xFF) {
    haDiscoveryEnabled = true; // default enabled
  } else {
    haDiscoveryEnabled = (haFlag != 0);
  }
  Serial.print("HomeAssistant Discovery enabled: "); Serial.println(haDiscoveryEnabled);
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
  // EEPROM Debug: zeige gelesene Werte (vorsichtig, kann leer/garbage sein)
  Serial.println("EEPROM: gelesene Einstellungen:");
  Serial.print(" SSID: '"); Serial.print(user_connect.ssid); Serial.println("'");
  Serial.print(" MQTT Server: '"); Serial.print(user_connect.mqtt_server); Serial.println("'");
  Serial.print(" MQTT Port: "); Serial.println(user_connect.mqtt_port);
  
  

    
  

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_connect.ssid, user_connect.password);
  // Use modem sleep mode for better stability and power management
  WiFi.setSleepMode(WIFI_MODEM_SLEEP);
  
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (tries++ > 20) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      break;
    }
  }
  // Log Verbindung / AP Status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WLAN verbunden, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WLAN nicht verbunden, Access Point gestartet");
    Serial.print("AP SSID: ");
    Serial.println(ssid);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
  }
  if (MDNS.begin(dns_name.c_str())) {
    Serial.println("DNS gestartet, erreichbar unter: ");
    Serial.println("http://" + String(dns_name) + ".local/");
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
  server.begin();
  Serial.println("Webserver gestartet");

#ifndef USE_RCT
  configTime(MY_TZ, MY_NTP_SERVER);
  // Wait for NTP time sync at startup (increased timeout to 30 seconds)
  Serial.print("Warte auf NTP-Zeitsynchronisation...");
  time_t ntpTimeout = millis();
  while (time(nullptr) < 100000 && (millis() - ntpTimeout < 30000)) {
    delay(100);
    Serial.print(".");
  }
  if (time(nullptr) > 100000) {
    Serial.println(" erfolgreich!");
    lastNtpSync = millis(); // Mark initial sync time
  } else {
    Serial.println(" Timeout! Verwende lokale Zeit.");
  }
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
    Serial.print("Computed New Year countdown (RAM): "); Serial.println(newyear_countdown_ts);
  }

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


  Serial.print("Matrixmodulomap: ");
for (int i=0; i<4; i++) {
  Serial.print(matrixminmodulomap[i]);
  Serial.print(" ");
}
Serial.println();


    EEPROM.get(sizeof(settings) + sizeof(MyColor), user_design );
   mqttenable = user_design.mqttenable;
    Serial.print("MQTT ENABLE: ");
    Serial.println(mqttenable);

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
      Serial.println("MQTT-Konfiguration ungültig, MQTT deaktiviert");
      mqttenable = false;
    }
  }




    if(user_design.db>-1){
  dbv= user_design.db;
  dvv= user_design.dv;
  uvv =user_design.uv;
  an = user_design.an;
  aus = user_design.aus;
  nacht = user_design.nacht;
  sommerzeit = user_design.sommerzeit;
  dimm = user_design.dimm;
    }else{
      design customDesign = {
    dbv,
    dvv,
    uvv,
    an,
    aus,
    nacht,
    sommerzeit,
    dimm
  };

  EEPROM.put(sizeof(settings)+sizeof(MyColor), customDesign);
  EEPROM.commit();
    }

  Serial.print("user_design.db = ");
  Serial.println(user_design.db);
  
  // Device model set by build version
  #if VERSION_TYPE == 1
    DEVICE_MODEL = "bayrische Wortuhr";
    Serial.println("Bayrische Build-Version");
  #else
    DEVICE_MODEL = "deutsche Wortuhr";
    Serial.println("Deutsche Build-Version");
  #endif
  
  // iist and nexthour are now set at compile time via VERSION_TYPE
  // dvv (viertel vor) handled by buildLedMappings using alternate static list
 
 

 
 
  // rebuild LED index mappings (after possible bay/viertel substitutions)
  buildLedMappings();
  
  #ifdef USE_RCT
    Wire.begin(5, 13); // Initialize I2C for RTC
  #endif
  
  startup();
  neuefarbe();



  EEPROM.get( sizeof(settings) + sizeof(MyColor)+ sizeof(design) , geburtstage );

  
}

void loop() {
 
if(mode==1){
  
  milliaktuell = millis();

  static long sleft = 0;
  static bool secondMode = false;

  // minute-based update
  if (milliaktuell > threshold) {
    Serial.println("====================================");
    readTime();
    letzterstand = milliaktuell;
    
    if (countdown_ts > 0) {
      sleft = (long)countdown_ts - (long)now;
      Serial.print("Countdown TS:");
      Serial.print(countdown_ts);
      Serial.print(" Aktuelle Zeit (now): ");              Serial.println(now);
      Serial.print("Countdown Sekunden left: ");
      Serial.println(sleft);
      secondMode = (sleft >= 0 && sleft <= 120);
    } else {
      Serial.println("Kein Countdown aktiv");
      secondMode = false;
    }

    if(secondMode) {
      warten = 1000; 
    } else {
      warten = 60000 - (seconds * 1000) + 1000; // adjust wait to align with next minute
    }
    threshold = letzterstand + warten;
    showClock();
    Serial.println("Uhr aktualisiert");
    Serial.print("secondMode: ");Serial.println(secondMode);
    Serial.print("Nächste Aktualisierung in ms: "); Serial.println(warten);
  }

  if (aniMode > 0) {
    animationen();
  }

}
  //Listen to Buttons
  bt1.tick();
  bt2.tick();
  
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
      Serial.print("WARNING: Low heap memory: ");
      Serial.print(freeHeap);
      Serial.println(" bytes");
    }
  }
  
  #ifndef USE_RCT
  // Synchronize NTP time every hour
  static unsigned long lastNtpResync = 0;
  if (WiFi.status() == WL_CONNECTED && (millis() - lastNtpResync > 3600000)) { // 1 hour = 3600000 ms
    lastNtpResync = millis();
    Serial.println("NTP-Zeitsynchronisierung starten...");
    configTime(MY_TZ, MY_NTP_SERVER);
    delay(100);
    time_t ntpTimeout = millis();
    while (time(nullptr) < 100000 && (millis() - ntpTimeout < 15000)) {
      delay(100);
    }
    if (time(nullptr) > 100000) {
      Serial.println("NTP-Sync erfolgreich!");
      lastNtpSync = millis();
    } else {
      Serial.println("NTP-Sync fehlgeschlagen, verwende lokale Zeit");
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
    Serial.print("MQTT connected: "); Serial.print(client.connected());
    Serial.print(" state="); Serial.println(client.state());
  }
  // Publish sensor states every 60 seconds
  static unsigned long lastSensorPublish = 0;
  if (millis() - lastSensorPublish > 60000) {
    lastSensorPublish = millis();
    if (client.connected()) {
      publishSensorStates();
      Serial.println("Sensor states published");
    }
  }
  }
  
}

// write current FW_VERSION into EEPROM (used after successful discovery publishes)
void saveFirmwareVersion() {
  const int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + sizeof(unsigned long) + VERSION_STR_MAX + 1;
  int verOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage) + sizeof(unsigned long);
  char verBuf[VERSION_STR_MAX];
  memset(verBuf, 0, sizeof(verBuf));
  strncpy(verBuf, FW_VERSION, VERSION_STR_MAX - 1);
  EEPROM.begin(eepromTotalSize);
  EEPROM.put(verOffset, verBuf);
  EEPROM.commit();
  Serial.print("Firmware-Version gespeichert: ");
  Serial.println(verBuf);
}