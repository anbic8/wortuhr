#include "globals_birthday.h"

int geburtstage[5][3];

// Whether Home Assistant discovery publish should run on next MQTT connect
bool discoveryNeeded = true;
// Home Assistant discovery enabled (persisted in EEPROM)
bool haDiscoveryEnabled = false;

// Countdown timestamp (epoch seconds). 0 = disabled
unsigned long countdown_ts = 0;
unsigned long newyear_countdown_ts = 0;
