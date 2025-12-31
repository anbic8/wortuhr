# Wortuhr - Bedienungsanleitung

## Inhaltsverzeichnis
1. [Erste Inbetriebnahme](#1-erste-inbetriebnahme)
2. [WLAN-Verbindung einrichten](#2-wlan-verbindung-einrichten)
3. [Weboberfläche](#3-weboberfläche)
4. [Einstellungen](#4-einstellungen)
5. [Farben und Design](#5-farben-und-design)
6. [Geburtstage](#6-geburtstage)
7. [MQTT und Home Assistant](#7-mqtt-und-home-assistant)
8. [Firmware-Update](#8-firmware-update)
9. [Bedienung mit Tasten](#9-bedienung-mit-tasten)
10. [Fehlerbehebung](#10-fehlerbehebung)

---

## 1. Erste Inbetriebnahme

### Lieferumfang
- Wortuhr (11x11 oder 8x8 LEDs)
- USB-Kabel (optional, je nach Modell)

### Benötigtes Zubehör
- **USB-Netzteil** mit mindestens **2 Ampere** (5V)
- WLAN-Netzwerk (2.4 GHz)
- Computer, Smartphone oder Tablet zum Einrichten

### Erste Schritte

1. **Stromversorgung anschließen**
   - Verbinde die Uhr mit dem USB-Netzteil
   - Die Uhr startet automatisch
   - Bei der ersten Inbetriebnahme erscheint eine kurze Startanimation

2. **WLAN-Hotspot verbinden**
   - Die Uhr erstellt einen eigenen WLAN-Hotspot mit dem Namen: **"wortuhr"**
   - Passwort: **"123456789"**
   - Verbinde dich mit deinem Smartphone/Computer mit diesem Netzwerk

3. **Konfigurationsseite öffnen**
   - Öffne einen Webbrowser
   - Gehe zu: **http://192.168.4.1** oder **http://wortuhr.local**
   - Die Konfigurationsseite öffnet sich automatisch

---

## 2. WLAN-Verbindung einrichten

### Verbindung mit deinem Heimnetzwerk

1. **Öffne die Weboberfläche** (siehe oben)
2. **Klicke auf "Verbindung"** im Menü
3. **Fülle die WLAN-Daten aus:**
   - **SSID**: Name deines WLAN-Netzwerks
   - **Password**: WLAN-Passwort
   - *(MQTT-Felder können erstmal leer bleiben)*
4. **Klicke auf "Save"**
5. **Die Uhr startet neu** und verbindet sich mit deinem WLAN

### Nach dem Neustart

- Die Uhr ist nun in deinem Heimnetzwerk
- Du findest sie unter: **http://wortuhr.local**
- Oder über die IP-Adresse, die dein Router vergeben hat
- **Tipp:** Schaue in deinem Router nach, welche IP die Uhr bekommen hat

### Festen Hostnamen vergeben

Der Hostname basiert auf dem **MQTT Prefix** (Standard: `wortuhr`):
- Wenn du im Feld "MQTT Prefix" z.B. `wohnzimmer` eingibst, erreichst du die Uhr unter **http://wohnzimmer.local**
- So kannst du mehrere Uhren im selben Netzwerk betreiben

---

## 3. Weboberfläche

### Menü-Übersicht

Die Weboberfläche hat folgende Menüpunkte:

| Symbol | Menü | Funktion |
|--------|------|----------|
| 📶 | **Verbindung** | WLAN und MQTT konfigurieren |
| ⚙️ | **Einstellungen** | Zeitanzeige, Nachtmodus, Helligkeit |
| 🎨 | **Farben** | Farben, Effekte und Animationen |
| 🎂 | **Geburtstage** | Geburtstagserinnerungen eintragen |
| ℹ️ | **Info** | Informationen zur Uhr |
| 📤 | **Update** | Firmware aktualisieren |

### Zugriff von unterwegs

Die Weboberfläche ist nur im **lokalen Netzwerk** erreichbar. Für externen Zugriff:
- VPN ins Heimnetzwerk einrichten
- Oder über Home Assistant (siehe [MQTT-Anleitung](MQTT_ANLEITUNG.md))

---

## 4. Einstellungen

### Zeitanzeige konfigurieren

Gehe zu **Einstellungen** im Menü:

#### **Dreiviertel / Viertel vor**
- **Dreiviertel**: Bei 14:45 wird "dreiviertel drei" angezeigt
- **Viertel vor**: Bei 14:45 wird "viertel vor drei" angezeigt

#### **"UHR"-Anzeige**
- **Immer**: Das Wort "UHR" wird immer angezeigt
- **Zur vollen Stunde**: "UHR" nur bei xx:00 (z.B. "ZEHN UHR")
- **Nie**: Das Wort "UHR" wird nie angezeigt

### Nachtmodus

Der Nachtmodus schaltet die Uhr zu bestimmten Zeiten aus oder dimmt sie.

#### Einrichtung
1. Gehe zu **Einstellungen**
2. Wähle **Nachtmodus**:
   - **Aus**: Nachtmodus deaktiviert
   - **Gedimmt**: Uhr wird gedimmt (30% Helligkeit)
   - **Komplett aus**: Uhr schaltet sich aus
3. Stelle die Zeiten ein:
   - **Von (Stunde/Minute)**: Beginn des Nachtmodus
   - **Bis (Stunde/Minute)**: Ende des Nachtmodus

**Beispiel:**
- Von: 22:00
- Bis: 07:00
- → Die Uhr ist von 22 Uhr abends bis 7 Uhr morgens aus/gedimmt

### Helligkeit

Die **Helligkeit** kannst du mit einem Schieberegler einstellen (0-255):
- **0**: Ausgeschaltet
- **50**: Sehr gedimmt
- **128**: Mittlere Helligkeit
- **255**: Maximale Helligkeit

**Empfehlung:** 
- Tagsüber: 200-255
- Abends: 100-150
- Nachtmodus gedimmt: 30-80

---

## 5. Farben und Design

### Farbschemas

Die Uhr unterscheidet zwischen **Vordergrund** (Buchstaben die die Uhrzeit angezeigen) und **Hintergrund** (nicht benötigte Buchstaben).

#### Vordergrund-Farben
Gehe zu **Farben** → **Vordergrund**

**Farbe 1 und Farbe 2:**
- Wähle per Color-Picker aus 14 vordefinierten Farben:
  - Weiß, Rot, Rosa, Magenta, Violet, Blau, Azure, Türkis, Hellgrün, Grün, Gelbgrün, Gelb, Orange, Aus

**Farbschema:**
- **Eine Farbe**: Nur Farbe 1 wird verwendet
- **Zwei Farben Schachbrett**: Farbe 1 und 2 im Schachbrettmuster
- **Zwei Farben Reihen**: Farbe 1 und 2 reihenweise
- **Zwei Farben Zeilen**: Farbe 1 und 2 spaltenweise
- **Zwei Farben Verlauf**: Sanfter Farbverlauf von Farbe 1 zu Farbe 2
- **Zufällige Farben**: Jeder Buchstabe hat eine zufällige Farbe

#### Hintergrund-Farben
Gleiche Optionen wie Vordergrund, aber für nicht leuchtende Buchstaben.

**Tipp:** Für klassischen Look:
- Vordergrund: Weiß (einfarbig)
- Hintergrund: Aus (schwarz)

### Effekte

Effekte steuern, **wie** neue Wörter eingeblendet werden:

| Effekt | Beschreibung |
|--------|--------------|
| **Kein Effekt** | Direkter Wechsel ohne Animation |
| **Zufällig** | Bei jedem Wechsel ein zufälliger Effekt |
| **Fade** | Sanftes Ein- und Ausblenden |
| **Running** | Buchstaben laufen herein |
| **Schlange** | Schlangen-Animation |
| **Zeilen** | Zeilenweise Animation |
| **Scrollen** | Scroll-Effekt |
| **Slide in** | Einschieben von der Seite |
| **Diagonal** | Diagonale Animation |
| **Rain** | Regen-Effekt von oben |
| **Spirale** | Spiralförmige Animation |
| **Schlangenfresser** | Snake-Game-ähnlich |

**Effekt-Geschwindigkeit:**
- Langsam, Mittel, Schnell

### Animationen

Animationen laufen **dauerhaft** im Hintergrund:

| Animation | Beschreibung |
|-----------|--------------|
| **Keine Animation** | Statische Anzeige |
| **Blinken** | Komplette Anzeige blinkt |
| **Vordergrundblinken** | Nur aktive Buchstaben blinken |
| **Pulsieren** | Sanftes Pulsieren der Helligkeit |
| **Verlauf** | Farbverlauf-Animation |
| **Fliegen** | Fliegende Pixel-Animation |

**Animations-Geschwindigkeit:**
- Langsam, Mittel, Schnell

**Animations-Intensität:**
- Schwach, Mittel, Stark

**Hinweis:** Animationen verbrauchen mehr Strom und können bei schwachen Netzteilen zu Problemen führen.

---

## 6. Geburtstage

Die Uhr kann bis zu **5 Geburtstage** speichern und anzeigen.

### Geburtstag eintragen

1. Gehe zu **Geburtstage**
2. Wähle einen freien Slot (1-5)
3. Trage ein:
   - **Tag** (1-31)
   - **Monat** (1-12)
   - **Jahr** (z.B. 1990)
4. Klicke auf **Save**

### Geburtstags-Anzeige

**Am Geburtstag selbst:**
- Die Uhr zeigt ein buntes Farbschema mit zufälligen Farben


**Deaktivieren:**
- Klicke auf Löschen und speichern

---

## 7. MQTT und Home Assistant

Mit MQTT kannst du die Uhr in dein Smart Home einbinden.

### Vorteile von MQTT
- Steuerung über Home Assistant, Node-RED, etc.
- Automatisierungen (z.B. Farbe ändern bei Sonnenuntergang)
- Status-Abfragen
- Fernsteuerung

### Einrichtung

Siehe die ausführliche **[MQTT-Anleitung](MQTT_ANLEITUNG.md)** für:
- MQTT-Broker einrichten
- Uhr mit MQTT verbinden
- Home Assistant Integration
- Befehle und Topics
- Beispiele und Automationen

### Quick-Start MQTT

1. **MQTT-Broker** installieren (z.B. Mosquitto in Home Assistant)
2. Gehe zu **Verbindung** in der Uhr
3. Aktiviere **Mqtt aktivieren** ✓
4. Aktiviere **Home Assistant Discovery** ✓
5. Trage ein:
   - MQTT Server (IP oder Hostname)
   - MQTT Port (Standard: 1883)
   - MQTT User und Password
   - MQTT Prefix (z.B. "wortuhr")
6. Speichern → Uhr startet neu
7. In Home Assistant erscheint die Uhr automatisch unter **MQTT**

---

## 8. Firmware-Update

### Warum Updates?
- Neue Funktionen
- Fehlerbehebungen
- Verbesserungen

### Update durchführen

1. **Firmware-Datei besorgen**
   - Datei endet mit `.bin` (z.B. `firmware_v4.2.1.bin`)
   - Von mir per E-Mail oder Download-Link

2. **Update hochladen**
   - Gehe zu **Update** im Menü
   - Klicke auf **Datei auswählen**
   - Wähle die `.bin` Datei
   - Klicke auf **Update Firmware**

3. **Warten**
   - Der Upload dauert ca. 30-60 Sekunden
   - **Nicht vom Strom trennen!**
   - Die Uhr zeigt "Update erfolgreich!"
   - Automatischer Neustart

4. **Fertig**
   - Nach dem Neustart läuft die neue Firmware
   - Alle Einstellungen bleiben erhalten

### Bei Problemen
- Falls das Update fehlschlägt, versuche es erneut
- Stelle sicher, dass die Datei korrekt ist (`.bin` Endung)
- Bei Verbindungsproblemen: Näher am Router platzieren

---

## 9. Bedienung mit Tasten

Die Uhr hat **3 Tasten** (je nach Modell an unterschiedlichen Positionen):

### Taste 1 (Links)
- **Kurzer Klick**: Uhr ein-/ausschalten
- **Langer Klick**: Nächster Effekt
- **Doppelklick**: Helligkeit erhöhen (+25)

### Taste 2 (Mitte)
- **Kurzer Klick**: Nächstes Farbschema (Vordergrund)
- **Langer Klick**: Nächste Animation
- **Doppelklick**: Helligkeit verringern (-25)

### Taste 3 (Rechts)
- Funktionen wie Taste 1 (Backup, falls Taste 1 defekt)

**Hinweis:** Die Tastenbelegung kann je nach Firmware-Version leicht variieren.

---

## 10. Fehlerbehebung

### Uhr zeigt keine Zeit / LEDs leuchten nicht

**Mögliche Ursachen:**
- Netzteil zu schwach → Verwende mindestens 2A
- WLAN nicht verbunden → Prüfe WLAN-Einstellungen
- Nachtmodus aktiv → Prüfe Einstellungen
- Helligkeit auf 0 → Erhöhe Helligkeit

**Lösung:**
1. Prüfe die Stromversorgung (USB-Kabel fest eingesteckt?)
2. Drücke Taste 1 (Ein/Aus)
3. Drücke Taste 1 doppelt (Helligkeit erhöhen)
4. Öffne Weboberfläche und prüfe Einstellungen

### WLAN-Verbindung schlägt fehl

**Mögliche Ursachen:**
- SSID oder Passwort falsch
- 5 GHz WLAN (nur 2.4 GHz wird unterstützt!)
- Router zu weit entfernt

**Lösung:**
1. Verbinde dich mit dem Hotspot der Uhr ("wortuhr", Passwort: "123456789")
2. Öffne http://192.168.4.1
3. Trage WLAN-Daten erneut ein (auf Groß-/Kleinschreibung achten!)
4. Stelle sicher, dass dein WLAN 2.4 GHz nutzt
5. Platziere die Uhr näher am Router

### Uhr ist nicht unter wortuhr.local erreichbar

**Mögliche Ursachen:**
- mDNS wird vom Router nicht unterstützt
- Firewall blockiert mDNS
- Anderes Gerät nutzt bereits den Namen

**Lösung:**
1. Finde die IP-Adresse in deinem Router (DHCP-Liste)
2. Öffne die IP direkt im Browser (z.B. http://192.168.1.123)
3. Ändere den MQTT Prefix zu einem eindeutigen Namen
4. Oder: Vergebe im Router eine feste IP für die Uhr

### Zeit ist falsch / Zeitzone falsch

**Mögliche Ursachen:**
- Keine WLAN-Verbindung
- NTP-Server nicht erreichbar
- Falsche Zeitzone (bei Custom-Firmware)

**Lösung:**
1. Stelle sicher, dass WLAN verbunden ist
2. Die Uhr synchronisiert die Zeit automatisch über NTP
3. Nach einigen Minuten sollte die Zeit korrekt sein
4. Zeitzone ist fest in der Firmware (CET/CEST für Deutschland/Österreich)

### LEDs flackern / Uhr startet ständig neu

**Mögliche Ursachen:**
- Netzteil zu schwach
- USB-Kabel defekt
- Zu viele LEDs gleichzeitig auf voller Helligkeit

**Lösung:**
1. Verwende ein stärkeres Netzteil (mindestens 2A, besser 3A)
2. Probiere ein anderes USB-Kabel
3. Reduziere die Helligkeit
4. Deaktiviere komplexe Animationen
5. Verwende weniger LEDs gleichzeitig (z.B. kein weißer Hintergrund)

### MQTT verbindet nicht

**Mögliche Ursachen:**
- MQTT-Server-Adresse falsch
- MQTT-Broker nicht erreichbar
- Benutzername/Passwort falsch
- Port falsch

**Lösung:**
1. Prüfe MQTT-Einstellungen (Server, Port, User, Password)
2. Teste MQTT-Broker mit einem Tool (z.B. MQTT Explorer)
3. Schaue in die serielle Konsole für Fehlermeldungen (115200 Baud)
4. Standard-Port ist 1883
5. Die Uhr versucht automatisch alle 15 Sekunden neu zu verbinden

### Home Assistant findet die Uhr nicht

**Lösung:**
1. Stelle sicher, dass MQTT in der Uhr aktiviert ist
2. Stelle sicher, dass "Home Assistant Discovery" aktiviert ist
3. Gehe zu http://wortuhr.local/ha
4. Klicke auf **Discovery jetzt senden**
5. Warte 10-20 Sekunden
6. Prüfe in Home Assistant: **Einstellungen → Geräte & Dienste → MQTT**

### Nach Update funktioniert etwas nicht mehr

**Lösung:**
1. Warte einige Minuten nach dem Update
2. Trenne die Uhr vom Strom, warte 10 Sekunden, verbinde sie wieder
3. Prüfe alle Einstellungen in der Weboberfläche
4. Bei MQTT: Sende Discovery erneut (http://wortuhr.local/ha)
5. Falls nötig: Spiele die vorherige Firmware-Version wieder ein

### Weboberfläche lädt nicht / bleibt hängen

**Lösung:**
1. Lösche Browser-Cache (Strg+F5)
2. Probiere einen anderen Browser
3. Probiere ein anderes Gerät (Smartphone statt PC)
4. Starte die Uhr neu (Strom trennen)
5. Verbinde dich direkt mit dem Hotspot der Uhr

---

## Technische Daten

### Hardware
- **Mikrocontroller**: ESP8266
- **LEDs**: WS2812B (121 LEDs bei 11x11, 64 LEDs bei 8x8)
- **Matrix-Größe**: 11x11 oder 8x8 (je nach Modell)
- **Stromversorgung**: 5V USB (min. 2A empfohlen)
- **WLAN**: 2.4 GHz (802.11 b/g/n)

### Software
- **Firmware**: Open Source (Arduino/ESP8266)
- **NTP**: Automatische Zeitsynchronisation
- **mDNS**: Zugriff über wortuhr.local
- **MQTT**: Smart Home Integration
- **Home Assistant**: Auto-Discovery

### Stromverbrauch
- **Aus**: ~0.5W
- **Wenige LEDs (Minimal)**: ~1-2W
- **Normal (mittlere Helligkeit)**: ~5-10W
- **Maximum (alle LEDs weiß, volle Helligkeit)**: ~20-30W

**Empfehlung:** Nutze ein 5V/3A Netzteil (15W) für normale Nutzung.

---

## Pflege und Wartung

### Reinigung
- Verwende ein **trockenes, weiches Tuch**
- Keine Flüssigkeiten verwenden!
- Nicht mit Wasser reinigen
- Bei Bedarf leicht angefeuchtetes Tuch (nur außen)

### Aufstellungsort
- Nicht direkter Sonneneinstrahlung aussetzen
- Ausreichende Belüftung (nicht abdecken)
- Vor Feuchtigkeit schützen
- Nicht in der Nähe von Heizungen

### Lebensdauer
- LED-Lebensdauer: ca. 50.000 Stunden
- Bei 12h Betrieb/Tag: ca. 11 Jahre
- Reduzierte Helligkeit erhöht Lebensdauer

---

## Version und Modelle

### Deutsche Wortuhr (11x11)
- Standard-Modell
- 121 LEDs
- Deutsche Sprache
- "ES IST FÜNF VOR ZWÖLF"

### Bayrische Wortuhr (11x11)
- 121 LEDs
- Bayrischer Dialekt
- "ES is FÜMF VOR ZWAIFE"

### Mini Wortuhr (8x8)
- Kompakteres Modell
- 64 LEDs
- Verkürzte Anzeige

---

## Rechtliche Hinweise

### Gewährleistung
- Verkauf von Privat gemäß § 13 BGB
- Keine Gewährleistung oder Garantie
- Nutzung auf eigene Verantwortung

### Sicherheitshinweise
- Nur mit geeignetem USB-Netzteil betreiben (min. 2A)
- Nicht öffnen oder modifizieren
- Vor Feuchtigkeit schützen
- Nicht für Kinder unter 3 Jahren geeignet

### Datenschutz
- Alle Daten werden nur lokal gespeichert
- Keine Cloud-Verbindung
- Zeitabfrage über NTP-Server (anonymisiert)
- MQTT-Daten nur im lokalen Netzwerk

---

## Support und Kontakt

### Bei Fragen oder Problemen

**E-Mail:** bichlmaier.andreas0+wortuhr@gmail.de

**Bitte angeben:**
- Modell (Deutsche/Bayrische/Mini)
- Firmware-Version (siehe /update)
- Beschreibung des Problems
- Wenn möglich: Serielle Konsolen-Ausgabe (115200 Baud)

### Nützliche Links
- [MQTT-Anleitung](MQTT_ANLEITUNG.md) - Ausführliche MQTT-Dokumentation
- Weboberfläche: http://wortuhr.local
- Home Assistant Discovery: http://wortuhr.local/ha

---

## Credits

**Entwicklung:** Andy B  
**Firmware-Version:** 4.2.1  
**Datum:** Dezember 2025

### Open-Source-Komponenten
- ESP8266WiFi
- NTPClient
- Adafruit_NeoPixel
- PubSubClient (MQTT)
- ArduinoJson
- OneButton

---

**Viel Spaß mit deiner Wortuhr! ⏰✨**
