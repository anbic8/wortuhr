#include "mqtt-callback.h"
#include "mqtt.h"
#include "mqtt-ha.h"
#include <PubSubClient.h>
#include "globals.h"
#include <ArduinoJson.h>
#include "show.h"
#include "color.h"


// --- Callback für eingehende Befehle ---
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = String((char*)payload).substring(0, length);

  if (String(topic) == topicOnCmd) {
    if(msg == "1"){
      mqtton = 1;
    }else{
      mqtton = 0;
    }; 
    mqttonset = 1; 
    
    
  }
  else if (String(topic) == topicEfxCmd) {
    String opt = msg;  // msg = empfangener Payload

    if      (opt == "kein Effekt")  effectMode = 0;
    else if (opt == "zufällig")     effectMode = 1;
    else if (opt == "Fade")         effectMode = 2;
    else if (opt == "Running")      effectMode = 3;
    else if (opt == "Schlange")     effectMode = 4;
    else if (opt == "Zeilen")       effectMode = 5;
    else if (opt == "Scrollen")      effectMode = 6;
    else if (opt == "Slide in")     effectMode = 7;
    else if (opt == "Diagonal")     effectMode = 8;
    else if (opt == "Rain")         effectMode = 9;
    else if (opt == "Spirale")      effectMode = 10;
    else if (opt == "Schlangenfresser") effectMode = 11;
    else if (opt == "Raute")        effectMode = 12;
    else if (opt == "Feuerwerk")    effectMode = 13;
    else {
      // unbekannte Option, ggf. Default setzen
      effectMode = 0;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    Serial.print("EFX_MODE: ");
    Serial.println(effectMode);
    
    // Trigger sofortigen Update
    mqttonset = 1;
  }
    else if (String(topic) == topicAniCmd) {
    String opt = msg;  // msg = empfangener Payload
    if      (opt == "keine Animation")  aniMode = 0;
    else if (opt == "Blinken")     aniMode = 1;
    else if (opt == "Vordergrundblinken")         aniMode = 2;
    else if (opt == "Pulsieren")      aniMode = 3;
    else if (opt == "Verlauf")     aniMode = 4;
    else if (opt == "Fliegen")       aniMode = 5;
    
    else {
      // unbekannte Option, ggf. Default setzen
      effectMode = 0;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    Serial.print("Ani_MODE: ");
    Serial.println(aniMode);
    
  }
  else if (String(topic) == topicV1Cmd)  {
    // 1) JSON parsen
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("Ungültiges JSON: ");
    Serial.println(err.c_str());
    return;
  }

  // 2) State auslesen (ON/OFF)

  // Überlegen was passiert wenn man die Vordergrundfarbe 1 ausschaltet!!!!

  
  if (doc.containsKey("state")) {
    const char* st = doc["state"];       // "ON" oder "OFF"
    bool newOn = (strcmp(st, "ON") == 0);
    if (newOn != on) {
      mqtton = newOn;
      
      mqttonset = 1;
      Serial.printf("Power: %s\n", on ? "AN":"AUS");
    }
  }

  if (doc.containsKey("brightness")) {
    dimm = doc["brightness"];       // brightness
    strip.setBrightness(dimm);
  }
  // 3) Farbe auslesen, falls vorhanden
  if (doc.containsKey("color")) {
    JsonObject c = doc["color"];
    uint8_t r = c["r"];   // 0–255
    uint8_t g = c["g"];
    uint8_t b = c["b"];
    // hier dein vf1-Array füllen
    vf1[0] = r;
    vf1[1] = g;
    vf1[2] = b;
    Serial.printf("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
  }

  // 4) Anzeige aktualisieren
 

  // 5) neuen Gesamt-State zurückpublishen
   
  }
  else if (String(topic) == topicV2Cmd)  {
    // 1) JSON parsen
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("Ungültiges JSON: ");
    Serial.println(err.c_str());
    return;
  }

  // 2) State auslesen (ON/OFF)

  // Überlegen was passiert wenn man die Vordergrundfarbe 1 ausschaltet!!!!

  
  if (doc.containsKey("state")) {
    const char* st = doc["state"];       // "ON" oder "OFF"
    bool newOn = (strcmp(st, "ON") == 0);
    if (newOn != on) {
      vordergrundschema=0;
      vf2[0] = 0;
    vf2[1] = 0;
    vf2[2] = 0;
    }else{
      vordergrundschema=4;
    }
  }

  // 3) Farbe auslesen, falls vorhanden
  if (doc.containsKey("color")) {
    JsonObject c = doc["color"];
    uint8_t r = c["r"];   // 0–255
    uint8_t g = c["g"];
    uint8_t b = c["b"];
    // hier dein vf1-Array füllen
    vf2[0] = r;
    vf2[1] = g;
    vf2[2] = b;
    Serial.printf("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
  }
  if (doc.containsKey("brightness")) {
    dimm = doc["brightness"];       // brightness
    strip.setBrightness(dimm);
  }
  // 4) Anzeige aktualisieren
 

 
  }
  // … weitere else if für v1, v2, vs, … …
  else if (String(topic) == topicH1Cmd)  {
    // 1) JSON parsen
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("Ungültiges JSON: ");
    Serial.println(err.c_str());
    return;
  }

  // 2) State auslesen (ON/OFF)

  // Überlegen was passiert wenn man die Vordergrundfarbe 1 ausschaltet!!!!

  
  if (doc.containsKey("state")) {
    const char* st = doc["state"];       // "ON" oder "OFF"
    bool newOn = (strcmp(st, "ON") == 0);
    if (newOn != on) {
      hf1[0] = 0;
    hf1[1] = 0;
    hf1[2] = 0;
    Serial.printf("Hintergrund1 ausgeschaltet.");
    }
  }

  // 3) Farbe auslesen, falls vorhanden
  if (doc.containsKey("color")) {
    JsonObject c = doc["color"];
    uint8_t r = c["r"];   // 0–255
    uint8_t g = c["g"];
    uint8_t b = c["b"];
    // hier dein hf1-Array füllen
    hf1[0] = r;
    hf1[1] = g;
    hf1[2] = b;
    Serial.printf("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
  }
  if (doc.containsKey("brightness")) {
    dimm = doc["brightness"];       // brightness
    strip.setBrightness(dimm);
  }
  // 4) Anzeige aktualisieren
 
 
  }
  else if (String(topic) == topicH2Cmd)  {
    // 1) JSON parsen
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("Ungültiges JSON: ");
    Serial.println(err.c_str());
    return;
  }

  // 2) State auslesen (ON/OFF)

  // Überlegen was passiert wenn man die Vordergrundfarbe 1 ausschaltet!!!!

  
  if (doc.containsKey("state")) {
    const char* st = doc["state"];       // "ON" oder "OFF"
    bool newOn = (strcmp(st, "ON") == 0);
    if (newOn != on) {
      hintergrundschema=0;
      hf2[0] = 0;
    hf2[1] = 0;
    hf2[2] = 0;
    }else{
      hintergrundschema=4;
    }
  }

  // 3) Farbe auslesen, falls vorhanden
  if (doc.containsKey("color")) {
    JsonObject c = doc["color"];
    uint8_t r = c["r"];   // 0–255
    uint8_t g = c["g"];
    uint8_t b = c["b"];
    // hier dein vf1-Array füllen
    hf2[0] = r;
    hf2[1] = g;
    hf2[2] = b;
    Serial.printf("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
  }
  if (doc.containsKey("brightness")) {
    dimm = doc["brightness"];       // brightness
    strip.setBrightness(dimm);
  }
  // 4) Anzeige aktualisieren
  

  }
  // … weitere else if für v1, v2, vs, … …
  else if (String(topic) == topicVsCmd) {
    String opt = msg;  // msg = empfangener Payload
    if      (opt == "einfarbig")  vordergrundschema = 0;
    else if (opt == "Schachbrett")     vordergrundschema = 1;
    else if (opt == "Spalten")         vordergrundschema = 2;
    else if (opt == "Zeilen")      vordergrundschema = 3;
    else if (opt == "Verlauf")     vordergrundschema = 4;
    else if (opt == "Zufällig")       vordergrundschema = 5;
    
    else {
      // unbekannte Option, ggf. Default setzen
      vordergrundschema = 0;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    Serial.print("Vs_MODE: ");
    Serial.println(vordergrundschema);
    
  }
  else if (String(topic) == topicHsCmd) {
    String opt = msg;  // msg = empfangener Payload
    if      (opt == "einfarbig")  hintergrundschema = 0;
    else if (opt == "Schachbrett")     hintergrundschema = 1;
    else if (opt == "Spalten")         hintergrundschema = 2;
    else if (opt == "Zeilen")      hintergrundschema = 3;
    else if (opt == "Verlauf")     hintergrundschema = 4;
    else if (opt == "Zufällig")       hintergrundschema = 5;
    
    else {
      // unbekannte Option, ggf. Default setzen
      hintergrundschema = 0;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    Serial.print("Hs_MODE: ");
    Serial.println(hintergrundschema);
    
  }
  else if (String(topic) == topicEfxTimeCmd) {
    String opt = msg;  // msg = empfangener Payload
    if      (opt == "langsam")  efxtimeint = 0;
    else if (opt == "mittel")     efxtimeint = 1;
    else if (opt == "schnell")         efxtimeint = 2;
    
    else {
      // unbekannte Option, ggf. Default setzen
      efxtimeint = 1;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    efxtime = htmlefxtimeint[efxtimeint];
    Serial.print("EfxTime_MODE: ");
    Serial.println(efxtimeint);
    
    
  }
    else if (String(topic) == topicAniTimeCmd) {
    String opt = msg;  // msg = empfangener Payload
    if      (opt == "langsam")  anitimeint = 0;
    else if (opt == "mittel")     anitimeint = 1;
    else if (opt == "schnell")         anitimeint = 2;
    
    else {
      // unbekannte Option, ggf. Default setzen
      anitimeint = 1;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    anitime= htmlanitimeint[aniMode][anitimeint];
    Serial.print("AniTime_MODE: ");
    Serial.println(anitimeint);
    
    
  }
  else if (String(topic) == topicAniDepthCmd) {
    String opt = msg;  // msg = empfangener Payload
    if      (opt == "schwach")  anidepth = 0;
    else if (opt == "mittel")     anidepth = 1;
    else if (opt == "stark")         anidepth = 2;
    
    else {
      // unbekannte Option, ggf. Default setzen
      anidepth = 1;
      Serial.print("Unbekannter Effekt-Payload: ");
      Serial.println(opt);
    }

    Serial.print("AniDepth_MODE: ");
    Serial.println(anidepth);
    
    
  }
  checkon();
  publishAll();
  readTime(); showClock();
}