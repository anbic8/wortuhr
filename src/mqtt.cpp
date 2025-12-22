#include "mqtt.h"
#include "mqtt-callback.h"
#include "mqtt-ha.h"
#include "globals.h"
#include <PubSubClient.h>

// Non-blocking connect: try once every mqttRetryMs
static unsigned long lastMqttAttempt = 0;
// Backoff between connect attempts (in ms). Increased to reduce reconnect storms.
static unsigned long mqttRetryMs = 15000;

static void mqttOnConnected() {
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
  // small settle time after connect
  delay(200);
  client.loop();
  // Only run discovery publishes when needed (e.g., after firmware update)
  if (discoveryNeeded) {
    const int pauseMs = 500; // pause between publishes (increased to reduce broker/TCP pressure)
    bool ok = true;
    ok = publishOnOffConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishEffectConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishV1LightConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishV2LightConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishAnimationConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishH1LightConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishH2LightConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishVsConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishHsConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishEfxTimeConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishAniTimeConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishAniDepthConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    // if all OK, store firmware version so discovery runs only after updates
    if (ok) {
      discoveryNeeded = false;
      saveFirmwareVersion();
    }
  }
  // initial states
  publishAll();
}

void connectToMQTT() {
  if (client.connected()) return;
  if (millis() - lastMqttAttempt < mqttRetryMs) return;
  lastMqttAttempt = millis();
  Serial.println("Versuche MQTT-Verbindung...");
  // build a unique client id using device id + chip id to avoid collisions
  char clientId[48];
  snprintf(clientId, sizeof(clientId), "%s_%lu", DEVICE_ID, ESP.getChipId());
  Serial.print("Using MQTT clientId: "); Serial.println(clientId);

  if (client.connect(clientId, user_connect.mqtt_user, user_connect.mqtt_password)) {
    // give library a moment to settle and ensure still connected
    delay(100);
    client.loop();
    if (client.connected()) {
      Serial.println("MQTT verbunden!");
      mqttOnConnected();
    } else {
      Serial.print("Verbunden, aber sofort getrennt, state="); Serial.println(client.state());
    }
  } else {
    Serial.print("Fehler, rc=");
    Serial.println(client.state());
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

// Force an immediate reconnect attempt: disconnect and reset timer
void forceMqttReconnect() {
  Serial.println("Forcing MQTT reconnect...");
  if (client.connected()) {
    client.disconnect();
  }
  // set last attempt to now so connectToMQTT will wait mqttRetryMs before next attempt
  lastMqttAttempt = millis();
}