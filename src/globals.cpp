#include "globals.h"

//Tasten
const int bt1Pin = 4; //version alte Platine pins 12, 14
const int bt2Pin = 14; // version neue Platine pins 4, 14
const int bt3Pin = 12; // Btn 1 in alter Version

OneButton bt1(bt1Pin, true); //button ist aktiv wenn Low
OneButton bt2(bt2Pin, true);
OneButton bt3(bt3Pin, true);