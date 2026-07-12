#include "globals_pomodoro.h"

uint8_t pomodoroActivityMinutes = 25;
uint8_t pomodoroPauseMinutes = 5;
uint8_t pomodoroScheme = 0;      // einfarbig
uint8_t pomodoroAnimationIdx = 0; // Zeilen

// Aktivitaet warm (Orange/Rot), Pause kuehl (Blau/Hellblau) - bewusst
// unterschiedlich, damit die Phase auf den ersten Blick erkennbar ist.
int pomodoroActivityColor1Idx = 12;
int pomodoroActivityColor2Idx = 1;
int pomodoroPauseColor1Idx = 5;
int pomodoroPauseColor2Idx = 6;

bool pomodoroModeActive = false;
int pomodoroPhase = 0;
unsigned long pomodoroPhaseStartMs = 0;
uint8_t pomodoroOrder[MATRIX_SIZE * MATRIX_SIZE][2];
int pomodoroLitCount = 0;

const char* pomodoroAnimationOptions[6] = {
  "Zeilen",
  "Spirale",
  "Diagonal",
  "Raute",
  "Fallend",
  "Zufall"
};
