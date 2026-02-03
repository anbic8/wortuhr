#include "birthday.h"
 

void handlebirthday() {
    if (server.method() == HTTP_POST) {
        for (int i = 0; i < 5; i++) {
            String dateArg = "date" + String(i);
            String deleteArg = "delete" + String(i);

            if (server.hasArg(dateArg)) {
                String dateStr = server.arg(dateArg);
                geburtstage[i][2] = dateStr.substring(0, 4).toInt();
                geburtstage[i][1] = dateStr.substring(5, 7).toInt();
                geburtstage[i][0] = dateStr.substring(8, 10).toInt();
            }

            if (server.hasArg(deleteArg) && server.arg(deleteArg) == "1") {
                geburtstage[i][2] = 0;
                geburtstage[i][1] = 0;
                geburtstage[i][0] = 0;
            }
        }
        EEPROM.put(sizeof(settings)+sizeof(MyColor)+sizeof(design), geburtstage);
        // save countdown timestamp (seconds since epoch) just after geburtstage
        int countdownOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
        if (server.hasArg("countdown")) {
            String dt = server.arg("countdown"); // expected format: YYYY-MM-DDTHH:MM
            if (dt.length() >= 16) {
                int y = dt.substring(0,4).toInt();
                int mo = dt.substring(5,7).toInt();
                int d = dt.substring(8,10).toInt();
                int h = dt.substring(11,13).toInt();
                int mi = dt.substring(14,16).toInt();
                struct tm t;
                t.tm_year = y - 1900;
                t.tm_mon = mo - 1;
                t.tm_mday = d;
                t.tm_hour = h;
                t.tm_min = mi;
                t.tm_sec = 0;
                t.tm_isdst = -1;
                time_t ts = mktime(&t);
                if (ts > 0) {
                    countdown_ts = (unsigned long)ts;
                    EEPROM.put(countdownOffset, countdown_ts);
                }
            }
        }
        EEPROM.commit();
        String body="<h2>Die Tage wurden gespeichert!</h2></div></body></html>";
        String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    server.send(200,   "text/html", htmlContent);

    } else {
        String body = "<p> Hier kannst du Tage eingeben, an denen die Buchstaben zufällige Farben bekommen, z.B. an Geburtstagen. <p><form action='/birthday' method='POST'>";
        // include countdown input (datetime-local)
        String countdownVal = "";
        if (countdown_ts > 0) {
            time_t t = (time_t)countdown_ts;
            struct tm tm_local;
            localtime_r(&t, &tm_local);
            char buf[20];
            snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d", tm_local.tm_year + 1900, tm_local.tm_mon + 1, tm_local.tm_mday, tm_local.tm_hour, tm_local.tm_min);
            countdownVal = String(buf);
        }
        body += "<label for='countdown'>Countdown (Datum & Zeit):</label> <input type='datetime-local' id='countdown' name='countdown' value='" + countdownVal + "'><br/><small>Wenn gesetzt, werden die letzten 99 Sekunden als Ziffern angezeigt.</small><br/>";
        for (int i = 0; i < 5; i++) {
            char buffer[11];
            snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", geburtstage[i][2], geburtstage[i][1], geburtstage[i][0]);
            body += "<label for='date" + String(i) + "'>Tag " + String(i + 1) + ":</label>";
            body += "<input type='date' id='date" + String(i) + "' name='date" + String(i) + "' value='" + String(buffer) + "'>";
            body += "<input type='checkbox' name='delete" + String(i) + "' value='1'> löschen<br>";
        }
        body += "<button type='submit'>Absenden</button></form></div></body></html>";
         String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    server.send(200,   "text/html", htmlContent);

    }
}


