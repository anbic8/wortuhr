#include "webserver_html.h"

#ifdef USE_RCT
const char htmlhead[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Wortuhr</title>
  <style>
    :root {
      --bg: #1b232c;
      --panel: rgba(255,255,255,0.06);
      --accent: #4caf50;
      --text: #f2f2f2;
      --muted: rgba(255,255,255,0.7);
      --border: rgba(255,255,255,0.12);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #19222b, #2f3b45);
      color: var(--text);
    }
    nav {
      position: sticky;
      top: 0;
      z-index: 10;
      background: #10161c;
      border-bottom: 1px solid var(--border);
      padding: 10px 12px;
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      align-items: center;
    }
    .nav-bar {
      flex: 1 1 180px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
    }
    .nav-title {
      font-weight: 700;
      letter-spacing: 0.2px;
    }
    .nav-toggle {
      position: absolute;
      opacity: 0;
      pointer-events: none;
    }
    .nav-btn {
      display: none;
      color: var(--text);
      background: rgba(255,255,255,0.08);
      border-radius: 6px;
      padding: 6px 10px;
      cursor: pointer;
    }
    .nav-links {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
    }
    nav a {
      color: var(--text);
      text-decoration: none;
      padding: 6px 10px;
      border-radius: 6px;
      background: rgba(255,255,255,0.04);
    }
    nav a:hover {
      background: rgba(255,255,255,0.12);
    }
    @media (max-width: 720px) {
      .nav-btn {
        display: inline-flex;
        align-items: center;
      }
      .nav-links {
        display: none;
        width: 100%;
        flex-direction: column;
        align-items: stretch;
      }
      .nav-toggle:checked + .nav-links {
        display: flex;
      }
    }
    main {
      max-width: 920px;
      margin: 20px auto 40px;
      padding: 0 16px;
    }
    h1, h2 { margin: 12px 0; }
    .card {
      background: var(--panel);
      border: 1px solid var(--border);
      border-radius: 10px;
      padding: 16px;
      margin: 12px 0;
    }
    label {
      display: block;
      margin: 10px 0 6px;
      color: var(--muted);
    }
    .form-row {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
    }
    .form-row .field {
      display: flex;
      flex-direction: column;
    }
    input, select {
      width: 100%;
      padding: 8px 10px;
      border-radius: 6px;
      border: 1px solid #555;
      background: #0f1419;
      color: var(--text);
    }
    button {
      width: 100%;
      margin-top: 12px;
      padding: 10px 12px;
      border: 0;
      border-radius: 8px;
      color: #fff;
      background: var(--accent);
      cursor: pointer;
    }
    small { color: var(--muted); }
    a.link { color: #8dd1ff; }
  </style>
</head>
<body>
  <nav>
    <div class="nav-bar">
      <span class="nav-title">Wortuhr</span>
      <label class="nav-btn" for="nav-toggle">Menue</label>
    </div>
    <input class="nav-toggle" type="checkbox" id="nav-toggle">
    <div class="nav-links">
      <a href="/wifi">Verbindung</a>
      <a href="/setting">Einstellungen</a>
      <a href="/settime">Zeit einstellen</a>
      <a href="/color">Farben</a>
      <a href="/birthday">Geburtstag/Countdown</a>
      <a href="/info">Info</a>
      <a href="/update">Update</a>
    </div>
  </nav>
  <main>

)rawliteral";
#else
const char htmlhead[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Wortuhr</title>
  <style>
    :root {
      --bg: #1b232c;
      --panel: rgba(255,255,255,0.06);
      --accent: #4caf50;
      --text: #f2f2f2;
      --muted: rgba(255,255,255,0.7);
      --border: rgba(255,255,255,0.12);
    }
    * { box-sizing: border-box; }
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: linear-gradient(135deg, #19222b, #2f3b45);
      color: var(--text);
    }
    nav {
      position: sticky;
      top: 0;
      z-index: 10;
      background: #10161c;
      border-bottom: 1px solid var(--border);
      padding: 10px 12px;
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
      align-items: center;
    }
    .nav-bar {
      flex: 1 1 180px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 12px;
    }
    .nav-title {
      font-weight: 700;
      letter-spacing: 0.2px;
    }
    .nav-toggle {
      position: absolute;
      opacity: 0;
      pointer-events: none;
    }
    .nav-btn {
      display: none;
      color: var(--text);
      background: rgba(255,255,255,0.08);
      border-radius: 6px;
      padding: 6px 10px;
      cursor: pointer;
    }
    .nav-links {
      display: flex;
      flex-wrap: wrap;
      gap: 10px;
    }
    nav a {
      color: var(--text);
      text-decoration: none;
      padding: 6px 10px;
      border-radius: 6px;
      background: rgba(255,255,255,0.04);
    }
    nav a:hover {
      background: rgba(255,255,255,0.12);
    }
    @media (max-width: 720px) {
      .nav-btn {
        display: inline-flex;
        align-items: center;
      }
      .nav-links {
        display: none;
        width: 100%;
        flex-direction: column;
        align-items: stretch;
      }
      .nav-toggle:checked + .nav-links {
        display: flex;
      }
    }
    main {
      max-width: 920px;
      margin: 20px auto 40px;
      padding: 0 16px;
    }
    h1, h2 { margin: 12px 0; }
    .card {
      background: var(--panel);
      border: 1px solid var(--border);
      border-radius: 10px;
      padding: 16px;
      margin: 12px 0;
    }
    label {
      display: block;
      margin: 10px 0 6px;
      color: var(--muted);
    }
    .form-row {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
    }
    .form-row .field {
      display: flex;
      flex-direction: column;
    }
    input, select {
      width: 100%;
      padding: 8px 10px;
      border-radius: 6px;
      border: 1px solid #555;
      background: #0f1419;
      color: var(--text);
    }
    button {
      width: 100%;
      margin-top: 12px;
      padding: 10px 12px;
      border: 0;
      border-radius: 8px;
      color: #fff;
      background: var(--accent);
      cursor: pointer;
    }
    small { color: var(--muted); }
    a.link { color: #8dd1ff; }
  </style>
</head>
<body>
  <nav>
    <div class="nav-bar">
      <span class="nav-title">Wortuhr</span>
      <label class="nav-btn" for="nav-toggle">Menue</label>
    </div>
    <input class="nav-toggle" type="checkbox" id="nav-toggle">
    <div class="nav-links">
      <a href="/wifi">Verbindung</a>
      <a href="/setting">Einstellungen</a>
      <a href="/color">Farben</a>
      <a href="/birthday">Geburtstag/Countdown</a>
      <a href="/info">Info</a>
      <a href="/update">Update</a>
    </div>
  </nav>
  <main>

)rawliteral";
#endif

const char htmlinfo[] PROGMEM = R"rawliteral(
<h1>Wortuhr Info</h1>

<div class="card">
  <p>Dieses Webinterface laeuft lokal im Heimnetzwerk. Hier richtest du WLAN, Farben, Geburtstage und Updates ein.</p>
</div>

<div class="card">
  <h2>Schnellzugriffe</h2>
  <ul>
    <li><a class="link" href="/wifi">Verbindung</a> einrichten</li>
    <li><a class="link" href="/color">Farben</a> anpassen</li>
    <li><a class="link" href="/birthday">Geburtstag/Countdown</a> verwalten</li>
    <li><a class="link" href="/update">Update</a> starten</li>
  </ul>
</div>

<div class="card">
  <h2>Tipps</h2>
  <ul>
    <li>Nach dem Speichern von WLAN oder MQTT startet die Uhr neu.</li>
    <li>Ein Countdown kann fuer ein wichtiges Datum gesetzt werden.</li>
    <li>Nutze ein stabiles 5V Netzteil (mindestens 2A).</li>
  </ul>
</div>

<div class="card">
  <p>Rechtliches findest du unter <a class="link" href="/datenschutz">Datenschutz</a>.</p>
</div>

<div class="card">
  <p>Anleitung: <a class="link" href="https://github.com/anbic8/wortuhr/blob/main/ANLEITUNG.md" target="_blank">ANLEITUNG.md</a></p>
</div>
)rawliteral";

const char htmlrecht[] PROGMEM = R"rawliteral(
<main style="max-width: 800px; margin: 0 auto;">
    <h1><i class="fa-solid fa-scale-balanced"></i> Rechtliche Hinweise</h1>
    
    <div style="background: rgba(33, 150, 243, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 30px; border-left: 4px solid #2196f3;">
        <p style="margin: 0; font-size: 1.05em;">
            <i class="fas fa-info-circle"></i> Transparenz ist mir wichtig! Hier findest du alle Informationen zu Datenschutz, 
            Gewährleistung und den verwendeten Open-Source-Komponenten deiner Wortuhr.
        </p>
    </div>

    <h2><i class="fas fa-shield-alt"></i> Datenschutzhinweise</h2>
    <div style="background: rgba(76, 175, 80, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 20px; border-left: 4px solid #4caf50;">
        <p><strong>Kurz und klar: Deine Daten bleiben bei dir!</strong></p>
        <p>Dieses Webinterface läuft ausschließlich lokal in deinem Heimnetzwerk. Es werden keine personenbezogenen Daten gespeichert, 
        verarbeitet oder an Dritte weitergegeben. Die Uhr kommuniziert nicht mit externen Servern (außer für die Zeitsynchronisation).</p>
    </div>

    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <h3><i class="fas fa-database"></i> Welche Daten werden wo gespeichert?</h3>
        <ul style="line-height: 1.8;">
            <li><strong>WLAN-Zugangsdaten & Einstellungen:</strong><br>
                Werden ausschließlich lokal auf dem ESP8266-Chip deiner Uhr gespeichert. Kein externer Zugriff, keine Cloud.</li>
            
            <li><strong>Zeitsynchronisation (NTP):</strong><br>
                Die Uhr synchronisiert die aktuelle Uhrzeit über das Internet von einem öffentlichen NTP-Server (z.B. pool.ntp.org). 
                Dabei wird technisch bedingt die IP-Adresse deiner Uhr übermittelt, aber es erfolgt keine Speicherung oder 
                Weitergabe deiner Daten. Dies ist vergleichbar mit jedem normalen Internetzugriff.</li>
            
            <li><strong>MQTT (optional):</strong><br>
                Wenn du MQTT aktivierst, kommuniziert die Uhr mit deinem lokalen MQTT-Broker (z.B. Mosquitto). 
                Auch hier bleiben alle Daten in deinem Netzwerk – es sei denn, du hast bewusst einen externen Cloud-Broker konfiguriert.</li>
        </ul>
    </div>

    <h2><i class="fas fa-handshake"></i> Produkthaftung und Gewährleistung</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <p>Diese Wortuhr ist ein handgefertigtes Einzelstück, das ich mit viel Liebe zum Detail für dich erstellt habe. 
        Da es sich um einen <strong>Privatverkauf nach § 13 BGB</strong> handelt, gelten folgende rechtliche Rahmenbedingungen:</p>
        
        <h3><i class="fas fa-gavel"></i> Gewährleistungsausschluss</h3>
        <ul style="line-height: 1.8;">
            <li>Als Privatverkäufer kann ich leider keine gesetzliche Gewährleistung oder Garantie anbieten.</li>
            <li>Die Beschreibung der Uhr und ihrer Funktionen erfolgt nach bestem Wissen und Gewissen.</li>
            <li>Trotzdem bin ich natürlich bei Fragen oder Problemen gerne für dich da und helfe, wo ich kann!</li>
        </ul>
        
        <h3><i class="fas fa-exclamation-triangle"></i> Nutzung und Sicherheit</h3>
        <ul style="line-height: 1.8;">
            <li><strong>Eigenverantwortung:</strong> Die Nutzung der Uhr erfolgt auf eigene Verantwortung. 
                Bitte gehe sorgsam mit der Elektronik um und beachte die Hinweise zur Stromversorgung.</li>
            
            <li><strong>Stromversorgung:</strong> Verwende unbedingt ein hochwertiges USB-Netzteil mit mindestens 2 Ampere (5V). 
                Minderwertige oder ungeeignete Netzteile können zu Fehlfunktionen führen.</li>
            
            <li><strong>Keine Modifikationen:</strong> Öffne die Uhr nicht und nimm keine eigenmächtigen Veränderungen an der 
                Hardware vor. Bei Schäden durch unsachgemäße Nutzung oder Modifikationen kann ich leider keine Haftung übernehmen.</li>
            
            <li><strong>Aufstellort:</strong> Vermeide direkte Sonneneinstrahlung, Feuchtigkeit und extreme Temperaturen.</li>
        </ul>
    </div>

    <h2><i class="fas fa-code-branch"></i> Open-Source & Verwendete Bibliotheken</h2>
    <div style="background: rgba(255, 255, 255, 0.05); padding: 15px; border-radius: 8px; margin-bottom: 20px;">
        <p>Diese Wortuhr steht auf den Schultern von Giganten! Folgende großartige Open-Source-Bibliotheken machen 
        das Projekt erst möglich. Ein herzliches Dankeschön an alle Entwickler:</p>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-microchip"></i> Hardware & Netzwerk</h3>
            <ul style="line-height: 1.6;">
                <li><strong>ESP8266WiFi:</strong> WLAN-Konnektivität für den ESP8266</li>
                <li><strong>ESP8266WebServer:</strong> Webserver für das Interface, das du gerade nutzt</li>
                <li><strong>ESP8266mDNS:</strong> Ermöglicht den Zugriff per "wortuhr.local" statt IP-Adresse</li>
                <li><strong>EEPROM:</strong> Dauerhafte Speicherung deiner Einstellungen</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-clock"></i> Zeit & Synchronisation</h3>
            <ul style="line-height: 1.6;">
                <li><strong>NTPClient:</strong> Zeitsynchronisation über NTP-Server</li>
                <li><strong>WiFiUdp:</strong> UDP-Kommunikation für NTP</li>
                <li><strong>Wire:</strong> I²C-Kommunikation mit dem optionalen RTC-Modul (DS1307)</li>
                <li><strong>time.h:</strong> Zeitfunktionen und -berechnungen</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-lightbulb"></i> LEDs & Steuerung</h3>
            <ul style="line-height: 1.6;">
                <li><strong>Adafruit_NeoPixel:</strong> Ansteuerung des WS2812s LED-Strips für brillante Farben</li>
                <li><strong>OneButton:</strong> Intelligente Verarbeitung von Tasteneingaben</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-network-wired"></i> Smarthome-Integration</h3>
            <ul style="line-height: 1.6;">
                <li><strong>PubSubClient:</strong> MQTT-Unterstützung für Home Assistant & Co.</li>
                <li><strong>ArduinoJson:</strong> JSON-Verarbeitung für strukturierte Datenübertragung</li>
            </ul>
        </div>
        
        <div style="margin-top: 15px;">
            <h3 style="font-size: 1.1em; margin-bottom: 10px;"><i class="fas fa-cogs"></i> Basis-Framework</h3>
            <ul style="line-height: 1.6;">
                <li><strong>Arduino:</strong> Grundlegende Funktionen und Framework für den ESP8266</li>
            </ul>
        </div>
        
        <p style="margin-top: 20px; padding: 15px; background: rgba(76, 175, 80, 0.2); border-radius: 5px; border-left: 3px solid #4caf50;">
            <i class="fas fa-heart"></i> <strong>Open-Source-Gedanke:</strong> Alle diese Bibliotheken werden von der 
            Community kostenlos bereitgestellt und weiterentwickelt. Wenn dir das Projekt gefällt, denk doch mal darüber nach, 
            die Entwickler mit einer kleinen Spende zu unterstützen!
        </p>
    </div>

    <h2><i class="fas fa-envelope"></i> Kontakt & Unterstützung</h2>
    <div style="background: rgba(33, 150, 243, 0.2); padding: 20px; border-radius: 10px; margin-bottom: 30px; border-left: 4px solid #2196f3;">
        <p style="line-height: 1.8;">
            Hast du Fragen, Anregungen oder benötigst Hilfe? Ich bin gerne für dich da!<br>
            Schreib mir einfach eine E-Mail an 
            <a href="mailto:bichlmaier.andreas0+wortuhr@gmail.de" style="color: #64b5f6; font-weight: bold;">
                bichlmaier.andreas0@gmail.de
            </a>
        </p>
        <p style="margin: 15px 0 0 0; color: rgba(255,255,255,0.8);">
            <i class="fas fa-clock"></i> <em>Ich antworte in der Regel innerhalb von 24-48 Stunden.</em>
        </p>
    </div>

    <div style="text-align: center; padding: 20px;">
        <a href="/info" style="display: inline-block; padding: 12px 30px; background: linear-gradient(135deg, #4caf50, #2e7d32); 
           color: white; border-radius: 5px; text-decoration: none; font-weight: bold;">
            <i class="fas fa-arrow-left"></i> Zurück zur Startseite
        </a>
    </div>

    <div style="text-align: center; padding: 20px; margin-top: 20px; border-top: 1px solid rgba(255,255,255,0.1);">
        <p style="color: rgba(255,255,255,0.5);">
            <i class="fas fa-code"></i> Mit <i class="fas fa-heart" style="color: #e74c3c;"></i> handgefertigt von Andy B
        </p>
    </div>
</main>
</body></html>
)rawliteral";
