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
  EEPROM.commit();
        EEPROM.commit();
        String body="<h2>Die Tage wurden gespeichert!</h2></div></body></html>";
        String htmlContent;

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);
    htmlContent += body;
    server.send(200,   "text/html", htmlContent);

    } else {
        String body = "<p> Hier kannst du Tage eingeben, an denen die Buchstaben zufällige Farben bekommen, z.B. an Geburtstagen. <p><form action='/birthday' method='POST'>";
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


