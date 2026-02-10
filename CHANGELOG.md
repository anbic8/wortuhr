# Changelog - Wortuhr Firmware

Alle bemerkenswerten Änderungen an diesem Projekt werden in dieser Datei dokumentiert.
Das Format basiert auf [Keep a Changelog](https://keepachangelog.com/).

## [4.2.12] - 2026-02-10

### Fixed
- Countdown-Anzeige nutzt jetzt denselben `now`-Zeitstempel wie die Berechnung, um Sekunden-Drift zu vermeiden.
- RTC funktioniert, setzt Sommer und Winterzeit automatisch


### Notes
- Version-Bump auf `4.2.12`.

## [4.2.11] - 2026-02-07

### Added
- Neue Animation: **Glitter** — einzelne Vordergrund-Pixel blitzen weiß auf und faden zurück.
- Web UI: Auswahl für `Glitter` hinzugefügt (Einstellungen → Animationseffekt).
- Home Assistant MQTT Discovery: `Glitter` als Auswahloption in `Animationseffekt` verfügbar.

### Changed
- `Glitter`-Verhalten: nur Vordergrund-Pixel werden verwendet; Pixel werden nacheinander aktiviert und langsam ausgeblendet. `anidepth` nutzt jetzt die vorhandenen Stärke-Indizes (schwach/mittel/stark).

### Notes
- Version-Bump auf `4.2.11`.

## [4.2.10] - 2026-02-03

### Added
- Countdown: `datetime-local` Eingabe unter dem Bereich "Geburtstage" zur Konfiguration eines Countdowns
- Große Ziffern-Darstellung auf der Matrix für die letzten 99 Sekunden
- Unabhängiger Neujahrs-Countdown (RAM-only, automatisch auf nächsten 01.01 00:00 gesetzt)

### Fixed
- `/color` Webseite: Speicherauslastung behoben (HTML-Streaming statt großer String-Allokation)

### Changed
- Anzeige aktualisiert jede Sekunde während aktiver Countdown-Phase

## [4.2.9] - 2026-01-14

### Added
- **8 neue MQTT Sensoren mit Home Assistant Discovery:**
  - IP-Adresse (Netzwerk-Diagnostik)
  - Uptime (Laufzeit seit Neustart)
  - WiFi Signalstärke (RSSI in dBm)
  - Freier Speicher (Heap Memory in Bytes)
  - LED Helligkeit (% 0-100)
  - Letzter NTP Sync (ISO 8601 Zeitstempel)
  - CPU Temperatur (°C, ESP8266 intern)
  - System Auslastung (Heap-Nutzung %)
- Automatische Sensor-Veröffentlichung alle 60 Sekunden
- Alle Sensoren als "diagnostic" in Home Assistant kategorisiert
- Vollständige Dokumentation: SENSOR_MQTT_DOKU.md
- Automation-Beispiele für Warnungen und Überwachung
- Technische Implementierungsdetails: SENSOR_IMPLEMENTATION.md

### Technical
- MQTT Topics: `{prefix}/ip_address`, `uptime`, `rssi`, `heap_memory`, `brightness`, `last_ntp_sync`, `temperature`, `system_load`
- Home Assistant Discovery mit korrekten Device-Klassen und Einheiten
- Non-blocking Sensor Publishing (60s Interval in main.cpp loop)
- Retain-Flag für alle Sensor States
- Fehlertoleranz bei MQTT-Disconnect

## [4.2.8] - 2026-01-14

### Added
- NTP-Zeitsynchronisierung alle Stunde (bisher nur beim Start)
- Erhöhtes NTP-Startup-Timeout von 10 auf 30 Sekunden für zuverlässigere Zeitabfrage
- Feuerwerkeffekt: LED leuchtet weiß beim Explodieren und fadet während Funken-Animation zur eigentlichen Farbe
- Feuerwerkeffekt: Verbindung mit Übergangsgeschwindigkeit (langsam/mittel/schnell)
- Debug-Ausgaben für Zeitabfrage (alle 10 Sekunden)

### Fixed
- Problem mit fehlerhafter Zeit nach dem Start (readTimeNet() Logik korrigiert)
- NTP-Synchronisierung wird nicht mehr mehrfach pro Minute durchgeführt

### Changed
- Feuerwerk-Effekt: Kontinuierliches Fade während Funkensprühen statt danach

## [4.2.7] - 2026-01-10

### Added
- Initiale Feuerwerkeffekt-Implementierung

### Fixed
- Diverse Stabilisierungsmaßnahmen

## [4.2.6] - Ältere Versionen

Frühere Versionen - siehe Git-Historie

---

## Versionierungsschema

Das Projekt folgt [Semantic Versioning](https://semver.org/):
- **MAJOR**: Inkompatible Änderungen (z.B. Hardware-Anforderungen)
- **MINOR**: Neue Features (z.B. neue Effekte, neue MQTT-Befehle)
- **PATCH**: Bugfixes und kleinere Verbesserungen

## Anleitung zum Aktualisieren der Version

1. **Version ändern**: Bearbeite `version.txt` mit der neuen Versionsnummer (z.B. `4.2.9`)
2. **Changelog aktualisieren**: Füge einen neuen Eintrag oben in `CHANGELOG.md` hinzu
3. **Neuer Eintrag-Kopfzeile**: `## [X.X.X] - YYYY-MM-DD`
4. **Kategorien verwenden**:
   - `### Added` - Neue Features
   - `### Changed` - Verhaltensänderungen
   - `### Fixed` - Bugfixes
   - `### Deprecated` - Veraltete Features
   - `### Removed` - Gelöschte Features
   - `### Security` - Sicherheitspatches

5. **Kompilieren und hochladen** mit der neuen Version
6. **Git commit**: `git add CHANGELOG.md version.txt && git commit -m "v4.2.9: [Kurzbeschreibung]"`

## Firmware-Version im Code

Die aktuelle Version ist in [globals.h](src/globals.h) definiert:
```cpp
#define FW_VERSION "4.2.11"
```

Synchronisiere diese mit `version.txt`!
