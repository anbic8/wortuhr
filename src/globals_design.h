#ifndef GLOBALS_DESIGN_H
#define GLOBALS_DESIGN_H

#include <Arduino.h>

extern int dbv;
extern int dvv;
extern int uvv;
extern int aus;
extern int an;
extern int nacht;
extern int sommerzeit;
extern int dimm;

extern int v1;
extern int v2;
extern int h1;
extern int h2;

extern int vf1[3];
extern int vf2[3];
extern int hf1[3];
extern int hf2[3];
extern int effectMode;
extern int efxtime;

extern int aniMode;
extern unsigned long anitime;
extern int anidepth;
extern int anistate;

// "Zufällig aus Liste" beim Übergangseffekt (effectMode ==
// EFFECT_RANDOM_FROM_LIST_INDEX): Bit i gesetzt = Effekt-Index i (2..13)
// ist Teil der Zufallsauswahl. Nur über die Web-UI konfigurierbar.
extern uint16_t effectRandomPoolMask;

extern String htmlfarben[14];
extern int htmlefxtimeint[3];
extern int efxtimeint;

extern int htmlanitimeint[7][3];
extern int anitimeint;
extern int flypos[3][2];

// Effekte-Modus: zeigt fortlaufende WLED-artige Lichteffekte statt der Uhr.
// Persistiert in EEPROM (EepromLayout::LIGHT_EFFECTS_ENABLED_OFFSET /
// LIGHT_EFFECT_INDEX_OFFSET), unabhängig von der (ungenutzten) mode-Variable.
extern bool effectsModeActive;
extern uint8_t selectedLightEffect; // 0..LIGHT_EFFECT_OPTIONS_COUNT-1

// Geschwindigkeit der Lichteffekte (0=langsam,1=mittel,2=schnell - nutzt
// dieselben Labels wie effecttimeOptions).
extern uint8_t lightEffectSpeedIdx;

// Minutengenaue Anzeige (nur 8x8 Mini): zeigt in der untersten Zeile
// zusätzliche "Minuten-Pixel" in einer eigenen Farbe (siehe show.cpp
// setmatrixanzeige()/showClock()). Nur über die Web-UI (/setting) konfigurierbar.
extern bool minuteDotsEnabled;
extern int minuteDotsColorIdx; // Palettenindex 0..13

#endif // GLOBALS_DESIGN_H
