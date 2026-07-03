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

extern String htmlfarben[14];
extern int htmlefxtimeint[3];
extern int efxtimeint;

extern int htmlanitimeint[7][3];
extern int anitimeint;
extern int flypos[3][2];

#endif // GLOBALS_DESIGN_H
