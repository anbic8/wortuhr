#include "secure_storage.h"

extern "C" {
  #include "aes.h"
}

namespace {

void deriveDeviceKey(uint8_t key[16]) {
  uint32_t chipId = ESP.getChipId();
  static const char pepper[] = "Wortuhr-EEPROM-Pepper-v1";
  const size_t pepperLen = sizeof(pepper) - 1; // exclude trailing '\0'
  for (int i = 0; i < 16; ++i) {
    uint8_t chipByte = (uint8_t)(chipId >> ((i % 4) * 8));
    uint8_t pepperByte = (uint8_t)pepper[i % pepperLen];
    key[i] = chipByte ^ pepperByte ^ (uint8_t)(i * 41 + 7);
  }
}

// AES-CTR keystream XOR applied to one fixed-size field. `fieldTag`
// perturbs the IV so different fields don't share an identical keystream.
void cryptField(char *field, size_t len, const uint8_t key[16], uint8_t fieldTag) {
  uint8_t iv[16];
  for (int i = 0; i < 16; ++i) iv[i] = key[15 - i];
  iv[0] ^= fieldTag;

  struct AES_ctx ctx;
  AES_init_ctx_iv(&ctx, key, iv);
  AES_CTR_xcrypt_buffer(&ctx, reinterpret_cast<uint8_t *>(field), len);
}

} // namespace

namespace SecureStorage {

void cryptFields(settings &s) {
  uint8_t key[16];
  deriveDeviceKey(key);

  cryptField(s.ssid, sizeof(s.ssid), key, 0);
  cryptField(s.password, sizeof(s.password), key, 1);
  cryptField(s.mqtt_user, sizeof(s.mqtt_user), key, 2);
  cryptField(s.mqtt_password, sizeof(s.mqtt_password), key, 3);
}

void cryptBuffer(char *buf, size_t len, uint8_t fieldTag) {
  uint8_t key[16];
  deriveDeviceKey(key);
  cryptField(buf, len, key, fieldTag);
}

} // namespace SecureStorage
