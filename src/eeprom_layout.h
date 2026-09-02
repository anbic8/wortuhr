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

// Light-effects mode state, appended purely additively after the layout
// version byte. This does NOT shift any offset above, so it needs no
// CURRENT_LAYOUT_VERSION bump and has no interaction with the credential
// encryption migration gated on that version (see main.cpp).
constexpr int LIGHT_EFFECTS_ENABLED_OFFSET = LAYOUT_VERSION_OFFSET + 1;
constexpr int LIGHT_EFFECT_INDEX_OFFSET = LIGHT_EFFECTS_ENABLED_OFFSET + 1;

// Effect speed (0/1/2, reuses effecttimeOptions labels) for the light
// effects. Again purely additive - see note above.
constexpr int LIGHT_EFFECT_SPEED_OFFSET = LIGHT_EFFECT_INDEX_OFFSET + 1;

// "Zufällig aus Liste" random-pool bitmask for the clock's transition
// effect (effectMode == EFFECT_RANDOM_FROM_LIST_INDEX; bit i = effect
// index i included, i in 2..15). Also purely additive.
constexpr int EFFECT_RANDOM_POOL_MASK_OFFSET = LIGHT_EFFECT_SPEED_OFFSET + 1;

// OTA-Update-Passwort (HTTP Basic Auth auf /update, /upload). Ein neu
// angeschlossenes Geraet hat OTA_PASSWORD_SET_OFFSET == 0xFF (erased) ->
// Update bleibt offen, bis der Nutzer ueber die Web-UI ein Passwort setzt.
// Der eigentliche Passwort-Puffer wird geraete-gebunden verschluesselt
// gespeichert (siehe secure_storage.h), da es nie eine Klartext-Aera fuer
// dieses Feld gab, ist keine Versions-Gate-Logik wie bei den WLAN/MQTT-
// Zugangsdaten noetig.
constexpr int OTA_PASSWORD_SET_OFFSET = EFFECT_RANDOM_POOL_MASK_OFFSET + 2;
constexpr int OTA_PASSWORD_OFFSET = OTA_PASSWORD_SET_OFFSET + 1;
constexpr int OTA_PASSWORD_MAX = 20;

// Minutengenaue Anzeige (nur 8x8 Mini): Enable-Flag + Palettenindex fuer
// die Farbe der Minuten-Pixel in der untersten Zeile. Rein additiv.
constexpr int MINUTE_DOTS_ENABLED_OFFSET = OTA_PASSWORD_OFFSET + OTA_PASSWORD_MAX;
constexpr int MINUTE_DOTS_COLOR_OFFSET = MINUTE_DOTS_ENABLED_OFFSET + 1;

// Pomodoro-Modus: Aktivitaets-/Pausenminuten, Anzeige-Schema, Aktivierungs-
// Animation und vier eigene Farben (Aktivitaet 1/2, Pause 1/2) als
// Palettenindizes. Der Laufzeitstatus (aktiv/Phase/Restzeit) wird bewusst
// NICHT persistiert (RAM-only, wie newyear_countdown_ts) - nur die
// Einstellungen. Rein additiv.
constexpr int POMODORO_ACTIVITY_MIN_OFFSET    = MINUTE_DOTS_COLOR_OFFSET + 1;
constexpr int POMODORO_PAUSE_MIN_OFFSET       = POMODORO_ACTIVITY_MIN_OFFSET + 1;
constexpr int POMODORO_SCHEME_OFFSET          = POMODORO_PAUSE_MIN_OFFSET + 1;
constexpr int POMODORO_ANIMATION_OFFSET       = POMODORO_SCHEME_OFFSET + 1;
constexpr int POMODORO_ACTIVITY_COLOR1_OFFSET = POMODORO_ANIMATION_OFFSET + 1;
constexpr int POMODORO_ACTIVITY_COLOR2_OFFSET = POMODORO_ACTIVITY_COLOR1_OFFSET + 1;
constexpr int POMODORO_PAUSE_COLOR1_OFFSET    = POMODORO_ACTIVITY_COLOR2_OFFSET + 1;
constexpr int POMODORO_PAUSE_COLOR2_OFFSET    = POMODORO_PAUSE_COLOR1_OFFSET + 1;

// Konfigurierbare Tasten-Funktionen (nur auf NTP-Builds editierbar; RTC-
// Builds lassen diese Bytes ungenutzt, da ihre Tasten fest fuer die
// Uhrzeiteinstellung reserviert sind). Rein additiv.
constexpr int BTN1_CLICK_FUNCTION_OFFSET = POMODORO_PAUSE_COLOR2_OFFSET + 1;
constexpr int BTN1_LONG_FUNCTION_OFFSET  = BTN1_CLICK_FUNCTION_OFFSET + 1;
constexpr int BTN2_CLICK_FUNCTION_OFFSET = BTN1_LONG_FUNCTION_OFFSET + 1;
constexpr int BTN2_LONG_FUNCTION_OFFSET  = BTN2_CLICK_FUNCTION_OFFSET + 1;

constexpr int TOTAL_SIZE = BTN2_LONG_FUNCTION_OFFSET + 1;

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
