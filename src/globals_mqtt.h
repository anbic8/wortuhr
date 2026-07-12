#ifndef GLOBALS_MQTT_H
#define GLOBALS_MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern String topicOnState;
extern String topicOnCmd;
extern String topicEfxState;
extern String topicEfxCmd;
// 17 Einträge: die 16 bisherigen Übergangseffekte + "Zufällig aus Liste"
extern const char* effectOptions[17];
extern String topicAniState;
extern String topicAniCmd;
extern const char* aniOptions[7];
extern String topicV1State;
extern String topicV1Cmd;
extern String topicV2State;
extern String topicV2Cmd;
extern String topicH1State;
extern String topicH1Cmd;
extern String topicH2State;
extern String topicH2Cmd;
extern String topicVsState;
extern String topicVsCmd;
extern String topicHsState;
extern String topicHsCmd;
extern const char* farbschemaOptions[6];
extern String topicEfxTimeState;
extern String topicEfxTimeCmd;
extern const char* effecttimeOptions[3];
extern String topicAniTimeState;
extern String topicAniTimeCmd;
extern String topicAniDepthState;
extern String topicAniDepthCmd;
extern const char* effectdepthOptions[3];

// --- Topics für Effekte-Modus (WLED-artige Matrixeffekte statt der Uhr) ---
extern String topicEfxModeState;
extern String topicEfxModeCmd;
extern String topicLightEffectState;
extern String topicLightEffectCmd;
extern const char* lightEffectOptions[20];
extern String topicLightEffectSpeedState;
extern String topicLightEffectSpeedCmd;

// --- Topics für den Pomodoro-Modus ---
extern String topicPomodoroActiveState;
extern String topicPomodoroActiveCmd;
extern String topicPomodoroActivityMinState;
extern String topicPomodoroActivityMinCmd;
extern String topicPomodoroPauseMinState;
extern String topicPomodoroPauseMinCmd;
extern String topicPomodoroPhaseState;      // nur State, kein /set
extern String topicPomodoroRemainingState;  // nur State, kein /set

// MQTT Last-Will / Verfügbarkeits-Topic: "online" (retained) nach jedem
// erfolgreichen Connect, vom Broker automatisch auf "offline" gesetzt,
// wenn die Verbindung unsauber abbricht (Stromausfall, WLAN-Verlust).
extern String topicAvailability;

extern String topicIpAddress;
extern String topicUptime;
extern String topicRssi;
extern String topicHeapMemory;
extern String topicBrightness;
extern String topicLastNtpSync;
extern String topicTemperature;
extern String topicSystemLoad;

extern WiFiClient espClient;
extern PubSubClient client;

// Build MQTT topics with prefix
void buildMqttTopics();

// Single source of truth for the option-array sizes above - used by the
// incoming MQTT callback, outgoing state publishers, the web UI dropdowns
// and Home Assistant discovery, so all four stay in sync.
constexpr int EFFECT_OPTIONS_COUNT = 17;
constexpr int ANI_OPTIONS_COUNT = 7;
constexpr int FARBSCHEMA_OPTIONS_COUNT = 6;
constexpr int EFFECTTIME_OPTIONS_COUNT = 3;
constexpr int EFFECTDEPTH_OPTIONS_COUNT = 3;
constexpr int LIGHT_EFFECT_OPTIONS_COUNT = 20;
// Index der "Zufällig aus Liste"-Option innerhalb effectOptions
// (Übergangseffekt der Uhr-Anzeige, nicht der Effekte-Modus).
constexpr int EFFECT_RANDOM_FROM_LIST_INDEX = 16;

// Looks up `value` among the first `count` entries of `options`.
// Returns -1 if no entry matches.
int findOptionIndex(const char* const* options, int count, const String& value);

#endif // GLOBALS_MQTT_H
