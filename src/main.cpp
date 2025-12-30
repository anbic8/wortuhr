#include <Arduino.h>
#define MQTT_MAX_PACKET_SIZE 1024

#include <ESP8266WiFi.h> //Aufbau des Wlans
#include <NTPClient.h> 
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#include "OneButton.h"
#include <Arduino.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Kommentiere aus für RCT
//#include <Wire.h>
//#define DS1307_ADDRESS 0x68 // I²C Address


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
  const int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + VERSION_STR_MAX + 1; // +1 for HA flag
  EEPROM.begin(eepromTotalSize );
  EEPROM.get( 0, user_connect );
  // read stored firmware version (fixed-size string at end of used area)
  int verOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
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
  if (!haDiscoveryEnabled) discoveryNeeded = false;
  // NOTE: do NOT overwrite stored firmware version here.
  // The current firmware version will be written after successful discovery publishes.
  // Prepare verBuf (can be used later by saveFirmwareVersion)
  char verBuf[VERSION_STR_MAX];
  memset(verBuf, 0, sizeof(verBuf));
  strncpy(verBuf, FW_VERSION, VERSION_STR_MAX - 1);
  // EEPROM Debug: zeige gelesene Werte (vorsichtig, kann leer/garbage sein)
  Serial.println("EEPROM: gelesene Einstellungen:");
  Serial.print(" SSID: '"); Serial.print(user_connect.ssid); Serial.println("'");
  Serial.print(" MQTT Server: '"); Serial.print(user_connect.mqtt_server); Serial.println("'");
  Serial.print(" MQTT Port: "); Serial.println(user_connect.mqtt_port);
  
  

    
  

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_connect.ssid, user_connect.password);
  // Prefer no WiFi sleep to improve stability
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  
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
  server.on("/wifi", handleWifi);
  server.on("/birthday", handlebirthday);
  //server.on("/settime", handlesettime);
  server.on("/datenschutz", handledatenschutz);
  server.on("/update", handleUpload);
  server.on("/upload", HTTP_POST, handleUpdate, handleUploading);
  server.on("/checkUpdate", handleCheckUpdate);
  server.on("/doUpdate", handleDoUpdate);
  server.on("/ha", handleHAConfig);
  server.on("/ha/discover", handleHADiscover);
  server.begin();
  Serial.println("Webserver gestartet");

configTime(MY_TZ, MY_NTP_SERVER); 

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
  
  if(user_design.db == 1){
      DEVICE_MODEL = "bayrische Wortuhr";
//Bayrische Version

  Serial.println("Bayrisches Setting wird geladen");
  // Use bayrisch lists at runtime (buildLedMappings will pick bay lists when dbv==1)
  iist = bayiist;
  nexthour = baynexthour;
  
  };
    //Umschalten auf viertel vor
    if(dvv==1){
      // dvv (viertel vor) handled by buildLedMappings using alternate static list
    }
 
 

  //Wire.begin(5, 13);
 
  // rebuild LED index mappings (after possible bay/viertel substitutions)
  buildLedMappings();
  startup();
  neuefarbe();



  EEPROM.get( sizeof(settings) + sizeof(MyColor)+ sizeof(design) , geburtstage );
}

void loop() {
 
if(mode==1){
  milliaktuell = millis();
    
  if(milliaktuell>threshold){
    
    readTime();
    //readTimeRCT();
    letzterstand=milliaktuell;
    threshold = letzterstand+warten-(seconds*1000)+1000;
    

    showClock();
    Serial.println();
    
  };

  if(aniMode>0){
    animationen();
  };

};
  //Listen to Buttons
  bt1.tick();
  bt2.tick();
  
  server.handleClient();
  MDNS.update();
  
  
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
  }
  
}

// write current FW_VERSION into EEPROM (used after successful discovery publishes)
void saveFirmwareVersion() {
  const int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + VERSION_STR_MAX + 1;
  int verOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
  char verBuf[VERSION_STR_MAX];
  memset(verBuf, 0, sizeof(verBuf));
  strncpy(verBuf, FW_VERSION, VERSION_STR_MAX - 1);
  EEPROM.begin(eepromTotalSize);
  EEPROM.put(verOffset, verBuf);
  EEPROM.commit();
  Serial.print("Firmware-Version gespeichert: ");
  Serial.println(verBuf);
}