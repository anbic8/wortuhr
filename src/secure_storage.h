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

// Same transform for an arbitrary fixed-size buffer (e.g. the OTA
// password). `fieldTag` must be distinct from the tags used internally by
// cryptFields() (0-3) so the keystream doesn't repeat across fields -
// use 4 or higher for new callers.
void cryptBuffer(char *buf, size_t len, uint8_t fieldTag);

} // namespace SecureStorage

#endif // SECURE_STORAGE_H
