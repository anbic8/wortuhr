#include "globals_state.h"

/* Globals */
time_t now;                         // this are the seconds since Epoch (1970) - UTC
struct tm tm;                              // the structure tm holds time information in a more convenient way

int mode=1;   //mode 0 for debugging mode 1 normaler modus
int settimemode=0; //mode für RCT Mode
int zeit; //
int hours=0, minutes=0, stunden=0, seconds=0, year=0, month=0, day=0;
int h=0, m=0, mb=0;
unsigned long letzterstand=0;
unsigned long letzterani=0;
unsigned long milliaktuell=0;
unsigned long warten=60000; //warten für die Uhr.
unsigned long threshold=0, zeitneu2=0;
unsigned long lastNtpSync=0; // Last NTP sync timestamp
int on = 1;  // Nachtmodus 0 für Nachtmodus 1 für Uhrzeit normal
int gebstat=0;

int mqtton=1;
int mqttonset=0;
