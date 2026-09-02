#include "globals_buttons.h"

// Defaults entsprechen dem bisherigen fest verdrahteten Verhalten fuer
// Taste 1 (Klick=Helligkeit, lang=Nachtmodus), Taste 2 bekommt neue,
// sinnvollere Defaults statt der bisherigen Demo-/No-op-Funktionen.
uint8_t btn1ClickFunction = 1; // Helligkeit erhoehen
uint8_t btn1LongFunction = 2;  // Nachtmodus umschalten
uint8_t btn2ClickFunction = 5; // Naechster Uebergangseffekt
uint8_t btn2LongFunction = 3;  // Pomodoro umschalten

const char* buttonFunctionOptions[6] = {
  "Keine Funktion",
  "Helligkeit erhöhen",
  "Nachtmodus umschalten",
  "Pomodoro umschalten",
  "Effekte-Modus umschalten",
  "Nächster Übergangseffekt"
};
