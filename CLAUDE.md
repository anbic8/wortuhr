# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

ESP8266-based WiFi word clock ("Wortuhr") firmware written in Arduino C++ using PlatformIO. An 11x11 or 8x8 grid of WS2812B/NeoPixel LEDs spells out the time in German words. Three hardware/language variants (Deutsche, Bayrisch, Mini 8x8) and two time-source modes (NTP vs. DS1307 RTC) are all built from the same source tree, selected purely via build flags — there is no runtime hardware detection.

## Build commands

Build with PlatformIO CLI (`pio`). Each variant/mode combination is its own environment in `platformio.ini`:

```bash
# Build (compile only)
pio run -e deutsche_11x11
pio run -e bayrisch_11x11
pio run -e mini_8x8
pio run -e deutsche_11x11_rct   # RTC variant of each (suffix _rct)
pio run -e bayrisch_11x11_rct
pio run -e mini_8x8_rct

# Build + upload to a connected device
pio run -e deutsche_11x11 -t upload

# Serial monitor (115200 baud)
pio device monitor

# Build all environments
pio run
```

There is no test suite (`test/` only contains PlatformIO's placeholder README and standalone JS files for Home Assistant Lovelace cards). There is no separate lint step; correctness is checked via compilation.

A post-build script (`rename_firmware.py`) copies each environment's compiled `.bin` into the project root as `firmware_<variant>[_rct].bin` — these root-level `.bin` files are build artifacts checked into the repo for easy OTA distribution, not source.

## Environment / build-flag matrix

Defined in `platformio.ini`, consumed via `#ifdef`/`#if` in `src/globals.h` and throughout `src/`:

- `VERSION_TYPE`: `0` = Deutsche, `1` = Bayrisch, `2` = Mini — selects word lists, device model string, and matrix layout.
- `MATRIX_SIZE`: `11` or `8` — grid dimensions.
- `LED_COUNT`: `121` or `64` — NeoPixel strip length.
- `USE_RCT`: when defined, uses a DS1307 RTC over I2C (`rct.cpp`) instead of NTP (`readTimeNet()` in `show.cpp`) for timekeeping; this also gates the `/settime` web endpoint and disables the periodic NTP resync in `main.cpp`'s `loop()`.

When changing time-related or matrix-layout logic, always consider whether the change needs to work identically across all three `VERSION_TYPE`s and both RTC/NTP modes.

## Architecture

### Startup and main loop (`src/main.cpp`)
`setup()` restores all persisted state from EEPROM, connects WiFi (falls back to AP mode `"wortuhr"`/`"123456789"` after ~20s), starts mDNS + the web server, syncs time (NTP or waits for RTC read), computes the New Year's countdown in RAM, wires up the three physical buttons, and conditionally initializes MQTT if a valid broker is configured. `loop()` is a single non-blocking dispatcher: every fixed interval (usually until the next minute boundary, or every second while a countdown is in its final 99 seconds) it calls `readTime()` → `showClock()`, then services buttons, mDNS, the web server, and MQTT (`client.loop()`, periodic reconnect, periodic sensor publish every 60s).

### EEPROM layout — read this before touching any persisted struct
There is no single EEPROM struct; state is a sequence of distinct typed blocks written back-to-back, and **every reader recomputes each block's offset from `sizeof()` of the preceding blocks** rather than using a fixed layout:

```
settings (WiFi/MQTT config) -> MyColor -> design -> geburtstage[5][3]
  -> unsigned long (countdown_ts) -> char[VERSION_STR_MAX] (FW_VERSION) -> uint8_t (HA discovery flag)
```

This pattern is repeated with hand-computed offsets in `main.cpp`, `show.cpp`, `webserver.cpp`, `buttons.cpp`, `birthday.cpp`, and `color.cpp`. If you resize any struct (`settings`, `MyColor`, `design` in `globals.h`, or `geburtstage`), every one of these offset calculations must be updated consistently, or later fields silently read/write garbage. There is no version-tagged or self-describing EEPROM format.

Firmware-version tracking piggybacks on this same region: on boot, the stored version string is compared to `FW_VERSION` (`globals.h`); a mismatch sets `discoveryNeeded = true` so Home Assistant MQTT discovery is republished (see `VERSIONIERUNG.md` for the full version-bump workflow across `version.txt`, `CHANGELOG.md`, and `globals.h`).

### Display pipeline (`show.cpp`, `color.cpp`, `effects.cpp`, `ledmap.h`)
Rendering time-as-words is a multi-stage pipeline:
1. `readTime()` (NTP or RTC) fills `stunden`/`minutes`/`seconds` and derives `mb` (5-minute bucket, 0-11) and `h` (hour, 0-11).
2. `setmatrixanzeige()` (`show.cpp`) builds a boolean `matrixanzeige[MATRIX_SIZE][MATRIX_SIZE]` grid marking which cells should be lit, by looking up precomputed per-word LED index lists (`getWordLed`/`getWordLedCount`, built by `buildLedMappings()` from the static tables in `ledmap.h`) and mapping LED index → row/col via the inverse `ledRow`/`ledCol` arrays. Separate static tables exist per `VERSION_TYPE` (`std_min_lists`/`bay_min_lists` etc.) and are selected at `buildLedMappings()` time, not via runtime branching in the render path.
3. `color.cpp` (`vordergrunderstellen`/`hintergrunderstellen`) fills the `vordergrund`/`hintergrund` color grids based on the active color scheme (solid, two-color, rainbow, random, birthday).
4. `showClock()` (`show.cpp`) merges `matrixanzeige` + foreground/background colors into `anzeige[][][3]`, applies night-mode/on-off brightness rules, then dispatches to one of the transition effects in `effects.cpp` (fade, running, "Schlange", diagonal, rain, spiral, "Schlangenfresser"/snake-eater, firework, etc.) which animate from `anzeigealt` to `anzeige` and finally push pixels via `showmystrip()`.
5. Countdown mode (`showCountdown()` in `show.cpp`) bypasses the word-clock rendering entirely and draws large digits directly onto `matrixanzeige` using a hardcoded 5x7 font, for the final 99 seconds of a user-set countdown or the New Year countdown.

Two independent countdowns exist: `countdown_ts` (user-configured, persisted to EEPROM) and `newyear_countdown_ts` (always next Jan 1 00:00, RAM-only, recomputed on every boot).

### MQTT / Home Assistant integration (`mqtt.cpp`, `mqtt-callback.cpp`, `mqtt-ha.cpp`)
`mqtt.cpp` owns the connection lifecycle and per-feature state publishers (effect, animation, colors, brightness, etc.); `mqtt-callback.cpp` is the single `mqttCallback()` dispatching on topic string equality, translating human-readable German payload strings (e.g. `"Schlangenfresser"`, `"Feuerwerk"`) into the internal integer mode enums used by `effects.cpp`/`show.cpp`; `mqtt-ha.cpp` builds and publishes Home Assistant MQTT Discovery config payloads (entities, sensors) — this only fires when `discoveryNeeded` is true (fresh boot, firmware version change, or HA discovery freshly enabled), not on every MQTT reconnect. Topic strings are built at runtime by `buildMqttTopics()` from the user-configurable MQTT prefix, not hardcoded. See `MQTT_ANLEITUNG.md` for the full topic list and Home Assistant setup, and `test/ha-cards/*.js` for the companion custom Lovelace card front-ends (not built/compiled by PlatformIO — plain JS shipped separately for HA users).

### Web interface (`webserver.cpp`, `webserver.h`)
Routes are registered in `main.cpp`'s `setup()` (not in `webserver.cpp`). Handlers stream HTML directly (`server.sendContent()`-style) rather than building large `String` buffers in memory — this was a deliberate fix for heap exhaustion on the `/color` page (see CHANGELOG 4.2.10) and should be followed for any new page. `/settime` only exists when `USE_RCT` is defined. OTA firmware upload is handled by `handleUpload`/`handleUpdate`/`handleUploading` using the ESP8266 `Updater` library.

### Buttons (`buttons.cpp`)
Three physical buttons via the `OneButton` library: single-click, double-click, and long-press-start are all wired to handlers in `main.cpp`'s `setup()`. Note `bt3` is wired to the same handlers as `bt1` (legacy board layout had only 2 buttons; `bt3` maps to what used to be button 1 on older PCB revisions).

### Companion project
There is a sibling project, **Zeitlinie**, in an adjacent working directory (`../Zeitlinie`) — check whether changes here have a counterpart there before assuming this is the only relevant codebase.

## Documentation map

- `README.md` — user-facing overview, hardware pinout, install/flash instructions (German).
- `ANLEITUNG.md` — detailed German user manual.
- `MQTT_ANLEITUNG.md` — MQTT topic reference and Home Assistant setup.
- `SENSOR_IMPLEMENTATION.md`, `SENSOR_MQTT_DOKU.md`, `MQTT_SENSORS_CHANGELOG.md`/`MQTT_SENSORS_CHANGES.md` — the MQTT sensor-publishing feature (uptime, RSSI, heap, temperature, etc.) specifically.
- `VERSIONIERUNG.md` — required steps for bumping the version (`version.txt`, `CHANGELOG.md`, `src/globals.h` `FW_VERSION` must stay in sync — see EEPROM section above for *why* this matters at runtime).
- `CHANGELOG.md` — Keep a Changelog format, most recent release first.
