#include "rct.h"
#include "globals.h"
#include "show.h"
#include "effects.h"

#ifdef USE_RCT
#include <Wire.h>

void readTimeRCT()
{
    Serial.println("Lese Zeit von RTC...");
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
  
    Serial.print("Zeit von RTC: ");
  Serial.print(stunden); Serial.print(":");
  if (minutes < 10) Serial.print("0"); 
  Serial.print(minutes); Serial.print(":");
  if (seconds < 10) Serial.print("0");
  Serial.println(seconds);
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
    Serial.print("Zeit in RTC gespeichert");
    Serial.print(" "); Serial.print(h); Serial.print(":"); Serial.print(m); Serial.print(":"); Serial.println(s);
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
    
    String htmlContent;
    htmlContent += FPSTR(htmlhead);
    String body ="<h1>Uhrzeit einstellen</h1> <br/> <p>Deine Uhrzeit wurden gespeichert!<br /> </p></main></body></html>";
    htmlContent += body;
    server.send(200, "text/html", htmlContent);
  } else {
    String body ="<main class='form-signin'> <form action='/settime' method='post'> <h1 class=''>Zeit einstellen</h1><br/>";
    body += "<div class='form-floating'><label>Uhrzeit hh:mm:ss</label><br>";
    body += "<input type='number' id='hours' name='hours' min='0' max='23' step='1' value='" + String(stunden) + "'>:";
    body += "<input type='number' id='minutes' name='minutes' min='0' max='59' step='1' value='" + String(minutes) + "'>:";
    body += "<input type='number' id='seconds' name='seconds' min='0' max='59' step='1' value='" + String(seconds) + "'></div><br/>";
    body += "<div class='form-floating'><label>Datum einstellen dd.mm.yyyy</label><br>";
    body += "<input type='number' id='day' name='day' min='1' max='31' step='1' value='" + String(day) + "'>.";
    body += "<input type='number' id='month' name='month' min='1' max='12' step='1' value='" + String(month) + "'>.";
    body += "<input type='number' id='year' name='year' min='2025' max='3000' step='1' value='" + String(year) + "'></div><br/>";
    body += "<br/><button type='submit'>Save</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>";
    server.send(200, "text/html", htmlhead + body);
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