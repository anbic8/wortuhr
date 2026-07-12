#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>

// Baut pomodoroOrder[] einmalig anhand der gewaehlten Animations-Strategie.
void buildPomodoroOrder(uint8_t animationIdx);

// Startet/stoppt eine Pomodoro-Sitzung (Web-UI/MQTT-Einstiegspunkte).
void startPomodoro();
void stopPomodoro();

// Rendert den aktuellen Pomodoro-Zustand - wird jeden loop()-Durchlauf
// aufgerufen (self-paced ueber millis()), analog zu renderLightEffects().
void renderPomodoro();

// Sekunden, die in der aktuellen Phase noch verbleiben (0 wenn inaktiv).
long getPomodoroRemainingSeconds();

#endif // POMODORO_H
