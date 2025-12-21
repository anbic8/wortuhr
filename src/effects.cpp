#include "effects.h"
#include "show.h"
#include "globals.h"

void noeffect(){
  setanzeige();
  showmystrip();

}

void fadeefx(){
  for(int step=0;step<11;step++){
    for(int row=0;row<11; row++){
      for(int col=0;col<11; col++){
        for(int i=0;i<3;i++){
          int k = anzeigealt[row][col][i]*(1-(0.1*step));
          anzeige[row][col][i]=k ;
        }
      }
    }
  showmystrip();
  delay(efxtime);
  }//down

  for(int step=0;step<11;step++){

    for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
      if(matrixanzeige[row][col]==1){
        for(int i=0;i<3;i++){
          int k = vordergrund[row][col][i]*((0.1*step));
        anzeige[row][col][i]=k ;
        }
      }else{
        for(int i=0;i<3;i++){
          int k = hintergrund[row][col][i]*((0.1*step));
        anzeige[row][col][i]= k;
        }
      }

    }
  }
  showmystrip();
  delay(efxtime);
  }//up

}

void showmystrip(){
  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
        strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
    }
  }
  strip.show();

  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
        for(int i=0;i<3;i++){
        anzeigealt[row][col][i]=anzeige[row][col][i];
        }
      }
  }
}
void setanzeige(){
  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
      if(matrixanzeige[row][col]==1){
        for(int i=0;i<3;i++){
        anzeige[row][col][i]=vordergrund[row][col][i];
        }
      }else{
        for(int i=0;i<3;i++){
        anzeige[row][col][i]=hintergrund[row][col][i];
        }
      }

    }
  }
}

void running(){
  setanzeige();

  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  } // alle Lichter aus

  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
        if(row%2==0){
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
        }else{
          strip.setPixelColor(matrix[row][10-col], strip.Color(anzeige[row][10-col][0], anzeige[row][10-col][1], anzeige[row][10-col][2]));
        }
        strip.show();
        delay(efxtime);
    }
  }
  
}
void schlange(){
  setanzeige();

  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  } // alle Lichter aus

  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
        if(row%2==0){
          strip.setPixelColor(matrix[row][col], strip.Color(255,255,255));
          strip.show();
          delay(efxtime);
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
        }else{
          strip.setPixelColor(matrix[row][10-col], strip.Color(255,255,255));
          strip.show();
          delay(efxtime);
          strip.setPixelColor(matrix[row][10-col], strip.Color(anzeige[row][10-col][0], anzeige[row][10-col][1], anzeige[row][10-col][2]));
        }
        
    }
  }
  delay(efxtime);
  strip.show();
}

void zeilenefx(){
  setanzeige();

  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  } // alle Lichter aus
  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
    }
    strip.show();
    delay(efxtime);
  }


}

void scrollall(){
  setanzeige();

  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  } // alle Lichter aus

  for(int step=1;step<12; step++){

  for(int row=0;row<step; row++){
    for(int col=0;col<11; col++){
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[11-(step-row)][col][0], anzeige[11-(step-row)][col][1], anzeige[11-(step-row)][col][2]));
    }
  }
  strip.show();
    delay(efxtime);
  }

}

void slidein(){
  setanzeige();

  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  } // alle Lichter aus

  for(int step=1;step<23; step++){
    int k = min (step, 11);
  for(int row=0;row<k; row++){
    int l = min(step-row,11);
    for(int col=0;col<l; col++){
          int n = 11 - l + col;
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][n][0], anzeige[row][n][1], anzeige[row][n][2]));
    }
  }
  strip.show();
    delay(efxtime);
  }

}

void diagonal(){
  vordergrunderstellen(vf1,vf2);
  hintergrunderstellen(hf1,hf2);
  setanzeige();

  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  } // alle Lichter aus

  for(int step=1;step<23; step++){
    int k = min (step, 11);
  for(int row=0;row<k; row++){
    int l = min(step-row,11);
    for(int col=0;col<l; col++){
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
    }
  }
  strip.show();
    delay(efxtime);
  }

}

void rain(){
  vordergrunderstellen(vf1,vf2);
  hintergrunderstellen(hf1,hf2);

  for(int row=0;row<11; row++){
    for(int col=0;col<11; col++){
        strip.setPixelColor(matrix[row][col], strip.Color(hintergrund[row][col][0], hintergrund[row][col][1], hintergrund[row][col][2]));
    }
  }//hintergrund wird angezeigt
  strip.show();
  delay(efxtime);
  //active initieren

  int active[11];
  for(int i=0;i<11;i++){
    active[i]=random(-30,-1);
  }

  for(int step=0;step<48;step++){
    for(int i=0;i<11;i++){
    active[i]=active[i]+1;
  } // Zeile rutscht eins nach unten

  for(int col=0;col<11;col++){ // die Spalten werden durchlaufen
    int pos=active[col];  //aktuelle position der Spalte = Zeile

    if(pos>=0 && pos<11){
      strip.setPixelColor(matrix[pos][col], strip.Color(vordergrund[pos][col][0], vordergrund[pos][col][1], vordergrund[pos][col][2]));
    }

    for(int k=1;k<3;k++){
      int pos2=pos-k;
      if(pos2>=0 && pos2<11){
        if(matrixanzeige[pos2][col]==0){
        strip.setPixelColor(matrix[pos2][col], strip.Color(vordergrund[pos2][col][0]*(1-(k*0.3)), vordergrund[pos2][col][1]*(1-(k*0.3)), vordergrund[pos2][col][2]*(1-(k*0.3))));
    }}
    }
    for(int m=3;m<6;m++){
      int pos3=pos-m;
      if(pos3>=0 && pos3<11){
        if(matrixanzeige[pos3][col]==0){
        strip.setPixelColor(matrix[pos3][col], strip.Color(hintergrund[pos3][col][0]*(1-((5-m)*0.3)), hintergrund[pos3][col][1]*(1-((5-m)*0.3)), hintergrund[pos3][col][2]*(1-((5-m)*0.3))));
    }}
    }
  }

  strip.show();
  delay(efxtime);
  }
  setanzeige();
}
