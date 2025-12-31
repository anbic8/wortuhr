#ifndef RCT_H
#define RCT_H

#include <Arduino.h>

#define DS1307_ADDRESS 0x68 // I²C Address

void readTimeRCT();
void setDate(int s, int m, int h, int d, int month, int y);
int decToBcd(int value);
int bcdToDec(int value);

#endif // RCT_H