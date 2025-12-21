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

  // Daten ins EEPROM schreiben
  EEPROM.put(0, user_connect);

  // Checkbox prüfen (nur vorhanden, wenn angehakt)
  user_design.mqttenable = server.hasArg("mqttenable") ? true : false;

  EEPROM.put(sizeof(struct settings)+sizeof(struct MyColor), user_design);
  EEPROM.commit();


  String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);

    
  htmlContent = htmlContent +"<h1>Wifi Setup</h1> <br/> <p>Deine Netzwerkeinstellungen wurden gespeichert!<br />Die Uhr wird jetzt neu gestartet. </p></main></body></html>";

     server.send(200,   "text/html", htmlContent );
    delay(2000);
    ESP.restart();
  } else {

    String htmlContent;
      int mqttenablenr;
    if (mqttenable == true){
      mqttenablenr = 1;
    }else {
      mqttenablenr = 0;
    }
    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
  htmlContent =htmlContent + "<main class='form-signin'> <form action='/wifi' method='post'> <h1 class=''>Wifi Setup</h1><br/><div class='form-floating'><label>SSID</label><input type='text' class='form-control' name='ssid' value='"+user_connect.ssid+"'> <br/><label>Password</label><input type='password' class='form-control' name='password' value='"+user_connect.password+"'><br/>Mqtt aktivieren <input type='checkbox' name='mqttenable' value='"+mqttenablenr+"'><br><label>MQTT server</label><input type='text' class='form-control' name='mqtt_server' value='"+user_connect.mqtt_server+"'><br><label>MQTT port</label><input type='text' class='form-control' name='mqtt_port' value='"+user_connect.mqtt_port+"'><br><label>MQTT user</label><input type='text' class='form-control' name='mqtt_user' value='"+user_connect.mqtt_user+"'><br/><label>Mqtt Password</label><input type='password' class='form-control' name='mqtt_password' value='"+user_connect.mqtt_password+"'></div><br/><button type='submit'>Save</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>";

    server.send(200,   "text/html", htmlContent );
  }
}

void handledesignPath() {

  if (server.method() == HTTP_POST) {

    int anp = server.arg("anh").toInt()*60+server.arg("anm").toInt();
    int ausp = server.arg("aush").toInt()*60+server.arg("ausm").toInt();

      design customDesign = {
    server.arg("db").toInt(),
    server.arg("dv").toInt(),
    server.arg("uv").toInt(),
    anp,
    ausp,
    server.arg("nacht").toInt(),
    sommerzeit,
    dimm,
    mqttenable
  };

  EEPROM.put(sizeof(struct settings)+sizeof(struct MyColor), customDesign);
  EEPROM.commit();
    String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent = htmlContent + "<main class='form-signin'> <h1>Design Setup</h1> <br/> <p>Deine Einstellungen wurden gespeichert!<br />Die Uhr wird jetzt neu gestartet. </p></main></body></html>";
    server.send(200,   "text/html",  htmlContent );
   delay(2000);
  ESP.restart();
   
  } else {

    String body ="<main class='form-signin'><form action='/setting' method='post'>";
    body += "<h1 class=''>Design Setup</h1><br/><div class='form-floating'><label for='db'>Version</label>";
    String selected1="";
    String selected2="";
    String selected3="";
    if(dbv==0){
      selected1="selected";
    }else{
      selected2="selected";
    };
    body += "<select name='db' id='db'><option value='0' "+selected1+">deutsch</option><option value='1' "+selected2+">bayrisch</option></select> </div>";
    body += "<div class='form-floating'><br/><label for='dv'>Anzeige xx:45 </label>";
  if(dvv==0){
      selected1="selected";
      selected2=" ";
    }else{
      selected2="selected";
      selected1=" ";
    };
    body += "<select name='dv' id='dv'><option value='0' "+selected1+" >dreiviertel</option><option value='1' "+selected2+" >viertel vor</option></select></div>";
    body += "<div class='form-floating'><label for='uv'>Anzeige des Wortes &bdquo; Uhr &rdquo;  (nur in der deutschen Version möglich) </label>";
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
    
    body += "<br/><button type='submit'>Save</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>";
    String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    
    server.send(200,   "text/html", htmlContent );
  }
}
void handlecolorPath() {

  String body ="<main class='form-signin'><form action='/color' method='post'> <h1 class=''>Color Setup</h1><br/>";
    String vf1form ="<div class='form-floating'><label for='vf1'>Vordergrundfarbe 1</label><select name='vf1' id='vf1'>";

    String myauswahl1 = " ";

      for(int i=0;i<anzahlfarben;i++){
      if(i==v1){
        myauswahl1 ="selected";
      }else{
        myauswahl1 = " ";
      }
      vf1form=vf1form+"<option value="+i+" "+myauswahl1+">"+htmlfarben[i]+"</option>";
    };

    vf1form = vf1form + "</select> </div>";


   String vf2form = "<div class='form-floating'><label for='vf2'>Vordergrundfarbe 2</label><select name='vf2' id='vf2'>";

  String myauswahl2 = " ";

    for(int i=0;i<anzahlfarben;i++){
      if(i==v2){
        myauswahl2 ="selected";
      }else{
        myauswahl2 = " ";
      }
      vf2form=vf2form+"<option value="+i+" "+myauswahl2+">"+htmlfarben[i]+"</option>";
    }

    vf2form = vf2form + "</select> </div>";

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

    String hf1form ="<div class='form-floating'><label for='hf1'>Hintergrundfarbe 1</label><select name='hf1' id='hf1'>";

     myauswahl1 = " ";

      for(int i=0;i<anzahlfarben;i++){
      if(i==h1){
        myauswahl1 ="selected";
      }else{
        myauswahl1 = " ";
      }
      hf1form=hf1form+"<option value="+i+" "+myauswahl1+">"+htmlfarben[i]+"</option>";
    };

    hf1form = hf1form + "</select> </div>";


   String hf2form = "<div class='form-floating'><label for='hf2'>Hintergrundfarbe 2</label><select name='hf2' id='hf2'>";

   myauswahl2 = " ";

    for(int i=0;i<anzahlfarben;i++){
      if(i==h2){
        myauswahl2 ="selected";
      }else{
        myauswahl2 = " ";
      }
      hf2form=hf2form+"<option value="+i+" "+myauswahl2+">"+htmlfarben[i]+"</option>";
    }

    hf2form = hf2form + "</select> </div>";

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

    for(int i=0;i<10;i++){
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

  if (server.method() == HTTP_POST) {

      MyColor customVar = {
    server.arg("vf1").toInt(),
    server.arg("vf2").toInt(),
    server.arg("vs").toInt(),
    server.arg("hf1").toInt(),
    server.arg("hf2").toInt(),
    server.arg("hs").toInt(),
    server.arg("efx").toInt(),
    server.arg("efxtime").toInt(),
    server.arg("ani").toInt(),
    server.arg("anitime").toInt(),
    server.arg("anidepth").toInt()
  };
  EEPROM.put(sizeof(struct settings), customVar);
  EEPROM.commit();
  readTime();
  //readTimeRCT();
  neuefarbe();
    body = body+"<div><p>Deine Farbeinstellung wurde gespeichert!</p></div>"+"<p style='text-align: right'>(c) by Andy B</p></form></main></div> </body></html>";
String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    server.send(200,   "text/html", htmlContent  );
    
  } else {
    

    body = body+vf1form+vf2form+vsform+hf1form+hf2form+hsform+efxform+efxtimeform+aniform+anitimeform+anidepthform+"<button type='submit'>Save</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main></div> </body></html>";
    String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    server.send(200,   "text/html", htmlContent);
  }
}

void handleUpload(){
      String body = "<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='firmware' accept='.bin'><input type='submit' value='Update Firmware'></form><p>  aktuelle Version deiner Wortuhr "+String(FW_VERSION)+" </p></div></body></html>";
    String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    server.send(200,   "text/html", htmlContent);

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
