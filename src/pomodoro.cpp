#include "pomodoro.h"
#include "globals.h"
#include "color.h"
#include "eeprom_layout.h"
#include "mqtt.h"
#include <EEPROM.h>

// ---- Aktivierungs-Reihenfolgen ----
// Jede Strategie baut einmalig eine vollstaendige Liste aller (row,col)
// Zellen in der Reihenfolge, in der sie an-/ausgehen sollen - adaptiert aus
// den Koordinaten-Enumerationen der bestehenden Uebergangseffekte in
// effects.cpp (spiral/diagonal/diamond/firework), nur ohne sofortiges
// Rendern.

static void buildOrderRowMajor() {
  int idx = 0;
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int col = 0; col < MATRIX_SIZE; col++) {
      pomodoroOrder[idx][0] = (uint8_t)row;
      pomodoroOrder[idx][1] = (uint8_t)col;
      idx++;
    }
  }
}

// Adaptiert aus effects.cpp's spiral() - identischer Aufbau-Algorithmus,
// befuellt aber pomodoroOrder statt sofort zu animieren.
static void buildOrderSpiral() {
  int idx = 0;
  int row = MATRIX_SIZE / 2;
  int col = MATRIX_SIZE / 2;
  pomodoroOrder[idx][0] = (uint8_t)row;
  pomodoroOrder[idx][1] = (uint8_t)col;
  idx++;

  int steps = 1;
  int total = MATRIX_SIZE * MATRIX_SIZE;
  while (idx < total) {
    for (int i = 0; i < steps && idx < total; i++) {
      col--;
      if (row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = (uint8_t)col; idx++;
      }
    }
    for (int i = 0; i < steps && idx < total; i++) {
      row--;
      if (row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = (uint8_t)col; idx++;
      }
    }
    steps++;
    for (int i = 0; i < steps && idx < total; i++) {
      col++;
      if (row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = (uint8_t)col; idx++;
      }
    }
    for (int i = 0; i < steps && idx < total; i++) {
      row++;
      if (row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = (uint8_t)col; idx++;
      }
    }
    steps++;
  }
}

// Anti-Diagonale nach row+col gruppiert (diagonal()/rainbowSwipe()-Vorbild).
static void buildOrderDiagonal() {
  int idx = 0;
  int maxStep = (MATRIX_SIZE - 1) * 2;
  for (int step = 0; step <= maxStep; step++) {
    for (int row = 0; row < MATRIX_SIZE; row++) {
      int col = step - row;
      if (col >= 0 && col < MATRIX_SIZE) {
        pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = (uint8_t)col; idx++;
      }
    }
  }
}

// Aufsteigende Manhattan-Distanz zur Mitte (diamond()-Vorbild).
static void buildOrderDiamond() {
  int idx = 0;
  int centerRow = MATRIX_SIZE / 2;
  int centerCol = MATRIX_SIZE / 2;
  int maxDist = centerRow + centerCol;
  for (int dist = 0; dist <= maxDist; dist++) {
    for (int row = 0; row < MATRIX_SIZE; row++) {
      for (int col = 0; col < MATRIX_SIZE; col++) {
        if (abs(row - centerRow) + abs(col - centerCol) == dist) {
          pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = (uint8_t)col; idx++;
        }
      }
    }
  }
}

// Naeherung an "fallende" Pixel: zeilenweise von oben nach unten, innerhalb
// jeder Zeile in einer einmalig gemischten Spaltenreihenfolge.
static void buildOrderFalling() {
  uint8_t cols[MATRIX_SIZE];
  for (int i = 0; i < MATRIX_SIZE; i++) cols[i] = (uint8_t)i;
  for (int i = MATRIX_SIZE - 1; i > 0; i--) {
    int j = random(0, i + 1);
    uint8_t tmp = cols[i]; cols[i] = cols[j]; cols[j] = tmp;
  }
  int idx = 0;
  for (int row = 0; row < MATRIX_SIZE; row++) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
      pomodoroOrder[idx][0] = (uint8_t)row; pomodoroOrder[idx][1] = cols[i]; idx++;
    }
  }
}

// Fisher-Yates-Shuffle einer zeilenweisen Liste (firework()-Vorbild).
static void buildOrderRandom() {
  buildOrderRowMajor();
  int total = MATRIX_SIZE * MATRIX_SIZE;
  for (int i = total - 1; i > 0; i--) {
    int j = random(0, i + 1);
    uint8_t tr = pomodoroOrder[i][0], tc = pomodoroOrder[i][1];
    pomodoroOrder[i][0] = pomodoroOrder[j][0]; pomodoroOrder[i][1] = pomodoroOrder[j][1];
    pomodoroOrder[j][0] = tr; pomodoroOrder[j][1] = tc;
  }
}

void buildPomodoroOrder(uint8_t animationIdx) {
  switch (animationIdx) {
    case 1: buildOrderSpiral(); break;
    case 2: buildOrderDiagonal(); break;
    case 3: buildOrderDiamond(); break;
    case 4: buildOrderFalling(); break;
    case 5: buildOrderRandom(); break;
    default: buildOrderRowMajor(); break;
  }
}

// ---- Phasen-/Renderlogik ----

static void applyPhaseColors() {
  int c1[3], c2[3];
  if (pomodoroPhase == 0) {
    getPaletteColor((uint8_t)pomodoroActivityColor1Idx, c1);
    getPaletteColor((uint8_t)pomodoroActivityColor2Idx, c2);
  } else {
    getPaletteColor((uint8_t)pomodoroPauseColor1Idx, c1);
    getPaletteColor((uint8_t)pomodoroPauseColor2Idx, c2);
  }
  pomodoroFarberstellen(pomodoroScheme, c1, c2);
}

static unsigned long pomodoroPhaseDurationMs() {
  unsigned long mins = (pomodoroPhase == 0) ? pomodoroActivityMinutes : pomodoroPauseMinutes;
  return mins * 60000UL;
}

void startPomodoro() {
  // Gegenseitiger Ausschluss: nur ein alternativer Vollbild-Modus aktiv.
  if (effectsModeActive) {
    effectsModeActive = false;
    EEPROM.write(EepromLayout::LIGHT_EFFECTS_ENABLED_OFFSET, 0);
    EEPROM.commit();
    publishEffectsModeState();
  }

  pomodoroModeActive = true;
  pomodoroPhase = 0;
  pomodoroPhaseStartMs = millis();
  pomodoroLitCount = 0;
  buildPomodoroOrder(pomodoroAnimationIdx);
  applyPhaseColors();

  publishPomodoroActiveState();
  publishPomodoroPhaseState();
  publishPomodoroRemainingState();
}

void stopPomodoro() {
  pomodoroModeActive = false;
  threshold = 0; // sofortiger Uhr-Refresh statt bis zum naechsten Minutenwechsel

  publishPomodoroActiveState();
  publishPomodoroPhaseState();
  publishPomodoroRemainingState();
}

long getPomodoroRemainingSeconds() {
  if (!pomodoroModeActive) return 0;
  unsigned long durationMs = pomodoroPhaseDurationMs();
  unsigned long elapsed = millis() - pomodoroPhaseStartMs;
  if (elapsed >= durationMs) return 0;
  return (long)((durationMs - elapsed) / 1000UL);
}

void renderPomodoro() {
  static unsigned long lastFrameMs = 0;
  if (millis() - lastFrameMs < 400) return;
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

  unsigned long durationMs = pomodoroPhaseDurationMs();
  unsigned long elapsed = millis() - pomodoroPhaseStartMs;
  int total = MATRIX_SIZE * MATRIX_SIZE;

  if (elapsed >= durationMs) {
    pomodoroPhase = (pomodoroPhase == 0) ? 1 : 0;
    pomodoroPhaseStartMs = millis();
    applyPhaseColors();
    publishPomodoroPhaseState();
    publishPomodoroRemainingState();
    elapsed = 0;
    durationMs = pomodoroPhaseDurationMs();
  }

  float fraction = (durationMs > 0) ? ((float)elapsed / (float)durationMs) : 1.0f;
  if (fraction > 1.0f) fraction = 1.0f;

  int targetLit = (pomodoroPhase == 0)
    ? (int)(fraction * total)
    : total - (int)(fraction * total);
  if (targetLit < 0) targetLit = 0;
  if (targetLit > total) targetLit = total;

  if (targetLit == pomodoroLitCount) return;
  pomodoroLitCount = targetLit;

  for (int i = 0; i < total; i++) {
    int row = pomodoroOrder[i][0];
    int col = pomodoroOrder[i][1];
    if (i < pomodoroLitCount) {
      strip.setPixelColor(matrix[row][col], strip.Color(pomodoroGrid[row][col][0], pomodoroGrid[row][col][1], pomodoroGrid[row][col][2]));
    } else {
      strip.setPixelColor(matrix[row][col], strip.Color(0, 0, 0));
    }
  }
  strip.show();
}
