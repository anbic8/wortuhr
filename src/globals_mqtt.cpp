#include "globals_mqtt.h"
#include "globals_network.h" // DEVICE_ID/DEVICE_NAME/dns_name/CONFIG_URL
#include "persistence.h"     // user_connect.mqtt_prefix
#include "log.h"

String topicOnState;
String topicOnCmd;
String topicEfxState;
String topicEfxCmd;
const char* effectOptions[17] = {
  "kein Effekt",
  "zufällig",
  "Fade",
  "Running",
  "Schlange",
  "Zeilen",
  "Scrollen",
  "Slide in",
  "Diagonal",
  "Rain",
  "Spirale",
  "Schlangenfresser",
  "Raute",
  "Feuerwerk",
  "Rainbow Swipe",
  "Rainbow Cycle",
  "Zufällig aus Liste"
};
String topicAniState;
String topicAniCmd;
const char* aniOptions[7] = {
    "keine Animation", "Blinken", "Vordergrundblinken", "Pulsieren", "Verlauf","Fliegen", "Glitter"
};
// --- Topics für Licht v1 ---
String topicV1State;
String topicV1Cmd;
// --- Topics für Licht v2 ---
String topicV2State;
String topicV2Cmd;

// --- Topics für Licht h1 ---
String topicH1State;
String topicH1Cmd;
// --- Topics für Licht h2 ---
String topicH2State;
String topicH2Cmd;
// --- Topics für Lichtschema vs ---
String topicVsState;
String topicVsCmd;
// --- Topics für Lichtschema hs ---
String topicHsState;
String topicHsCmd;

const char* farbschemaOptions[6] = {
  "einfarbig",
  "Schachbrett",
  "Spalten",
  "Zeilen",
  "Verlauf",
  "Zufällig"
};
// --- Topics für efx time  ---
String topicEfxTimeState;
String topicEfxTimeCmd;
const char* effecttimeOptions[3] = {
  "langsam",
  "mittel",
  "schnell"
};
// --- Topics für ani time  ---
String topicAniTimeState;
String topicAniTimeCmd;
// --- Topics für ani depth  ---
String topicAniDepthState;
String topicAniDepthCmd;
const char* effectdepthOptions[3] = {
  "schwach",
  "mittel",
  "stark"
};

// --- Topics für Effekte-Modus (WLED-artige Matrixeffekte statt der Uhr) ---
String topicEfxModeState;
String topicEfxModeCmd;
String topicLightEffectState;
String topicLightEffectCmd;
const char* lightEffectOptions[20] = {
  "Rainbow Cycle",
  "Theater Chase",
  "Theater Chase Rainbow",
  "Color Wipe",
  "Breathing",
  "Sparkle",
  "Comet",
  "Fire",
  "Plasma",
  "Confetti",
  "Diagonal Rainbow Sweep",
  "Color Wave",
  "Larson Scanner",
  "Random Color Fade",
  "Bouncing Balls",
  "TV Static",
  "Matrix Rain",
  "Sunrise Sunset",
  "Strobe",
  "Color Wheel Pulse"
};
String topicLightEffectSpeedState;
String topicLightEffectSpeedCmd;

// --- Topics für den Pomodoro-Modus ---
String topicPomodoroActiveState;
String topicPomodoroActiveCmd;
String topicPomodoroActivityMinState;
String topicPomodoroActivityMinCmd;
String topicPomodoroPauseMinState;
String topicPomodoroPauseMinCmd;
String topicPomodoroPhaseState;
String topicPomodoroRemainingState;

String topicAvailability;

// --- Topics für Sensoren ---
String topicIpAddress;
String topicUptime;
String topicRssi;
String topicHeapMemory;
String topicBrightness;
String topicLastNtpSync;
String topicTemperature;
String topicSystemLoad;

WiFiClient espClient;
PubSubClient client(espClient);

// Build MQTT topics based on mqtt_prefix from settings
void buildMqttTopics() {
  String prefix = String(user_connect.mqtt_prefix);

  // Default to "wortuhr" if prefix is empty or invalid
  if (prefix.length() == 0 || (uint8_t)user_connect.mqtt_prefix[0] == 0xFF) {
    prefix = "wortuhr";
  }

  // Update DEVICE_ID, DEVICE_NAME, dns_name and CONFIG_URL from mqtt_prefix
  // Remove trailing slash for device ID
  String cleanPrefix = prefix;
  if (cleanPrefix.endsWith("/")) {
    cleanPrefix = cleanPrefix.substring(0, cleanPrefix.length() - 1);
  }
  DEVICE_ID = cleanPrefix;
  DEVICE_NAME = "Wortuhr " + cleanPrefix;
  dns_name = cleanPrefix;
  CONFIG_URL = "http://" + cleanPrefix + ".local";

  // Ensure prefix ends with "/"
  if (!prefix.endsWith("/")) {
    prefix += "/";
  }

  // Build all topics with the prefix
  topicOnState = prefix + "on";
  topicOnCmd = prefix + "on/set";

  topicEfxState = prefix + "efx";
  topicEfxCmd = prefix + "efx/set";

  topicAniState = prefix + "ani";
  topicAniCmd = prefix + "ani/set";

  topicV1State = prefix + "v1";
  topicV1Cmd = prefix + "v1/set";

  topicV2State = prefix + "v2";
  topicV2Cmd = prefix + "v2/set";

  topicH1State = prefix + "h1";
  topicH1Cmd = prefix + "h1/set";

  topicH2State = prefix + "h2";
  topicH2Cmd = prefix + "h2/set";

  topicVsState = prefix + "vs";
  topicVsCmd = prefix + "vs/set";

  topicHsState = prefix + "hs";
  topicHsCmd = prefix + "hs/set";

  topicEfxTimeState = prefix + "efxtime";
  topicEfxTimeCmd = prefix + "efxtime/set";

  topicAniTimeState = prefix + "anitime";
  topicAniTimeCmd = prefix + "anitime/set";

  topicAniDepthState = prefix + "anidepth";
  topicAniDepthCmd = prefix + "anidepth/set";

  topicEfxModeState = prefix + "effectsmode";
  topicEfxModeCmd = prefix + "effectsmode/set";

  topicLightEffectState = prefix + "lighteffect";
  topicLightEffectCmd = prefix + "lighteffect/set";

  topicLightEffectSpeedState = prefix + "lighteffectspeed";
  topicLightEffectSpeedCmd = prefix + "lighteffectspeed/set";

  topicPomodoroActiveState = prefix + "pomodoroactive";
  topicPomodoroActiveCmd = prefix + "pomodoroactive/set";
  topicPomodoroActivityMinState = prefix + "pomodoroactivitymin";
  topicPomodoroActivityMinCmd = prefix + "pomodoroactivitymin/set";
  topicPomodoroPauseMinState = prefix + "pomodoropausemin";
  topicPomodoroPauseMinCmd = prefix + "pomodoropausemin/set";
  topicPomodoroPhaseState = prefix + "pomodorophase";
  topicPomodoroRemainingState = prefix + "pomodororemaining";

  topicAvailability = prefix + "availability";

  // Sensor topics
  topicIpAddress = prefix + "ip_address";
  topicUptime = prefix + "uptime";
  topicRssi = prefix + "rssi";
  topicHeapMemory = prefix + "heap_memory";
  topicBrightness = prefix + "brightness";
  topicLastNtpSync = prefix + "last_ntp_sync";
  topicTemperature = prefix + "temperature";
  topicSystemLoad = prefix + "system_load";

  LOGLN("MQTT Topics initialized with prefix: " + prefix);
}

int findOptionIndex(const char* const* options, int count, const String& value) {
  for (int i = 0; i < count; ++i) {
    if (value == options[i]) return i;
  }
  return -1;
}
