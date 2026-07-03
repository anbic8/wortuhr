#ifndef SECURE_STORAGE_H
#define SECURE_STORAGE_H

#include "globals.h"

// Device-bound obfuscation for the WiFi/MQTT credential fields stored in
// EEPROM (settings.ssid/password/mqtt_user/mqtt_password). The key is
// derived from ESP.getChipId(), so this is NOT protection against someone
// with the firmware image and effort (the key derivation lives in the same
// binary) - it only prevents plaintext exposure from a raw EEPROM/flash
// dump (e.g. when a device is passed on or a backup image leaks).
namespace SecureStorage {

// Symmetric AES-128-CTR transform applied in place to the sensitive fields
// of `s`. Calling this twice with the same on-device key returns the
// original plaintext - encrypt and decrypt are the same operation.
void cryptFields(settings &s);

} // namespace SecureStorage

#endif // SECURE_STORAGE_H
