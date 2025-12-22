#include "mqtt-ha.h"
#include "globals.h"
#include "mqtt.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Helper: publish with debug about sizes and connection
static bool pubWithCheck(const char* topic, const char* payload, bool retain=true) {
  size_t len = strlen(payload);
  Serial.printf("Publish -> %s len=%u, MQTT_MAX_PACKET_SIZE=%d\n", topic, (unsigned)len, MQTT_MAX_PACKET_SIZE);
  // ensure client is still connected
  if (!client.connected()) {
    Serial.printf("Publish FAILED for %s (connected=0 before publish, client.state()=%d) - skipping publish\n", topic, client.state());
    return false;
  }

  bool ok = client.publish(topic, payload, retain);
  if (!ok) {
    // single retry after yielding to network
    Serial.printf("Publish attempt failed for %s, retrying after client.loop()\n", topic);
    client.loop();
    delay(200);
    ok = client.publish(topic, payload, retain);
  }
  if (!ok) {
    Serial.printf("Publish FAILED for %s (connected=%d, client.state()=%d)\n", topic, client.connected(), client.state());
  }
  return ok;
}

// --- Funktion, die Home Assistant per Discovery konfiguriert ---
bool publishOnOffConfig() {
    StaticJsonDocument<512> cfg;
  cfg["name"]          = "Wortuhr Power";
  cfg["unique_id"]     = "wortuhr_power";
  cfg["state_topic"]   = topicOnState;
  cfg["command_topic"] = topicOnCmd;
  cfg["payload_on"]    = "1";
  cfg["payload_off"]   = "0";

JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Buffer mit Null-Terminator
  char buf[512];
  size_t n = serializeJson(cfg, buf, sizeof(buf));

  // Debug: zeige JSON
  Serial.print("Config JSON: ");
  Serial.println(buf);

  // Publish und Ergebnis
  bool ok = pubWithCheck("homeassistant/switch/wortuhr_power/config", buf, false);
  Serial.printf("Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishEffectConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Übergangseffekt";
  cfg["unique_id"]    = "wortuhr_efx";
  cfg["state_topic"]  = topicEfxState;
  cfg["command_topic"]= topicEfxCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("kein Effekt");
  opts.add("zufällig");
  opts.add("Fade");
  opts.add("Running");
  opts.add("Schlange");
  opts.add("Zeilen");
  opts.add("Scrollen");
  opts.add("Slide in");
  opts.add("Diagonal");
  opts.add("Rain");
  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_efx/config", buf, false);
  Serial.printf("Effect Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishAnimationConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Animationseffekt";
  cfg["unique_id"]    = "wortuhr_ani";
  cfg["state_topic"]  = topicAniState;
  cfg["command_topic"]= topicAniCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("keine Animation");
  opts.add("Blinken");
  opts.add("Vordergrundblinken");
  opts.add("Pulsieren");
  opts.add("Verlauf");
  opts.add("Fliegen");
  
  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_ani/config", buf, false);
  Serial.printf("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishV1LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Vordergrundfarbe 1";
  cfg["unique_id"]     = "wortuhr_v1";
  cfg["state_topic"]   = topicV1State;
  cfg["command_topic"] = topicV1Cmd;

  cfg["schema"]        = "json";   // JSON Schema!
  

  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck("homeassistant/light/wortuhr_v1/config", buf, false);
  Serial.printf("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishV2LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Vordergrundfarbe 2";
  cfg["unique_id"]     = "wortuhr_v2";
  cfg["state_topic"]   = topicV2State;
  cfg["command_topic"] = topicV2Cmd;

  cfg["schema"]        = "json";   // JSON Schema!
  

  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck("homeassistant/light/wortuhr_v2/config", buf, false);
  Serial.printf("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishH1LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Hintergrundfarbe 1";
  cfg["unique_id"]     = "wortuhr_h1";
  cfg["state_topic"]   = topicH1State;
  cfg["command_topic"] = topicH1Cmd;

  cfg["schema"]        = "json";   // JSON Schema!
  

  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck("homeassistant/light/wortuhr_h1/config", buf, false);
  Serial.printf("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishH2LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Hintergrundfarbe 2";
  cfg["unique_id"]     = "wortuhr_h2";
  cfg["state_topic"]   = topicH2State;
  cfg["command_topic"] = topicH2Cmd;

  cfg["schema"]        = "json";   // JSON Schema!
  

  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck("homeassistant/light/wortuhr_h2/config", buf, false);
  Serial.printf("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishVsConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Vordergrundfarbschema";
  cfg["unique_id"]    = "wortuhr_vs";
  cfg["state_topic"]  = topicVsState;
  cfg["command_topic"]= topicVsCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("einfarbig");
  opts.add("Schachbrett");
  opts.add("Spalten");
  opts.add("Zeilen");
  opts.add("Verlauf");
  opts.add("Zufällig");
  
  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_vs/config", buf, false);
  Serial.printf("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishHsConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Hintergrundfarbschema";
  cfg["unique_id"]    = "wortuhr_hs";
  cfg["state_topic"]  = topicHsState;
  cfg["command_topic"]= topicHsCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("einfarbig");
  opts.add("Schachbrett");
  opts.add("Spalten");
  opts.add("Zeilen");
  opts.add("Verlauf");
  opts.add("Zufällig");
  
  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_hs/config", buf, false);
  Serial.printf("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishEfxTimeConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Übergangsgeschwindigkeit";
  cfg["unique_id"]    = "wortuhr_efxtime";
  cfg["state_topic"]  = topicEfxTimeState;
  cfg["command_topic"]= topicEfxTimeCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("langsam");
  opts.add("mittel");
  opts.add("schnell");

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_efxtime/config", buf, false);
  Serial.printf("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishAniTimeConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Animationsgeschwindigkeit";
  cfg["unique_id"]    = "wortuhr_anitime";
  cfg["state_topic"]  = topicAniTimeState;
  cfg["command_topic"]= topicAniTimeCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("langsam");
  opts.add("mittel");
  opts.add("schnell");

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_anitime/config", buf, false);
  Serial.printf("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishAniDepthConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Animationsstärke";
  cfg["unique_id"]    = "wortuhr_anidepth";
  cfg["state_topic"]  = topicAniDepthState;
  cfg["command_topic"]= topicAniDepthCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  opts.add("schwach");
  opts.add("mittel");
  opts.add("stark");

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID;
  dev["name"]           = DEVICE_NAME;
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck("homeassistant/select/wortuhr_anidepth/config", buf, false);
  Serial.printf("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}