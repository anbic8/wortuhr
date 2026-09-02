# Changelog - Wortuhr Firmware

Alle bemerkenswerten Änderungen an diesem Projekt werden in dieser Datei dokumentiert.
Das Format basiert auf [Keep a Changelog](https://keepachangelog.com/).

## [4.7.0] - 2026-07-12

### Added
- Konfigurierbare Tasten-Funktionen (nur NTP-Builds): unter `/setting` kann für Taste 1 und Taste 2 je eine Funktion für Klick und langen Druck gewählt werden - zur Auswahl stehen Keine Funktion, Helligkeit erhöhen, Nachtmodus umschalten, Pomodoro umschalten, Effekte-Modus umschalten und Nächster Übergangseffekt. Taste 3 spiegelt weiterhin Taste 1. RTC-Builds zeigen stattdessen informativ (nicht editierbar) an, was ihre Tasten fest für die Uhrzeiteinstellung tun.
- Bedienungsanleitung (`ANLEITUNG.md`) überarbeitet: neue Abschnitte für Pomodoro-Modus, Effekte-Modus, Countdown und Tasten-Funktionen, aktualisierte Effekt-Liste und Menü-Übersicht.

### Fixed
- Taste 1 reagierte auf Geräten mit älterer Platinen-Revision (Taste 1 physisch an GPIO12 statt GPIO4) gar nicht: `bt3.tick()` wurde nirgends aufgerufen, wodurch dieser Taster nie abgefragt/entprellt wurde, obwohl ihm dieselben Aktionen wie Taste 1 zugewiesen sind.
- `/settime`-Seite (RTC-Zeiteinstellung) konnte bei knappem Speicher mit einer Exception abstürzen: `htmlhead + body` verkettete das mehrere KB große Seiten-Grundgerüst mit einer `String` in einer einzigen, zusammenhängenden Heap-Allokation. Auf Streaming-Ausgabe umgestellt (wie bei allen anderen Seiten).
- Web-UI war "mal erreichbar, mal nicht": `WiFi.setSleepMode(WIFI_MODEM_SLEEP)` ließ das WLAN-Modul periodisch schlafen, wodurch eingehende Verbindungen verpasst wurden. Auf `WIFI_NONE_SLEEP` umgestellt (auf einem dauerhaft am Strom betriebenen Gerät ohne Nachteil).
- Statischer RAM-Verbrauch deutlich gesenkt (u.a. `vordergrund`/`hintergrund`/`anzeige`/`anzeigealt` sowie die Glitter-Animation von `int` auf `uint8_t` umgestellt, da nur Werte 0-255 vorkommen) und die MQTT/Home-Assistant-Discovery-Funktionen von String-Verkettung auf Stack-Puffer umgestellt - behebt Heap-Fragmentierung, die zu den beiden oben genannten Symptomen (leere Web-Antworten, Abstürze) beigetragen hat.

### Notes
- Version-Bump auf `4.7.0`.

## [4.6.0] - 2026-07-12

### Added
- Neuer "Pomodoro-Modus": eigene Seite `/pomodoro` mit konfigurierbarer Aktivitäts-/Pausenzeit, Anzeige-Schema (einfarbig/Schachbrett/Spalten/Zeilen/Verlauf/Zufällig, wie beim Uhr-Vordergrund) und 6 Aktivierungs-Animationen (Zeilen, Spirale, Diagonal, Raute, Fallend, Zufall). Während der Aktivitätszeit gehen die Matrix-Pixel nach und nach an, während der Pause in umgekehrter Reihenfolge wieder aus - in eigenen, unterscheidbaren Farben für beide Phasen. Der Zyklus wiederholt automatisch bis zum manuellen Stop; Start schaltet automatisch den Effekte-Modus aus (und umgekehrt).
- Pomodoro über Home Assistant steuerbar: Start/Stop-Schalter, Aktivitäts-/Pausenminuten (neue MQTT-`number`-Entities), sowie ein Status- und ein Restzeit-Sensor (Restzeit-Sensor aktualisiert alle 5 Sekunden während einer laufenden Sitzung).
- Neue Build-Environment `mini_8x8_kkg-makerspace` erzeugt jetzt automatisch ihre eigene `firmware_mini_8x8_kkg-makerspace.bin` (siehe Fixed).

### Fixed
- Übergangseffekt ("Uebergangseffekt") ließ sich über Home Assistant nicht mehr steuern: die HA-Discovery-Konfiguration nutzte noch einen zu kleinen JSON-Puffer (512 Bytes), der durch die inzwischen auf 17 Einträge gewachsene Effekt-Liste (Rainbow Swipe/Cycle, "Zufällig aus Liste") überlief und das Discovery-Payload beim Senden abschnitt. Puffer auf 1024 Bytes vergrößert.
- `.bin`-Datei-Erstellung nach dem Bauen: das Post-Build-Skript hatte eine fest gepflegte Namensliste, die neue Environments (z.B. `mini_8x8_kkg-makerspace`) nicht automatisch enthielt - jetzt generisch nach dem Environment-Namen benannt, funktioniert automatisch für alle aktuellen und künftigen Environments.

### Notes
- Version-Bump auf `4.6.0`.

## [4.5.0] - 2026-07-12

### Added
- Neue Build-Environment `mini_8x8_kkg-makerspace`: identisch zu `mini_8x8`, aber für die kkg-makerspace-Platine mit abweichender Pin-Belegung (LED-Datenpin 4 statt 2, Taster 1 an Pin 5 statt 4). In CI-Build- und Release-Workflow mit aufgenommen.

### Changed
- `LED_PIN` und `bt1Pin` sind jetzt per Build-Flag in `platformio.ini` überschreibbar, statt fest im Code zu stehen (Default-Werte für alle bestehenden Varianten unverändert).

### Notes
- Version-Bump auf `4.5.0`.

## [4.4.1] - 2026-07-09

### Fixed
- mDNS-Hostname (`http://<mqtt_prefix>.local/`) wurde beim Boot immer als `wortuhr.local` registriert, unabhängig vom in der Web-UI konfigurierten MQTT-Prefix, weil dieser erst nach `MDNS.begin()` ausgewertet wurde.
- MQTT und Home-Assistant-Discovery waren auf frisch geflashten oder per Werksreset gelöschten Geräten fälschlich standardmäßig aktiviert, weil ein unbeschriebenes (`0xFF`) EEPROM-Byte als aktivierter Schalter gelesen wurde; beide sind jetzt standardmäßig deaktiviert.
- Nachtmodus-Zeitplan: bei identischer Ein-/Ausschaltzeit (kein Zeitplan konfiguriert) blieb der Ein/Aus-Zustand der Anzeige am zuletzt berechneten Wert "kleben", statt zuverlässig eingeschaltet zu bleiben.
- 8x8-Mini: LED-Zuordnung für die Stunde "SIEBEN" korrigiert (fehlendes Pixel).

### Notes
- Version-Bump auf `4.4.1`.

## [4.4.0] - 2026-07-05

### Added
- Neuer Übergangseffekt "Rainbow Swipe": ein einmaliger Regenbogen-Sweep läuft diagonal von der unteren rechten zur oberen linken Ecke, bevor die Uhrzeit angezeigt wird.
- Neuer Übergangseffekt "Rainbow Cycle": 3 Sekunden lang rotiert ein Regenbogen-Farbverlauf über die gesamte Matrix, danach erscheint die Uhrzeit. Beide neuen Effekte sind auch bei "Zufällig" und "Zufällig aus Liste" wählbar.

### Fixed
- Minuten-Pixel der 8x8-Mini-Variante wurden von laufenden Übergangseffekten (Rain, Spirale, Feuerwerk, ...) zwischenzeitlich überschrieben und erst am Ende der Animation wieder korrekt angezeigt; sie werden jetzt in jedem einzelnen Animations-Frame erzwungen und bleiben dadurch durchgehend stabil.
- Spalten für die Fünf-Uhr-Ausnahme der Minuten-Pixel korrigiert (`0,1,6,7` statt fälschlich `0,2,6,7`).
- Frisch geflashtes oder gelöschtes EEPROM zeigte auf der WLAN-Setup-Seite kaputte/unleserliche Zeichen im SSID- und Passwort-Feld, statt leerer Felder (rohes `0xFF` aus dem unbeschriebenen EEPROM wurde direkt ins Formular übernommen).

### Notes
- Version-Bump auf `4.4.0`.

## [4.3.0] - 2026-07-05

### Added
- Neuer "Effekte-Modus": 20 fortlaufende, WLED-artige Lichteffekte (Rainbow Cycle, Theater Chase, Fire, Plasma, Confetti, Bouncing Balls, Matrix Rain, Larson Scanner, u.v.m.) zeigen bei Aktivierung Lichteffekte statt der Uhrzeit. Aktivierbar über Web-UI (wirkt sofort, kein Speichern nötig) und Home Assistant (MQTT switch + select), inkl. Geschwindigkeitsregler (langsam/mittel/schnell).
- Neue Option "Zufällig aus Liste" beim Übergangseffekt der Uhr-Anzeige: eine über die Web-UI konfigurierbare Teilmenge der Übergangseffekte, aus der bei jedem Wechsel zufällig einer gewählt wird.
- Live-Farbvorschau auf der Farben-Seite: ein Pixelgrid zeigt Vorder-/Hintergrundfarben und -schema als Beispielmuster und reagiert sofort auf Änderungen, ohne zu speichern.
- MQTT-Verfügbarkeits-Topic (Last Will): Home Assistant zeigt jetzt korrekt "nicht verfügbar" an, wenn die Uhr die Verbindung unsauber verliert (Stromausfall, WLAN-Abbruch).
- OTA-Update-Passwortschutz: `/update` und `/upload` können per HTTP Basic Auth abgesichert werden (geräte-gebunden verschlüsselt im EEPROM abgelegt); standardmäßig offen, bis über die Web-UI ein Passwort gesetzt wird.
- Button "Erkennung jetzt senden" direkt auf der Verbindungsseite, neben der Home-Assistant-Erkennung-Option.
- Minutengenaue Anzeige für die 8x8-Mini-Variante: zusätzliche Minuten-Pixel in der untersten Zeile (analog zum bestehenden 11x11-Mechanismus), Farbe frei wählbar unter Einstellungen.

### Changed
- Logging (`LOG()`/`LOGLN()`/`LOGF()`) ist in allen 6 Release-Environments jetzt standardmäßig abgeschaltet (`-DLOG_ENABLED=0`) für schlankere, schnellere Builds.
- Web-Interface optisch aufgewertet: Farbverläufe, Toggle-Switches statt Standard-Checkboxen, Hover-/Fokus-Effekte, konsistentere Typografie/Abstände.
- `nexthour` für die 8x8-Mini-Variante auf 4 gesetzt (vorher unbeabsichtigt identisch zur deutschen 11x11-Version).

### Fixed
- Countdown-Anzeige auf der 8x8-Matrix: der bisherige 5x7-Ziffernfont passte nicht in die 8 Spalten und wurde an den Rändern abgeschnitten; ein neuer kompakter 3x5-Font behebt das.
- Color-Picker (Vorder-/Hintergrundfarbe) zeigten die aktuelle Farbe erst nach Anklicken an, nicht sofort beim Laden der Seite.
- Horizontaler Scrollbalken auf schmalen Bildschirmen durch lange, nicht umbrechende Wörter (z.B. "Übergangsgeschwindigkeit") behoben.

### Notes
- Version-Bump auf `4.3.0`.

## [4.2.14] - 2026-07-03

### Added
- WLAN-/MQTT-Zugangsdaten (`ssid`, `password`, `mqtt_user`, `mqtt_password`) werden jetzt geräte-gebunden verschlüsselt im EEPROM abgelegt (AES-128-CTR, Schlüssel aus der Chip-ID abgeleitet). Bestehende Geräte migrieren beim nächsten Speichern über `/wifi` automatisch, ohne Neueingabe der Zugangsdaten.
- Neuer Endpunkt `/factory-reset` im Web-Interface für einen echten Zurücksetzen-auf-Werkseinstellungen.
- Zentrales EEPROM-Layout (`eeprom_layout.h`) mit einem Layout-Versions-Byte als Basis für zukünftige, sichere Migrationen.
- Logging jetzt über `LOG()`/`LOGLN()`/`LOGF()`-Makros steuerbar; per Build-Flag `-DLOG_ENABLED=0` für schlankere Release-Builds abschaltbar.
- GitHub Actions: Build-Matrix (`build.yml`) kompiliert bei jedem Push/PR alle 6 Firmware-Varianten.
- GitHub Actions: Release-Workflow (`release.yml`) baut bei einem `vX.Y.Z`-Tag automatisch alle 6 Varianten und hängt die `.bin`-Dateien an ein GitHub Release an.

### Changed
- `globals.h`/`globals.cpp` in themenbezogene Module aufgeteilt (`persistence`, `globals_network`, `globals_mqtt`, `globals_display`, `globals_state`, `globals_design`, `globals_birthday`, `webserver_html`) statt einer 1450-Zeilen-Datei.
- MQTT-Optionslisten (Effekte, Animationen, Farbschemata, Zeit/Stärke) dedupliziert: ein gemeinsames Array pro Kategorie wird jetzt von eingehendem Callback, ausgehendem State, Web-UI-Dropdowns und Home-Assistant-Discovery gemeinsam genutzt statt vierfach gepflegt zu werden.
- `setup()` beschleunigt: Boot-Animation und Taster reagieren sofort beim Einschalten statt erst nach WLAN- und NTP-Verbindungsaufbau; NTP-Synchronisierung blockiert den Start nicht mehr, sondern läuft im Hintergrund weiter.
- Firmware-`.bin`-Dateien werden nicht mehr im Repository getrackt, sondern ausschließlich über GitHub Releases verteilt (siehe oben).

### Fixed
- MQTT: ein unbekannter Animations-Payload setzte faelschlicherweise `effectMode` statt `aniMode` zurueck.
- MQTT: Off-by-one-Fehler beim Publizieren von Übergangs-/Animationszeit und -stärke konnte auf ein ungültiges Array-Element zugreifen.
- EEPROM: das Speichern der Anzeige-Einstellungen über bestimmte Pfade (RTC-Sommerzeit-Umschaltung, initiale Anlage) setzte den `mqttenable`-Wert stillschweigend zurück.
- `clearEEPROM()` berechnete seine Löschgröße zu klein und liess Countdown-Zeitstempel sowie das Home-Assistant-Flag unberührt.

### Notes
- Version-Bump auf `4.2.14`.

## [4.2.13] - 2026-02-15

### Added
- Web UI: responsive navigation for small screens (toggle menu).
- Web UI: compact form rows for time inputs.

### Changed
- Home Assistant discovery is sent on every boot when enabled.

### Fixed
- Transition effect "Schlangenfresser": background colors are preserved while eating the foreground.

### Notes
- Version-Bump auf `4.2.13`.

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
