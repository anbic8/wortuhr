#include "mqtt.h"
#include "mqtt-callback.h"
#include "mqtt-ha.h"
#include <PubSubClient.h>
#include "globals.h"

void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Verbinde mit MQTT...");
    if (client.connect("WortuhrClient", user_connect.mqtt_user, user_connect.mqtt_password)) {
      Serial.println("MQTT verbunden!");


      client.subscribe(topicOnCmd);
      client.subscribe(topicEfxCmd);
      client.subscribe(topicAniCmd);
      client.subscribe(topicV1Cmd);
      client.subscribe(topicV2Cmd);
      client.subscribe(topicH1Cmd);
      client.subscribe(topicH2Cmd);
      client.subscribe(topicVsCmd);
      client.subscribe(topicHsCmd);
      client.subscribe(topicEfxTimeCmd);
      client.subscribe(topicAniTimeCmd);
      client.subscribe(topicAniDepthCmd);
  // … subscribe für alle Topics …
  
  publishOnOffConfig();
  publishEffectConfig();
  publishV1LightConfig();
  publishV2LightConfig();
  publishAnimationConfig();
  publishH1LightConfig();
  publishH2LightConfig();
  publishVsConfig();
  publishHsConfig();
  publishEfxTimeConfig();
  publishAniTimeConfig();
  publishAniDepthConfig();
  publishOnOffConfig();
  // … publish Config für alle weiteren Entitäten …
  // initiale States senden:
    publishAll();
  // … und so weiter …
      
    } else {
      Serial.print("Fehler, rc=");
      Serial.println(client.state());
      Serial.println("Erneuter Versuch in 5 Sekunden...");
      delay(5000);
    }
  }
}

void publishAll(){
  client.publish(topicOnState, on ? "1" : "0", true);
  publishEffectState();
  publishAniState();
  publishV1State();
  publishV2State();
  publishH1State();
  publishH2State();
  publishVsState();
  publishHsState();
  publishEfxTimeState();
  publishAniTimeState();
  publishAniDepthState();
}


void publishEffectState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(effectMode, 0, 9);
  const char* opt = effectOptions[idx];
  client.publish(topicEfxState, opt, true);
}
void publishAniState() {
  // safeguard: aniMode in [0..5]
  uint8_t idx = constrain(aniMode, 0, 5);
  const char* opt = aniOptions[idx];
  client.publish(topicAniState, opt, true);
}


void publishV1State() {
  StaticJsonDocument<256> s;
  if( vf1[0] == 0 && vf1[1] == 0 && vf1[2]==0){
    s["state"] = "OFF";
  }else{
    s["state"] = "ON";
  }
  if( vordergrundschema==0){
      s["state"] = "OFF";
    }
  s["color_mode"] = "rgb";  // <<< NEU: Angabe des aktiven Farbschemas

  s["brightness"] = dimm; 
  // nested color-Objekt
  JsonObject c = s.createNestedObject("color");
  c["r"] = vf1[0];
  c["g"] = vf1[1];
  c["b"] = vf1[2];

  char buf[256];
  serializeJson(s, buf, sizeof(buf));
  client.publish(topicV1State, buf, true);
}
void publishV2State() {
  StaticJsonDocument<256> s;
  // ON/OFF als String

  if( vf2[0] == 0 && vf2[1] == 0 && vf2[2]==0){
    s["state"] = "OFF";
  }else{
    s["state"] = "ON";
  }
  if( vordergrundschema==0){
      s["state"] = "OFF";
    }
  s["brightness"] = dimm;
  s["color_mode"] = "rgb";    
  // nested color-Objekt
  JsonObject c = s.createNestedObject("color");
  c["r"] = vf2[0];
  c["g"] = vf2[1];
  c["b"] = vf2[2];

  char buf[256];
  serializeJson(s, buf, sizeof(buf));
  client.publish(topicV2State, buf, true);
}
void publishH1State() {
  StaticJsonDocument<256> s;

    if( hf1[0] == 0 && hf1[1] == 0 && hf1[2]==0){
    s["state"] = "OFF";
  }else{
    s["state"] = "ON";
  }
  s["brightness"] = dimm; 
  // nested color-Objekt
  s["color_mode"] = "rgb";
  JsonObject c = s.createNestedObject("color");
  c["r"] = hf1[0];
  c["g"] = hf1[1];
  c["b"] = hf1[2];

  char buf[256];
  serializeJson(s, buf, sizeof(buf));
  client.publish(topicH1State, buf, true);
}
void publishH2State() {
  StaticJsonDocument<256> s;
    if( hf2[0] == 0 && hf2[1] == 0 && hf2[2]==0){
    s["state"] = "OFF";
  }else{
    s["state"] = "ON";
  }
    if( hintergrundschema==0){
      s["state"] = "OFF";
    }
  s["brightness"] = dimm; 
  s["color_mode"] = "rgb";
  // nested color-Objekt
  JsonObject c = s.createNestedObject("color");
  c["r"] = hf2[0];
  c["g"] = hf2[1];
  c["b"] = hf2[2];

  char buf[256];
  serializeJson(s, buf, sizeof(buf));
  client.publish(topicH2State, buf, true);
}
void publishVsState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(vordergrundschema, 0, 5);
  const char* opt = farbschemaOptions[idx];
  client.publish(topicVsState, opt, true);
}
void publishHsState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(hintergrundschema, 0, 5);
  const char* opt = farbschemaOptions[idx];
  client.publish(topicHsState, opt, true);
}
void publishEfxTimeState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(efxtimeint, 0, 3);
  const char* opt = effecttimeOptions[idx];
  client.publish(topicEfxTimeState, opt, true);
}
void publishAniTimeState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(anitimeint, 0, 3);
  const char* opt = effecttimeOptions[idx];
  client.publish(topicAniTimeState, opt, true);
}
void publishAniDepthState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(anidepth, 0, 3);
  const char* opt = effectdepthOptions[idx];
  client.publish(topicAniDepthState, opt, true);
}