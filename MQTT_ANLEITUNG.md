# MQTT Anleitung für die Wortuhr

## 1. MQTT in der Uhr einrichten

### Grundkonfiguration
1. Öffne die Weboberfläche der Uhr: `http://wortuhr.local` (oder die IP-Adresse)
2. Gehe zu **Verbindung** im Menü
3. Fülle folgende Felder aus:
   - **SSID**: Dein WLAN-Name
   - **Password**: Dein WLAN-Passwort
   - **Mqtt aktivieren**: ✓ Haken setzen
   - **Home Assistant Discovery**: ✓ Haken setzen (falls du Home Assistant nutzt)
   - **MQTT server**: IP-Adresse oder Hostname deines MQTT-Brokers (z.B. `192.168.1.100` oder `homeassistant.local`)
   - **MQTT port**: Standard ist `1883`
   - **MQTT user**: Benutzername für MQTT (falls erforderlich)
   - **MQTT Password**: Passwort für MQTT (falls erforderlich)
   - **MQTT Prefix**: Name für deine Uhr (z.B. `wortuhr` oder `wohnzimmer`)
4. Klicke auf **Save**
5. Die Uhr startet neu und verbindet sich mit dem MQTT-Broker

## 2. MQTT Topics

Alle Topics basieren auf dem **MQTT Prefix**, den du in der Konfiguration festgelegt hast.
Beispiel: Wenn dein Prefix `wortuhr` ist, lauten die Topics:

### Status-Topics (Status der Uhr)
Die Uhr veröffentlicht ihren aktuellen Status auf folgenden Topics:

| Topic | Beschreibung | Beispielwert |
|-------|--------------|--------------|
| `wortuhr/on` | Ein/Aus Status | `1` (an) oder `0` (aus) |
| `wortuhr/efx` | Aktueller Effekt | `kein Effekt`, `Fade`, `Running`, etc. |
| `wortuhr/ani` | Aktuelle Animation | `keine Animation`, `Blinken`, `Pulsieren`, etc. |
| `wortuhr/v1` | Vordergrundfarbe 1 (JSON) | `{"state":"ON","brightness":250,"color_mode":"rgb","color":{"r":128,"g":128,"b":128}}` |
| `wortuhr/v2` | Vordergrundfarbe 2 (JSON) | `{"state":"ON","brightness":250,"color_mode":"rgb","color":{"r":128,"g":128,"b":0}}` |
| `wortuhr/h1` | Hintergrundfarbe 1 (JSON) | `{"state":"OFF","brightness":250,"color_mode":"rgb","color":{"r":0,"g":0,"b":0}}` |
| `wortuhr/h2` | Hintergrundfarbe 2 (JSON) | `{"state":"OFF","brightness":250,"color_mode":"rgb","color":{"r":0,"g":0,"b":0}}` |
| `wortuhr/vs` | Vordergrund Farbschema | `einfarbig`, `Schachbrett`, `Spalten`, etc. |
| `wortuhr/hs` | Hintergrund Farbschema | `einfarbig`, `Schachbrett`, `Spalten`, etc. |
| `wortuhr/efxtime` | Effekt-Geschwindigkeit | `langsam`, `mittel`, `schnell` |
| `wortuhr/anitime` | Animations-Geschwindigkeit | `langsam`, `mittel`, `schnell` |
| `wortuhr/anidepth` | Animations-Intensität | `schwach`, `mittel`, `stark` |

### Befehls-Topics (Steuerung der Uhr)
Um die Uhr zu steuern, sende MQTT-Nachrichten an folgende Topics:

| Topic | Beschreibung | Mögliche Werte |
|-------|--------------|----------------|
| `wortuhr/on/set` | Uhr ein/ausschalten | `1` oder `0` |
| `wortuhr/efx/set` | Effekt setzen | `kein Effekt`, `zufällig`, `Fade`, `Running`, `Schlange`, `Zeilen`, `Scrollen`, `Slide in`, `Diagonal`, `Rain`, `Spirale`, `Schlangenfresser`, `Raute` |
| `wortuhr/ani/set` | Animation setzen | `keine Animation`, `Blinken`, `Vordergrundblinken`, `Pulsieren`, `Verlauf`, `Fliegen` |
| `wortuhr/v1/set` | Vordergrundfarbe 1 (JSON) | Siehe Beispiel unten |
| `wortuhr/v2/set` | Vordergrundfarbe 2 (JSON) | Siehe Beispiel unten |
| `wortuhr/h1/set` | Hintergrundfarbe 1 (JSON) | Siehe Beispiel unten |
| `wortuhr/h2/set` | Hintergrundfarbe 2 (JSON) | Siehe Beispiel unten |
| `wortuhr/vs/set` | Vordergrund Farbschema | `einfarbig`, `Schachbrett`, `Spalten`, `Zeilen`, `Verlauf`, `Zufällig` |
| `wortuhr/hs/set` | Hintergrund Farbschema | `einfarbig`, `Schachbrett`, `Spalten`, `Zeilen`, `Verlauf`, `Zufällig` |
| `wortuhr/efxtime/set` | Effekt-Geschwindigkeit | `langsam`, `mittel`, `schnell` |
| `wortuhr/anitime/set` | Animations-Geschwindigkeit | `langsam`, `mittel`, `schnell` |
| `wortuhr/anidepth/set` | Animations-Intensität | `schwach`, `mittel`, `stark` |

## 3. MQTT-Befehle senden

### Mit Mosquitto (Kommandozeile)

**Uhr einschalten:**
```bash
mosquitto_pub -h 192.168.1.100 -t wortuhr/on/set -m "1"
```

**Uhr ausschalten:**
```bash
mosquitto_pub -h 192.168.1.100 -t wortuhr/on/set -m "0"
```

**Effekt ändern:**
```bash
mosquitto_pub -h 192.168.1.100 -t wortuhr/efx/set -m "Fade"
```

**Animation setzen:**
```bash
mosquitto_pub -h 192.168.1.100 -t wortuhr/ani/set -m "Pulsieren"
```

**Farbe ändern (JSON):**
```bash
mosquitto_pub -h 192.168.1.100 -t wortuhr/v1/set -m '{"state":"ON","brightness":255,"color":{"r":255,"g":0,"b":0}}'
```

**Helligkeit ändern:**
```bash
mosquitto_pub -h 192.168.1.100 -t wortuhr/v1/set -m '{"brightness":100}'
```

### Mit MQTT Explorer (GUI)

1. Öffne MQTT Explorer
2. Verbinde dich mit deinem MQTT-Broker
3. Navigiere zum Topic `wortuhr/on/set`
4. Gebe den Wert ein (z.B. `1` für Ein)
5. Klicke auf **Publish**

### Mit Node-RED

Beispiel Flow zum Einschalten der Uhr:
```json
[
    {
        "id": "mqtt_out",
        "type": "mqtt out",
        "topic": "wortuhr/on/set",
        "qos": "0",
        "retain": "false",
        "broker": "mqtt_broker",
        "name": "Wortuhr Ein"
    },
    {
        "id": "inject",
        "type": "inject",
        "payload": "1",
        "payloadType": "str",
        "topic": "",
        "name": "Ein",
        "wires": [["mqtt_out"]]
    }
]
```

## 4. Home Assistant Integration

### Automatische Discovery

Wenn **Home Assistant Discovery** aktiviert ist:
1. Die Uhr meldet sich automatisch bei Home Assistant an
2. Alle Entities werden automatisch erstellt
3. Du findest die Uhr unter **Einstellungen → Geräte & Dienste → MQTT**
4. Suche nach "Wortuhr" (oder deinem Prefix)

### Manuell Discovery senden

Falls die Entities nicht erscheinen:
1. Gehe zu `http://wortuhr.local/ha`
2. Klicke auf **Discovery jetzt senden**
3. Warte einige Sekunden
4. Die Entities sollten in Home Assistant erscheinen

### Verfügbare Entities in Home Assistant

Nach erfolgreicher Discovery sind folgende Entities verfügbar:
- **Switch**: `switch.wortuhr` - Ein/Aus
- **Select**: `select.wortuhr_effekt` - Effekt auswählen
- **Select**: `select.wortuhr_animation` - Animation auswählen
- **Light**: `light.wortuhr_vordergrund_1` - Vordergrundfarbe 1
- **Light**: `light.wortuhr_vordergrund_2` - Vordergrundfarbe 2
- **Light**: `light.wortuhr_hintergrund_1` - Hintergrundfarbe 1
- **Light**: `light.wortuhr_hintergrund_2` - Hintergrundfarbe 2
- **Select**: `select.wortuhr_vordergrund_schema` - Vordergrund Farbschema
- **Select**: `select.wortuhr_hintergrund_schema` - Hintergrund Farbschema
- **Select**: `select.wortuhr_effekt_geschwindigkeit` - Effekt-Geschwindigkeit
- **Select**: `select.wortuhr_animation_geschwindigkeit` - Animations-Geschwindigkeit
- **Select**: `select.wortuhr_animation_intensität` - Animations-Intensität

### Beispiel Home Assistant Automation

**Uhr morgens einschalten:**
```yaml
automation:
  - alias: "Wortuhr morgens einschalten"
    trigger:
      - platform: time
        at: "07:00:00"
    action:
      - service: switch.turn_on
        target:
          entity_id: switch.wortuhr
```

**Farbe abhängig von Sonnenstand:**
```yaml
automation:
  - alias: "Wortuhr Farbe bei Sonnenuntergang"
    trigger:
      - platform: sun
        event: sunset
    action:
      - service: light.turn_on
        target:
          entity_id: light.wortuhr_vordergrund_1
        data:
          rgb_color: [255, 100, 0]
          brightness: 200
```

## 5. Beispiele für Farbsteuerung (JSON)

### Farbe setzen (Rot)
```json
{
  "state": "ON",
  "brightness": 255,
  "color": {
    "r": 255,
    "g": 0,
    "b": 0
  }
}
```

### Nur Helligkeit ändern
```json
{
  "brightness": 100
}
```

### Farbe ausschalten
```json
{
  "state": "OFF"
}
```

### Vollständiges Beispiel (Grün mit 50% Helligkeit)
```json
{
  "state": "ON",
  "brightness": 128,
  "color": {
    "r": 0,
    "g": 255,
    "b": 0
  }
}
```

## 6. Farbschemata

### Vordergrund-Farbschemata
- **einfarbig**: Nur Farbe 1 wird verwendet
- **Schachbrett**: Farbe 1 und 2 im Schachbrettmuster
- **Spalten**: Farbe 1 und 2 spaltenweise
- **Zeilen**: Farbe 1 und 2 zeilenweise
- **Verlauf**: Farbverlauf von Farbe 1 zu Farbe 2
- **Zufällig**: Zufällige Farben

### Hintergrund-Farbschemata
Gleiche Optionen wie Vordergrund, werden auf nicht leuchtende Buchstaben angewendet.

## 7. Effekte

- **kein Effekt**: Direkter Übergang zwischen Zeitanzeigen
- **zufällig**: Zufälliger Effekt bei jedem Übergang
- **Fade**: Sanftes Ein-/Ausblenden
- **Running**: Laufende Animation
- **Schlange**: Schlangen-Animation
- **Zeilen**: Zeilenweise Animation
- **Scrollen**: Scroll-Effekt
- **Slide in**: Einschieben
- **Diagonal**: Diagonale Animation
- **Rain**: Regen-Effekt
- **Spirale**: Spiral-Animation
- **Schlangenfresser**: Snake-Game-ähnlich

## 8. Animationen

- **keine Animation**: Statische Anzeige
- **Blinken**: Komplette Anzeige blinkt
- **Vordergrundblinken**: Nur aktive Buchstaben blinken
- **Pulsieren**: Sanftes Pulsieren der Helligkeit
- **Verlauf**: Farbverlauf-Animation
- **Fliegen**: Fliegende Pixel-Animation

## 9. Troubleshooting

### Uhr verbindet sich nicht mit MQTT
- Prüfe die MQTT-Server-Adresse und Port
- Prüfe Benutzername und Passwort
- Schaue in die serielle Konsole für Fehlermeldungen
- Die Uhr versucht alle 15 Sekunden erneut zu verbinden

### Home Assistant findet die Uhr nicht
- Prüfe, ob MQTT in Home Assistant konfiguriert ist
- Prüfe, ob "Home Assistant Discovery" in der Uhr aktiviert ist
- Gehe zu `/ha` und klicke auf "Discovery jetzt senden"
- Prüfe in Home Assistant unter **MQTT** → **Geräte**

### Befehle werden nicht ausgeführt
- Prüfe, ob die Topics korrekt sind (achte auf den Prefix)
- Prüfe, ob die Werte korrekt geschrieben sind (Groß-/Kleinschreibung beachten)
- Prüfe mit einem MQTT-Client wie MQTT Explorer, ob Nachrichten ankommen

### Status wird nicht aktualisiert
- Die Uhr sendet Status-Updates nach jeder Änderung
- Prüfe mit MQTT Explorer, ob die Topics erscheinen
- Stelle sicher, dass die MQTT-Verbindung stabil ist

## 10. MQTT-Broker einrichten (falls noch nicht vorhanden)

### Mit Home Assistant (Mosquitto Add-on)
1. Gehe zu **Einstellungen → Add-ons**
2. Suche nach "Mosquitto broker"
3. Installiere und starte den Add-on
4. Erstelle einen Benutzer unter **Konfiguration**

### Standalone Mosquitto (Raspberry Pi)
```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients
sudo systemctl enable mosquitto
sudo systemctl start mosquitto
```

Benutzer erstellen:
```bash
sudo mosquitto_passwd -c /etc/mosquitto/passwd dein_benutzer
```

## Support

Bei Fragen oder Problemen:
- Email: bichlmaier.andreas0+wortuhr@gmail.de
- Prüfe die serielle Konsole für Debug-Ausgaben (115200 Baud)
