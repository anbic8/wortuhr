#ifndef GLOBALS_H
#define GLOBALS_H

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_NeoPixel.h>
#ifndef MQTT_MAX_PACKET_SIZE
#define MQTT_MAX_PACKET_SIZE 1024
#endif
#include "OneButton.h"
#include <Arduino.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <time.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <Updater.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "buttons.h"
#include "log.h"

// Which pin on the Arduino is connected to the NeoPixels?
#define LED_PIN    2 //Pin 4 für die kkg-makerspace wortuhr

// How many NeoPixels are attached to the Arduino?
// Can be overridden by build flags in platformio.ini
#ifndef LED_COUNT
#define LED_COUNT 121
#endif

// Matrix size (11x11 for standard, 8x8 for mini)
#ifndef MATRIX_SIZE
#define MATRIX_SIZE 11
#endif

// Version type (0=deutsch, 1=bayrisch, 2=mini)
#ifndef VERSION_TYPE
#define VERSION_TYPE 0
#endif

// This header is now a thin aggregator: the declarations that used to live
// here directly are split across topic-focused modules below. Every file
// that already does #include "globals.h" keeps compiling unchanged, since
// everything it could previously see is still reachable transitively.
#include "persistence.h"
#include "globals_network.h"
#include "globals_mqtt.h"
#include "globals_display.h"
#include "globals_state.h"
#include "globals_design.h"
#include "globals_birthday.h"

#endif // GLOBALS_H
