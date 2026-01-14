# 🆕 MQTT Sensoren hinzugefügt

## Version 4.2.8+ - MQTT Sensor Update

### Neue Features

#### 8 neue Home Assistant MQTT Sensoren:
1. **IP-Adresse** - Netzwerk-Diagnostik
2. **Uptime** - Laufzeit seit Neustart
3. **WiFi Signalstärke (RSSI)** - Verbindungsqualität
4. **Freier Speicher** - RAM-Überwachung
5. **LED Helligkeit** - Aktuelle Helligkeit
6. **Letzter NTP Sync** - Zeit-Synchronisations-Status
7. **CPU Temperatur** - Thermalüberwachung
8. **System Auslastung** - Speicherauslastung %

### Features

- ✅ Automatische Home Assistant Discovery
- ✅ Alle Sensoren als "diagnostic" kategorisiert
- ✅ Alle 60 Sekunden aktualisiert
- ✅ Retain-Flag für Persistenz
- ✅ Fehlersicher bei WiFi-Disconnect
- ✅ Minimal RAM-Footprint
- ✅ Vollständige Dokumentation

### Home Assistant

**Automatische Erkennung:**
- Sensoren erscheinen automatisch im "Wortuhr" Device
- Keine manuelle Konfiguration nötig
- Funktioniert mit jedem MQTT Broker

**Entity-IDs (Beispiel):**
```
sensor.wortuhr_testuhr11_ip_address
sensor.wortuhr_testuhr11_uptime
sensor.wortuhr_testuhr11_rssi
sensor.wortuhr_testuhr11_heap_memory
sensor.wortuhr_testuhr11_brightness
sensor.wortuhr_testuhr11_last_ntp_sync
sensor.wortuhr_testuhr11_temperature
sensor.wortuhr_testuhr11_system_load
```

### Automations-Beispiele

📋 Siehe [SENSOR_MQTT_DOKU.md](SENSOR_MQTT_DOKU.md) für Automations-Vorlagen:
- WiFi Signal Warnung
- Speicher-Warnung
- CPU Temperatur Warnung
- System-Last Warnung
- NTP Sync Fehler
- Uptime Formatierung

### Implementierungsdetails

**Modifizierte Dateien:**
- `src/mqtt.h` - Funktions-Deklarationen
- `src/mqtt-ha.h` - Discovery Config Deklarationen
- `src/mqtt-ha.cpp` - 8 Discovery Config Funktionen
- `src/mqtt.cpp` - Sensor Publishing + Discovery Calls
- `src/globals.h/cpp` - 8 neue MQTT Topics
- `src/main.cpp` - Periodisches Publishing

**Publishing-Zeitplan:**
- Bei Connect: Discovery Configs
- Danach: Alle 60 Sekunden Aktualisierung

### Dokumentation

📚 **Neue Dateien:**
- `SENSOR_MQTT_DOKU.md` - Komplette Sensor-Dokumentation
- `SENSOR_IMPLEMENTATION.md` - Technische Implementierungsdetails

### Troubleshooting

**Sensoren nicht sichtbar?**
- MQTT enable prüfen
- Firmware neu laden (triggert Discovery)

**Werte aktualisieren sich nicht?**
- MQTT-Verbindung prüfen
- WiFi-Status überprüfen

Siehe [SENSOR_MQTT_DOKU.md](SENSOR_MQTT_DOKU.md#troubleshooting) für ausführliches Troubleshooting.
