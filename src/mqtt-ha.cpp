#include "mqtt-ha.h"
#include "globals.h"
#include "mqtt.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Helper: publish with debug about sizes and connection
static bool pubWithCheck(const char* topic, const char* payload, bool retain=true) {
  size_t len = strlen(payload);
  LOGF("Publish -> %s len=%u, MQTT_MAX_PACKET_SIZE=%d\n", topic, (unsigned)len, MQTT_MAX_PACKET_SIZE);
  // ensure client is still connected
  if (!client.connected()) {
    LOGF("Publish FAILED for %s (connected=0 before publish, client.state()=%d) - skipping publish\n", topic, client.state());
    return false;
  }

  bool ok = client.publish(topic, payload, retain);
  if (!ok) {
    // single retry after yielding to network
    LOGF("Publish attempt failed for %s, retrying after client.loop()\n", topic);
    client.loop();
    delay(200);
    ok = client.publish(topic, payload, retain);
  }
  if (!ok) {
    LOGF("Publish FAILED for %s (connected=%d, client.state()=%d)\n", topic, client.connected(), client.state());
  }
  return ok;
}

// Baut "<DEVICE_ID>_<suffix>" (fuer unique_id) und
// "homeassistant/<domain>/<DEVICE_ID>_<suffix>/config" (fuer den Discovery-
// Topic) in vom Aufrufer bereitgestellte Puffer, statt wie zuvor bei jeder
// der ~28 Discovery-Funktionen zwei temporaere String-Objekte per
// Verkettung zu erzeugen. Dieser Burst von über 50 Kurzzeit-Allokationen
// bei jedem MQTT-(Re-)Connect fragmentierte den ohnehin knappen
// ESP8266-Heap spuerbar (siehe CHANGELOG).
static void buildHaIds(const char* domain, const char* suffix, char* idOut, size_t idOutSize, char* topicOut, size_t topicOutSize) {
  snprintf(idOut, idOutSize, "%s_%s", DEVICE_ID.c_str(), suffix);
  snprintf(topicOut, topicOutSize, "homeassistant/%s/%s/config", domain, idOut);
}

// --- Funktion, die Home Assistant per Discovery konfiguriert ---
bool publishOnOffConfig() {
    StaticJsonDocument<512> cfg;
  cfg["name"]          = "Wortuhr Power";
  char uniqueId[48], configTopic[80];
  buildHaIds("switch", "power", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicOnState;
  cfg["command_topic"] = topicOnCmd;
  cfg["payload_on"]    = "1";
  cfg["payload_off"]   = "0";

JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Buffer mit Null-Terminator
  char buf[512];
  size_t n = serializeJson(cfg, buf, sizeof(buf));

  // Debug: zeige JSON
  LOG("Config JSON: ");
  LOGLN(buf);

  // Publish und Ergebnis
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishEffectConfig() {
  // 1024 statt 512: die Optionsliste ist inzwischen auf 17 Eintraege
  // gewachsen (Rainbow Swipe/Cycle, "Zufällig aus Liste") - mit 512 Bytes
  // wurde das serialisierte JSON abgeschnitten, wodurch Home Assistant das
  // Discovery-Payload nicht mehr korrekt lesen konnte.
  StaticJsonDocument<1024> cfg;
  cfg["name"]         = "Übergangseffekt";
  char uniqueId[48], configTopic[80];
  buildHaIds("select", "efx", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicEfxState;
  cfg["command_topic"]= topicEfxCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 16
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < EFFECT_OPTIONS_COUNT; ++i) opts.add(effectOptions[i]);
  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[1024];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Effect Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishAnimationConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Animationseffekt";

  char uniqueId[48], configTopic[80];
  buildHaIds("select", "ani", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicAniState;
  cfg["command_topic"]= topicAniCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < ANI_OPTIONS_COUNT; ++i) opts.add(aniOptions[i]);

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishV1LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Vordergrundfarbe 1";

  char uniqueId[48], configTopic[80];
  buildHaIds("light", "v1", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicV1State;
  cfg["command_topic"] = topicV1Cmd;

  cfg["schema"]        = "json";   // JSON Schema!


  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishV2LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Vordergrundfarbe 2";

  char uniqueId[48], configTopic[80];
  buildHaIds("light", "v2", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicV2State;
  cfg["command_topic"] = topicV2Cmd;

  cfg["schema"]        = "json";   // JSON Schema!


  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishH1LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Hintergrundfarbe 1";

  char uniqueId[48], configTopic[80];
  buildHaIds("light", "h1", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicH1State;
  cfg["command_topic"] = topicH1Cmd;

  cfg["schema"]        = "json";   // JSON Schema!


  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishH2LightConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Hintergrundfarbe 2";

  char uniqueId[48], configTopic[80];
  buildHaIds("light", "h2", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicH2State;
  cfg["command_topic"] = topicH2Cmd;

  cfg["schema"]        = "json";   // JSON Schema!


  JsonArray modes = cfg.createNestedArray("supported_color_modes");
  modes.add("rgb");

  cfg["qos"] = 1;

  // Gerätedaten
  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("v1-Light Config Publish: %s\n%s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishVsConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Vordergrundfarbschema";

  char uniqueId[48], configTopic[80];
  buildHaIds("select", "vs", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicVsState;
  cfg["command_topic"]= topicVsCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < FARBSCHEMA_OPTIONS_COUNT; ++i) opts.add(farbschemaOptions[i]);

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishHsConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Hintergrundfarbschema";

  char uniqueId[48], configTopic[80];
  buildHaIds("select", "hs", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicHsState;
  cfg["command_topic"]= topicHsCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < FARBSCHEMA_OPTIONS_COUNT; ++i) opts.add(farbschemaOptions[i]);

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishEfxTimeConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Übergangsgeschwindigkeit";

  char uniqueId[48], configTopic[80];
  buildHaIds("select", "efxtime", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicEfxTimeState;
  cfg["command_topic"]= topicEfxTimeCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < EFFECTTIME_OPTIONS_COUNT; ++i) opts.add(effecttimeOptions[i]);

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishAniTimeConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Animationsgeschwindigkeit";

  char uniqueId[48], configTopic[80];
  buildHaIds("select", "anitime", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicAniTimeState;
  cfg["command_topic"]= topicAniTimeCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < EFFECTTIME_OPTIONS_COUNT; ++i) opts.add(effecttimeOptions[i]);

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}
bool publishAniDepthConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Animationsstärke";

  char uniqueId[48], configTopic[80];
  buildHaIds("select", "anidepth", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicAniDepthState;
  cfg["command_topic"]= topicAniDepthCmd;
  // Liste der Optionen – Reihenfolge entspricht 0 bis 9
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < EFFECTDEPTH_OPTIONS_COUNT; ++i) opts.add(effectdepthOptions[i]);

  // Optional: Optimistic, falls Dein Gerät den State nicht unmittelbar zurückmeldet
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

 JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  // Serialize mit Null-Terminator
  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  // Publish mit retain=true
  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("Ani Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishEffectsModeConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Effekte-Modus";
  char uniqueId[48], configTopic[80];
  buildHaIds("switch", "effectsmode", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicEfxModeState;
  cfg["command_topic"] = topicEfxModeCmd;
  cfg["payload_on"]    = "1";
  cfg["payload_off"]   = "0";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("EffectsMode Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishLightEffectConfig() {
  // Larger buffer than the other selects: 20 options (vs. 17 for effectOptions)
  // push this closer to the 512-byte size used elsewhere.
  StaticJsonDocument<1024> cfg;
  cfg["name"]         = "Lichteffekt";
  char uniqueId[48], configTopic[80];
  buildHaIds("select", "lighteffect", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicLightEffectState;
  cfg["command_topic"]= topicLightEffectCmd;
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < LIGHT_EFFECT_OPTIONS_COUNT; ++i) opts.add(lightEffectOptions[i]);
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[1024];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("LightEffect Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

bool publishLightEffectSpeedConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Lichteffekt-Geschwindigkeit";
  char uniqueId[48], configTopic[80];
  buildHaIds("select", "lighteffectspeed", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicLightEffectSpeedState;
  cfg["command_topic"]= topicLightEffectSpeedCmd;
  JsonArray opts = cfg.createNestedArray("options");
  for (int i = 0; i < EFFECTTIME_OPTIONS_COUNT; ++i) opts.add(effecttimeOptions[i]);
  cfg["optimistic"]     = false;
  cfg["qos"]            = 1;

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("LightEffectSpeed Config Publish: %s\nJSON: %s\n", ok ? "OK" : "FEHLER", buf);
  return ok;
}

// ============ Pomodoro-Modus Discovery Configs ============

bool publishPomodoroActiveConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Pomodoro";
  char uniqueId[48], configTopic[80];
  buildHaIds("switch", "pomodoroactive", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicPomodoroActiveState;
  cfg["command_topic"] = topicPomodoroActiveCmd;
  cfg["payload_on"]    = "1";
  cfg["payload_off"]   = "0";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("PomodoroActive Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishPomodoroActivityMinConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Pomodoro Aktivität (Minuten)";
  char uniqueId[48], configTopic[80];
  buildHaIds("number", "pomodoroactivitymin", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicPomodoroActivityMinState;
  cfg["command_topic"] = topicPomodoroActivityMinCmd;
  cfg["min"] = 1;
  cfg["max"] = 90;
  cfg["step"] = 1;
  cfg["unit_of_measurement"] = "min";
  cfg["mode"] = "box";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("PomodoroActivityMin Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishPomodoroPauseMinConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]          = "Pomodoro Pause (Minuten)";
  char uniqueId[48], configTopic[80];
  buildHaIds("number", "pomodoropausemin", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]     = uniqueId;
  cfg["state_topic"]   = topicPomodoroPauseMinState;
  cfg["command_topic"] = topicPomodoroPauseMinCmd;
  cfg["min"] = 1;
  cfg["max"] = 30;
  cfg["step"] = 1;
  cfg["unit_of_measurement"] = "min";
  cfg["mode"] = "box";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("PomodoroPauseMin Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishPomodoroPhaseConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Pomodoro Status";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "pomodorophase", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicPomodoroPhaseState;
  cfg["icon"]         = "mdi:timer-sand";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("PomodoroPhase Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

bool publishPomodoroRemainingConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Pomodoro Restzeit";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "pomodororemaining", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicPomodoroRemainingState;
  cfg["unit_of_measurement"] = "s";
  cfg["device_class"] = "duration";
  cfg["icon"]         = "mdi:timer-outline";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));

  bool ok = pubWithCheck(configTopic, buf, false);
  LOGF("PomodoroRemaining Config Publish: %s\n", ok ? "OK" : "FEHLER");
  return ok;
}

// ============ Sensor Discovery Configs ============

bool publishIpAddressSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "IP Adresse";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "ip_address", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicIpAddress;
  cfg["icon"]         = "mdi:ip";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishUptimeSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Uptime";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "uptime", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicUptime;
  cfg["unit_of_measurement"] = "s";
  cfg["device_class"] = "duration";
  cfg["icon"]         = "mdi:clock-outline";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishRssiSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "WiFi Signalstärke";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "rssi", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicRssi;
  cfg["unit_of_measurement"] = "dBm";
  cfg["device_class"] = "signal_strength";
  cfg["icon"]         = "mdi:wifi";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishHeapMemorySensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Freier Speicher";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "heap_memory", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicHeapMemory;
  cfg["unit_of_measurement"] = "Byte";
  cfg["device_class"] = "data_size";
  cfg["icon"]         = "mdi:memory";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishBrightnessSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "LED Helligkeit";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "brightness", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicBrightness;
  cfg["unit_of_measurement"] = "%";
  cfg["icon"]         = "mdi:brightness-6";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishLastNtpSyncSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "Letzter NTP Sync";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "last_ntp_sync", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicLastNtpSync;
  cfg["device_class"] = "timestamp";
  cfg["icon"]         = "mdi:clock-check";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishTemperatureSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "CPU Temperatur";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "temperature", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicTemperature;
  cfg["unit_of_measurement"] = "°C";
  cfg["device_class"] = "temperature";
  cfg["icon"]         = "mdi:thermometer";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}

bool publishSystemLoadSensorConfig() {
  StaticJsonDocument<512> cfg;
  cfg["name"]         = "System Auslastung";
  char uniqueId[48], configTopic[80];
  buildHaIds("sensor", "system_load", uniqueId, sizeof(uniqueId), configTopic, sizeof(configTopic));
  cfg["unique_id"]    = uniqueId;
  cfg["state_topic"]  = topicSystemLoad;
  cfg["unit_of_measurement"] = "%";
  cfg["icon"]         = "mdi:cpu-64-bit";
  cfg["entity_category"] = "diagnostic";

  JsonObject dev = cfg.createNestedObject("device");
  dev["identifiers"][0] = DEVICE_ID.c_str();
  dev["name"]           = DEVICE_NAME.c_str();
  dev["manufacturer"]   = DEVICE_VENDOR;
  dev["model"]          = DEVICE_MODEL;
  dev["sw_version"] = FW_VERSION;
  dev["configuration_url"] = CONFIG_URL.c_str();
  cfg["availability_topic"] = topicAvailability;

  char buf[512];
  serializeJson(cfg, buf, sizeof(buf));
  bool ok = pubWithCheck(configTopic, buf, false);
  return ok;
}
