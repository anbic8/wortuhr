#ifndef RCT_H
#define RCT_H

#include <Arduino.h>

void readTimeRCT();
void setDate(int s, int m, int h, int d, int month, int y);
int decToBcd(int value);
int bcdToDec(int value);

#endif // RCT_H