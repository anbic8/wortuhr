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

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  EEPROM.begin(sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) );
  EEPROM.get( 0, user_connect );
  
  

    
  

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_connect.ssid, user_connect.password);
  
  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (tries++ > 20) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(ssid, password);
      break;
    }
  }

if (MDNS.begin(dns_name)) {
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
  server.begin();

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

  if (WiFi.status() == WL_CONNECTED && mqttenable == true){
// MQTT-Client initialisieren
  client.setServer(user_connect.mqtt_server, user_connect.mqtt_port);

   // Callback-Funktion registrieren
  client.setCallback(mqttCallback);
  client.setBufferSize(1024);

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
  for(int i=0;i<12;i++){
      for(int k=0;k<9;k++){
        matrixminmap[i][k]=baymatrixminmap[i][k];
      }
    };
    iist = bayiist;
  for(int i=0;i<12;i++){
      for(int k=0;k<6;k++){
        matrixstundenmap[i][k]=baymatrixstundenmap[i][k];
      }
    };
  nexthour = baynexthour;
  
  };
    //Umschalten auf viertel vor
    if(dvv==1){
      
    matrixminmap[9][0] = 2;
    matrixminmap[9][1] = 4;
    matrixminmap[9][2] = 10;
    matrixminmap[9][3] = 3;
    matrixminmap[9][4] = 0;
    matrixminmap[9][5] = 2;
    }
 
 

  //Wire.begin(5, 13);
 
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
  }
  
}