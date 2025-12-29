#include "effects.h"
#include "show.h"
#include "globals.h"

void noeffect(){
  setanzeige();
  showmystrip();

}

void fadeefx(){
  for(int step=0;step<MATRIX_SIZE;step++){
    for(int row=0;row<MATRIX_SIZE; row++){
      for(int col=0;col<MATRIX_SIZE; col++){
        for(int i=0;i<3;i++){
          int k = anzeigealt[row][col][i]*(1-((1.0/MATRIX_SIZE)*step));
          anzeige[row][col][i]=k ;
        }
      }
    }
  showmystrip();
  delay(efxtime);
  }//down

  for(int step=0;step<MATRIX_SIZE;step++){

    for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
      if(matrixanzeige[row][col]==1){
        for(int i=0;i<3;i++){
          int k = vordergrund[row][col][i]*((1.0/MATRIX_SIZE)*step);
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
  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
        strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
    }
  }
  strip.show();

  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
        for(int i=0;i<3;i++){
        anzeigealt[row][col][i]=anzeige[row][col][i];
        }
      }
  }
}
void setanzeige(){
  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
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

  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
        if(row%2==0){
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
        }else{
          strip.setPixelColor(matrix[row][MATRIX_SIZE-1-col], strip.Color(anzeige[row][MATRIX_SIZE-1-col][0], anzeige[row][MATRIX_SIZE-1-col][1], anzeige[row][MATRIX_SIZE-1-col][2]));
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

  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
        if(row%2==0){
          strip.setPixelColor(matrix[row][col], strip.Color(255,255,255));
          strip.show();
          delay(efxtime);
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[row][col][0], anzeige[row][col][1], anzeige[row][col][2]));
        }else{
          strip.setPixelColor(matrix[row][MATRIX_SIZE-1-col], strip.Color(255,255,255));
          strip.show();
          delay(efxtime);
          strip.setPixelColor(matrix[row][MATRIX_SIZE-1-col], strip.Color(anzeige[row][MATRIX_SIZE-1-col][0], anzeige[row][MATRIX_SIZE-1-col][1], anzeige[row][MATRIX_SIZE-1-col][2]));
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
  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
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

  for(int step=1;step<MATRIX_SIZE+1; step++){

  for(int row=0;row<step; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
          strip.setPixelColor(matrix[row][col], strip.Color(anzeige[MATRIX_SIZE-(step-row)][col][0], anzeige[MATRIX_SIZE-(step-row)][col][1], anzeige[MATRIX_SIZE-(step-row)][col][2]));
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

  for(int step=1;step<(MATRIX_SIZE*2)+1; step++){
    int k = min (step, MATRIX_SIZE);
  for(int row=0;row<k; row++){
    int l = min(step-row,MATRIX_SIZE);
    for(int col=0;col<l; col++){
          int n = MATRIX_SIZE - l + col;
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

  for(int step=1;step<(MATRIX_SIZE*2)+1; step++){
    int k = min (step, MATRIX_SIZE);
  for(int row=0;row<k; row++){
    int l = min(step-row,MATRIX_SIZE);
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

  for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
        strip.setPixelColor(matrix[row][col], strip.Color(hintergrund[row][col][0], hintergrund[row][col][1], hintergrund[row][col][2]));
    }
  }//hintergrund wird angezeigt
  strip.show();
  delay(efxtime);
  //active initieren

  int active[MATRIX_SIZE];
  for(int i=0;i<MATRIX_SIZE;i++){
    active[i]=random(-30,-1);
  }

  for(int step=0;step<48;step++){
    for(int i=0;i<MATRIX_SIZE;i++){
    active[i]=active[i]+1;
  } // Zeile rutscht eins nach unten

  for(int col=0;col<MATRIX_SIZE;col++){ // die Spalten werden durchlaufen
    int pos=active[col];  //aktuelle position der Spalte = Zeile

    if(pos>=0 && pos<MATRIX_SIZE){
      strip.setPixelColor(matrix[pos][col], strip.Color(vordergrund[pos][col][0], vordergrund[pos][col][1], vordergrund[pos][col][2]));
    }

    for(int k=1;k<3;k++){
      int pos2=pos-k;
      if(pos2>=0 && pos2<MATRIX_SIZE){
        if(matrixanzeige[pos2][col]==0){
        strip.setPixelColor(matrix[pos2][col], strip.Color(vordergrund[pos2][col][0]*(1-(k*0.3)), vordergrund[pos2][col][1]*(1-(k*0.3)), vordergrund[pos2][col][2]*(1-(k*0.3))));
    }}
    }
    for(int m=3;m<6;m++){
      int pos3=pos-m;
      if(pos3>=0 && pos3<MATRIX_SIZE){
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

void spiral(){
  setanzeige();

  // Alle LEDs ausschalten
  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0,0,0));
  }
  strip.show();
  
  // Spiralreihenfolge für 11x11 oder 8x8 Matrix
  // Wir erstellen eine Liste von Koordinaten in Spiralreihenfolge von der Mitte aus
  int spiralCoords[MATRIX_SIZE * MATRIX_SIZE][2];
  int spiralIndex = 0;
  
  // Startposition: Zeile 4, Pos 4 (0-indiziert)
  int row = MATRIX_SIZE / 2;
  int col = MATRIX_SIZE / 2;
  
  // Startposition hinzufügen
  spiralCoords[spiralIndex][0] = row;
  spiralCoords[spiralIndex][1] = col;
  spiralIndex++;
  
  // Spirale aufbauen: links, hoch, rechts (2x), runter (2x), links (3x), hoch (3x), ...
  // Richtung: links, hoch, dann jeweils paarweise rechts/runter, links/hoch
  int steps = 1;
  
  while(spiralIndex < MATRIX_SIZE * MATRIX_SIZE) {
    // Nach links (steps mal)
    for(int i = 0; i < steps && spiralIndex < MATRIX_SIZE * MATRIX_SIZE; i++) {
      col--;
      if(row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        spiralCoords[spiralIndex][0] = row;
        spiralCoords[spiralIndex][1] = col;
        spiralIndex++;
      }
    }
    
    // Nach oben (steps mal)
    for(int i = 0; i < steps && spiralIndex < MATRIX_SIZE * MATRIX_SIZE; i++) {
      row--;
      if(row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        spiralCoords[spiralIndex][0] = row;
        spiralCoords[spiralIndex][1] = col;
        spiralIndex++;
      }
    }
    
    steps++;
    
    // Nach rechts (steps mal)
    for(int i = 0; i < steps && spiralIndex < MATRIX_SIZE * MATRIX_SIZE; i++) {
      col++;
      if(row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        spiralCoords[spiralIndex][0] = row;
        spiralCoords[spiralIndex][1] = col;
        spiralIndex++;
      }
    }
    
    // Nach unten (steps mal)
    for(int i = 0; i < steps && spiralIndex < MATRIX_SIZE * MATRIX_SIZE; i++) {
      row++;
      if(row >= 0 && row < MATRIX_SIZE && col >= 0 && col < MATRIX_SIZE) {
        spiralCoords[spiralIndex][0] = row;
        spiralCoords[spiralIndex][1] = col;
        spiralIndex++;
      }
    }
    
    steps++;
  }
  
  // Spirale durchlaufen und anzeigen
  for(int i = 0; i < spiralIndex; i++) {
    int r = spiralCoords[i][0];
    int c = spiralCoords[i][1];
    
    // Weißer Punkt an der aktuellen Position (Spitze)
    strip.setPixelColor(matrix[r][c], strip.Color(255, 255, 255));
    strip.show();
    delay(efxtime);
    
    // Aktuelle Position auf die endgültige Farbe setzen
    strip.setPixelColor(matrix[r][c], strip.Color(anzeige[r][c][0], anzeige[r][c][1], anzeige[r][c][2]));
  }
  
  strip.show();
}

void snakeeater(){
  // Aktuelle Anzeige vorbereiten (mit Hintergrund)
  vordergrunderstellen(vf1, vf2);
  hintergrunderstellen(hf1, hf2);
  
  // Schlangenlänge
  const int snakeLen = 5;
  int snake[snakeLen][2];
  
  // Startposition in der Mitte
  int centerRow = MATRIX_SIZE / 2;
  int centerCol = MATRIX_SIZE / 2;
  
  // Schlange initialisieren
  for(int i = 0; i < snakeLen; i++) {
    snake[i][0] = centerRow;
    snake[i][1] = centerCol;
  }
  
  // Speichere alte matrixanzeige (welche Buchstaben waren an)
  int oldMatrix[MATRIX_SIZE][MATRIX_SIZE];
  for(int row = 0; row < MATRIX_SIZE; row++) {
    for(int col = 0; col < MATRIX_SIZE; col++) {
      oldMatrix[row][col] = matrixanzeige[row][col];
    }
  }
  
  // Zeige alte Zeit mit Hintergrund
  for(int row = 0; row < MATRIX_SIZE; row++) {
    for(int col = 0; col < MATRIX_SIZE; col++) {
      if(oldMatrix[row][col] == 1) {
        strip.setPixelColor(matrix[row][col], strip.Color(vordergrund[row][col][0], vordergrund[row][col][1], vordergrund[row][col][2]));
      } else {
        strip.setPixelColor(matrix[row][col], strip.Color(hintergrund[row][col][0], hintergrund[row][col][1], hintergrund[row][col][2]));
      }
    }
  }
  strip.show();
  delay(efxtime);
  
  // Sammle alle Buchstaben-Positionen der alten Zeit (nur Vordergrund, max 50)
  int targets[50][2];
  int targetCount = 0;
  
  for(int row = 0; row < MATRIX_SIZE && targetCount < 50; row++) {
    for(int col = 0; col < MATRIX_SIZE && targetCount < 50; col++) {
      if(oldMatrix[row][col] == 1) {
        targets[targetCount][0] = row;
        targets[targetCount][1] = col;
        targetCount++;
      }
    }
  }
  
  // Phase 1: Alte Buchstaben auffressen (wenn es welche gibt)
  if(targetCount > 0) {
    bool visited[50];
    for(int i = 0; i < 50; i++) visited[i] = false;
    
    int currentRow = centerRow;
    int currentCol = centerCol;
    
    // Jeden Zielpixel besuchen
    for(int eaten = 0; eaten < targetCount; eaten++) {
      // Finde nächsten unbesuchten Pixel
      int nearest = -1;
      int minDist = 9999;
      for(int i = 0; i < targetCount; i++) {
        if(!visited[i]) {
          int dist = abs(targets[i][0] - currentRow) + abs(targets[i][1] - currentCol);
          if(dist < minDist) {
            minDist = dist;
            nearest = i;
          }
        }
      }
      
      if(nearest == -1) break;
      visited[nearest] = true;
      
      int targetRow = targets[nearest][0];
      int targetCol = targets[nearest][1];
      
      // Bewege Schlange Schritt für Schritt zum Ziel (mit Sicherheitslimit)
      int steps = 0;
      while((currentRow != targetRow || currentCol != targetCol) && steps < 100) {
        steps++;
        
        // Einen Schritt in Richtung Ziel
        if(currentRow < targetRow) currentRow++;
        else if(currentRow > targetRow) currentRow--;
        else if(currentCol < targetCol) currentCol++;
        else if(currentCol > targetCol) currentCol--;
        
        // Schlange nachziehen
        for(int i = snakeLen - 1; i > 0; i--) {
          snake[i][0] = snake[i-1][0];
          snake[i][1] = snake[i-1][1];
        }
        snake[0][0] = currentRow;
        snake[0][1] = currentCol;
        
        // Anzeige updaten
        for(int r = 0; r < MATRIX_SIZE; r++) {
          for(int c = 0; c < MATRIX_SIZE; c++) {
            // Prüfe ob Buchstabe schon gefressen
            bool eaten = false;
            for(int v = 0; v < targetCount; v++) {
              if(visited[v] && targets[v][0] == r && targets[v][1] == c) {
                eaten = true;
                break;
              }
            }
            
            // Wenn Buchstabe gefressen oder war nie Buchstabe: zeige Hintergrund
            if(eaten || oldMatrix[r][c] == 0) {
              strip.setPixelColor(matrix[r][c], strip.Color(hintergrund[r][c][0], hintergrund[r][c][1], hintergrund[r][c][2]));
            } else {
              // Noch nicht gefressen: zeige Vordergrund
              strip.setPixelColor(matrix[r][c], strip.Color(vordergrund[r][c][0], vordergrund[r][c][1], vordergrund[r][c][2]));
            }
          }
        }
        
        // Schlange zeichnen
        for(int i = 0; i < snakeLen; i++) {
          int bright = 255 - (i * 40);
          if(bright < 0) bright = 0;
          strip.setPixelColor(matrix[snake[i][0]][snake[i][1]], strip.Color(bright, bright, bright));
        }
        
        strip.show();
        delay(efxtime / 3);
        yield(); // Watchdog reset
      }
    }
  }
  
  // Alle Buchstaben sind gefressen - zeige nur Hintergrund
  for(int r = 0; r < MATRIX_SIZE; r++) {
    for(int c = 0; c < MATRIX_SIZE; c++) {
      strip.setPixelColor(matrix[r][c], strip.Color(hintergrund[r][c][0], hintergrund[r][c][1], hintergrund[r][c][2]));
    }
  }
  strip.show();
  delay(efxtime);
  
  // Neue Anzeige vorbereiten
  vordergrunderstellen(vf1, vf2);
  hintergrunderstellen(hf1, hf2);
  
  // Phase 2: Neue Buchstaben platzieren (nur Vordergrund der neuen Zeit, max 50)
  targetCount = 0;
  for(int row = 0; row < MATRIX_SIZE && targetCount < 50; row++) {
    for(int col = 0; col < MATRIX_SIZE && targetCount < 50; col++) {
      if(matrixanzeige[row][col] == 1) {
        targets[targetCount][0] = row;
        targets[targetCount][1] = col;
        targetCount++;
      }
    }
  }
  
  if(targetCount > 0) {
    bool visited[50];
    for(int i = 0; i < 50; i++) visited[i] = false;
    
    int currentRow = snake[0][0];
    int currentCol = snake[0][1];
    
    for(int placed = 0; placed < targetCount; placed++) {
      // Finde nächsten
      int nearest = -1;
      int minDist = 9999;
      for(int i = 0; i < targetCount; i++) {
        if(!visited[i]) {
          int dist = abs(targets[i][0] - currentRow) + abs(targets[i][1] - currentCol);
          if(dist < minDist) {
            minDist = dist;
            nearest = i;
          }
        }
      }
      
      if(nearest == -1) break;
      visited[nearest] = true;
      
      int targetRow = targets[nearest][0];
      int targetCol = targets[nearest][1];
      
      // Bewege Schlange (mit Sicherheitslimit)
      int steps = 0;
      while((currentRow != targetRow || currentCol != targetCol) && steps < 100) {
        steps++;
        
        if(currentRow < targetRow) currentRow++;
        else if(currentRow > targetRow) currentRow--;
        else if(currentCol < targetCol) currentCol++;
        else if(currentCol > targetCol) currentCol--;
        
        for(int i = snakeLen - 1; i > 0; i--) {
          snake[i][0] = snake[i-1][0];
          snake[i][1] = snake[i-1][1];
        }
        snake[0][0] = currentRow;
        snake[0][1] = currentCol;
        
        // Anzeige updaten
        for(int r = 0; r < MATRIX_SIZE; r++) {
          for(int c = 0; c < MATRIX_SIZE; c++) {
            bool placed = false;
            for(int v = 0; v < targetCount; v++) {
              if(visited[v] && targets[v][0] == r && targets[v][1] == c) {
                placed = true;
                break;
              }
            }
            
            if(placed) {
              // Buchstabe platziert: zeige Vordergrund
              strip.setPixelColor(matrix[r][c], strip.Color(vordergrund[r][c][0], vordergrund[r][c][1], vordergrund[r][c][2]));
            } else {
              // Noch nicht platziert: zeige Hintergrund
              strip.setPixelColor(matrix[r][c], strip.Color(hintergrund[r][c][0], hintergrund[r][c][1], hintergrund[r][c][2]));
            }
          }
        }
        
        // Schlange zeichnen
        for(int i = 0; i < snakeLen; i++) {
          int bright = 255 - (i * 40);
          if(bright < 0) bright = 0;
          strip.setPixelColor(matrix[snake[i][0]][snake[i][1]], strip.Color(bright, bright, bright));
        }
        
        strip.show();
        delay(efxtime / 3);
        yield(); // Watchdog reset
      }
    }
  }
  
  // Finale Anzeige
  showmystrip();
}
