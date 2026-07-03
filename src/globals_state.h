#ifndef GLOBALS_STATE_H
#define GLOBALS_STATE_H

#include <Arduino.h>
#include <time.h>

// Time variables
extern time_t now;
extern struct tm tm;

// Mode and state variables
extern int mode;
extern int settimemode;
extern int zeit;
extern int hours, minutes, stunden, seconds, year, month, day;
extern int h, m, mb;
extern unsigned long letzterstand;
extern unsigned long letzterani;
extern unsigned long milliaktuell;
extern unsigned long warten;
extern unsigned long threshold, zeitneu2;
extern unsigned long lastNtpSync; // Last NTP sync timestamp (millis)
extern int on;
extern int gebstat;

extern int mqtton;
extern int mqttonset;

#endif // GLOBALS_STATE_H
