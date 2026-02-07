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
  client.subscribe(topicOnCmd.c_str());
  client.subscribe(topicEfxCmd.c_str());
  client.subscribe(topicAniCmd.c_str());
  client.subscribe(topicV1Cmd.c_str());
  client.subscribe(topicV2Cmd.c_str());
  client.subscribe(topicH1Cmd.c_str());
  client.subscribe(topicH2Cmd.c_str());
  client.subscribe(topicVsCmd.c_str());
  client.subscribe(topicHsCmd.c_str());
  client.subscribe(topicEfxTimeCmd.c_str());
  client.subscribe(topicAniTimeCmd.c_str());
  client.subscribe(topicAniDepthCmd.c_str());
  // small settle time after connect
  delay(200);
  client.loop();
  // Only run discovery publishes when needed (e.g., after firmware update)
  if (discoveryNeeded) {
    const int pauseMs = 100; // pause between publishes (reduced to minimize blocking)
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
    // Sensor discovery configs
    ok = publishIpAddressSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishUptimeSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishRssiSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishHeapMemorySensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishBrightnessSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    // Sensor discovery configs (new sensors)
    ok = publishLastNtpSyncSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishTemperatureSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
    ok = publishSystemLoadSensorConfig(); if(!ok) { discoveryNeeded = true; return; } delay(pauseMs); client.loop();
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
  snprintf(clientId, sizeof(clientId), "%s_%lu", DEVICE_ID.c_str(), ESP.getChipId());
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
  client.publish(topicOnState.c_str(), on ? "1" : "0", true);
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
  publishSensorStates();
}


void publishEffectState() {
  // safeguard: effectMode in [0..13]
  uint8_t idx = constrain(effectMode, 0, 13);
  const char* opt = effectOptions[idx];
  client.publish(topicEfxState.c_str(), opt, true);
}
void publishAniState() {
  // safeguard: aniMode in [0..6]
  uint8_t idx = constrain(aniMode, 0, 6);
  const char* opt = aniOptions[idx];
  client.publish(topicAniState.c_str(), opt, true);
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
  const char* topic = topicV1State.c_str();
  client.publish(topic, buf, true);
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
  const char* topic = topicV2State.c_str();
  client.publish(topic, buf, true);
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
  const char* topic = topicH1State.c_str();
  client.publish(topic, buf, true);
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
  client.publish(topicH2State.c_str(), buf, true);
}
void publishVsState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(vordergrundschema, 0, 5);
  const char* opt = farbschemaOptions[idx];
  client.publish(topicVsState.c_str(), opt, true);
}
void publishHsState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(hintergrundschema, 0, 5);
  const char* opt = farbschemaOptions[idx];
  client.publish(topicHsState.c_str(), opt, true);
}
void publishEfxTimeState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(efxtimeint, 0, 3);
  const char* opt = effecttimeOptions[idx];
  client.publish(topicEfxTimeState.c_str(), opt, true);
}
void publishAniTimeState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(anitimeint, 0, 3);
  const char* opt = effecttimeOptions[idx];
  client.publish(topicAniTimeState.c_str(), opt, true);
}
void publishAniDepthState() {
  // safeguard: effectMode in [0..9]
  uint8_t idx = constrain(anidepth, 0, 3);
  const char* opt = effectdepthOptions[idx];
  client.publish(topicAniDepthState.c_str(), opt, true);
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

// ============ Sensor Publishing ============

void publishIpAddress() {
  if (!client.connected()) return;
  String ip = WiFi.localIP().toString();
  client.publish(topicIpAddress.c_str(), ip.c_str(), true);
}

void publishUptime() {
  if (!client.connected()) return;
  unsigned long uptimeSeconds = millis() / 1000;
  char buf[32];
  snprintf(buf, sizeof(buf), "%lu", uptimeSeconds);
  client.publish(topicUptime.c_str(), buf, true);
}

void publishRssi() {
  if (!client.connected()) return;
  int rssi = WiFi.RSSI();
  char buf[16];
  snprintf(buf, sizeof(buf), "%d", rssi);
  client.publish(topicRssi.c_str(), buf, true);
}

void publishHeapMemory() {
  if (!client.connected()) return;
  uint32_t freeHeap = ESP.getFreeHeap();
  char buf[16];
  snprintf(buf, sizeof(buf), "%u", freeHeap);
  client.publish(topicHeapMemory.c_str(), buf, true);
}

void publishBrightness() {
  if (!client.connected()) return;
  // dimm ranges from 1-255, convert to percentage
  uint16_t brightnessPercent = map(dimm, 1, 255, 1, 100);
  char buf[8];
  snprintf(buf, sizeof(buf), "%u", brightnessPercent);
  client.publish(topicBrightness.c_str(), buf, true);
}

void publishLastNtpSync() {
  if (!client.connected()) return;
  // Check if NTP was ever synced
  if (lastNtpSync == 0) {
    client.publish(topicLastNtpSync.c_str(), "never", true);
    return;
  }
  // Calculate time since last NTP sync in seconds
  unsigned long secondsSinceSync = (millis() - lastNtpSync) / 1000;
  
  // Get current time
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  
  // Format as ISO 8601 timestamp
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", timeinfo);
  client.publish(topicLastNtpSync.c_str(), buf, true);
}

void publishTemperature() {
  if (!client.connected()) return;
  // ESP8266 internal temperature sensor
  // Note: accuracy is limited (~±10°C), but useful for general monitoring
  float tempC = (ESP.getVcc() / 1024.0f) * 0.5f;  // Very rough approximation
  // Better: use ESP8266's built-in thermal sensor via ADC
  // For a more accurate reading, we'd need to calibrate against a known reference
  // For now, use a simple estimation based on analog reading
  uint16_t raw = analogRead(A0);
  // This is a simplified formula - ESP8266 temp sensor is not very accurate
  // Actual formula depends on calibration, but typically around 55°C at room temp
  float tempEstimate = 55.0f - (raw / 1024.0f) * 20.0f;  // Rough estimate
  
  // Clamp to reasonable values
  tempEstimate = constrain(tempEstimate, -40.0f, 125.0f);
  
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f", tempEstimate);
  client.publish(topicTemperature.c_str(), buf, true);
}

void publishSystemLoad() {
  if (!client.connected()) return;
  // Estimate system load based on available heap
  // ESP8266 typically has ~40KB heap, use this as reference max
  const uint32_t estimatedMaxHeap = 40960;  // ~40KB typical for ESP8266
  uint32_t freeHeap = ESP.getFreeHeap();
  
  // Calculate heap usage percentage
  uint16_t heapUsagePercent = 100 - ((freeHeap * 100) / estimatedMaxHeap);
  heapUsagePercent = constrain(heapUsagePercent, 0, 100);
  
  char buf[8];
  snprintf(buf, sizeof(buf), "%u", heapUsagePercent);
  client.publish(topicSystemLoad.c_str(), buf, true);
}

void publishSensorStates() {
  publishIpAddress();
  publishUptime();
  publishRssi();
  publishHeapMemory();
  publishBrightness();
  publishLastNtpSync();
  publishTemperature();
  publishSystemLoad();
}