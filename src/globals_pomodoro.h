#ifndef GLOBALS_POMODORO_H
#define GLOBALS_POMODORO_H

#include <Arduino.h>

#ifndef MATRIX_SIZE
#define MATRIX_SIZE 11
#endif

// Persisted settings (EEPROM, see eeprom_layout.h POMODORO_* offsets)
extern uint8_t pomodoroActivityMinutes;   // 1..90, default 25
extern uint8_t pomodoroPauseMinutes;      // 1..30, default 5
extern uint8_t pomodoroScheme;            // farbschemaOptions index
extern uint8_t pomodoroAnimationIdx;      // pomodoroAnimationOptions index
extern int pomodoroActivityColor1Idx, pomodoroActivityColor2Idx; // Palettenindex 0..13
extern int pomodoroPauseColor1Idx, pomodoroPauseColor2Idx;       // Palettenindex 0..13

// RAM-only Laufzeitstatus - wird nie in EEPROM gespeichert, startet nach
// jedem Boot immer deaktiviert (siehe eeprom_layout.h Kommentar).
extern bool pomodoroModeActive;
extern int pomodoroPhase; // 0 = Aktivitaet, 1 = Pause
extern unsigned long pomodoroPhaseStartMs;
extern uint8_t pomodoroOrder[MATRIX_SIZE * MATRIX_SIZE][2];
extern int pomodoroLitCount;

extern const char* pomodoroAnimationOptions[6];
constexpr int POMODORO_ANIMATION_OPTIONS_COUNT = 6;

#endif // GLOBALS_POMODORO_H
