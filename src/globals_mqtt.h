#ifndef GLOBALS_MQTT_H
#define GLOBALS_MQTT_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

extern String topicOnState;
extern String topicOnCmd;
extern String topicEfxState;
extern String topicEfxCmd;
extern const char* effectOptions[14];
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
constexpr int EFFECT_OPTIONS_COUNT = 14;
constexpr int ANI_OPTIONS_COUNT = 7;
constexpr int FARBSCHEMA_OPTIONS_COUNT = 6;
constexpr int EFFECTTIME_OPTIONS_COUNT = 3;
constexpr int EFFECTDEPTH_OPTIONS_COUNT = 3;

// Looks up `value` among the first `count` entries of `options`.
// Returns -1 if no entry matches.
int findOptionIndex(const char* const* options, int count, const String& value);

#endif // GLOBALS_MQTT_H
