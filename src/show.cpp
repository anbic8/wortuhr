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

void readTimeNet(){
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
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
    welchereffekt= random(2,12);
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
    default:
      noeffect();
      break;
  }
  }
  

  //Serial output
 
  Serial.println("Uhr wurde angezeigt");

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
