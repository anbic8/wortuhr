#include "webserver.h"


void handlePortal() {
  if(user_connect.ssid){
    handleInfo();
  }else{
  handleWifi();
}
}

void handleInfo(){
   String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += FPSTR(htmlinfo);
  server.send(200,   "text/html", htmlContent);
}

void handledatenschutz(){
  String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += FPSTR(htmlrecht);
  server.send(200,   "text/html", htmlContent);
}

void handleWifi() {

  if (server.method() == HTTP_POST) {

    // WLAN-Zugangsdaten übernehmen
  strncpy(user_connect.ssid,     server.arg("ssid").c_str(),     sizeof(user_connect.ssid));
  strncpy(user_connect.password, server.arg("password").c_str(), sizeof(user_connect.password));
  user_connect.ssid[sizeof(user_connect.ssid) - 1] = '\0';
  user_connect.password[sizeof(user_connect.password) - 1] = '\0';

  // MQTT-Daten übernehmen
  strncpy(user_connect.mqtt_server,   server.arg("mqtt_server").c_str(),   sizeof(user_connect.mqtt_server));
  user_connect.mqtt_server[sizeof(user_connect.mqtt_server) - 1] = '\0';

  user_connect.mqtt_port = server.arg("mqtt_port").toInt();

  strncpy(user_connect.mqtt_user,     server.arg("mqtt_user").c_str(),     sizeof(user_connect.mqtt_user));
  user_connect.mqtt_user[sizeof(user_connect.mqtt_user) - 1] = '\0';

  strncpy(user_connect.mqtt_password, server.arg("mqtt_password").c_str(), sizeof(user_connect.mqtt_password));
  user_connect.mqtt_password[sizeof(user_connect.mqtt_password) - 1] = '\0';
  
  strncpy(user_connect.mqtt_prefix, server.arg("mqtt_prefix").c_str(), sizeof(user_connect.mqtt_prefix));
  user_connect.mqtt_prefix[sizeof(user_connect.mqtt_prefix) - 1] = '\0';

  // Checkbox prüfen (nur vorhanden, wenn angehakt)
  user_design.mqttenable = server.hasArg("mqttenable") ? true : false;
  
  // Home Assistant discovery flag (optional checkbox)
  bool ha_flag = server.hasArg("ha_enable") ? (server.arg("ha_enable")=="1") : false;
  haDiscoveryEnabled = ha_flag;

  // EEPROM-Operationen: begin() einmal, alle Daten schreiben, dann commit()
  int verOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
  int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + VERSION_STR_MAX + 1;
  int haFlagOffset = verOffset + VERSION_STR_MAX;
  
  EEPROM.begin(eepromTotalSize);
  
  // Daten ins EEPROM schreiben
  EEPROM.put(0, user_connect);
  EEPROM.put(sizeof(settings)+sizeof(MyColor), user_design);
  // persist HA flag (single byte after version slot)
  EEPROM.write(haFlagOffset, ha_flag ? 1 : 0);
  
  // Alles auf einmal committen
  EEPROM.commit();
  EEPROM.end();
  
  // Rebuild MQTT topics with new prefix
  buildMqttTopics();


  String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);

    
  htmlContent = htmlContent +"<h1>WLAN Einstellungen</h1> <br/> <p>Deine Netzwerkeinstellungen wurden gespeichert!<br />Die Uhr wird jetzt neu gestartet. </p></main></body></html>";

     server.send(200,   "text/html", htmlContent );
    delay(2000);
    ESP.restart();
  } else {

    String htmlContent;
    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
  htmlContent =htmlContent + "<main class='form-signin'> <form action='/wifi' method='post'> <h1 class=''>WLAN Einstellungen</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid' value='"+user_connect.ssid+"'> <br/><label>Passwort</label><input type='password' class='form-control' name='password' value='"+user_connect.password+"'><br/>MQTT aktivieren <input type='checkbox' name='mqttenable' value='1' "+(mqttenable?"checked":"")+" ><br/>Home Assistant Erkennung <input type='checkbox' name='ha_enable' value='1' "+(haDiscoveryEnabled?"checked":"")+" ><br><label>MQTT Server</label><input type='text' class='form-control' name='mqtt_server' value='"+user_connect.mqtt_server+"'><br><label>MQTT Port</label><input type='text' class='form-control' name='mqtt_port' value='"+user_connect.mqtt_port+"'><br><label>MQTT Benutzer</label><input type='text' class='form-control' name='mqtt_user' value='"+user_connect.mqtt_user+"'><br/><label>MQTT Passwort</label><input type='password' class='form-control' name='mqtt_password' value='"+user_connect.mqtt_password+"'><br/><label>MQTT Präfix (z.B. 'wortuhr' oder 'wohnzimmer')</label><input type='text' class='form-control' name='mqtt_prefix' value='"+String(user_connect.mqtt_prefix)+"' placeholder='wortuhr'></div><br/><button type='submit'>Speichern</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>";

    server.send(200,   "text/html", htmlContent );
  }
}

void handledesignPath() {

  if (server.method() == HTTP_POST) {

    int anp = server.arg("anh").toInt()*60+server.arg("anm").toInt();
    int ausp = server.arg("aush").toInt()*60+server.arg("ausm").toInt();

      design customDesign = {
    VERSION_TYPE,  // Fixed at compile time
    server.arg("dv").toInt(),
    server.arg("uv").toInt(),
    anp,
    ausp,
    server.arg("nacht").toInt(),
    sommerzeit,
    dimm,
    mqttenable
  };

  EEPROM.put(sizeof(settings)+sizeof(MyColor), customDesign);
  EEPROM.commit();
  
  // Einstellungen direkt übernehmen ohne Neustart
  dvv = customDesign.dv;
  uvv = customDesign.uv;
  an = customDesign.an;
  aus = customDesign.aus;
  nacht = customDesign.nacht;
  
  // LED-Mappings neu aufbauen falls dvv geändert wurde
  buildLedMappings();
  
  String htmlContent;
  // Aus Flash lesen und in String schreiben
  htmlContent += FPSTR(htmlhead);
  htmlContent = htmlContent + "<main class='form-signin'> <h1>Einstellungen</h1> <br/> <p>Deine Einstellungen wurden gespeichert und übernommen!</p></main></body></html>";
  server.send(200, "text/html", htmlContent);
   
  } else {

    String body ="<main class='form-signin'><form action='/setting' method='post'>";
    body += "<h1 class=''>Einstellungen</h1>";
#if VERSION_TYPE == 0
    body += "<p><strong>Version:</strong> Deutsch</p>";
#elif VERSION_TYPE == 1
    body += "<p><strong>Version:</strong> Bayrisch</p>";
#elif VERSION_TYPE == 2
    body += "<p><strong>Version:</strong> Mini (8x8)</p>";
#endif
    String selected1="";
    String selected2="";
    String selected3="";
#if VERSION_TYPE == 0
    body += "<br/><div class='form-floating'><label for='dv'>Anzeige xx:45 </label>";
  if(dvv==0){
      selected1="selected";
      selected2=" ";
    }else{
      selected2="selected";
      selected1=" ";
    };
    body += "<select name='dv' id='dv'><option value='0' "+selected1+" >dreiviertel</option><option value='1' "+selected2+" >viertel vor</option></select></div>";
#endif
#if VERSION_TYPE == 0
    body += "<div class='form-floating'><label for='uv'>Anzeige des Wortes &bdquo; Uhr &rdquo;</label>";
    if(uvv==0){
      selected1="selected";
      selected2=" ";
    };
    if(uvv==1)    {
      selected2="selected";
      selected1=" ";
    };
    if(uvv==2)    {
      selected2=" ";
      selected1=" ";
      selected3="selected";
    };
    body += "<select name='uv' id='uv'><option value='0' "+selected1+">nie</option><option value='1' "+selected2+">immer</option><option value='2' "+selected3+">zur vollen Stunde</option></select> </div><br>";
#endif
    
    body += "<div class='form-floating'><label for='nacht'>Im Nachtmodus werden die LEDs</label>";
    
    if(nacht==0){
      selected1="selected";
      selected2=" ";
    };
    if(nacht==1)    {
      selected2="selected";
      selected1=" ";
    };

    body += "<select name='nacht' id='nacht'><option value='0' "+selected1+">ausgeschaltet</option><option value='1' "+selected2+">gedimmt</option></select> </div>";
    int anh=an/60;
    int anm=an%60;
    int aush=aus/60;
    int ausm=aus%60;

    body += "<div class='form-floating'><br/><label for='auh'>Nachtmodus einschalten:</label>";
    body += "<input type='number' id='aush' name='aush' min='0' max='23' step='1' value='"+String(aush)+"'> :<input type='number' id='auzm' name='ausm' min='0' max='59' step='1' value='"+String(ausm)+"'> Uhr <br/> ";
    body += "<div class='form-floating'><label for='anh'>Nachtmodus ausschalten:</label>";
    body += "<input type='number' id='anh' name='anh' min='0' max='23' step='1' value='"+String(anh)+"'> :<input type='number' id='anm' name='anm' min='0' max='59' step='1' value='"+String(anm)+"'>Uhr<br/> ";
    
    body += "<br/><button type='submit'>Speichern</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>";
    String htmlContent;
    htmlContent.reserve(2048); // Pre-allocate to reduce fragmentation

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    
    server.send(200,   "text/html", htmlContent );
  }
}
void handlecolorPath() {

  String body;
  body.reserve(8192); // Pre-allocate large buffer for HTML
  body = "<main class='form-signin'><form action='/color' method='post'> <h1 class=''>Farbeinstellungen</h1>";
  
  // Kompakte Vorschau mit minimalem JavaScript
  body += "<div style='margin:20px 0;text-align:center;background:rgba(0,0,0,0.1);padding:20px;border-radius:10px'>";
  body += "<h3>Vorschau</h3>";
  body += "<div id='p' style='display:inline-grid;grid-template-columns:repeat(" + String(MATRIX_SIZE) + ",20px);gap:2px;background:#000;padding:5px;border-radius:5px'></div></div>";
  
  body += "<script>let v1={r:" + String(vf1[0]) + ",g:" + String(vf1[1]) + ",b:" + String(vf1[2]) + "},";
  body += "v2={r:" + String(vf2[0]) + ",g:" + String(vf2[1]) + ",b:" + String(vf2[2]) + "},";
  body += "h1={r:" + String(hf1[0]) + ",g:" + String(hf1[1]) + ",b:" + String(hf1[2]) + "},";
  body += "h2={r:" + String(hf2[0]) + ",g:" + String(hf2[1]) + ",b:" + String(hf2[2]) + "},";
  body += "vs=" + String(vordergrundschema) + ",hs=" + String(hintergrundschema) + ",br=" + String(dimm) + ",ms=" + String(MATRIX_SIZE) + ";";
  
  #if VERSION_TYPE == 0 && MATRIX_SIZE == 11
  body += "let w=[[1,1,0,1,1,1,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[1,1,1,1,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,1,1,1,0,0,0,0]];";
  #elif VERSION_TYPE == 1 && MATRIX_SIZE == 11
  body += "let w=[[1,1,0,1,1,1,1,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[1,1,1,1,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,1,1,1,0,0,0,0]];";
  #elif VERSION_TYPE == 2 && MATRIX_SIZE == 8
  body += "let w=[[1,1,1,1,1,1,1,1],[0,0,0,0,0,0,0,0],[1,1,1,1,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0],[0,0,0,0,1,1,1,1]];";
  #else
  body += "let w=Array(" + String(MATRIX_SIZE) + ").fill(0).map(()=>Array(" + String(MATRIX_SIZE) + ").fill(0));";
  #endif
  
  body += "function gc(c1,c2,s,r,c){if(s==0)return c1;if(s==1)return(r+c)%2?c1:c2;if(s==2)return c%2?c1:c2;if(s==3)return r%2?c1:c2;";
  body += "if(s==4){let t=r/(ms-1);return{r:Math.round(c1.r*(1-t)+c2.r*t),g:Math.round(c1.g*(1-t)+c2.g*t),b:Math.round(c1.b*(1-t)+c2.b*t)};}";
  body += "let x=[{r:255,g:255,b:255},{r:255,g:0,b:0},{r:255,g:0,b:128},{r:255,g:0,b:255},{r:128,g:0,b:255},{r:0,g:0,b:255},{r:0,g:128,b:255},{r:0,g:255,b:255},{r:0,g:255,b:128},{r:0,g:255,b:0},{r:128,g:255,b:0},{r:255,g:255,b:0},{r:255,g:128,b:0}];return x[(r*ms+c)%13];}";
  body += "function u(){let p=document.getElementById('p'),cs=p.children;if(cs.length==0){for(let i=0;i<ms*ms;i++){let d=document.createElement('div');d.style='width:20px;height:20px;border-radius:2px';p.appendChild(d);}}";
  body += "for(let r=0;r<ms;r++)for(let c=0;c<ms;c++){let i=r*ms+c,a=w[r][c],cl=gc(a?v1:h1,a?v2:h2,a?vs:hs,r,c);";
  body += "cs[i].style.background='rgb('+Math.round(cl.r*br/255)+','+Math.round(cl.g*br/255)+','+Math.round(cl.b*br/255)+')';}}";
  body += "document.addEventListener('DOMContentLoaded',()=>{u();";
  body += "['vf1_color','vf2_color','hf1_color','hf2_color'].forEach(id=>{let e=document.getElementById(id);if(e)e.oninput=()=>{let h=e.value,r=parseInt(h.substring(1,3),16),g=parseInt(h.substring(3,5),16),b=parseInt(h.substring(5,7),16);";
  body += "if(id=='vf1_color'){v1.r=r;v1.g=g;v1.b=b;}else if(id=='vf2_color'){v2.r=r;v2.g=g;v2.b=b;}else if(id=='hf1_color'){h1.r=r;h1.g=g;h1.b=b;}else{h2.r=r;h2.g=g;h2.b=b;}u();};});";
  body += "let ve=document.getElementById('vs');if(ve)ve.onchange=()=>{vs=parseInt(ve.value);u();};";
  body += "let he=document.getElementById('hs');if(he)he.onchange=()=>{hs=parseInt(he.value);u();};";
  body += "let de=document.getElementById('dimm');if(de)de.oninput=()=>{br=Math.round(parseInt(de.value)*255/100);u();};});</script>";
  
  body += "<br/>";
    // Color pickers: show current palette color as default
    char buf[8];
    int tmpc[3];
    getPaletteColor((uint8_t)v1, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String vf1_hex = String(buf);
    getPaletteColor((uint8_t)v2, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String vf2_hex = String(buf);

    String vf1form = "<div class='form-floating'><label for='vf1_color'>Vordergrundfarbe 1</label><input type='color' id='vf1_color' name='vf1_color' value='"+vf1_hex+"'></div>";
    String vf2form = "<div class='form-floating'><label for='vf2_color'>Vordergrundfarbe 2</label><input type='color' id='vf2_color' name='vf2_color' value='"+vf2_hex+"'></div>";

    String vsform = "<div class='form-floating'><label for='vs'>Vordergrundschema</label><select name='vs' id='vs'>";

  String myauswahl3 = " ";

    for(int i=0;i<6;i++){
      if(i==vordergrundschema){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      vsform=vsform+"<option value="+i+" "+myauswahl3+">"+htmlfarbschema[i]+"</option>";
    }

    vsform = vsform + "</select> </div><br>";

    getPaletteColor((uint8_t)h1, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String hf1_hex = String(buf);
    getPaletteColor((uint8_t)h2, tmpc);
    sprintf(buf, "#%02X%02X%02X", tmpc[0], tmpc[1], tmpc[2]);
    String hf2_hex = String(buf);

    String hf1form = "<div class='form-floating'><label for='hf1_color'>Hintergrundfarbe 1</label><input type='color' id='hf1_color' name='hf1_color' value='"+hf1_hex+"'></div>";
    String hf2form = "<div class='form-floating'><label for='hf2_color'>Hintergrundfarbe 2</label><input type='color' id='hf2_color' name='hf2_color' value='"+hf2_hex+"'></div>";

    String hsform = "<div class='form-floating'><label for='hs'>Hintergrundschema</label><select name='hs' id='hs'>";

   myauswahl3 = " ";

    for(int i=0;i<6;i++){
      if(i==hintergrundschema){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      hsform=hsform+"<option value="+i+" "+myauswahl3+">"+htmlfarbschema[i]+"</option>";
    }

    hsform = hsform + "</select> </div><br>";

    String efxform = "<div class='form-floating'><label for='efx'>Übergangseffekt</label><select name='efx' id='efx'>";

   myauswahl3 = " ";

    for(int i=0;i<13;i++){
      if(i==effectMode){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      efxform=efxform+"<option value="+i+" "+myauswahl3+">"+htmlefx[i]+"</option>";
    }

    efxform = efxform + "</select> </div>";

    String efxtimeform = "<div class='form-floating'><label for='efxtime'>Übergangseffektgeschwindigkeit</label><select name='efxtime' id='efxtime'>";

   myauswahl3 = " ";

    for(int i=0;i<3;i++){
      if(i==efxtimeint){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      efxtimeform=efxtimeform+"<option value="+i+" "+myauswahl3+">"+htmlefxtime[i]+"</option>";
    }

    efxtimeform = efxtimeform + "</select> </div><br>";

    String aniform = "<div class='form-floating'><label for='ani'>Animationseffekt</label><select name='ani' id='ani'>";

   myauswahl3 = " ";

    for(int i=0;i<6;i++){
      if(i==aniMode){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      aniform=aniform+"<option value="+i+" "+myauswahl3+">"+htmlani[i]+"</option>";
    }

    aniform = aniform + "</select> </div>";

    String anitimeform = "<div class='form-floating'><label for='anitime'>Animationseffektzeit</label><select name='anitime' id='anitime'>";

   myauswahl3 = " ";

    for(int i=0;i<3;i++){
      if(i==anitimeint){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      anitimeform=anitimeform+"<option value="+i+" "+myauswahl3+">"+htmlanitime[i]+"</option>";
    }

    anitimeform = anitimeform + "</select> </div>";



    String anidepthform = "<div class='form-floating'><label for='anidepth'>Animationseffektstärke</label><select name='anidepth' id='anidepth'>";

   myauswahl3 = " ";

    for(int i=0;i<3;i++){
      if(i==anidepth){
        myauswahl3 ="selected";
      }else{
        myauswahl3 = " ";
      }
      anidepthform=anidepthform+"<option value="+i+" "+myauswahl3+">"+htmlanidepth[i]+"</option>";
    }

    anidepthform = anidepthform + "</select> </div>";

    // Helligkeitsregler (0-100% Anzeige, intern 0-255)
    int dimm_percent = (dimm * 100) / 255;
    String dimmform = "<br/><div class='form-floating'><label for='dimm'>Helligkeit:</label><br/>";
    dimmform += "<input type='range' id='dimm' name='dimm' min='0' max='100' step='1' value='"+String(dimm_percent)+"' ";
    dimmform += "oninput='this.nextElementSibling.value = this.value' style='width: 80%;'>";
    dimmform += "<output style='margin-left: 10px; font-weight: bold;'>"+String(dimm_percent)+"%</output>";
    dimmform += "</div><br/>";

  if (server.method() == HTTP_POST) {

    // Helper: convert #RRGGBB into nearest palette index
    auto hexToNearestIndex = [&](const String &hex, int fallback)->int {
      if (hex.length() == 0) return fallback;
      String s = hex;
      if (s.charAt(0) == '#') s = s.substring(1);
      if (s.length() != 6) return fallback;
      int r = (int)strtol(s.substring(0,2).c_str(), nullptr, 16);
      int g = (int)strtol(s.substring(2,4).c_str(), nullptr, 16);
      int b = (int)strtol(s.substring(4,6).c_str(), nullptr, 16);
      unsigned long bestDist = 0xFFFFFFFFUL;
      int bestIdx = fallback;
      for (int i=0;i<anzahlfarben;i++){
        int rgb[3];
        getPaletteColor((uint8_t)i, rgb);
        long dr = r - rgb[0];
        long dg = g - rgb[1];
        long db = b - rgb[2];
        unsigned long dist = (unsigned long)(dr*dr + dg*dg + db*db);
        if (dist < bestDist) { bestDist = dist; bestIdx = i; }
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

    // Helligkeit verarbeiten (0-100% → 0-255)
    int dimm_percent = server.arg("dimm").toInt();
    if (dimm_percent < 0) dimm_percent = 0;
    if (dimm_percent > 100) dimm_percent = 100;
    int new_dimm = (dimm_percent * 255) / 100;
    dimm = new_dimm;
    
    // Helligkeit in design-Struktur speichern
    user_design.dimm = new_dimm;
    EEPROM.put(sizeof(settings)+sizeof(MyColor), user_design);

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
    // GET Request - Formular anzeigen
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    
    // Header senden
    server.sendContent(FPSTR(htmlhead));
    server.sendContent(body);
    
    // Formulare senden
    server.sendContent(vf1form);
    server.sendContent(vf2form);
    server.sendContent(vsform);
    server.sendContent(hf1form);
    server.sendContent(hf2form);
    server.sendContent(hsform);
    server.sendContent(efxform);
    server.sendContent(efxtimeform);
    server.sendContent(aniform);
    server.sendContent(anitimeform);
    server.sendContent(anidepthform);
    server.sendContent(dimmform);
    server.sendContent("<button type='submit'>Speichern</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main></div> </body></html>");
    server.sendContent("");
  }
}

void handleHAConfig() {
  // Simple page to enable/disable Home Assistant discovery
  if (server.method() == HTTP_POST) {
    bool enabled = server.hasArg("ha_enable") && server.arg("ha_enable") == "1";
    haDiscoveryEnabled = enabled;
    // persist single byte after version slot
    int verOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
    int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + VERSION_STR_MAX + 1;
    int haFlagOffset = verOffset + VERSION_STR_MAX;
    EEPROM.begin(eepromTotalSize);
    EEPROM.write(haFlagOffset, enabled ? 1 : 0);
    EEPROM.commit();
    EEPROM.end();
    String html = FPSTR(htmlhead);
    html += "<main class='form-signin'><h1>Home Assistant</h1><p>Einstellung gespeichert. Die Uhr wird neu gestartet.</p></main></body></html>";
    server.send(200, "text/html", html);
    delay(1200);
    ESP.restart();
  } else {
    String html = FPSTR(htmlhead);
    html += "<main class='form-signin'><form action='/ha' method='post'><h1>Home Assistant</h1>";
    html += "<label>Home Assistant Erkennung aktivieren</label> ";
    html += "<input type='checkbox' name='ha_enable' value='1' ";
    if (haDiscoveryEnabled) html += "checked";
    html += "> <br/><button type='submit'>Speichern</button></form>";
    html += "<br/><hr/><br/>";
    html += "<h2>Erkennung manuell senden</h2>";
    html += "<p>Sendet die Erkennungs-Konfiguration erneut an Home Assistant.</p>";
    html += "<form action='/ha/discover' method='post'>";
    html += "<button type='submit'>Erkennung jetzt senden</button>";
    html += "</form></main></body></html>";
    server.send(200, "text/html", html);
  }
}

void handleHADiscover() {
  // Trigger manual discovery
  if (haDiscoveryEnabled && mqttenable) {
    discoveryNeeded = true;
    String html = FPSTR(htmlhead);
    html += "<main class='form-signin'><h1>Home Assistant Erkennung</h1>";
    html += "<p>Erkennung wird beim nächsten MQTT-Verbindungsaufbau gesendet.</p>";
    html += "<p>Dies kann einige Sekunden dauern...</p>";
    html += "<br/><a href='/ha'>Zurück</a></main></body></html>";
    server.send(200, "text/html", html);
    // Force MQTT reconnect to trigger discovery
    if (client.connected()) {
      client.disconnect();
    }
  } else {
    String html = FPSTR(htmlhead);
    html += "<main class='form-signin'><h1>Home Assistant Erkennung</h1>";
    html += "<p>Erkennung kann nicht gesendet werden:</p><ul>";
    if (!haDiscoveryEnabled) html += "<li>Home Assistant Erkennung ist deaktiviert</li>";
    if (!mqttenable) html += "<li>MQTT ist nicht aktiviert</li>";
    html += "</ul><br/><a href='/ha'>Zurück</a></main></body></html>";
    server.send(200, "text/html", html);
  }
}

void handleUpload(){
  String body = "<main class='form-signin'><h1>Firmware Aktualisierung</h1>";
  body += "<p>Aktuelle Version: <strong>" + String(FW_VERSION) + "</strong></p>";
  
  // GitHub Releases Section
  body += "<div style='margin:20px 0;padding:15px;background:rgba(255,255,255,0.1);border-radius:5px;border-left:4px solid #4caf50;color:#fff'>";
  body += "<h3 style='color:#4caf50'><i class='fab fa-github'></i> Neue Versionen auf GitHub</h3>";
  body += "<p style='color:#fff'>Lade die neueste Firmware von GitHub herunter:</p>";
  body += "<ol style='line-height:1.8;color:#fff'>";
  body += "<li>Gehe zu <a href='https://github.com/anbic8/wortuhr/releases' target='_blank' style='color:#66bb6a;font-weight:bold'>GitHub Releases</a></li>";
  body += "<li>Lade die passende .bin Datei herunter:<ul>";
  
  #if VERSION_TYPE == 0
  body += "<li><strong>firmware_deutsche_11x11.bin</strong> (deine Version)</li>";
  #elif VERSION_TYPE == 1
  body += "<li><strong>firmware_bayrisch_11x11.bin</strong> (deine Version)</li>";
  #elif VERSION_TYPE == 2
  body += "<li><strong>firmware_mini_8x8.bin</strong> (deine Version)</li>";
  #endif
  
  body += "</ul></li>";
  body += "<li>Lade die Datei unten hoch</li>";
  body += "</ol></div>";
  
  // Manual Upload Section
  body += "<div style='margin:20px 0;padding:15px;background:rgba(255,255,255,0.1);border-radius:5px;color:#fff'>";
  body += "<h3 style='color:#fff'><i class='fas fa-upload'></i> Firmware hochladen</h3>";
  body += "<p style='color:#fff'>Wähle die heruntergeladene .bin Datei aus:</p>";
  body += "<form method='POST' action='/upload' enctype='multipart/form-data'>";
  body += "<input type='file' name='firmware' accept='.bin' style='margin:10px 0;padding:10px;border:2px solid #555;border-radius:5px;background:#2c3e50;color:#fff'>";
  body += "<br><input type='submit' value='Firmware hochladen' style='padding:12px 30px;background:#4caf50;color:white;border:none;border-radius:5px;cursor:pointer;font-size:16px'>";
  body += "</form></div>";
  
  body += "<div style='margin:20px 0;padding:10px;background:rgba(255,193,7,0.2);border-radius:5px;border-left:4px solid #ffc107;color:#fff'>";
  body += "<p style='margin:0;color:#ffc107'><strong>⚠️ Hinweis:</strong> Die Uhr wird während des Updates neu gestartet. Trenne die Stromversorgung nicht!</p>";
  body += "</div>";
  
  body += "</main></body></html>";
  
  String htmlContent = String(FPSTR(htmlhead)) + body;
  server.send(200, "text/html", htmlContent);
}

void handleUpdate() {
  String body = "<h1>";
    String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    

  if (Update.hasError()) {
    htmlContent += body;
    htmlContent +="Update fehlgeschlagen!</h1><a href='/'>Zurück</a></div></body></html>";
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", htmlContent);
  } else {
    htmlContent += body;
    htmlContent +="Update erfolgreich!</h1><a href='/'>Zurück</a></div></body></html>";
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", htmlContent);
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
