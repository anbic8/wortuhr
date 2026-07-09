#include <Adafruit_NeoPixel.h>

void readTime();
void readTimeNet();
void checkon();
void showClock();
void setmatrixanzeige();
void setanzeige();
#if MATRIX_SIZE == 8
void minuteDotColumnsForHour(int hour, int cols[4]);
#endif