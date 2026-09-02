#ifndef GLOBALS_BUTTONS_H
#define GLOBALS_BUTTONS_H

#include <Arduino.h>

// Konfigurierbare Tasten-Funktionen (nur NTP-Builds; siehe buttons.cpp
// applyButtonFunction()). Werte sind Indizes in buttonFunctionOptions.
extern uint8_t btn1ClickFunction;
extern uint8_t btn1LongFunction;
extern uint8_t btn2ClickFunction;
extern uint8_t btn2LongFunction;

extern const char* buttonFunctionOptions[6];
constexpr int BUTTON_FUNCTION_OPTIONS_COUNT = 6;

#endif // GLOBALS_BUTTONS_H
