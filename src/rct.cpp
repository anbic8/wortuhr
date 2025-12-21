/*
void readTimeRCT()
{
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
    
    
  hours   = stunden%12;
 zeit = stunden*60+minutes; 
   mb = int(minutes/5);
  h = hours;
  m = minutes%5;


}

void setDate(int s, int m, int h, int d, int month, int y )
{
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0x00);
    Wire.write(decToBcd(s));   // seconds
    Wire.write(decToBcd(m));   // minutes
    Wire.write(decToBcd(h));   // hours (24-hour format)
    Wire.write(decToBcd(d));   // day
    Wire.write(decToBcd(month));   // month
    Wire.write(decToBcd(y)); // year
    Wire.write(0x00);
    Wire.endTransmission();
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

    // Aus Flash lesen und in String schreiben
    htmlContent += FPSTR(htmlhead);

  String body ="<h1>Uhrzeit einstellen</h1> <br/> <p>Deine Uhrzeit wurden gespeichert!<br /> </p></main></body></html>";

    htmlContent += body;
    server.send(200,   "text/html", htmlContent );
  } else {

  String body ="<main class='form-signin'> <form action='/' method='post'> <h1 class=''>Wifi Setup</h1><br/>";
  body += "<div class='form-floating'><label>Uhrzeit hh:mm:ss</label><br>";
  body += "<input type='number' id='hours' name='hours' min='0' max='23' step='1'>:<input type='number' id='minutes' name='minutes' min='0' max='59' step='1'>:<input type='number' id='seconds' name='seconds' min='0' max='59' step='1'></div><br/>";
  body += "<div class='form-floating'><label>datum einstellen dd.mm.yyyy</label><br>";
  body += "<input type='number' id='day' name='day' min='0' max='31' step='1'>:<input type='number' id='month' name='mont' min='0' max='12' step='1'>.<input type='number' id='year' name='year' min='2025' max='3000' step='1'></div><br/>";
  body += "<br/><button type='submit'>Save</button><p></p><p style='text-align: right'>(c) by Andy B</p></form></main> </body></html>";

    server.send(200,   "text/html", htmlhead + body );
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
*/