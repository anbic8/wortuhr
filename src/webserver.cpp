#include "webserver.h"

static void sendPageStart(const char *title) {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(FPSTR(htmlhead));
  if (title && title[0] != '\0') {
    server.sendContent("<h1>");
    server.sendContent(title);
    server.sendContent("</h1>");
  }
}

static void sendPageEnd() {
  server.sendContent("</main></body></html>");
}

void handlePortal() {
  if (user_connect.ssid[0] != '\0') {
    handleInfo();
  } else {
    handleWifi();
  }
}

void handleInfo() {
  sendPageStart("Info");
  server.sendContent(FPSTR(htmlinfo));
  sendPageEnd();
}

void handleInfoText() {
  server.send(200, "text/plain", "OK - info endpoint reachable");
}

void handledatenschutz() {
  String htmlContent;
  htmlContent += FPSTR(htmlhead);
  htmlContent += FPSTR(htmlrecht);
  server.send(200, "text/html", htmlContent);
}

void handleWifi() {
  if (server.method() == HTTP_POST) {
    strncpy(user_connect.ssid, server.arg("ssid").c_str(), sizeof(user_connect.ssid));
    strncpy(user_connect.password, server.arg("password").c_str(), sizeof(user_connect.password));
    user_connect.ssid[sizeof(user_connect.ssid) - 1] = '\0';
    user_connect.password[sizeof(user_connect.password) - 1] = '\0';

    strncpy(user_connect.mqtt_server, server.arg("mqtt_server").c_str(), sizeof(user_connect.mqtt_server));
    user_connect.mqtt_server[sizeof(user_connect.mqtt_server) - 1] = '\0';

    user_connect.mqtt_port = server.arg("mqtt_port").toInt();

    strncpy(user_connect.mqtt_user, server.arg("mqtt_user").c_str(), sizeof(user_connect.mqtt_user));
    user_connect.mqtt_user[sizeof(user_connect.mqtt_user) - 1] = '\0';

    strncpy(user_connect.mqtt_password, server.arg("mqtt_password").c_str(), sizeof(user_connect.mqtt_password));
    user_connect.mqtt_password[sizeof(user_connect.mqtt_password) - 1] = '\0';

    strncpy(user_connect.mqtt_prefix, server.arg("mqtt_prefix").c_str(), sizeof(user_connect.mqtt_prefix));
    user_connect.mqtt_prefix[sizeof(user_connect.mqtt_prefix) - 1] = '\0';

    user_design.mqttenable = server.hasArg("mqttenable") ? true : false;
    bool ha_flag = server.hasArg("ha_enable") ? (server.arg("ha_enable") == "1") : false;
    haDiscoveryEnabled = ha_flag;

    int countdownOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
    int verOffset = countdownOffset + sizeof(unsigned long);
    int eepromTotalSize = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage) + sizeof(unsigned long) + VERSION_STR_MAX + 1;
    int haFlagOffset = verOffset + VERSION_STR_MAX;

    EEPROM.begin(eepromTotalSize);
    EEPROM.put(0, user_connect);
    EEPROM.put(sizeof(settings) + sizeof(MyColor), user_design);
    EEPROM.write(haFlagOffset, ha_flag ? 1 : 0);
    EEPROM.commit();
    EEPROM.end();

    buildMqttTopics();

    sendPageStart("Verbindung gespeichert");
    server.sendContent("<div class='card'><p>Deine Netzwerkeinstellungen wurden gespeichert. Die Uhr startet jetzt neu.</p></div>");
    sendPageEnd();
    delay(2000);
    ESP.restart();
  } else {
    sendPageStart("Verbindung");
    server.sendContent("<form action='/wifi' method='post'>");
    server.sendContent("<div class='card'>");

    server.sendContent("<label>SSID</label>");
    server.sendContent(String("<input type='text' name='ssid' value='") + user_connect.ssid + "'>");

    server.sendContent("<label>Passwort</label>");
    server.sendContent(String("<input type='password' name='password' value='") + user_connect.password + "'>");

    server.sendContent("<label>MQTT aktivieren</label>");
    server.sendContent(String("<input type='checkbox' name='mqttenable' value='1' ") + (mqttenable ? "checked" : "") + ">");

    server.sendContent("<label>Home Assistant Erkennung</label>");
    server.sendContent(String("<input type='checkbox' name='ha_enable' value='1' ") + (haDiscoveryEnabled ? "checked" : "") + ">");

    server.sendContent("<label>MQTT Server</label>");
    server.sendContent(String("<input type='text' name='mqtt_server' value='") + user_connect.mqtt_server + "'>");

    server.sendContent("<label>MQTT Port</label>");
    server.sendContent(String("<input type='text' name='mqtt_port' value='") + user_connect.mqtt_port + "'>");

    server.sendContent("<label>MQTT Benutzer</label>");
    server.sendContent(String("<input type='text' name='mqtt_user' value='") + user_connect.mqtt_user + "'>");

    server.sendContent("<label>MQTT Passwort</label>");
    server.sendContent(String("<input type='password' name='mqtt_password' value='") + user_connect.mqtt_password + "'>");

    server.sendContent("<label>MQTT Praefix</label>");
    server.sendContent(String("<input type='text' name='mqtt_prefix' value='") + String(user_connect.mqtt_prefix) + "' placeholder='wortuhr'>");

    server.sendContent("</div>");
    server.sendContent("<button type='submit'>Speichern</button>");
    server.sendContent("</form>");
    sendPageEnd();
  }
}

void handledesignPath() {
  if (server.method() == HTTP_POST) {
    int anp = server.arg("anh").toInt() * 60 + server.arg("anm").toInt();
    int ausp = server.arg("aush").toInt() * 60 + server.arg("ausm").toInt();

    int dvvNew = dvv;
    int uvvNew = uvv;
#if VERSION_TYPE == 0
    dvvNew = server.arg("dv").toInt();
    uvvNew = server.arg("uv").toInt();
#endif

    design customDesign = {
      VERSION_TYPE,
      dvvNew,
      uvvNew,
      anp,
      ausp,
      server.arg("nacht").toInt(),
      sommerzeit,
      dimm,
      mqttenable
    };

    EEPROM.put(sizeof(settings) + sizeof(MyColor), customDesign);
    EEPROM.commit();

    dvv = customDesign.dv;
    uvv = customDesign.uv;
    an = customDesign.an;
    aus = customDesign.aus;
    nacht = customDesign.nacht;

    buildLedMappings();

    sendPageStart("Einstellungen");
    server.sendContent("<div class='card'><p>Deine Einstellungen wurden gespeichert und uebernommen.</p></div>");
    sendPageEnd();
  } else {
    int anh = an / 60;
    int anm = an % 60;
    int aush = aus / 60;
    int ausm = aus % 60;

    sendPageStart("Einstellungen");
    server.sendContent("<form action='/setting' method='post'>");
    server.sendContent("<div class='card'>");

#if VERSION_TYPE == 0
    server.sendContent("<label for='dv'>Anzeige xx:45</label>");
    server.sendContent("<select name='dv' id='dv'>");
    server.sendContent(String("<option value='0'") + (dvv == 0 ? " selected" : "") + ">dreiviertel</option>");
    server.sendContent(String("<option value='1'") + (dvv == 1 ? " selected" : "") + ">viertel vor</option>");
    server.sendContent("</select>");

    server.sendContent("<label for='uv'>Wort Uhr</label>");
    server.sendContent("<select name='uv' id='uv'>");
    server.sendContent(String("<option value='0'") + (uvv == 0 ? " selected" : "") + ">nie</option>");
    server.sendContent(String("<option value='1'") + (uvv == 1 ? " selected" : "") + ">immer</option>");
    server.sendContent(String("<option value='2'") + (uvv == 2 ? " selected" : "") + ">zur vollen Stunde</option>");
    server.sendContent("</select>");
#endif

    server.sendContent("<label for='nacht'>Nachtmodus</label>");
    server.sendContent("<select name='nacht' id='nacht'>");
    server.sendContent(String("<option value='0'") + (nacht == 0 ? " selected" : "") + ">ausgeschaltet</option>");
    server.sendContent(String("<option value='1'") + (nacht == 1 ? " selected" : "") + ">gedimmt</option>");
    server.sendContent("</select>");

    server.sendContent("<label>Nachtmodus einschalten (HH:MM)</label>");
    server.sendContent("<div class='form-row'>");
    server.sendContent(String("<div class='field'><input type='number' name='aush' min='0' max='23' value='") + aush + "'></div>");
    server.sendContent(String("<div class='field'><input type='number' name='ausm' min='0' max='59' value='") + ausm + "'></div>");
    server.sendContent("</div>");

    server.sendContent("<label>Nachtmodus ausschalten (HH:MM)</label>");
    server.sendContent("<div class='form-row'>");
    server.sendContent(String("<div class='field'><input type='number' name='anh' min='0' max='23' value='") + anh + "'></div>");
    server.sendContent(String("<div class='field'><input type='number' name='anm' min='0' max='59' value='") + anm + "'></div>");
    server.sendContent("</div>");

    server.sendContent("</div>");
    server.sendContent("<button type='submit'>Speichern</button>");
    server.sendContent("</form>");
    sendPageEnd();
  }
}

void handlecolorPath() {
  if (server.method() == HTTP_POST) {
    auto hexToNearestIndex = [&](const String &hex, int fallback) -> int {
      if (hex.length() == 0) return fallback;
      String s = hex;
      if (s.charAt(0) == '#') s = s.substring(1);
      if (s.length() != 6) return fallback;
      int r = (int)strtol(s.substring(0, 2).c_str(), nullptr, 16);
      int g = (int)strtol(s.substring(2, 4).c_str(), nullptr, 16);
      int b = (int)strtol(s.substring(4, 6).c_str(), nullptr, 16);
      unsigned long bestDist = 0xFFFFFFFFUL;
      int bestIdx = fallback;
      for (int i = 0; i < anzahlfarben; i++) {
        int rgb[3];
        getPaletteColor((uint8_t)i, rgb);
        long dr = r - rgb[0];
        long dg = g - rgb[1];
        long db = b - rgb[2];
        unsigned long dist = (unsigned long)(dr * dr + dg * dg + db * db);
        if (dist < bestDist) {
          bestDist = dist;
          bestIdx = i;
        }
      }
      return bestIdx;
    };

    int vf1_idx = v1;
    if (server.hasArg("vf1_color")) {
      vf1_idx = hexToNearestIndex(server.arg("vf1_color"), v1);
    } else if (server.hasArg("vf1")) {
      vf1_idx = server.arg("vf1").toInt();
    }

    int vf2_idx = v2;
    if (server.hasArg("vf2_color")) {
      vf2_idx = hexToNearestIndex(server.arg("vf2_color"), v2);
    } else if (server.hasArg("vf2")) {
      vf2_idx = server.arg("vf2").toInt();
    }

    int hf1_idx = h1;
    if (server.hasArg("hf1_color")) {
      hf1_idx = hexToNearestIndex(server.arg("hf1_color"), h1);
    } else if (server.hasArg("hf1")) {
      hf1_idx = server.arg("hf1").toInt();
    }

    int hf2_idx = h2;
    if (server.hasArg("hf2_color")) {
      hf2_idx = hexToNearestIndex(server.arg("hf2_color"), h2);
    } else if (server.hasArg("hf2")) {
      hf2_idx = server.arg("hf2").toInt();
    }

    int dimm_percent = server.arg("dimm").toInt();
    if (dimm_percent < 0) dimm_percent = 0;
    if (dimm_percent > 100) dimm_percent = 100;
    int new_dimm = (dimm_percent * 255) / 100;
    dimm = new_dimm;

    user_design.dimm = new_dimm;
    EEPROM.put(sizeof(settings) + sizeof(MyColor), user_design);

    MyColor customVar = {
      vf1_idx,
      vf2_idx,
      server.arg("vs").toInt(),
      hf1_idx,
      hf2_idx,
      server.arg("hs").toInt(),
      server.arg("efx").toInt(),
      server.arg("efxtime").toInt(),
      server.arg("ani").toInt(),
      server.arg("anitime").toInt(),
      server.arg("anidepth").toInt()
    };

    EEPROM.put(sizeof(settings), customVar);
    EEPROM.commit();
    readTime();
    neuefarbe();

    server.sendHeader("Location", "/color");
    server.send(303);
  } else {
    sendPageStart("Farben");
    server.sendContent("<form action='/color' method='post'>");
    server.sendContent("<div class='card'>");

    char buf[8];
    int tmpc[3];

    getPaletteColor((uint8_t)v1, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String vf1_hex = String(buf);

    getPaletteColor((uint8_t)v2, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String vf2_hex = String(buf);

    getPaletteColor((uint8_t)h1, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String hf1_hex = String(buf);

    getPaletteColor((uint8_t)h2, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String hf2_hex = String(buf);

    server.sendContent("<label for='vf1_color'>Vordergrundfarbe 1</label>");
    server.sendContent(String("<input type='color' id='vf1_color' name='vf1_color' value='") + vf1_hex + "'>");

    server.sendContent("<label for='vf2_color'>Vordergrundfarbe 2</label>");
    server.sendContent(String("<input type='color' id='vf2_color' name='vf2_color' value='") + vf2_hex + "'>");

    server.sendContent("<label for='hf1_color'>Hintergrundfarbe 1</label>");
    server.sendContent(String("<input type='color' id='hf1_color' name='hf1_color' value='") + hf1_hex + "'>");

    server.sendContent("<label for='hf2_color'>Hintergrundfarbe 2</label>");
    server.sendContent(String("<input type='color' id='hf2_color' name='hf2_color' value='") + hf2_hex + "'>");

    server.sendContent("<label for='vs'>Vordergrundschema</label>");
    server.sendContent("<select name='vs' id='vs'>");
    for (int i = 0; i < 6; i++) {
      String opt = String("<option value='") + i + "'" + (i == vordergrundschema ? " selected" : "") + ">" + htmlfarbschema[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='hs'>Hintergrundschema</label>");
    server.sendContent("<select name='hs' id='hs'>");
    for (int i = 0; i < 6; i++) {
      String opt = String("<option value='") + i + "'" + (i == hintergrundschema ? " selected" : "") + ">" + htmlfarbschema[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='efx'>Uebergangseffekt</label>");
    server.sendContent("<select name='efx' id='efx'>");
    for (int i = 0; i < 14; i++) {
      String opt = String("<option value='") + i + "'" + (i == effectMode ? " selected" : "") + ">" + htmlefx[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='efxtime'>Uebergangsgeschwindigkeit</label>");
    server.sendContent("<select name='efxtime' id='efxtime'>");
    for (int i = 0; i < 3; i++) {
      String opt = String("<option value='") + i + "'" + (i == efxtimeint ? " selected" : "") + ">" + htmlefxtime[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='ani'>Animation</label>");
    server.sendContent("<select name='ani' id='ani'>");
    for (int i = 0; i < 7; i++) {
      String opt = String("<option value='") + i + "'" + (i == aniMode ? " selected" : "") + ">" + htmlani[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='anitime'>Animationszeit</label>");
    server.sendContent("<select name='anitime' id='anitime'>");
    for (int i = 0; i < 3; i++) {
      String opt = String("<option value='") + i + "'" + (i == anitimeint ? " selected" : "") + ">" + htmlanitime[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='anidepth'>Animationsstaerke</label>");
    server.sendContent("<select name='anidepth' id='anidepth'>");
    for (int i = 0; i < 3; i++) {
      String opt = String("<option value='") + i + "'" + (i == anidepth ? " selected" : "") + ">" + htmlanidepth[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    int dimm_percent = (dimm * 100) / 255;
    server.sendContent("<label for='dimm'>Helligkeit (0-100)</label>");
    server.sendContent(String("<input type='number' id='dimm' name='dimm' min='0' max='100' value='") + dimm_percent + "'>");

    server.sendContent("</div>");
    server.sendContent("<button type='submit'>Speichern</button>");
    server.sendContent("</form>");
    sendPageEnd();
  }
}

void handleHAConfig() {
  if (server.method() == HTTP_POST) {
    bool enabled = server.hasArg("ha_enable") && server.arg("ha_enable") == "1";
    haDiscoveryEnabled = enabled;

    int countdownOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
    int verOffset = countdownOffset + sizeof(unsigned long);
    int eepromTotalSize = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage) + sizeof(unsigned long) + VERSION_STR_MAX + 1;
    int haFlagOffset = verOffset + VERSION_STR_MAX;

    EEPROM.begin(eepromTotalSize);
    EEPROM.write(haFlagOffset, enabled ? 1 : 0);
    EEPROM.commit();
    EEPROM.end();

    sendPageStart("Home Assistant");
    server.sendContent("<div class='card'><p>Einstellung gespeichert. Die Uhr wird neu gestartet.</p></div>");
    sendPageEnd();
    delay(1200);
    ESP.restart();
  } else {
    sendPageStart("Home Assistant");
    server.sendContent("<form action='/ha' method='post'>");
    server.sendContent("<div class='card'>");
    server.sendContent("<label>Home Assistant Erkennung aktivieren</label>");
    server.sendContent(String("<input type='checkbox' name='ha_enable' value='1' ") + (haDiscoveryEnabled ? "checked" : "") + ">");
    server.sendContent("</div>");
    server.sendContent("<button type='submit'>Speichern</button>");
    server.sendContent("</form>");
    server.sendContent("<div class='card'>");
    server.sendContent("<p>Erkennung manuell senden</p>");
    server.sendContent("<form action='/ha/discover' method='post'>");
    server.sendContent("<button type='submit'>Erkennung jetzt senden</button>");
    server.sendContent("</form>");
    server.sendContent("</div>");
    sendPageEnd();
  }
}

void handleHADiscover() {
  sendPageStart("Home Assistant");
  if (haDiscoveryEnabled && mqttenable) {
    discoveryNeeded = true;
    server.sendContent("<div class='card'><p>Erkennung wird beim naechsten MQTT Verbindungsaufbau gesendet.</p></div>");
    if (client.connected()) {
      client.disconnect();
    }
  } else {
    server.sendContent("<div class='card'><p>Erkennung kann nicht gesendet werden.</p></div>");
  }
  sendPageEnd();
}

void handleUpload() {
  sendPageStart("Update");
  server.sendContent("<div class='card'>");
  server.sendContent(String("<p>Aktuelle Version: <strong>") + String(FW_VERSION) + "</strong></p>");
  server.sendContent("<p>Die Firmware kann per .bin Datei aktualisiert werden.</p>");
  server.sendContent("</div>");

  server.sendContent("<div class='card'>");
  server.sendContent("<p>Download der neuesten Firmware:</p>");
  server.sendContent("<a class='link' href='https://github.com/anbic8/wortuhr/releases' target='_blank'>GitHub Releases</a>");
  server.sendContent("</div>");

  server.sendContent("<div class='card'>");
  server.sendContent("<form method='POST' action='/upload' enctype='multipart/form-data'>");
  server.sendContent("<label>Firmware Datei (.bin)</label>");
  server.sendContent("<input type='file' name='firmware' accept='.bin'>");
  server.sendContent("<button type='submit'>Firmware hochladen</button>");
  server.sendContent("</form>");
  server.sendContent("<small>Waehren des Updates nicht vom Strom trennen.</small>");
  server.sendContent("</div>");

  sendPageEnd();
}

void handleUpdate() {
  sendPageStart("Update");
  if (Update.hasError()) {
    server.sendContent("<div class='card'><p>Update fehlgeschlagen.</p></div>");
    server.sendContent("<a class='link' href='/'>Zurueck</a>");
    sendPageEnd();
  } else {
    server.sendContent("<div class='card'><p>Update erfolgreich.</p></div>");
    server.sendContent("<a class='link' href='/'>Zurueck</a>");
    sendPageEnd();
    delay(2000);
    ESP.restart();
  }
}

void handleUploading() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.setDebugOutput(true);
    Serial.printf("Update gestartet: %s\n", upload.filename.c_str());
    if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (Update.end(true)) {
      Serial.printf("Update abgeschlossen: %u bytes\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  } else {
    Serial.printf("Update abgebrochen\n");
  }
}
