# Wortuhr MQTT Sensor Dokumentation

## Neue Sensoren

Die Wortuhr sendet jetzt automatisch 8 Diagnose-Sensoren via MQTT an Home Assistant (mit Home Assistant Discovery):

### 1. **IP-Adresse** (`ip_address`)
- **Topic**: `{prefix}/ip_address`
- **Wert**: Aktuelle IP-Adresse des Geräts (z.B. `192.168.1.42`)
- **Icon**: 📡 IP
- **Aktualisierung**: Alle 60 Sekunden

### 2. **Uptime** (`uptime`)
- **Topic**: `{prefix}/uptime`
- **Wert**: Sekunden seit Neustart
- **Einheit**: Sekunden (s)
- **Icon**: ⏱️ Uhr
- **Beispiel**: `3600` = 1 Stunde

### 3. **WiFi Signalstärke** (`rssi`)
- **Topic**: `{prefix}/rssi`
- **Wert**: RSSI in dBm
- **Einheit**: dBm
- **Icon**: 📶 WiFi
- **Richtwerte**: -50 bis -70 dBm normal, < -90 dBm kritisch

### 4. **Freier Speicher** (`heap_memory`)
- **Topic**: `{prefix}/heap_memory`
- **Wert**: Verfügbarer RAM in Bytes
- **Einheit**: Bytes
- **Icon**: 💾 Memory
- **Hinweis**: ~40KB normal, < 8KB kritisch

### 5. **LED Helligkeit** (`brightness`)
- **Topic**: `{prefix}/brightness`
- **Wert**: Aktuelle LED-Helligkeit
- **Einheit**: % (0-100)
- **Icon**: 💡 Helligkeit

### 6. **Letzter NTP Sync** (`last_ntp_sync`)
- **Topic**: `{prefix}/last_ntp_sync`
- **Wert**: Zeitstempel letzter erfolgreicher Sync (ISO 8601)
- **Format**: `2024-01-14T15:30:45Z`
- **Icon**: 🕐 Zeit-Check
- **Hinweis**: "never" wenn noch nicht synchronisiert

### 7. **CPU Temperatur** (`temperature`)
- **Topic**: `{prefix}/temperature`
- **Wert**: Interne CPU-Temperatur
- **Einheit**: °C
- **Icon**: 🌡️ Thermometer
- **Hinweis**: Genauigkeit ±10°C, ~40-60°C normal

### 8. **System Auslastung** (`system_load`)
- **Topic**: `{prefix}/system_load`
- **Wert**: Speicherauslastung
- **Einheit**: % (0-100)
- **Icon**: 🖥️ CPU
- **Berechnung**: Basiert auf verfügbarem Heap
- **Warnung**: > 80% = Speicherdruck

## MQTT Topics Übersicht

```
{prefix}/ip_address          → IP-Adresse
{prefix}/uptime              → Uptime in Sekunden
{prefix}/rssi                → WiFi-Signalstärke (dBm)
{prefix}/heap_memory         → Freier RAM (Bytes)
{prefix}/brightness          → LED-Helligkeit (%)
{prefix}/last_ntp_sync       → Letzter NTP Sync (ISO 8601)
{prefix}/temperature         → CPU-Temperatur (°C)
{prefix}/system_load         → System-Auslastung (%)
```

Wobei `{prefix}` = MQTT-Prefix aus den Einstellungen (z.B. `wortuhr/testuhr11/`)

## Home Assistant Integration

### Automatische Entity-IDs

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

### Discovery Topics

```
homeassistant/sensor/{device_id}_ip_address/config
homeassistant/sensor/{device_id}_uptime/config
homeassistant/sensor/{device_id}_rssi/config
homeassistant/sensor/{device_id}_heap_memory/config
homeassistant/sensor/{device_id}_brightness/config
homeassistant/sensor/{device_id}_last_ntp_sync/config
homeassistant/sensor/{device_id}_temperature/config
homeassistant/sensor/{device_id}_system_load/config
```

## Publishing-Zeitplan

Alle Sensoren werden:
- Bei MQTT-Connect: Discovery-Config übertragen
- Danach: Alle 60 Sekunden aktualisiert (Retain=True)

## Empfohlene Automationen

### 1. Warnung bei schlechtem WiFi-Signal
```yaml
automation:
  - alias: "Wortuhr - WiFi Signal schwach"
    trigger:
      platform: numeric_state
      entity_id: sensor.wortuhr_testuhr11_rssi
      below: -80
    action:
      service: notify.mobile_app
      data:
        message: "Wortuhr WiFi-Signal schwach ({{ trigger.to_state.state }} dBm)"
```

### 2. Warnung bei niedrigem Speicher
```yaml
automation:
  - alias: "Wortuhr - Speicher kritisch"
    trigger:
      platform: numeric_state
      entity_id: sensor.wortuhr_testuhr11_heap_memory
      below: 8192
    action:
      service: notify.mobile_app
      data:
        message: "Wortuhr: Speicher kritisch ({{ trigger.to_state.state }} Bytes)"
```

### 3. Warnung bei hoher System-Auslastung
```yaml
automation:
  - alias: "Wortuhr - System überlastet"
    trigger:
      platform: numeric_state
      entity_id: sensor.wortuhr_testuhr11_system_load
      above: 80
    action:
      service: notify.mobile_app
      data:
        message: "Wortuhr: System überlastet ({{ trigger.to_state.state }}%)"
```

### 4. Warnung bei CPU Überhitzung
```yaml
automation:
  - alias: "Wortuhr - CPU zu heiß"
    trigger:
      platform: numeric_state
      entity_id: sensor.wortuhr_testuhr11_temperature
      above: 80
    action:
      service: notify.mobile_app
      data:
        message: "Wortuhr: CPU-Temperatur zu hoch ({{ trigger.to_state.state }}°C)"
```

### 5. Warnung bei NTP Sync-Fehler
```yaml
automation:
  - alias: "Wortuhr - Zeitsynchronisierung fehlgeschlagen"
    trigger:
      platform: state
      entity_id: sensor.wortuhr_testuhr11_last_ntp_sync
      to: "never"
    action:
      service: notify.mobile_app
      data:
        message: "Wortuhr: Zeit wurde noch nie synchronisiert!"
```

### 6. Uptime formatiert anzeigen
```yaml
template:
  - sensor:
      - name: "Wortuhr Uptime (formatiert)"
        unique_id: wortuhr_uptime_formatted
        state: >
          {%- set seconds = states('sensor.wortuhr_testuhr11_uptime') | int(0) -%}
          {%- set days = seconds // 86400 -%}
          {%- set hours = (seconds % 86400) // 3600 -%}
          {%- set mins = (seconds % 3600) // 60 -%}
          {% if days > 0 %}{{ days }}d {{ hours }}h{% else %}{{ hours }}h {{ mins }}m{% endif %}
```

## Quelldateien

Implementierung in:
- `src/mqtt.h` - 8 neue Funktions-Deklarationen
- `src/mqtt.cpp` - Sensor Publishing + Discovery Calls
- `src/mqtt-ha.cpp` - 8 Discovery Config Funktionen
- `src/globals.h/cpp` - 8 neue MQTT Topics
- `src/main.cpp` - Periodisches Publishing (60s Intervall)

## Troubleshooting

### Sensoren werden nicht angezeigt
- MQTT enable prüfen
- Firmware neu laden (triggert Discovery)
- MQTT-Broker Logs prüfen

### Werte werden nicht aktualisiert
- MQTT-Verbindung prüfen (Serial Console)
- WiFi-Disconnect verhindert Publishing

### Speicher-Warnung
- Heap < 8KB ist kritisch
- Kann auf Speicherlecks hindeuten
- Restart hilft meist

### Temperatur unrealistisch
- ESP8266 Sensor hat begrenzte Genauigkeit (±10°C)
- Für Trends geeignet, nicht für absolute Messungen

### NTP Sync zeigt "never"
- Gerät war beim Boot nicht mit Netzwerk verbunden
- Zeigt sich nach erfolgreicher Sync oder Neustart korrekt
