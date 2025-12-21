#include "color.h"
#include "globals.h"
#include "show.h"
#include <EEPROM.h>


void vordergrunderstellen(int farbe1[3], int farbe2[3]){
switch (vordergrundschema) {
    case 1:
       schachbrett(1, farbe1, farbe2);
      break;
    case 2:
      spalten(1, farbe1, farbe2);
      break;
    case 3:
      zeilen(1, farbe1, farbe2);
      break;
    case 4:
      fade(1, farbe1, farbe2);
      break;
    case 5:
      rainbow(1);
      break;  
    default:
      einfarbig(1, farbe1);
      break;
  }

 
}

void hintergrunderstellen(int farbe1[3], int farbe2[3]){
  switch (hintergrundschema) {
    case 1:
      schachbrett(0, farbe1, farbe2);
      break;
    case 2:
      spalten(0, farbe1, farbe2);
      break;
    case 3:
      zeilen(0, farbe1, farbe2);
      break;
    case 4:
      fade(0, farbe1, farbe2);
      break;
    case 5:
      rainbow(0);
      break;  
    default:
      einfarbig(0, farbe1);
      break;
  }
  
   
  }
  


void neuefarbe(){

 EEPROM.get(sizeof(struct settings), user_color );
  
  if(user_color.vf1>-1){
  v1 = user_color.vf1;
  v2 = user_color.vf2;
  h1 = user_color.hf1;
  h2 = user_color.hf2;
  vordergrundschema = user_color.vs;
  hintergrundschema = user_color.hs;
  effectMode = user_color.efx;
  efxtimeint = user_color.efxtime;
  efxtime = htmlefxtimeint[efxtimeint];
  aniMode = user_color.ani;
  anitimeint = user_color.anitime;
  anitime= htmlanitimeint[aniMode][anitimeint];
  anidepth = user_color.anidepth;
  }

    vf1[0]=farben[v1][0];
  vf1[1]=farben[v1][1];
  vf1[2]=farben[v1][2];
  vf2[0]=farben[v2][0];
  vf2[1]=farben[v2][1];
  vf2[2]=farben[v2][2];
   hf1[0]=farben[h1][0];
  hf1[1]=farben[h1][1];
  hf1[2]=farben[h1][2];
    hf2[0]=farben[h2][0];
  hf2[1]=farben[h2][1];
  hf2[2]=farben[h2][2];
    hintergrunderstellen(hf1, hf2);
    vordergrunderstellen(vf1, vf2);
    readTime();

    //readTimeRCT();
  showClock();
  //sendJsonMessage();
}

void savecolor(){
  MyColor customVar = {
    v1,
    v2,
    vordergrundschema,
    h1,
    h2,
    hintergrundschema,
    effectMode,
    efxtimeint,
    aniMode,
    anitimeint,
    anidepth
  };
  EEPROM.put(sizeof(struct settings), customVar);
  EEPROM.commit();
}

void einfarbig(int m, int farbe[3] ){
  if(m==1){
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
        vordergrund[row][col][0]=farbe[0];
        vordergrund[row][col][1]=farbe[1];
        vordergrund[row][col][2]=farbe[2];
      }
    }
  }else{
    for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
        hintergrund[row][col][0]=farbe[0];
        hintergrund[row][col][1]=farbe[1];
        hintergrund[row][col][2]=farbe[2];
      }
    }
  }
}

void schachbrett(int m, int farbe1[3], int farbe2[3]){
  if(m==1){
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
        if(row%2==0){
          if(col%2==0){
          vordergrund[row][col][0]=farbe1[0];
          vordergrund[row][col][1]=farbe1[1];
          vordergrund[row][col][2]=farbe1[2];
          }else{
          vordergrund[row][col][0]=farbe2[0];
          vordergrund[row][col][1]=farbe2[1];
          vordergrund[row][col][2]=farbe2[2];
          }
        }else{
          if(col%2==1){
          vordergrund[row][col][0]=farbe1[0];
          vordergrund[row][col][1]=farbe1[1];
          vordergrund[row][col][2]=farbe1[2];
          }else{
          vordergrund[row][col][0]=farbe2[0];
          vordergrund[row][col][1]=farbe2[1];
          vordergrund[row][col][2]=farbe2[2];
          }
        }
      }
    }
  }else{
    {
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
        if(row%2==0){
          if(col%2==0){
          hintergrund[row][col][0]=farbe1[0];
          hintergrund[row][col][1]=farbe1[1];
          hintergrund[row][col][2]=farbe1[2];
          }else{
          hintergrund[row][col][0]=farbe2[0];
          hintergrund[row][col][1]=farbe2[1];
          hintergrund[row][col][2]=farbe2[2];
          }
        }else{
          if(col%2==1){
          hintergrund[row][col][0]=farbe1[0];
          hintergrund[row][col][1]=farbe1[1];
          hintergrund[row][col][2]=farbe1[2];
          }else{
          hintergrund[row][col][0]=farbe2[0];
          hintergrund[row][col][1]=farbe2[1];
          hintergrund[row][col][2]=farbe2[2];
          }
        }
      }
    }
  }
  }
}

void spalten(int m, int farbe1[3], int farbe2[3]){
  if(m==1){
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
         if(col%2==0){
          vordergrund[row][col][0]=farbe1[0];
          vordergrund[row][col][1]=farbe1[1];
          vordergrund[row][col][2]=farbe1[2];
          }else{
          vordergrund[row][col][0]=farbe2[0];
          vordergrund[row][col][1]=farbe2[1];
          vordergrund[row][col][2]=farbe2[2];
          }
      }
  }
  }else{
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
         if(col%2==0){
          hintergrund[row][col][0]=farbe1[0];
          hintergrund[row][col][1]=farbe1[1];
          hintergrund[row][col][2]=farbe1[2];
          }else{
          hintergrund[row][col][0]=farbe2[0];
          hintergrund[row][col][1]=farbe2[1];
          hintergrund[row][col][2]=farbe2[2];
          }
      }
  }
  }

}

void zeilen(int m, int farbe1[3], int farbe2[3]){
  if(m==1){
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
         if(row%2==0){
          vordergrund[row][col][0]=farbe1[0];
          vordergrund[row][col][1]=farbe1[1];
          vordergrund[row][col][2]=farbe1[2];
          }else{
          vordergrund[row][col][0]=farbe2[0];
          vordergrund[row][col][1]=farbe2[1];
          vordergrund[row][col][2]=farbe2[2];
          }
      }
  }
  }else{
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
         if(row%2==0){
          hintergrund[row][col][0]=farbe1[0];
          hintergrund[row][col][1]=farbe1[1];
          hintergrund[row][col][2]=farbe1[2];
          }else{
          hintergrund[row][col][0]=farbe2[0];
          hintergrund[row][col][1]=farbe2[1];
          hintergrund[row][col][2]=farbe2[2];
          }
      }
  }
  }

}

void fade(int m, int farbe1[3], int farbe2[3]){
  if(m==1){
    for(int row=0;row<11;row++){
      int r=farbe1[0]+((farbe2[0]-farbe1[0])*row/10);
      int g=farbe1[1]+((farbe2[1]-farbe1[1])*row/10);
      int b=farbe1[2]+((farbe2[2]-farbe1[2])*row/10);
      for(int col=0;col<11;col++){
        vordergrund[row][col][0]=r;
        vordergrund[row][col][1]=g;
        vordergrund[row][col][2]=b;

      }
  }
  }else{
    for(int row=0;row<11;row++){
      int r=farbe1[0]+((farbe2[0]-farbe1[0])*row/10);
      int g=farbe1[1]+((farbe2[1]-farbe1[1])*row/10);
      int b=farbe1[2]+((farbe2[2]-farbe1[2])*row/10);
      for(int col=0;col<11;col++){
        hintergrund[row][col][0]=r;
        hintergrund[row][col][1]=g;
        hintergrund[row][col][2]=b;

      }
  }
  }
}

void rainbow(int m ){
  if(m==1){
  for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
        int k = random(0, 12);
        vordergrund[row][col][0]=farben[k][0];
        vordergrund[row][col][1]=farben[k][1];
        vordergrund[row][col][2]=farben[k][2];
      }
    }
  }else{
    for(int row=0;row<11;row++){
      for(int col=0;col<11;col++){
        int k = random(0, 12);
        hintergrund[row][col][0]=farben[k][0];
        hintergrund[row][col][1]=farben[k][1];
        hintergrund[row][col][2]=farben[k][2];
      }
    }
  }
}