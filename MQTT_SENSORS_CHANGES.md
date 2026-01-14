# MQTT Sensoren - Implementierungsübersicht

## Neue Sensoren hinzugefügt

✅ **IP-Adresse** - Aktuelle IP des Geräts
✅ **Uptime** - Laufzeit seit letztem Neustart (Sekunden)
✅ **WiFi Signalstärke (RSSI)** - Verbindungsqualität in dBm
✅ **Freier Speicher** - Verfügbarer RAM in Bytes
✅ **LED Helligkeit** - Aktuelle Einstellung in %

## Implementierte Features

### Home Assistant Discovery
- Automatische Erkennung aller 5 Sensoren
- Alle Sensoren unter Gerät "Wortuhr" organisiert
- Mit Icons und Kategorien ("diagnostic")
- Proper device info (Hersteller, Modell, FW-Version, Config-URL)

### MQTT Topics
```
{prefix}/ip_address        # z.B. "192.168.1.42"
{prefix}/uptime            # z.B. "3600" (in Sekunden)
{prefix}/rssi              # z.B. "-55" (dBm)
{prefix}/heap_memory       # z.B. "28960" (Bytes)
{prefix}/brightness        # z.B. "75" (%)
```

### Automatisches Publishing
- Discovery beim MQTT-Connect
- Sensor-Werte alle 60 Sekunden
- Retain-Flag: true (Werte bleiben persistent)

## Dateien-Änderungen

### Headers
- `src/mqtt.h` - Sensor Publishing Functions
- `src/mqtt-ha.h` - Sensor Discovery Functions

### Source Files
- `src/globals.h/cpp` - Sensor MQTT Topics deklariert
- `src/mqtt-ha.cpp` - 5x Discovery Config Functions hinzugefügt
- `src/mqtt.cpp` - 6x Sensor Publishing Functions + Koordination
- `src/main.cpp` - Regelmäßiges Publishing alle 60 Sekunden

### Dokumentation
- `SENSOR_MQTT_DOKU.md` - Vollständige Sensor-Doku mit HA-Automationen

## Technische Details

### Publishing-Frequenz
- **Discovery**: Einmalig beim Connect (oder nach Firmware-Update)
- **Sensoren**: Alle 60 Sekunden via `publishSensorStates()`

### Memory/Performance
- Minimale Overhead (kleine Char-Buffer für Konvertierung)
- Non-blocking MQTT Publishing (async)
- Heap-Monitoring ohne zusätzliche Last

### Fehlerbehandlung
- Publish nur wenn MQTT connected
- Automatischer Retry bei Publish-Fehler
- Warnung in Console wenn Heap < 8KB

## Testing

### Manual Testing
```bash
# MQTT Topics abonnieren:
mosquitto_sub -h {mqtt_broker} -t '{prefix}/#'

# Sollte folgende Messages anzeigen:
{prefix}/ip_address       {"state": "192.168.1.42"}
{prefix}/uptime           {"state": "3600"}
{prefix}/rssi             {"state": "-55"}
{prefix}/heap_memory      {"state": "28960"}
{prefix}/brightness       {"state": "75"}
```

### Home Assistant
1. MQTT Device sollte 5 neue Sensoren zeigen
2. Diagnostisch-Kategorie sichtbar
3. Values aktualisieren sich alle 60 Sekunden

## Nächste Mögliche Erweiterungen

Weitere sensorische Daten die sinnvoll sein könnten:
- 🌡️ **Temperatur** - ESP8266 interne Temperatur
- 🔌 **Spannung** - ADC analog input für externe Messung
- ⏰ **NTP Sync Status** - Letzter erfolgreicher NTP-Sync
- 🎨 **Aktive Effekte/Animation** - Aktuell laufender Effekt-Index
- 📊 **LED Count** - Aktuelle LED-Zahl bei Matrix
- 🔋 **System-Load** - CPU-Auslastung/Loop-Frequenz

Diese können auf Nachfrage hinzugefügt werden.
