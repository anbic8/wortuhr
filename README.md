# Wortuhr - WiFi LED Word Clock ⏰

Eine ESP8266-basierte Wortuhr mit NeoPixel-LEDs, die die Uhrzeit als Text anzeigt. Verfügbar in drei Varianten: Deutsche 11x11, Bayrische 11x11 und Mini 8x8 Version.

## ✨ Features (Auswahl)

- **Zeitanzeige als Wörter** - Die Uhrzeit wird ausgeschrieben angezeigt
- **Drei Varianten:**
  - Deutsche Wortuhr (11x11 Matrix, 121 LEDs)
  - Bayrische Wortuhr (11x11 Matrix, 121 LEDs)  
  - Mini Wortuhr (8x8 Matrix, 64 LEDs)
- **WiFi-Konfiguration** - Einfache Einrichtung über Web-Interface
- **Automatische Zeitsynchronisation** - Via NTP-Server
- **Web-Interface** - Vollständige Steuerung über Browser
- **MQTT-Unterstützung** - Integration in Smart Home Systeme
- **Home Assistant Auto-Discovery** - Automatische Erkennung in Home Assistant
- **Farbkonfiguration** - Individuelle Vorder- und Hintergrundfarben
- **Effekte & Animationen** - Verschiedene Übergangseffekte
- **Nachtmodus** - Automatische Helligkeitsanpassung
- **Geburtstagserinnerungen** - Bis zu 5 Geburtstage mit bunter Anzeige
- **Tasten-Steuerung** - Lokale Bedienung über drei Taster
- **OTA-Updates** - Firmware-Updates über Web-Interface

## 🛠️ Hardware

### Benötigte Komponenten

- **ESP8266** (z.B. Wemos D1 Mini)
- **WS2812B LED-Strip** (NeoPixel):
  - 11x11 Versionen: 121 LEDs
  - 8x8 Version: 64 LEDs
- **USB-Netzteil** mit mindestens 2A bei 5V
- **3 Taster** (optional, für lokale Bedienung)
- **Gehäuse** mit Buchstaben-Front

### Pinout

```
GPIO 14 (D5) - LED Data Pin
GPIO 13 (D7) - Button 1
GPIO 12 (D6) - Button 2
GPIO 4  (D2) - Button 3
```

## 📥 Installation

### 1. PlatformIO

Das Projekt verwendet PlatformIO für die Entwicklung und das Flashen:

```bash
# Repository klonen
git clone https://github.com/DEIN_USERNAME/Wortuhr.git
cd Wortuhr

# Projekt mit PlatformIO öffnen
pio run
```

### 2. Firmware auswählen und hochladen

Wähle die passende Version aus:

```bash
# Deutsche Version (11x11)
pio run -e deutsche_11x11 -t upload

# Bayrische Version (11x11)
pio run -e bayrisch_11x11 -t upload

# Mini Version (8x8)
pio run -e mini_8x8 -t upload
```

### 3. Erste Konfiguration

Nach dem ersten Start:

1. Die Uhr erstellt einen WiFi-Hotspot: **"wortuhr"**
2. Passwort: **"123456789"**
3. Mit dem Hotspot verbinden
4. Browser öffnen und zu `http://192.168.4.1` navigieren
5. WLAN-Daten eingeben und speichern
6. Die Uhr verbindet sich mit deinem Netzwerk
7. Zugriff über `http://wortuhr.local`

## 📖 Verwendung

### Web-Interface

Die Uhr kann vollständig über das Web-Interface gesteuert werden:

- **Verbindung**: WLAN und MQTT-Einstellungen
- **Einstellungen**: Zeitformat, Nachtmodus, Helligkeit
- **Farben**: Vorder-/Hintergrundfarben, Effekte, Animationen
- **Geburtstage**: Spezielle Anzeige für bis zu 5 Geburtstage
- **Info**: Systeminformationen und Status
- **Update**: OTA-Firmware-Updates

### Taster-Bedienung

- **Taster 1**: Ein/Aus
- **Taster 2**: Helligkeit anpassen
- **Taster 3**: Effekt-Modus wechseln

### MQTT & Home Assistant

Die Uhr unterstützt MQTT für Smart Home Integration:

- Automatische Home Assistant Discovery
- Steuerung über MQTT Commands
- Status-Updates in Echtzeit

Siehe [MQTT_ANLEITUNG.md](MQTT_ANLEITUNG.md) für Details.

## 🎨 Farbschemata

Verschiedene Farbschemata stehen zur Verfügung:

1. **Einfarbig** - Eine Farbe für alle Buchstaben
2. **Zweifarbig** - Vorder- und Hintergrundfarbe
3. **Regenbogen** - Automatischer Regenbogen-Effekt
4. **Zufällig** - Jeder Buchstabe in anderer Farbe
5. **Geburtstag** - Buntes Farbschema an Geburtstagen

## 🔄 Updates

Firmware-Updates können direkt über das Web-Interface hochgeladen werden:

1. Im Web-Interface zu "Update" navigieren
2. Neue `.bin` Datei auswählen
3. Upload starten
4. Die Uhr startet nach erfolgreichem Update neu

## 📂 Projekt-Struktur

```
Wortuhr/
├── src/
│   ├── main.cpp           # Hauptprogramm
│   ├── animation.cpp/h    # Startanimationen
│   ├── birthday.cpp/h     # Geburtstags-Funktionen
│   ├── buttons.cpp/h      # Taster-Steuerung
│   ├── color.cpp/h        # Farbverwaltung
│   ├── effects.cpp/h      # Übergangseffekte
│   ├── globals.cpp/h      # Globale Variablen
│   ├── ledmap.h           # LED-Mapping für alle Versionen
│   ├── mqtt.cpp/h         # MQTT-Funktionen
│   ├── mqtt-ha.cpp/h      # Home Assistant Integration
│   ├── mqtt-callback.cpp/h # MQTT-Callback-Handler
│   ├── rct.cpp/h          # Rainbow Color Table
│   ├── show.cpp/h         # Display-Funktionen
│   └── webserver.cpp/h    # Web-Interface
├── platformio.ini         # PlatformIO-Konfiguration
├── ANLEITUNG.md          # Ausführliche deutsche Anleitung
├── MQTT_ANLEITUNG.md     # MQTT-Dokumentation
├── LICENSE               # AGPLv3 Lizenz
└── README.md             # Diese Datei
```

## 🔧 Konfiguration

### Wichtige Build-Flags

Die verschiedenen Versionen werden durch Build-Flags unterschieden:

- `VERSION_TYPE`: 0=Deutsch, 1=Bayrisch, 2=Mini
- `MATRIX_SIZE`: 11 oder 8
- `LED_COUNT`: 121 oder 64

### Dependencies

Das Projekt verwendet folgende Bibliotheken:

- `NTPClient` - Zeitsynchronisation
- `Adafruit NeoPixel` - LED-Ansteuerung
- `OneButton` - Taster-Verwaltung
- `PubSubClient` - MQTT-Client
- `ArduinoJson` - JSON-Verarbeitung

## 🐛 Fehlerbehebung

### Uhr verbindet sich nicht mit WiFi

- Prüfe SSID und Passwort
- Nur 2.4 GHz Netze werden unterstützt
- Nach 10 Sekunden erscheint wieder der Hotspot

### LEDs zeigen falsche Farben

- Netzteil mit mindestens 2A verwenden
- LED-Typ in Code prüfen (WS2812B)

### Uhrzeit ist falsch

- NTP-Server in den Einstellungen prüfen
- Zeitzone korrekt einstellen
- Internet-Verbindung prüfen

Weitere Hilfe in [ANLEITUNG.md](ANLEITUNG.md)

## 📄 Lizenz

Dieses Projekt steht unter der **GNU Affero General Public License v3.0**.

Siehe [LICENSE](LICENSE) für Details.

## 🙏 Credits

- Entwickelt mit [PlatformIO](https://platformio.org/)
- LED-Ansteuerung mit [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- MQTT-Integration für Home Assistant

## 📝 Version

Aktuelle Version: **4.2.6**

---

**Viel Spaß mit deiner Wortuhr! ⏰✨**
