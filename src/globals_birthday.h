#ifndef GLOBALS_BIRTHDAY_H
#define GLOBALS_BIRTHDAY_H

extern int geburtstage[5][3];

// Whether Home Assistant discovery publish should run on next MQTT connect
extern bool discoveryNeeded;
extern bool haDiscoveryEnabled;

// Countdown timestamp (epoch seconds). Stored in EEPROM.
extern unsigned long countdown_ts;
extern unsigned long newyear_countdown_ts;

#endif // GLOBALS_BIRTHDAY_H
