#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

// Gate all firmware debug output behind one build flag. Default: on
// (matches the previous always-on Serial.print* behavior). A leaner/
// faster release build can pass -DLOG_ENABLED=0 in platformio.ini.
#ifndef LOG_ENABLED
#define LOG_ENABLED 1
#endif

#if LOG_ENABLED
  #define LOG(...)   Serial.print(__VA_ARGS__)
  #define LOGLN(...) Serial.println(__VA_ARGS__)
  #define LOGF(...)  Serial.printf(__VA_ARGS__)
#else
  #define LOG(...)
  #define LOGLN(...)
  #define LOGF(...)
#endif

#endif // LOG_H
