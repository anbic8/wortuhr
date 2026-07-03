#ifndef GLOBALS_DISPLAY_H
#define GLOBALS_DISPLAY_H

#include <Arduino.h>
#include <pgmspace.h>

#ifndef LED_COUNT
#define LED_COUNT 121
#endif
#ifndef MATRIX_SIZE
#define MATRIX_SIZE 11
#endif

#define MAX_WORD_LEDS 15

// Palette
extern const int anzahlfarben;
extern const uint8_t farben[][3] PROGMEM;
// Read palette color from PROGMEM into int array[3]
void getPaletteColor(uint8_t idx, int out[3]);

// Matrix mappings
extern int matrixminmap[12][9];
extern const int matrixminmodulomap[4];
extern int ste;
extern int iist;  // Set by VERSION_TYPE at compile time
extern int matrixstundenmap[12][3];
extern int nexthour;  // Set by VERSION_TYPE at compile time

extern int baymatrixminmap[12][9];
extern int baymatrixstundenmap[12][3];

extern int vordergrundschema;
extern int vordergrund[MATRIX_SIZE][MATRIX_SIZE][3];
extern int hintergrundschema;
extern int hintergrund[MATRIX_SIZE][MATRIX_SIZE][3];
extern int anzeige[MATRIX_SIZE][MATRIX_SIZE][3];
extern int anzeigealt[MATRIX_SIZE][MATRIX_SIZE][3];
extern int matrix[MATRIX_SIZE][MATRIX_SIZE];
extern int matrixanzeige[MATRIX_SIZE][MATRIX_SIZE];

// LED index mappings (built at startup)
extern int16_t matrixmin_leds[12][MAX_WORD_LEDS];
extern uint8_t matrixmin_count[12];
extern int16_t matrixstunden_leds[12][MAX_WORD_LEDS];
extern uint8_t matrixstunden_count[12];

// inverse mapping: led index -> row/col in matrix
extern int16_t ledRow[LED_COUNT];
extern int16_t ledCol[LED_COUNT];

// build led mapping after any change to the textual maps
void buildLedMappings();
// validate runtime led lists (checks bounds)
bool validateLedLists();
// helper getters
uint8_t getWordLedCount(uint8_t minuteOrHourIndex, bool isHour);
int16_t getWordLed(uint8_t minuteOrHourIndex, bool isHour, uint8_t idx);

// Animation arrays (only for 11x11 matrix)
#if MATRIX_SIZE == 11
extern int t1[11];
extern int t2[28];
extern int t3[36];
extern int t4[26];
extern int t5[12];
extern int startcolors[13][3];
#endif

#endif // GLOBALS_DISPLAY_H
