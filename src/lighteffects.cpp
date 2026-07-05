#include "lighteffects.h"
#include "globals.h"
#include <math.h>

// Classic hue (0-255) -> RGB color-wheel helper. Nothing like this existed
// in the codebase before (color.cpp only interpolates between two fixed
// palette entries) - needed for the rainbow-family effects below.
static uint32_t effectsColorWheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return strip.Color(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return strip.Color(pos * 3, 255 - pos * 3, 0);
}

// Fades every currently-lit pixel toward black by the given factor (0..1).
// Shared helper for the "trail"-style effects (Sparkle, Confetti, Matrix Rain).
static void effectsFadeAll(float factor) {
  for (int i = 0; i < LED_COUNT; i++) {
    uint32_t c = strip.getPixelColor(i);
    uint8_t r = (uint8_t)(((c >> 16) & 0xFF) * factor);
    uint8_t g = (uint8_t)(((c >> 8) & 0xFF) * factor);
    uint8_t b = (uint8_t)((c & 0xFF) * factor);
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
}

// 0: Rainbow Cycle - smooth rotating rainbow gradient across the whole matrix.
static void effectRainbowCycle() {
  static uint8_t offset = 0;
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      uint8_t hue = (uint8_t)(offset + (row + col) * (256 / (2 * MATRIX_SIZE)));
      strip.setPixelColor(matrix[row][col], effectsColorWheel(hue));
    }
  }
  strip.show();
  offset += 2;
}

// 1: Theater Chase - classic marquee, one white dot in every 3 LEDs, running.
static void effectTheaterChase() {
  static uint8_t step = 0;
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, ((i + step) % 3 == 0) ? strip.Color(255, 255, 255) : 0);
  }
  strip.show();
  step = (step + 1) % 3;
}

// 2: Theater Chase Rainbow - same marquee, but the lit dots cycle hue.
static void effectTheaterChaseRainbow() {
  static uint8_t step = 0;
  static uint8_t hue = 0;
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, ((i + step) % 3 == 0) ? effectsColorWheel(hue) : 0);
  }
  strip.show();
  step = (step + 1) % 3;
  hue += 4;
}

// 3: Color Wipe - fills the strip pixel-by-pixel, then wipes the next
// palette color over it the same way.
static void effectColorWipe() {
  static int pos = 0;
  static uint8_t paletteIdx = 0;
  int rgb[3];
  getPaletteColor(paletteIdx, rgb);
  strip.setPixelColor(pos, strip.Color(rgb[0], rgb[1], rgb[2]));
  strip.show();
  pos++;
  if (pos >= LED_COUNT) {
    pos = 0;
    paletteIdx = (uint8_t)((paletteIdx + 1) % anzahlfarben);
  }
}

// 4: Breathing - whole matrix in one hue, slowly pulsing brightness.
static void effectBreathing() {
  static uint8_t hue = 0;
  float phase = (float)(millis() % 4000) / 4000.0f * 2.0f * (float)M_PI;
  float b = (sinf(phase) + 1.0f) / 2.0f;
  uint32_t c = effectsColorWheel(hue);
  uint8_t r = (uint8_t)(((c >> 16) & 0xFF) * b);
  uint8_t g = (uint8_t)(((c >> 8) & 0xFF) * b);
  uint8_t bch = (uint8_t)((c & 0xFF) * b);
  uint32_t scaled = strip.Color(r, g, bch);
  for (int i = 0; i < LED_COUNT; i++) strip.setPixelColor(i, scaled);
  strip.show();
  hue++;
}

// 5: Sparkle/Twinkle - a small pool of pixels randomly flashes and fades.
#define SPARKLE_POOL 8
static void effectSparkle() {
  static int16_t pos[SPARKLE_POOL];
  static uint8_t life[SPARKLE_POOL];
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < SPARKLE_POOL; i++) life[i] = 0;
    initialized = true;
  }
  effectsFadeAll(0.85f);
  for (int i = 0; i < SPARKLE_POOL; i++) {
    if (life[i] == 0 && random(0, 6) == 0) {
      pos[i] = (int16_t)random(0, LED_COUNT);
      life[i] = 255;
    }
    if (life[i] > 0) {
      int rgb[3];
      getPaletteColor((uint8_t)random(0, anzahlfarben - 1), rgb);
      strip.setPixelColor(pos[i], strip.Color(rgb[0], rgb[1], rgb[2]));
      life[i] = (life[i] > 40) ? (uint8_t)(life[i] - 40) : 0;
    }
  }
  strip.show();
}

// 6: Comet/Meteor - a bright head with a fading tail races around the strip.
static void effectComet() {
  static int headPos = 0;
  const int tailLen = 8;
  int rgb[3];
  getPaletteColor(1, rgb);
  for (int i = 0; i < LED_COUNT; i++) {
    int dist = headPos - i;
    if (dist < 0) dist += LED_COUNT;
    if (dist < tailLen) {
      float f = 1.0f - (float)dist / tailLen;
      strip.setPixelColor(i, strip.Color((uint8_t)(rgb[0] * f), (uint8_t)(rgb[1] * f), (uint8_t)(rgb[2] * f)));
    } else {
      strip.setPixelColor(i, 0);
    }
  }
  strip.show();
  headPos = (headPos + 1) % LED_COUNT;
}

// 7: Fire - classic heat-simulation fire, rising per column.
static void effectFire() {
  static uint8_t heat[MATRIX_SIZE][MATRIX_SIZE];
  static bool seeded = false;
  if (!seeded) {
    memset(heat, 0, sizeof(heat));
    seeded = true;
  }
  for (int col = 0; col < MATRIX_SIZE; col++) {
    for (int row = 0; row < MATRIX_SIZE; row++) {
      int cooldown = random(0, ((55 * 10) / MATRIX_SIZE) + 2);
      heat[row][col] = (heat[row][col] > cooldown) ? (uint8_t)(heat[row][col] - cooldown) : 0;
    }
    for (int row = 0; row < MATRIX_SIZE - 2; row++) {
      heat[row][col] = (uint8_t)((heat[row + 1][col] + heat[row + 2][col] + heat[row + 2][col]) / 3);
    }
    if (random(0, 255) < 120) {
      int row = MATRIX_SIZE - 1;
      int newHeat = heat[row][col] + random(160, 255);
      heat[row][col] = (newHeat > 255) ? 255 : (uint8_t)newHeat;
    }
    for (int row = 0; row < MATRIX_SIZE; row++) {
      uint8_t h = heat[row][col];
      uint8_t r, g, b;
      if (h < 85) {
        r = (uint8_t)(h * 3); g = 0; b = 0;
      } else if (h < 170) {
        r = 255; g = (uint8_t)((h - 85) * 3); b = 0;
      } else {
        r = 255; g = 255; b = (uint8_t)((h - 170) * 3);
      }
      strip.setPixelColor(matrix[row][col], strip.Color(r, g, b));
    }
  }
  strip.show();
}

// 8: Plasma - classic 2D sine-sum plasma field.
static void effectPlasma() {
  static uint16_t t = 0;
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      float v = sinf(row * 0.5f + t * 0.03f)
              + sinf(col * 0.5f + t * 0.04f)
              + sinf((row + col) * 0.3f + t * 0.02f);
      uint8_t hue = (uint8_t)((v + 3.0f) * 42.5f);
      strip.setPixelColor(matrix[row][col], effectsColorWheel(hue));
    }
  }
  strip.show();
  t++;
}

// 9: Confetti - random colored sparkles with a slow fade (WLED classic).
static void effectConfetti() {
  effectsFadeAll(0.90f);
  int pos = random(0, LED_COUNT);
  strip.setPixelColor(pos, effectsColorWheel((uint8_t)random(0, 256)));
  strip.show();
}

// 10: Diagonal Rainbow Sweep - a colored diagonal band sweeps across the matrix.
static void effectDiagonalRainbowSweep() {
  static int sweepPos = 0;
  int maxDiag = 2 * MATRIX_SIZE;
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      int diag = row + col;
      int dist = sweepPos - diag;
      if (dist < 0) dist += maxDiag;
      if (dist < 6) {
        float f = 1.0f - dist / 6.0f;
        uint32_t c = effectsColorWheel((uint8_t)(sweepPos * 8));
        uint8_t r = (uint8_t)(((c >> 16) & 0xFF) * f);
        uint8_t g = (uint8_t)(((c >> 8) & 0xFF) * f);
        uint8_t b = (uint8_t)((c & 0xFF) * f);
        strip.setPixelColor(matrix[row][col], strip.Color(r, g, b));
      } else {
        strip.setPixelColor(matrix[row][col], 0);
      }
    }
  }
  strip.show();
  sweepPos = (sweepPos + 1) % maxDiag;
}

// 11: Color Wave - horizontal bands blending between the user's two
// configured foreground colors (vf1/vf2 palette indices).
static void effectColorWave() {
  static uint16_t t = 0;
  int rgb1[3], rgb2[3];
  getPaletteColor((uint8_t)v1, rgb1);
  getPaletteColor((uint8_t)v2, rgb2);
  for (int row = 0; row < MATRIX_SIZE; row++) {
    float phase = sinf(row * 0.8f + t * 0.05f) * 0.5f + 0.5f;
    uint8_t r = (uint8_t)(rgb1[0] + (rgb2[0] - rgb1[0]) * phase);
    uint8_t g = (uint8_t)(rgb1[1] + (rgb2[1] - rgb1[1]) * phase);
    uint8_t b = (uint8_t)(rgb1[2] + (rgb2[2] - rgb1[2]) * phase);
    for (int col = 0; col < MATRIX_SIZE; col++) {
      strip.setPixelColor(matrix[row][col], strip.Color(r, g, b));
    }
  }
  strip.show();
  t++;
}

// 12: Larson Scanner (Cylon eye) - a bright vertical bar bounces left-right.
static void effectLarsonScanner() {
  static int pos = 0;
  static int dir = 1;
  const int eyeSize = 2;
  int rgb[3];
  getPaletteColor((uint8_t)v1, rgb);
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      int dist = abs(col - pos);
      if (dist <= eyeSize) {
        float f = 1.0f - (float)dist / (eyeSize + 1);
        strip.setPixelColor(matrix[row][col], strip.Color((uint8_t)(rgb[0] * f), (uint8_t)(rgb[1] * f), (uint8_t)(rgb[2] * f)));
      } else {
        strip.setPixelColor(matrix[row][col], 0);
      }
    }
  }
  strip.show();
  pos += dir;
  if (pos >= MATRIX_SIZE - 1 || pos <= 0) dir = -dir;
}

// 13: Random Color Fade - the whole matrix slowly crossfades between
// randomly chosen palette colors.
static void effectRandomColorFade() {
  static int fromR = 0, fromG = 0, fromB = 0, toR = 255, toG = 0, toB = 0;
  static unsigned long startMs = 0;
  static bool started = false;
  const unsigned long duration = 3000;
  if (!started) {
    startMs = millis();
    started = true;
  }
  unsigned long elapsed = millis() - startMs;
  float f = (float)elapsed / duration;
  if (f > 1.0f) f = 1.0f;
  uint8_t r = (uint8_t)(fromR + (toR - fromR) * f);
  uint8_t g = (uint8_t)(fromG + (toG - fromG) * f);
  uint8_t b = (uint8_t)(fromB + (toB - fromB) * f);
  for (int i = 0; i < LED_COUNT; i++) strip.setPixelColor(i, strip.Color(r, g, b));
  strip.show();
  if (elapsed >= duration) {
    fromR = toR; fromG = toG; fromB = toB;
    int rgb[3];
    getPaletteColor((uint8_t)random(0, anzahlfarben - 1), rgb);
    toR = rgb[0]; toG = rgb[1]; toB = rgb[2];
    startMs = millis();
  }
}

// 14: Bouncing Balls - a few balls fall under simple gravity and bounce.
#define NUM_BALLS 3
static void effectBouncingBalls() {
  static float pos[NUM_BALLS];
  static float vel[NUM_BALLS];
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < NUM_BALLS; i++) { pos[i] = 0; vel[i] = 0; }
    initialized = true;
  }
  const float gravity = 0.03f;
  for (int i = 0; i < LED_COUNT; i++) strip.setPixelColor(i, 0);
  for (int b = 0; b < NUM_BALLS; b++) {
    vel[b] += gravity;
    pos[b] += vel[b];
    if (pos[b] >= MATRIX_SIZE - 1) {
      pos[b] = MATRIX_SIZE - 1;
      vel[b] = -vel[b] * 0.85f;
      if (fabsf(vel[b]) < 0.05f) vel[b] = -0.6f;
    }
    int row = (int)pos[b];
    int col = (MATRIX_SIZE * (b + 1)) / (NUM_BALLS + 1);
    if (col >= MATRIX_SIZE) col = MATRIX_SIZE - 1;
    int rgb[3];
    getPaletteColor((uint8_t)(1 + b * 4), rgb);
    strip.setPixelColor(matrix[row][col], strip.Color(rgb[0], rgb[1], rgb[2]));
  }
  strip.show();
}

// 15: TV Static/Noise - random grayscale flicker every frame.
static void effectStatic() {
  for (int i = 0; i < LED_COUNT; i++) {
    uint8_t v = (uint8_t)random(0, 256);
    strip.setPixelColor(i, strip.Color(v, v, v));
  }
  strip.show();
}

// 16: Matrix Rain - falling green drops, one per column, at randomized pace.
static void effectMatrixRain() {
  static int8_t dropRow[MATRIX_SIZE];
  static bool initialized = false;
  if (!initialized) {
    for (int c = 0; c < MATRIX_SIZE; c++) dropRow[c] = (int8_t)random(-MATRIX_SIZE, 0);
    initialized = true;
  }
  effectsFadeAll(0.75f);
  for (int col = 0; col < MATRIX_SIZE; col++) {
    if (dropRow[col] >= 0 && dropRow[col] < MATRIX_SIZE) {
      strip.setPixelColor(matrix[dropRow[col]][col], strip.Color(180, 255, 180));
    }
    dropRow[col]++;
    if (dropRow[col] > MATRIX_SIZE + (int)random(0, MATRIX_SIZE)) {
      dropRow[col] = (int8_t)random(-MATRIX_SIZE, 0);
    }
  }
  strip.show();
}

// 17: Sunrise/Sunset - slow full-hue vertical gradient, rotating over time.
static void effectSunriseSunset() {
  static uint16_t t = 0;
  uint8_t cyclePos = (uint8_t)(t / 4);
  for (int row = 0; row < MATRIX_SIZE; row++) {
    uint8_t rowHue = (uint8_t)(cyclePos + row * (128 / MATRIX_SIZE));
    uint32_t c = effectsColorWheel(rowHue);
    for (int col = 0; col < MATRIX_SIZE; col++) {
      strip.setPixelColor(matrix[row][col], c);
    }
  }
  strip.show();
  t++;
}

// 18: Strobe - short, sharp full-matrix flashes.
static void effectStrobe() {
  static bool flashOn = false;
  static unsigned long lastToggle = 0;
  const unsigned long onMs = 40;
  const unsigned long offMs = 300;
  unsigned long elapsed = millis() - lastToggle;
  if (flashOn && elapsed >= onMs) {
    flashOn = false;
    lastToggle = millis();
  } else if (!flashOn && elapsed >= offMs) {
    flashOn = true;
    lastToggle = millis();
  }
  uint32_t c = flashOn ? strip.Color(255, 255, 255) : 0;
  for (int i = 0; i < LED_COUNT; i++) strip.setPixelColor(i, c);
  strip.show();
}

// 19: Color Wheel Pulse - whole matrix slowly rotates through every hue,
// combined with a gentle brightness pulse.
static void effectColorWheelPulse() {
  static uint8_t hue = 0;
  static uint16_t t = 0;
  float phase = (float)(t % 200) / 200.0f * 2.0f * (float)M_PI;
  float b = 0.4f + 0.6f * ((sinf(phase) + 1.0f) / 2.0f);
  uint32_t c = effectsColorWheel(hue);
  uint8_t r = (uint8_t)(((c >> 16) & 0xFF) * b);
  uint8_t g = (uint8_t)(((c >> 8) & 0xFF) * b);
  uint8_t bch = (uint8_t)((c & 0xFF) * b);
  uint32_t scaled = strip.Color(r, g, bch);
  for (int i = 0; i < LED_COUNT; i++) strip.setPixelColor(i, scaled);
  strip.show();
  t++;
  if (t % 8 == 0) hue++;
}

// Dispatches to one of the 20 real effect functions (idx 0..19).
static void dispatchEffect(uint8_t idx) {
  switch (idx) {
    case 0: effectRainbowCycle(); break;
    case 1: effectTheaterChase(); break;
    case 2: effectTheaterChaseRainbow(); break;
    case 3: effectColorWipe(); break;
    case 4: effectBreathing(); break;
    case 5: effectSparkle(); break;
    case 6: effectComet(); break;
    case 7: effectFire(); break;
    case 8: effectPlasma(); break;
    case 9: effectConfetti(); break;
    case 10: effectDiagonalRainbowSweep(); break;
    case 11: effectColorWave(); break;
    case 12: effectLarsonScanner(); break;
    case 13: effectRandomColorFade(); break;
    case 14: effectBouncingBalls(); break;
    case 15: effectStatic(); break;
    case 16: effectMatrixRain(); break;
    case 17: effectSunriseSunset(); break;
    case 18: effectStrobe(); break;
    case 19: effectColorWheelPulse(); break;
    default: effectRainbowCycle(); break;
  }
}

void renderLightEffects() {
  static const unsigned long speedMsBySetting[3] = {70, 40, 20}; // langsam/mittel/schnell
  static unsigned long lastFrameMs = 0;
  unsigned long frameIntervalMs = speedMsBySetting[constrain(lightEffectSpeedIdx, 0, 2)];
  if (millis() - lastFrameMs < frameIntervalMs) return;
  lastFrameMs = millis();

  if (on == 0 && nacht == 0) {
    strip.clear();
    strip.show();
    return;
  }
  if (nacht == 1) {
    strip.setBrightness(50);
  } else if (on == 1) {
    strip.setBrightness(dimm);
  }

  // Clear stale pixels from whichever effect was showing before, so
  // switching effects doesn't leave visual leftovers on screen.
  static uint8_t lastRenderedEffect = 255;
  if (selectedLightEffect != lastRenderedEffect) {
    strip.clear();
    strip.show();
    lastRenderedEffect = selectedLightEffect;
  }

  dispatchEffect(selectedLightEffect);
}
