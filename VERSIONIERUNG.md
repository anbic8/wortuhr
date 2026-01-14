# Versionierungsrichtlinien - Wortuhr

Dieses Dokument beschreibt, wie die Firmware-Version verwaltet und aktualisiert wird.

## Struktur der Versionsverwaltung

### 1. `version.txt`
- **Einzige Quelle der Wahrheit** für die aktuelle Versionsnummer
- Format: `X.X.X` (Semantic Versioning)
- Wird direkt gelesen und in der Firmware verwendet
- Schnell editierbar

### 2. `CHANGELOG.md`
- **Detaillierte Dokumentation** aller Änderungen
- Gruppiert nach Release-Datum
- Strukturiert nach Feature-Kategorien
- Für Nutzer lesbar (Dokumentation)

### 3. `src/globals.h`
- Enthält `#define FW_VERSION`
- **MUSS mit `version.txt` synchron sein**
- Wird in EEPROM gespeichert nach erfolgreicher Synchronisierung

## Release-Workflow

### Schritt 1: Version vorbereiten
1. Öffne `version.txt`
2. Ändere Version auf neue Nummer (z.B. `4.2.8` → `4.2.9`)
3. **Speichern**

### Schritt 2: Changelog aktualisieren
1. Öffne `CHANGELOG.md`
2. Füge neue Sektion oben ein:
```markdown
## [4.2.9] - 2026-01-15

### Added
- Feature 1
- Feature 2

### Fixed
- Bug 1

### Changed
- Verbesserung 1
```
3. **Speichern**

### Schritt 3: globals.h aktualisieren
1. Öffne `src/globals.h`
2. Ändere: `#define FW_VERSION "4.2.9"`
3. **Speichern und kompilieren**

### Schritt 4: Git committen
```bash
git add version.txt CHANGELOG.md src/globals.h
git commit -m "Release v4.2.9: [Kurzbeschreibung der wichtigsten Änderungen]"
git tag -a v4.2.9 -m "Release 4.2.9"
git push origin main --tags
```

## Semantic Versioning Erklärung

Format: `MAJOR.MINOR.PATCH`

- **MAJOR** (erstes Feld): Inkompatible Änderungen
  - Beispiele: Hardware-Anforderungen ändern, API-Breaking-Changes
  - Beispiel: `4.2.9` → `5.0.0`

- **MINOR** (zweites Feld): Neue Features (abwärtskompatibel)
  - Beispiele: Neue Effekte, neue MQTT-Topics, neue Funktionen
  - Beispiel: `4.2.9` → `4.3.0`

- **PATCH** (drittes Feld): Bugfixes (abwärtskompatibel)
  - Beispiele: Fehlerkorrektionen, kleinere Verbesserungen
  - Beispiel: `4.2.9` → `4.2.10`

## Kategorie-Bedeutungen im Changelog

- **Added**: Neue Features oder Funktionalität
- **Changed**: Verhaltensänderungen bestehender Features
- **Deprecated**: Features die in Zukunft entfernt werden
- **Removed**: Gelöschte Features
- **Fixed**: Bugfixes
- **Security**: Sicherheitsrelevante Patches

## Tipps für gute Changelogs

✅ **Gutes Beispiel:**
```markdown
### Added
- NTP-Zeitsynchronisierung alle Stunde für präzisere Zeitanzeige
- Feuerwerkeffekt mit Übergangsgeschwindigkeit-Anpassung

### Fixed
- Problem mit fehlerhafter Zeit nach Neustart (readTimeNet() Logik korrigiert)
```

❌ **Schlechtes Beispiel:**
```markdown
### Changed
- Code geändert
- Bug gefixt
```

## Automatische Versionskontrolle in der Firmware

Die Firmware prüft beim Start die `FW_VERSION`:
1. Liest gespeicherte Version aus EEPROM
2. Vergleicht mit aktueller `FW_VERSION`
3. Bei Unterschied: `discoveryNeeded = true` (MQTT Home Assistant Discovery wird erneut durchgeführt)
4. Speichert neue Version im EEPROM

Das erlaubt automatische Erkennung von Firmware-Updates!
