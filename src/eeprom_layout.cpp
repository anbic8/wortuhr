#include "eeprom_layout.h"
#include <EEPROM.h>

namespace EepromLayout {

void beginAll() {
  EEPROM.begin(TOTAL_SIZE);
}

uint8_t readLayoutVersion() {
  return EEPROM.read(LAYOUT_VERSION_OFFSET);
}

void writeLayoutVersion(uint8_t version) {
  EEPROM.write(LAYOUT_VERSION_OFFSET, version);
}

void eraseAll() {
  EEPROM.begin(TOTAL_SIZE);
  for (int i = 0; i < TOTAL_SIZE; ++i) {
    EEPROM.write(i, 0xFF);
  }
  EEPROM.commit();
  EEPROM.end();
}

} // namespace EepromLayout
