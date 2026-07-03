#include "globals_design.h"

int dbv = 0; //Frontversion 0 für deutsch 1 für bayrisch
int dvv = 0; // Anzeige von xx:45 0 für dreiviertel 1 für viertel vor
int uvv = 0; // Anzeige der Stunde 0 für immer 1 für zur vollen Stunde 2 für nie
int aus = 0; // minuten nach Mitternacht an dem der Nachtmodus anfängt
int an = 0;  // minuten nach Mitternacht an dem der Nachtmodus aufhört
int nacht = 0; // Art Nachtmodus 0 für aus 1 für gedimmt
int sommerzeit; // Sommerzeit für RCT
int dimm = 250; // Dimm-Faktor

int v1=0;
int v2=13;
int h1=13;
int h2=13;

int vf1[3]={128,128,128};
int vf2[3]={128,128,0};
int hf1[3]={0,0,0};
int hf2[3]={0,0,0};
int effectMode=0;
int efxtime=100;

int aniMode=0;
unsigned long anitime=1000;
int anidepth=3;
int anistate=0;

String htmlfarben[14]={"weiß","rot","rosa","magenta","violet","blau","azure","turkis","hellgrün","grün","gelbgrün","gelb","orange","aus"};
int htmlefxtimeint[3]={150,100,50};
int efxtimeint=0;

int htmlanitimeint[7][3]={
    {0,0,0},
    {1500, 1000, 500},
    {1500, 1000, 500},
    {400, 250, 125},
    {400, 250, 125},
    {400, 250, 125},
    {500, 300, 150}
    };
int anitimeint=0;
int flypos[3][2]={
  {5,5},
  {5,5},
  {5,5}
};
