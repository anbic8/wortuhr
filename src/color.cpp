#include "color.h"
#include "globals.h"
#include "show.h"
#include <EEPROM.h>


// helper to write a color into the target matrix (m==1 -> vordergrund, else -> hintergrund)
static inline void setMatrixCell(int m, int row, int col, int r, int g, int b) {
  if (m == 1) {
    vordergrund[row][col][0] = r;
    vordergrund[row][col][1] = g;
    vordergrund[row][col][2] = b;
  } else {
    hintergrund[row][col][0] = r;
    hintergrund[row][col][1] = g;
    hintergrund[row][col][2] = b;
  }
}


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

 EEPROM.get(sizeof(settings), user_color );
  
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
  // load palette colors from PROGMEM
  getPaletteColor((uint8_t)v1, vf1);
  getPaletteColor((uint8_t)v2, vf2);
  getPaletteColor((uint8_t)h1, hf1);
  getPaletteColor((uint8_t)h2, hf2);
    hintergrunderstellen(hf1, hf2);
    vordergrunderstellen(vf1, vf2);
    readTime();
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
  EEPROM.put(sizeof(settings), customVar);
  EEPROM.commit();
}

void einfarbig(int m, int farbe[3] ){
  for(int row=0; row<MATRIX_SIZE; ++row){
    for(int col=0; col<MATRIX_SIZE; ++col){
      setMatrixCell(m, row, col, farbe[0], farbe[1], farbe[2]);
    }
  }
}

void schachbrett(int m, int farbe1[3], int farbe2[3]){
  for(int row=0; row<MATRIX_SIZE; ++row){
    for(int col=0; col<MATRIX_SIZE; ++col){
      bool use1 = ((row % 2 == 0) && (col % 2 == 0)) || ((row % 2 == 1) && (col % 2 == 1));
      int *c = use1 ? farbe1 : farbe2;
      setMatrixCell(m, row, col, c[0], c[1], c[2]);
    }
  }
}

void spalten(int m, int farbe1[3], int farbe2[3]){
  for(int row=0; row<MATRIX_SIZE; ++row){
    for(int col=0; col<MATRIX_SIZE; ++col){
      int *c = (col % 2 == 0) ? farbe1 : farbe2;
      setMatrixCell(m, row, col, c[0], c[1], c[2]);
    }
  }

}

void zeilen(int m, int farbe1[3], int farbe2[3]){
  for(int row=0; row<MATRIX_SIZE; ++row){
    for(int col=0; col<MATRIX_SIZE; ++col){
      int *c = (row % 2 == 0) ? farbe1 : farbe2;
      setMatrixCell(m, row, col, c[0], c[1], c[2]);
    }
  }

}

void fade(int m, int farbe1[3], int farbe2[3]){
  for(int row=0; row<MATRIX_SIZE; ++row){
    int r = farbe1[0] + ((farbe2[0] - farbe1[0]) * row / (MATRIX_SIZE-1));
    int g = farbe1[1] + ((farbe2[1] - farbe1[1]) * row / (MATRIX_SIZE-1));
    int b = farbe1[2] + ((farbe2[2] - farbe1[2]) * row / (MATRIX_SIZE-1));
    for(int col=0; col<MATRIX_SIZE; ++col){
      setMatrixCell(m, row, col, r, g, b);
    }
  }
}

void rainbow(int m ){
  for(int row=0; row<MATRIX_SIZE; ++row){
    for(int col=0; col<MATRIX_SIZE; ++col){
      int k = random(0, anzahlfarben);
      int rgb[3];
      getPaletteColor((uint8_t)k, rgb);
      setMatrixCell(m, row, col, rgb[0], rgb[1], rgb[2]);
    }
  }
}