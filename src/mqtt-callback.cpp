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
    int idx = findOptionIndex(effectOptions, EFFECT_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Effekt-Payload: ");
      LOGLN(msg);
      idx = 0;
    }
    effectMode = idx;

    LOG("EFX_MODE: ");
    LOGLN(effectMode);

    // Trigger sofortigen Update
    mqttonset = 1;
  }
  else if (String(topic) == topicAniCmd) {
    int idx = findOptionIndex(aniOptions, ANI_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Animations-Payload: ");
      LOGLN(msg);
      idx = 0;
    }
    aniMode = idx;

    LOG("Ani_MODE: ");
    LOGLN(aniMode);
  }
  else if (String(topic) == topicV1Cmd)  {
    // 1) JSON parsen
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    LOG("Ungültiges JSON: ");
    LOGLN(err.c_str());
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
      LOGF("Power: %s\n", on ? "AN":"AUS");
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
    LOGF("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
  }

  // 4) Anzeige aktualisieren
 

  // 5) neuen Gesamt-State zurückpublishen
   
  }
  else if (String(topic) == topicV2Cmd)  {
    // 1) JSON parsen
  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    LOG("Ungültiges JSON: ");
    LOGLN(err.c_str());
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
    LOGF("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
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
    LOG("Ungültiges JSON: ");
    LOGLN(err.c_str());
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
    LOGF("Hintergrund1 ausgeschaltet.");
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
    LOGF("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
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
    LOG("Ungültiges JSON: ");
    LOGLN(err.c_str());
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
    LOGF("Neue Farbe: R=%u G=%u B=%u\n", r, g, b);
  }
  if (doc.containsKey("brightness")) {
    dimm = doc["brightness"];       // brightness
    strip.setBrightness(dimm);
  }
  // 4) Anzeige aktualisieren
  

  }
  // … weitere else if für v1, v2, vs, … …
  else if (String(topic) == topicVsCmd) {
    int idx = findOptionIndex(farbschemaOptions, FARBSCHEMA_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Farbschema-Payload: ");
      LOGLN(msg);
      idx = 0;
    }
    vordergrundschema = idx;

    LOG("Vs_MODE: ");
    LOGLN(vordergrundschema);
  }
  else if (String(topic) == topicHsCmd) {
    int idx = findOptionIndex(farbschemaOptions, FARBSCHEMA_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Farbschema-Payload: ");
      LOGLN(msg);
      idx = 0;
    }
    hintergrundschema = idx;

    LOG("Hs_MODE: ");
    LOGLN(hintergrundschema);
  }
  else if (String(topic) == topicEfxTimeCmd) {
    int idx = findOptionIndex(effecttimeOptions, EFFECTTIME_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Zeit-Payload: ");
      LOGLN(msg);
      idx = 1;
    }
    efxtimeint = idx;

    efxtime = htmlefxtimeint[efxtimeint];
    LOG("EfxTime_MODE: ");
    LOGLN(efxtimeint);
  }
  else if (String(topic) == topicAniTimeCmd) {
    int idx = findOptionIndex(effecttimeOptions, EFFECTTIME_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Zeit-Payload: ");
      LOGLN(msg);
      idx = 1;
    }
    anitimeint = idx;

    anitime = htmlanitimeint[aniMode][anitimeint];
    LOG("AniTime_MODE: ");
    LOGLN(anitimeint);
  }
  else if (String(topic) == topicAniDepthCmd) {
    int idx = findOptionIndex(effectdepthOptions, EFFECTDEPTH_OPTIONS_COUNT, msg);
    if (idx < 0) {
      LOG("Unbekannter Staerke-Payload: ");
      LOGLN(msg);
      idx = 1;
    }
    anidepth = idx;

    LOG("AniDepth_MODE: ");
    LOGLN(anidepth);
  }
  checkon();
  publishAll();
  readTime(); showClock();
}