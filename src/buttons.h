#include "OneButton.h"
void bt1click();
void bt1double();
void bt1longs();
void bt2click();
void bt2double();
void bt2longs();

void showwhilesetting();

// Fuehrt eine der konfigurierbaren Tasten-Funktionen aus (siehe
// globals_buttons.h buttonFunctionOptions). Nur fuer NTP-Builds relevant -
// bei RTC-Builds bleiben die Tasten fest fuer die Uhrzeiteinstellung
// reserviert und rufen diese Funktion nicht auf.
void applyButtonFunction(uint8_t fn);