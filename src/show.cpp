#include "show.h"
#include "globals.h"
#include "color.h"
#include <EEPROM.h>
#include "effects.h"

void readTime(){
  readTimeNet();
  //readTimeRCT();
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
    EEPROM.get(sizeof(struct settings), user_color );
    vordergrundschema = user_color.vs;
    gebstat=0;
  }
  vordergrunderstellen(vf1,vf2);
  hintergrunderstellen(hf1,hf2);
  }


  int welchereffekt = 0;
  if(effectMode==1){
    welchereffekt= random(2,9);
  }else{
    welchereffekt=effectMode;
  }


  if(on==0 && nacht==0){
    for(int row=0; row<11; row++){
     for(int col=0; col<11; col++) {
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
  for(int row=0;row<11;row++){
    for(int col=0;col<11;col++){
      matrixanzeige[row][col]=0;
    }
  } // matrixanzeige leeren

    for (int i=0; i<2; i++){
      matrixanzeige[0][i]=1;
  }  //ES
   for (int i=3; i<iist; i++){
      matrixanzeige[0][i]=1;
  }  //ist oder is (bayrisch)

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

  
  for(int i=matrixminmap[mb][1]; i<matrixminmap[mb][2]+1; i++){
     matrixanzeige[matrixminmap[mb][0]][i]=1;
  } // zeige Minuten (Zahl)
  for(int i=matrixminmap[mb][4]; i<matrixminmap[mb][5]+1; i++){
     matrixanzeige[matrixminmap[mb][3]][i]=1;
  } // zeige Minuten (vor nach)
  for(int i=matrixminmap[mb][7]; i<matrixminmap[mb][8]+1; i++){
     matrixanzeige[matrixminmap[mb][6]][i]=1;
  } // zeige Minuten (halb)

  if(mb>=nexthour){
    h++;
  } // nächste Stunde ab Halb
  h = h%12; // 12 wird zu 0

  for(int i=matrixstundenmap[h][1]; i<matrixstundenmap[h][2]+1; i++){
    matrixanzeige[matrixstundenmap[h][0]][i]=1;
    // zeige Stunden
  }

  if (dbv==0 && uvv > 0 && h==1 && mb==0){
   matrixanzeige[5][3]=0;
  }// aus Eins Uhr wird Ein Uhr
  
  Serial.println("DEBUG MODULO:");
  for (int i=0; i<m; i++){
    matrixanzeige[10][matrixminmodulomap[i]] = 1;
    Serial.print("Modulo ");
    Serial.print(i);
    Serial.print(" LED ");
    Serial.println(matrixminmodulomap[i]);
  }  //Modulominitues


  

}
