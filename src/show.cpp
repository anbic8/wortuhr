#include "show.h"
#include "globals.h"
#include "eeprom_layout.h"
#include "color.h"
#include <EEPROM.h>
#include "effects.h"
#ifdef USE_RCT
  #include "rct.h"
#endif

// Picks a random effect index (2..15) from the "Zufällig aus Liste"
// pool. Falls back to the same behaviour as the "zufällig" option
// (random(2,16)) if the pool hasn't been configured yet (empty mask).
static int pickRandomEffectFromPool(uint16_t mask) {
  int candidates[14];
  int count = 0;
  for (int i = 2; i <= 15; i++) {
    if (mask & (1U << i)) candidates[count++] = i;
  }
  if (count == 0) return random(2, 16);
  return candidates[random(0, count)];
}

#if MATRIX_SIZE == 8
// Minute-Pixel-Spalten in der untersten Zeile (7). ZWÖLF/FÜNF/ACHT belegen
// dort selbst einige der Standard-Spalten (1,3,5,7), deshalb weichen diese
// drei Stunden auf andere Spalten aus.
void minuteDotColumnsForHour(int hour, int cols[4]) {
  static const int def[4]    = {1,3,5,7};
  static const int zwoelf[4] = {0,2,4,6}; // hour == 0
  static const int fuenf[4]  = {0,1,6,7}; // hour == 5
  static const int acht[4]   = {0,1,4,5}; // hour == 8
  const int *src = def;
  if (hour == 0) src = zwoelf;
  else if (hour == 5) src = fuenf;
  else if (hour == 8) src = acht;
  for (int i = 0; i < 4; i++) cols[i] = src[i];
}
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
#if MATRIX_SIZE == 8
  // Compact 3x5 font for the 8x8 mini matrix - the 5x7 font below is sized
  // for the 11x11 build (2*5+1 gap = 11 columns) and doesn't fit into 8
  // columns (startCol goes negative and both digits get clipped).
  const uint8_t font[10][5] = {
    {0b111,0b101,0b101,0b101,0b111}, //0
    {0b010,0b110,0b010,0b010,0b111}, //1
    {0b111,0b001,0b111,0b100,0b111}, //2
    {0b111,0b001,0b111,0b001,0b111}, //3
    {0b101,0b101,0b111,0b001,0b001}, //4
    {0b111,0b100,0b111,0b001,0b111}, //5
    {0b111,0b100,0b111,0b101,0b111}, //6
    {0b111,0b001,0b010,0b010,0b010}, //7
    {0b111,0b101,0b111,0b101,0b111}, //8
    {0b111,0b101,0b111,0b001,0b111}  //9
  };
  const int fw = 3, fh = 5, gap = 1;
#else
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
  const int fw = 5, fh = 7, gap = 1;
#endif

  int tens = secondsLeft / 10;
  int ones = secondsLeft % 10;
  // clear matrixanzeige
  for(int r=0;r<MATRIX_SIZE;r++) for(int c=0;c<MATRIX_SIZE;c++) matrixanzeige[r][c]=0;

  int totalW = fw*2 + gap; // 11 for the 11x11 font, 7 for the 8x8 mini font
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
    // persist only user countdown; newyear countdown is RAM-only
    if (which == 1) {
      newyear_countdown_ts = 0;
    } else {
      countdown_ts = 0;
      EepromLayout::beginAll();
      EEPROM.put(EepromLayout::COUNTDOWN_OFFSET, countdown_ts);
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
    LOG("Zeit von NTP: ");
    LOG(stunden);
    LOG(":");
    if (minutes < 10) LOG("0");
    LOG(minutes);
    LOG(":");
    if (seconds < 10) LOG("0");
    LOGLN(seconds);
  }

}

void checkon(){
  
 if(aus>an){
    if(zeit>an && zeit<aus){
      on=1;
    }else{
      on=0;
    }
  }else if(an>aus){
    if(zeit<an && zeit>aus){
      on=0;
    }else{
      on=1;
    }
  }else{
    // an == aus: kein Zeitplan konfiguriert -> Uhr schaltet nie ab.
    // Ohne diesen Zweig bleibt `on` unveraendert (siehe letzter Wert), was
    // "klebt": faellt `on` einmal auf 0, kommt es bei an==aus nie wieder
    // auf 1 zurueck.
    on=1;
  }

  if(mqtton==on){
    mqttonset=0;
  }

  if(mqttonset==1){
    on=mqtton;
  }

}

void showClock(){

  
    // If a countdown is active and within the last 99 seconds, show digits
    // NOTE: Don't call time(&now) here - it's already set in readTime()
    // Multiple time() calls cause timing drift/desync with countdown calculation
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
    EEPROM.get(EepromLayout::COLOR_OFFSET, user_color);
    vordergrundschema = user_color.vs;
    gebstat=0;
  }
  vordergrunderstellen(vf1,vf2);
  hintergrunderstellen(hf1,hf2);
  }

#if MATRIX_SIZE == 8
  // Minuten-Pixel bekommen eine eigene Farbe statt der normalen
  // Vordergrundfarbe - Overwrite direkt nach dem Aufbau von vordergrund,
  // damit es unabhängig vom gewählten Übergangseffekt greift (alle Effekte
  // lesen ihre Vordergrundfarbe pro Zelle aus vordergrund[][]).
  if (minuteDotsEnabled) {
    int cols[4];
    minuteDotColumnsForHour(h, cols);
    int dotRgb[3];
    getPaletteColor((uint8_t)minuteDotsColorIdx, dotRgb);
    for (int i = 0; i < 4; i++) {
      int col = cols[i];
      if (matrixanzeige[MATRIX_SIZE-1][col] == 1) {
        vordergrund[MATRIX_SIZE-1][col][0] = dotRgb[0];
        vordergrund[MATRIX_SIZE-1][col][1] = dotRgb[1];
        vordergrund[MATRIX_SIZE-1][col][2] = dotRgb[2];
      }
    }
  }
#endif

  int welchereffekt = 0;
  if(effectMode==1){
    welchereffekt= random(2,16);
  }else if(effectMode==EFFECT_RANDOM_FROM_LIST_INDEX){
    welchereffekt = pickRandomEffectFromPool(effectRandomPoolMask);
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
    case 14:
        rainbowSwipe();
      break;
    case 15:
        rainbowCycle();
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
  for (int i=0; i<m; i++){
    matrixanzeige[MATRIX_SIZE-1][matrixminmodulomap[i]] = 1;

  }  //Modulominitues
  #elif MATRIX_SIZE == 8
  if (minuteDotsEnabled) {
    int cols[4];
    minuteDotColumnsForHour(h, cols); // h ist hier schon die tatsächlich angezeigte Stunde
    for (int i=0; i<m; i++){
      matrixanzeige[MATRIX_SIZE-1][cols[i]] = 1;
    }
  }
  #endif


  

}
