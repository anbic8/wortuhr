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

    // Encrypt a copy for storage; user_connect itself must stay plaintext
    // in RAM so WiFi/MQTT keep working with the live credentials.
    settings encryptedForStorage = user_connect;
    SecureStorage::cryptFields(encryptedForStorage);

    EepromLayout::beginAll();
    EEPROM.put(EepromLayout::SETTINGS_OFFSET, encryptedForStorage);
    EEPROM.put(EepromLayout::DESIGN_OFFSET, user_design);
    EEPROM.write(EepromLayout::HA_FLAG_OFFSET, ha_flag ? 1 : 0);
    EepromLayout::writeLayoutVersion(EepromLayout::CURRENT_LAYOUT_VERSION);
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
    server.sendContent("<button type='button' onclick=\"fetch('/ha/discover',{method:'POST'})\">Erkennung jetzt senden</button>");

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

    EEPROM.put(EepromLayout::DESIGN_OFFSET, customDesign);
    EEPROM.commit();

    dvv = customDesign.dv;
    uvv = customDesign.uv;
    an = customDesign.an;
    aus = customDesign.aus;
    nacht = customDesign.nacht;

    buildLedMappings();

#if MATRIX_SIZE == 8
    minuteDotsEnabled = server.hasArg("minutedots");
    if (server.hasArg("minutedots_color")) {
      String hex = server.arg("minutedots_color");
      if (hex.charAt(0) == '#') hex = hex.substring(1);
      if (hex.length() == 6) {
        int r = (int)strtol(hex.substring(0, 2).c_str(), nullptr, 16);
        int g = (int)strtol(hex.substring(2, 4).c_str(), nullptr, 16);
        int b = (int)strtol(hex.substring(4, 6).c_str(), nullptr, 16);
        unsigned long bestDist = 0xFFFFFFFFUL;
        int bestIdx = minuteDotsColorIdx;
        for (int i = 0; i < anzahlfarben; i++) {
          int rgb[3];
          getPaletteColor((uint8_t)i, rgb);
          long dr = r - rgb[0], dg = g - rgb[1], db = b - rgb[2];
          unsigned long dist = (unsigned long)(dr * dr + dg * dg + db * db);
          if (dist < bestDist) { bestDist = dist; bestIdx = i; }
        }
        minuteDotsColorIdx = bestIdx;
      }
    }
    EEPROM.write(EepromLayout::MINUTE_DOTS_ENABLED_OFFSET, minuteDotsEnabled ? 1 : 0);
    EEPROM.write(EepromLayout::MINUTE_DOTS_COLOR_OFFSET, (uint8_t)minuteDotsColorIdx);
    EEPROM.commit();
#endif

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

#if MATRIX_SIZE == 8
    server.sendContent("<div class='card'>");
    server.sendContent("<label>Minutengenaue Anzeige (zeigt Minuten-Pixel in der untersten Zeile)</label>");
    server.sendContent(String("<input type='checkbox' name='minutedots' value='1' ") + (minuteDotsEnabled ? "checked" : "") + ">");

    int dotRgb[3];
    getPaletteColor((uint8_t)minuteDotsColorIdx, dotRgb);
    char dotBuf[8];
    sprintf(dotBuf, "#%02X%02X%02X", dotRgb[0], dotRgb[1], dotRgb[2]);
    server.sendContent("<label for='minutedots_color'>Farbe der Minuten-Pixel</label>");
    server.sendContent(String("<input type='color' id='minutedots_color' name='minutedots_color' value='") + dotBuf + "'>");
    server.sendContent("</div>");
#endif

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
    EEPROM.put(EepromLayout::DESIGN_OFFSET, user_design);

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

    EEPROM.put(EepromLayout::COLOR_OFFSET, customVar);

    bool newEffectsModeActive = server.hasArg("effectsmode");
    uint8_t newSelectedLightEffect = (uint8_t)constrain(server.arg("lighteffect").toInt(), 0, LIGHT_EFFECT_OPTIONS_COUNT - 1);
    if (effectsModeActive && !newEffectsModeActive) {
      threshold = 0; // sofortiger Uhr-Refresh statt bis zum nächsten Minutenwechsel zu warten
    }
    effectsModeActive = newEffectsModeActive;
    selectedLightEffect = newSelectedLightEffect;
    lightEffectSpeedIdx = (uint8_t)constrain(server.arg("lighteffectspeed").toInt(), 0, EFFECTTIME_OPTIONS_COUNT - 1);

    EEPROM.write(EepromLayout::LIGHT_EFFECTS_ENABLED_OFFSET, effectsModeActive ? 1 : 0);
    EEPROM.write(EepromLayout::LIGHT_EFFECT_INDEX_OFFSET, selectedLightEffect);
    EEPROM.write(EepromLayout::LIGHT_EFFECT_SPEED_OFFSET, lightEffectSpeedIdx);

    // "Zufällig aus Liste" Pool für den Übergangseffekt (echte Effekte 2..15)
    uint16_t newEffectPoolMask = 0;
    for (int i = 2; i <= 15; i++) {
      if (server.hasArg(String("txpool") + i)) {
        newEffectPoolMask |= (uint16_t)(1U << i);
      }
    }
    effectRandomPoolMask = newEffectPoolMask;
    EEPROM.put(EepromLayout::EFFECT_RANDOM_POOL_MASK_OFFSET, effectRandomPoolMask);

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
    for (int i = 0; i < FARBSCHEMA_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == vordergrundschema ? " selected" : "") + ">" + farbschemaOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='hs'>Hintergrundschema</label>");
    server.sendContent("<select name='hs' id='hs'>");
    for (int i = 0; i < FARBSCHEMA_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == hintergrundschema ? " selected" : "") + ">" + farbschemaOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");
    server.sendContent("</div>");

    server.sendContent("<div class='card'>");
    server.sendContent("<label>Vorschau</label>");
    server.sendContent("<small>Zeigt Vorder-/Hintergrundfarben und -schema als Beispielmuster - nicht die exakte Uhrzeit-Anzeige.</small>");
    server.sendContent("<div id='colorPreview' class='pixel-grid'></div>");
    server.sendContent(String("<script>(function(){\n"
      "var SIZE = ") + MATRIX_SIZE + ";\n" +
      "var PALETTE=[[255,255,255],[255,0,0],[255,0,128],[255,0,255],[128,0,255],[0,0,255],[0,128,255],[0,255,255],[0,255,128],[0,255,0],[128,255,0],[255,255,0],[255,128,0],[0,0,0]];\n"
      "var grid=document.getElementById('colorPreview');\n"
      "grid.style.gridTemplateColumns='repeat('+SIZE+',1fr)';\n"
      "var cells=[],mask=[];\n"
      "for(var r=0;r<SIZE;r++){mask[r]=[];cells[r]=[];for(var c=0;c<SIZE;c++){mask[r][c]=Math.random()<0.4;var d=document.createElement('div');d.className='px';grid.appendChild(d);cells[r][c]=d;}}\n"
      "function hexToRgb(hex){hex=hex.replace('#','');return [parseInt(hex.substring(0,2),16),parseInt(hex.substring(2,4),16),parseInt(hex.substring(4,6),16)];}\n"
      "function schemeColor(scheme,row,col,c1,c2){\n"
      "  switch(scheme){\n"
      "    case 1: return ((row%2==0)===(col%2==0))?c1:c2;\n"
      "    case 2: return (col%2==0)?c1:c2;\n"
      "    case 3: return (row%2==0)?c1:c2;\n"
      "    case 4:\n"
      "      var f=row/(SIZE-1);\n"
      "      return [Math.round(c1[0]+(c2[0]-c1[0])*f),Math.round(c1[1]+(c2[1]-c1[1])*f),Math.round(c1[2]+(c2[2]-c1[2])*f)];\n"
      "    case 5: return PALETTE[Math.floor(Math.random()*PALETTE.length)];\n"
      "    default: return c1;\n"
      "  }\n"
      "}\n"
      "window.renderColorPreview=function(){\n"
      "  var vf1=hexToRgb(document.getElementById('vf1_color').value);\n"
      "  var vf2=hexToRgb(document.getElementById('vf2_color').value);\n"
      "  var hf1=hexToRgb(document.getElementById('hf1_color').value);\n"
      "  var hf2=hexToRgb(document.getElementById('hf2_color').value);\n"
      "  var vs=parseInt(document.getElementById('vs').value,10);\n"
      "  var hs=parseInt(document.getElementById('hs').value,10);\n"
      "  for(var r=0;r<SIZE;r++){for(var c=0;c<SIZE;c++){\n"
      "    var col=mask[r][c]?schemeColor(vs,r,c,vf1,vf2):schemeColor(hs,r,c,hf1,hf2);\n"
      "    cells[r][c].style.background='rgb('+col[0]+','+col[1]+','+col[2]+')';\n"
      "  }}\n"
      "};\n"
      "['vf1_color','vf2_color','hf1_color','hf2_color'].forEach(function(id){document.getElementById(id).addEventListener('input',renderColorPreview);});\n"
      "['vs','hs'].forEach(function(id){document.getElementById(id).addEventListener('change',renderColorPreview);});\n"
      "renderColorPreview();\n"
      "})();</script>");
    server.sendContent("</div>");

    server.sendContent("<div class='card'>");
    server.sendContent("<label for='efx'>Uebergangseffekt</label>");
    server.sendContent("<select name='efx' id='efx'>");
    for (int i = 0; i < EFFECT_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == effectMode ? " selected" : "") + ">" + effectOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label>Zufaellige Liste konfigurieren (fuer Uebergangseffekt 'Zufaellig aus Liste')</label>");
    server.sendContent("<small>Waehle aus, welche Uebergangseffekte in die zufaellige Auswahl aufgenommen werden.</small>");
    for (int i = 2; i <= 15; i++) {
      bool checked = (effectRandomPoolMask & (1U << i)) != 0;
      server.sendContent("<label class='checkbox-row'>");
      server.sendContent(String("<input type='checkbox' name='txpool") + i + "' value='1' " + (checked ? "checked" : "") + ">");
      server.sendContent(String("<span>") + effectOptions[i] + "</span>");
      server.sendContent("</label>");
    }

    server.sendContent("<label for='efxtime'>Uebergangsgeschwindigkeit</label>");
    server.sendContent("<select name='efxtime' id='efxtime'>");
    for (int i = 0; i < EFFECTTIME_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == efxtimeint ? " selected" : "") + ">" + effecttimeOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='ani'>Animation</label>");
    server.sendContent("<select name='ani' id='ani'>");
    for (int i = 0; i < ANI_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == aniMode ? " selected" : "") + ">" + aniOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='anitime'>Animationszeit</label>");
    server.sendContent("<select name='anitime' id='anitime'>");
    for (int i = 0; i < EFFECTTIME_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == anitimeint ? " selected" : "") + ">" + effecttimeOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='anidepth'>Animationsstaerke</label>");
    server.sendContent("<select name='anidepth' id='anidepth'>");
    for (int i = 0; i < EFFECTDEPTH_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == anidepth ? " selected" : "") + ">" + effectdepthOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    int dimm_percent = (dimm * 100) / 255;
    server.sendContent("<label for='dimm'>Helligkeit (0-100)</label>");
    server.sendContent(String("<input type='number' id='dimm' name='dimm' min='0' max='100' value='") + dimm_percent + "'>");

    server.sendContent("</div>");

    server.sendContent("<div class='card'>");
    server.sendContent("<label>Effekte-Modus (Matrix zeigt Lichteffekte statt der Uhrzeit)</label>");
    server.sendContent(String("<input type='checkbox' name='effectsmode' value='1' onchange=\"fetch('/api/effectsmode?enabled='+(this.checked?1:0))\" ") + (effectsModeActive ? "checked" : "") + ">");

    server.sendContent("<label for='lighteffect'>Lichteffekt</label>");
    server.sendContent("<select name='lighteffect' id='lighteffect' onchange=\"fetch('/api/effectsmode?effect='+this.value)\">");
    for (int i = 0; i < LIGHT_EFFECT_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == selectedLightEffect ? " selected" : "") + ">" + lightEffectOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");

    server.sendContent("<label for='lighteffectspeed'>Lichteffekt-Geschwindigkeit</label>");
    server.sendContent("<select name='lighteffectspeed' id='lighteffectspeed' onchange=\"fetch('/api/effectsmode?speed='+this.value)\">");
    for (int i = 0; i < EFFECTTIME_OPTIONS_COUNT; i++) {
      String opt = String("<option value='") + i + "'" + (i == lightEffectSpeedIdx ? " selected" : "") + ">" + effecttimeOptions[i] + "</option>";
      server.sendContent(opt);
    }
    server.sendContent("</select>");
    server.sendContent("<small>Aenderungen hier wirken sofort, unabhaengig vom Speichern-Button unten.</small>");
    server.sendContent("</div>");

    server.sendContent("<button type='submit'>Speichern</button>");
    server.sendContent("</form>");
    sendPageEnd();
  }
}

// Sofort-Aktivierung fuer Effekte-Modus-Aenderungen (Checkbox/Lichteffekt/
// Geschwindigkeit), per fetch() aus der /color-Seite aufgerufen, damit ein
// ausgewaehlter Lichteffekt direkt anspringt statt erst nach "Speichern".
void handleEffectsModeApi() {
  if (server.hasArg("enabled")) {
    bool newActive = server.arg("enabled").toInt() == 1;
    if (effectsModeActive && !newActive) {
      threshold = 0; // sofortiger Uhr-Refresh statt bis zum naechsten Minutenwechsel
    }
    effectsModeActive = newActive;
    EEPROM.write(EepromLayout::LIGHT_EFFECTS_ENABLED_OFFSET, effectsModeActive ? 1 : 0);
  }
  if (server.hasArg("effect")) {
    selectedLightEffect = (uint8_t)constrain(server.arg("effect").toInt(), 0, LIGHT_EFFECT_OPTIONS_COUNT - 1);
    EEPROM.write(EepromLayout::LIGHT_EFFECT_INDEX_OFFSET, selectedLightEffect);
  }
  if (server.hasArg("speed")) {
    lightEffectSpeedIdx = (uint8_t)constrain(server.arg("speed").toInt(), 0, EFFECTTIME_OPTIONS_COUNT - 1);
    EEPROM.write(EepromLayout::LIGHT_EFFECT_SPEED_OFFSET, lightEffectSpeedIdx);
  }
  EEPROM.commit();
  server.send(200, "text/plain", "OK");
}

void handleHAConfig() {
  if (server.method() == HTTP_POST) {
    bool enabled = server.hasArg("ha_enable") && server.arg("ha_enable") == "1";
    haDiscoveryEnabled = enabled;

    EepromLayout::beginAll();
    EEPROM.write(EepromLayout::HA_FLAG_OFFSET, enabled ? 1 : 0);
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

void handleFactoryReset() {
  if (server.method() == HTTP_POST && server.hasArg("confirm") && server.arg("confirm") == "yes") {
    EepromLayout::eraseAll();
    sendPageStart("Werkseinstellungen");
    server.sendContent("<div class='card'><p>Alle Einstellungen wurden geloescht. Die Uhr startet jetzt neu.</p></div>");
    sendPageEnd();
    delay(1500);
    ESP.restart();
  } else {
    sendPageStart("Werkseinstellungen");
    server.sendContent("<div class='card'><p>Achtung: Dies loescht WLAN-, MQTT-, Farb- und Geburtstagseinstellungen unwiderruflich.</p></div>");
    server.sendContent("<form action='/factory-reset' method='post'>");
    server.sendContent("<input type='hidden' name='confirm' value='yes'>");
    server.sendContent("<button type='submit'>Werkseinstellungen wiederherstellen</button>");
    server.sendContent("</form>");
    sendPageEnd();
  }
}

// Persistiert ein neues OTA-Passwort (leer = Schutz wieder deaktivieren).
// Verschluesselte Kopie geht ins EEPROM, die Live-Kopie bleibt Klartext im
// RAM fuer die Basic-Auth-Vergleiche.
static void saveOtaPassword(const String &newPw) {
  if (newPw.length() > 0) {
    char plain[sizeof(otaPassword)];
    memset(plain, 0, sizeof(plain));
    strncpy(plain, newPw.c_str(), sizeof(plain) - 1);
    memcpy(otaPassword, plain, sizeof(otaPassword));
    otaPasswordSet = true;

    char encrypted[sizeof(otaPassword)];
    memcpy(encrypted, plain, sizeof(encrypted));
    SecureStorage::cryptBuffer(encrypted, sizeof(encrypted), 4);
    EEPROM.put(EepromLayout::OTA_PASSWORD_OFFSET, encrypted);
    EEPROM.write(EepromLayout::OTA_PASSWORD_SET_OFFSET, 1);
  } else {
    otaPasswordSet = false;
    memset(otaPassword, 0, sizeof(otaPassword));
    EEPROM.write(EepromLayout::OTA_PASSWORD_SET_OFFSET, 0);
  }
  EEPROM.commit();
}

void handleUpload() {
  if (server.method() == HTTP_POST) {
    // Ein bereits gesetztes Passwort darf nur geaendert/entfernt werden,
    // wenn man das aktuelle Passwort kennt. Das erstmalige Setzen (noch
    // kein Schutz aktiv) erfordert bewusst keine Auth ("offen bis
    // konfiguriert").
    if (otaPasswordSet && !server.authenticate(OTA_USERNAME, otaPassword)) {
      return server.requestAuthentication();
    }
    saveOtaPassword(server.arg("ota_password"));
    sendPageStart("Update");
    server.sendContent(String("<div class='card'><p>OTA-Passwortschutz ist jetzt ") + (otaPasswordSet ? "aktiv." : "deaktiviert.") + "</p></div>");
    server.sendContent("<a class='link' href='/update'>Zurueck</a>");
    sendPageEnd();
    return;
  }

  if (otaPasswordSet && !server.authenticate(OTA_USERNAME, otaPassword)) {
    return server.requestAuthentication();
  }

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

  server.sendContent("<div class='card'>");
  server.sendContent(String("<p>OTA-Passwortschutz: <strong>") + (otaPasswordSet ? "aktiv" : "nicht konfiguriert (offen)") + "</strong></p>");
  server.sendContent("<small>Solange kein Passwort gesetzt ist, kann jeder im WLAN eine neue Firmware hochladen.</small>");
  server.sendContent("<form method='POST' action='/update'>");
  server.sendContent(String("<label for='ota_password'>") + (otaPasswordSet ? "Neues OTA-Passwort (leer = Schutz entfernen)" : "OTA-Passwort setzen") + "</label>");
  server.sendContent("<input type='password' id='ota_password' name='ota_password' maxlength='19'>");
  server.sendContent("<button type='submit'>Speichern</button>");
  server.sendContent("</form>");
  server.sendContent("</div>");

  sendPageEnd();
}

void handleUpdate() {
  if (otaPasswordSet && !server.authenticate(OTA_USERNAME, otaPassword)) {
    return server.requestAuthentication();
  }
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
  // Wird einmal pro Chunk aufgerufen; die Autorisierung wird bei
  // UPLOAD_FILE_START geprueft und fuer die restlichen Chunks gemerkt, damit
  // ohne gueltige Auth nie Bytes an Update.write()/Update.begin() gehen -
  // die eigentliche 401-Antwort liefert handleUpdate() nach Abschluss.
  static bool authorized = false;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    authorized = !otaPasswordSet || server.authenticate(OTA_USERNAME, otaPassword);
    if (!authorized) {
      LOGLN("OTA-Upload abgelehnt: fehlende/ungueltige Authentifizierung");
      return;
    }
    Serial.setDebugOutput(true);
    LOGF("Update gestartet: %s\n", upload.filename.c_str());
    if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (!authorized) return;
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
      Update.printError(Serial);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (!authorized) return;
    if (Update.end(true)) {
      LOGF("Update abgeschlossen: %u bytes\n", upload.totalSize);
    } else {
      Update.printError(Serial);
    }
    Serial.setDebugOutput(false);
  } else {
    LOGF("Update abgebrochen\n");
  }
}
