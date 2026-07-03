#ifndef EEPROM_LAYOUT_H
#define EEPROM_LAYOUT_H

#include "globals.h"

// Single source of truth for the EEPROM byte layout. Every block's offset is
// derived here once instead of being re-computed independently in main.cpp,
// color.cpp, show.cpp, birthday.cpp, buttons.cpp and webserver.cpp.
//
// Layout (unchanged from the pre-existing scheme, byte-compatible with
// devices already in the field) with one new trailing byte appended:
//   settings -> MyColor -> design -> geburtstage[5][3] -> countdown_ts
//   -> FW version string -> HA-discovery flag -> layout version (NEW)
namespace EepromLayout {

constexpr int SETTINGS_OFFSET = 0;
constexpr int COLOR_OFFSET = SETTINGS_OFFSET + sizeof(settings);
constexpr int DESIGN_OFFSET = COLOR_OFFSET + sizeof(MyColor);
constexpr int BIRTHDAY_OFFSET = DESIGN_OFFSET + sizeof(design);
constexpr int COUNTDOWN_OFFSET = BIRTHDAY_OFFSET + sizeof(geburtstage);
constexpr int VERSION_STR_OFFSET = COUNTDOWN_OFFSET + sizeof(unsigned long);
constexpr int HA_FLAG_OFFSET = VERSION_STR_OFFSET + VERSION_STR_MAX;
constexpr int LAYOUT_VERSION_OFFSET = HA_FLAG_OFFSET + 1;
constexpr int TOTAL_SIZE = LAYOUT_VERSION_OFFSET + 1;

// Bump this whenever the *shape* of the layout changes in the future.
// A freshly-erased/legacy device reads 0xFF here (never written), which
// is used as the "pre-encryption, plaintext credentials" sentinel.
constexpr uint8_t CURRENT_LAYOUT_VERSION = 2;

// EEPROM.begin() sized for the whole layout.
void beginAll();

// Reads/writes the trailing layout-version byte.
uint8_t readLayoutVersion();
void writeLayoutVersion(uint8_t version);

// Erases the entire layout back to 0xFF (used by the factory-reset action;
// replaces the previously undersized/unused clearEEPROM() calculation).
void eraseAll();

} // namespace EepromLayout

#endif // EEPROM_LAYOUT_H
