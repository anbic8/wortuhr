#include "show.h"
#include "globals.h"
#include "color.h"
#include <EEPROM.h>
#include "effects.h"
#ifdef USE_RCT
  #include "rct.h"
#endif

void readTime(){
  #ifdef USE_RCT
    readTimeRCT();
  #else
    readTimeNet();
  #endif
  checkon();
}

// Render a two-digit countdown (00-99) centered on the matrix using a larger 5x7 font
// which: 0=user countdown, 1=newyear countdown
void showCountdown(int secondsLeft, int which) {
  // 5x7 font, each row is 5 bits (MSB on left)
  const uint8_t font[10][7] = {
    {0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110}, //0
    {0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110}, //1
    {0b01110,0b10001,0b00001,0b00010,0b00100,0b01000,0b11111}, //2
    {0b01110,0b10001,0b00001,0b00110,0b00001,0b10001,0b01110}, //3
    {0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010}, //4
    {0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110}, //5
    {0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110}, //6
    {0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000}, //7
    {0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110}, //8
    {0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100}  //9
  };

  int tens = secondsLeft / 10;
  int ones = secondsLeft % 10;
  // clear matrixanzeige
  for(int r=0;r<MATRIX_SIZE;r++) for(int c=0;c<MATRIX_SIZE;c++) matrixanzeige[r][c]=0;

  const int fw = 5, fh = 7, gap = 1;
  int totalW = fw*2 + gap; // for 11x11 matrix this is 11
  int startCol = (MATRIX_SIZE - totalW) / 2; // should be 0 for 11
  int startRow = (MATRIX_SIZE - fh) / 2; // center vertically

  // draw digits into matrixanzeige
  for(int row=0; row<fh; row++){
    uint8_t rowT = font[tens][row];
    uint8_t rowO = font[ones][row];
    for(int col=0; col<fw; col++){
      int bit = fw-1-col;
      if (rowT & (1 << bit)) {
        int rr = startRow + row;
        int cc = startCol + col;
        if (rr>=0 && rr<MATRIX_SIZE && cc>=0 && cc<MATRIX_SIZE) matrixanzeige[rr][cc]=1;
      }
      if (rowO & (1 << bit)) {
        int rr = startRow + row;
        int cc = startCol + fw + gap + col;
        if (rr>=0 && rr<MATRIX_SIZE && cc>=0 && cc<MATRIX_SIZE) matrixanzeige[rr][cc]=1;
      }
    }
  }

  // Map matrixanzeige -> anzeige colors
  for(int r=0;r<MATRIX_SIZE;r++){
    for(int c=0;c<MATRIX_SIZE;c++){
      if (matrixanzeige[r][c]==1){
        // use foreground color vf1 for digits
        anzeige[r][c][0] = vf1[0];
        anzeige[r][c][1] = vf1[1];
        anzeige[r][c][2] = vf1[2];
      } else {
        // dim background using hf1
        anzeige[r][c][0] = hf1[0];
        anzeige[r][c][1] = hf1[1];
        anzeige[r][c][2] = hf1[2];
      }
    }
  }

  showmystrip();

  // when hitting zero or negative, clear the corresponding countdown
  if (secondsLeft <= 0) {
    int countdownOffset = sizeof(settings) + sizeof(MyColor) + sizeof(design) + sizeof(geburtstage);
    // persist only user countdown; newyear countdown is RAM-only
    if (which == 1) {
      newyear_countdown_ts = 0;
    } else {
      countdown_ts = 0;
      int eepromTotalSize = sizeof(settings)+sizeof(MyColor)+sizeof(design)+sizeof(geburtstage) + sizeof(unsigned long) + VERSION_STR_MAX + 1;
      EEPROM.begin(eepromTotalSize);
      EEPROM.put(countdownOffset, countdown_ts);
      EEPROM.commit();
      EEPROM.end();
    }
  }
}

void readTimeNet(){
  // Get current time from NTP
  time(&now);                        // read the current time from NTP
  localtime_r(&now, &tm);            // update the structure tm with the current time
  stunden = tm.tm_hour;
  minutes = tm.tm_min;
  seconds = tm.tm_sec;
  month = tm.tm_mon+1;
  day = tm.tm_mday;
  year = tm.tm_year + 1900;
  
  hours = stunden%12;
  zeit = stunden*60+minutes;
  
  mb = int(minutes/5);
  h = hours;
  m = minutes%5;
  
  // Debug output every 10 seconds
  static unsigned long lastDebugOutput = 0;
  if (millis() - lastDebugOutput > 10000) {
    lastDebugOutput = millis();
    Serial.print("Zeit von NTP: ");
    Serial.print(stunden);
    Serial.print(":");
    if (minutes < 10) Serial.print("0");
    Serial.print(minutes);
    Serial.print(":");
    if (seconds < 10) Serial.print("0");
    Serial.println(seconds);
  }

}

void checkon(){
  Serial.print("an:");
  Serial.println(an);
  
  Serial.print("aus:");
  Serial.println(aus);
 if(aus>an){
    if(zeit>an && zeit<aus){
      on=1;
    }else{
      on=0;
    }
  }
  if(an>aus){
    if(zeit<an && zeit>aus){
      on=0;
    }else{
      on=1;
    }
  }

  Serial.print("Nachtmodus: ");
  Serial.println(on);

  if(mqtton==on){
    mqttonset=0;
  }

  if(mqttonset==1){
    on=mqtton;
  }

}

void showClock(){

  
    // If a countdown is active and within the last 99 seconds, show digits
    time(&now);
    localtime_r(&now, &tm);
    long sleft_user = -1;
    long sleft_new = -1;
    if (countdown_ts > 0) sleft_user = (long)countdown_ts - (long)now;
    if (newyear_countdown_ts > 0) sleft_new = (long)newyear_countdown_ts - (long)now;

    int which = -1;
    long showSeconds = -1;
    if (sleft_user >= 0 && sleft_user <= 99) { which = 0; showSeconds = sleft_user; }
    if (sleft_new >= 0 && sleft_new <= 99) {
      if (which == -1 || sleft_new < showSeconds) { which = 1; showSeconds = sleft_new; }
    }
    if (which != -1) {
      showCountdown((int)showSeconds, which);
      return;
    }

    setmatrixanzeige();

  if (nacht==1){ 
    strip.setBrightness(50);
  }

  if (on==1){
    strip.setBrightness(dimm);
  }
    
    
   
  int aktivegeb=0;
  for (int i=0;i<5;i++){
    if(geburtstage[i][0]==day && geburtstage[i][1]==month){
      aktivegeb=1;
    }
  }

  if(aktivegeb==1){
    vordergrundschema=5;
    vordergrunderstellen(vf1, vf2);
    int leer[3]={0,0,0};
    hintergrunderstellen(leer,leer);
    gebstat=1;
  }else{

  if(gebstat==1){
    EEPROM.get(sizeof(settings), user_color );
    vordergrundschema = user_color.vs;
    gebstat=0;
  }
  vordergrunderstellen(vf1,vf2);
  hintergrunderstellen(hf1,hf2);
  }


  int welchereffekt = 0;
  if(effectMode==1){
    welchereffekt= random(2,14);
  }else{
    welchereffekt=effectMode;
  }


  if(on==0 && nacht==0){
    for(int row=0; row<MATRIX_SIZE; row++){
     for(int col=0; col<MATRIX_SIZE; col++) {
        anzeige[row][col][0]=0;
        anzeige[row][col][1]=0;
        anzeige[row][col][2]= 0;
    } 
    }//alle Lichter aus
    showmystrip();
  }else{
  switch (welchereffekt) {
    case 2:
        fadeefx();
      break;
    case 3:
        running();
      break;
    case 4:
        schlange();
      break;
    case 5:
        zeilenefx();
      break;
    case 6:
        scrollall();
      break;  
    case 7:
        slidein();
      break; 
    case 8:
        diagonal();
      break;
    case 9:
        rain();
      break;
    case 10:
        spiral();
      break;
    case 11:
        snakeeater();
      break;
    case 12:
        diamond();
      break;
    case 13:
        firework();
      break;
    default:
      noeffect();
      break;
  }
  }
  

  anistate=0;
}



void setmatrixanzeige(){
  for(int row=0;row<MATRIX_SIZE;row++){
    for(int col=0;col<MATRIX_SIZE;col++){
      matrixanzeige[row][col]=0;
    }
  } // matrixanzeige leeren

  #if MATRIX_SIZE == 11
    for (int i=0; i<2; i++){
      matrixanzeige[0][i]=1;
  }  //ES
   for (int i=3; i<iist; i++){
      matrixanzeige[0][i]=1;
  }  //ist oder is (bayrisch)
  #endif

  #if MATRIX_SIZE == 11
  if (dbv==0 && uvv==1){
    for (int i=8; i<11; i++){
        matrixanzeige[9][i]=1;
    }  //UHR
  }
  if (dbv==0 && uvv==2 && mb== 0){
    for (int i=8; i<11; i++){
        matrixanzeige[9][i]=1;
    }   //UHR
  }
  #endif

  
    // Minute words: set matrixanzeige using prebuilt LED index lists
    for (uint8_t k = 0; k < getWordLedCount(mb, false); ++k) {
      int led = getWordLed(mb, false, k);
      if (led >= 0) {
        int rr = ledRow[led];
        int cc = ledCol[led];
        if (rr >= 0 && cc >= 0) matrixanzeige[rr][cc] = 1;
      }
    }

  if(mb>=nexthour){
    h++;
  } // nächste Stunde ab Halb
  h = h%12; // 12 wird zu 0

  // Hours: use prebuilt LED lists
  for (uint8_t k = 0; k < getWordLedCount(h, true); ++k) {
    int led = getWordLed(h, true, k);
    if (led >= 0) {
      int rr = ledRow[led];
      int cc = ledCol[led];
      if (rr >= 0 && cc >= 0) matrixanzeige[rr][cc] = 1;
    }
  }

  if (dbv==0 && uvv > 0 && h==1 && mb==0){
   matrixanzeige[5][3]=0;
  }// aus Eins Uhr wird Ein Uhr
  
  #if MATRIX_SIZE == 11
  Serial.println("DEBUG MODULO:");
  for (int i=0; i<m; i++){
    matrixanzeige[MATRIX_SIZE-1][matrixminmodulomap[i]] = 1;
    Serial.print("Modulo ");
    Serial.print(i);
    Serial.print(" LED ");
    Serial.println(matrixminmodulomap[i]);
  }  //Modulominitues
  #endif


  

}
