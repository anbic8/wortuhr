#include "rct.h"
#include "globals.h"
#include "webserver_html.h"
#include "show.h"
#include "effects.h"

#ifdef USE_RCT
#include <Wire.h>

void readTimeRCT()
{
    LOGLN("Lese Zeit von RTC...");
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom(DS1307_ADDRESS, 7);

    seconds = bcdToDec(Wire.read()& 0x7F);
    minutes = bcdToDec(Wire.read());
    stunden   = bcdToDec(Wire.read() & 0x3F);
    Wire.read();
    day = bcdToDec(Wire.read());
    month = bcdToDec(Wire.read());
    year = bcdToDec(Wire.read())+2000;
    
    // LokaleVariablen aktualisieren
    hours   = stunden%12;
    zeit = stunden*60+minutes; 
    mb = int(minutes/5);
    h = hours;
    m = minutes%5;
  
    // **NEU: Berechne Unix-Timestamp mit DST-Anpassung**
    struct tm rtc_time = {
        .tm_sec = seconds,
        .tm_min = minutes,
        .tm_hour = stunden,
        .tm_mday = day,
        .tm_mon = month - 1,
        .tm_year = year - 1900,
        .tm_isdst = -1  // Automatische DST-Erkennung
    };
    now = mktime(&rtc_time);  // Konvertiert mit Timezone zu Unix-Timestamp
  
    LOG("Zeit von RTC: ");
  LOG(stunden); LOG(":");
  if (minutes < 10) LOG("0"); 
  LOG(minutes); LOG(":");
  if (seconds < 10) LOG("0");
  LOGLN(seconds);
}

void setDate(int s, int m, int h, int d, int month, int y )
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0x00);
    Wire.write(decToBcd(s));   // seconds
    Wire.write(decToBcd(m));   // minutes
    Wire.write(decToBcd(h));   // hours (24-hour format)
    Wire.write(decToBcd(1));
    Wire.write(decToBcd(d));   // day
    Wire.write(decToBcd(month));   // month
    Wire.write(decToBcd(y % 100)); // year (two-digit)
    Wire.write(0x00);
    Wire.endTransmission();
    LOG("Zeit in RTC gespeichert");
    LOG(" "); LOG(h); LOG(":"); LOG(m); LOG(":"); LOGLN(s);
    readTimeRCT(); // Refresh time variables after setting
    showClock();
}

void handlesettime() {
  if (server.method() == HTTP_POST) {
    int s = server.arg("seconds").toInt();
    int m = server.arg("minutes").toInt();
    int h = server.arg("hours").toInt();
    int d = server.arg("day").toInt();
    int month = server.arg("month").toInt();
    int y = server.arg("year").toInt();

    setDate(s, m, h, d, month, y);

    // Streaming statt eine grosse String zusammenzubauen - `htmlhead` ist
    // ein mehrere KB grosser PROGMEM-Block; "htmlhead + body" (wie zuvor)
    // brauchte eine einzige zusammenhaengende Heap-Allokation dieser Groesse
    // und stuerzte bei knappem/fragmentiertem Heap ab (siehe CHANGELOG).
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(FPSTR(htmlhead));
    server.sendContent("<h1>Uhrzeit einstellen</h1> <br/> <p>Deine Uhrzeit wurden gespeichert!<br /> </p></main></body></html>");
  } else {
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send(200, "text/html", "");
    server.sendContent(FPSTR(htmlhead));
    server.sendContent("<main class='form-signin'> <form action='/settime' method='post'> <h1 class=''>Zeit einstellen</h1><br/>");
    server.sendContent("<div class='form-floating'><label>Uhrzeit hh:mm:ss</label><br>");
    server.sendContent(String("<input type='number' id='hours' name='hours' min='0' max='23' step='1' value='") + stunden + "'>:");
    server.sendContent(String("<input type='number' id='minutes' name='minutes' min='0' max='59' step='1' value='") + minutes + "'>:");
    server.sendContent(String("<input type='number' id='seconds' name='seconds' min='0' max='59' step='1' value='") + seconds + "'></div><br/>");
    server.sendContent("<div class='form-floating'><label>Datum einstellen dd.mm.yyyy</label><br>");
    server.sendContent(String("<input type='number' id='day' name='day' min='1' max='31' step='1' value='") + day + "'>.");
    server.sendContent(String("<input type='number' id='month' name='month' min='1' max='12' step='1' value='") + month + "'>.");
    server.sendContent(String("<input type='number' id='year' name='year' min='2025' max='3000' step='1' value='") + year + "'></div><br/>");
    server.sendContent("<br/><button type='submit'>Save</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>");
  }
}

int decToBcd(int value)
{
    return ((value/10*16) + (value%10));
}

int bcdToDec(int value)
{
    return ((value/16*10) + (value%16));
}

#endif // USE_RCT