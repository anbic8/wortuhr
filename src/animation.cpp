#include "animation.h"


void animationen(){
  if(on==0 && nacht==0){

  }else{
  if(millis()>letzterani+anitime){
    letzterani=millis();

    switch (aniMode) {
    case 1:
        blinken();
      break;
    case 2:
        vordergrundblinken();
      break;
    case 3:
        pulsieren();
      break;
    case 4:
        anifade();
      break;
    case 5:
        fly();
      break;  
    case 6:
        glitter();
      break; 
    case 7:
        //diagonal();
      break;
    default:
      //noeffect();
      break;
  }

  }
  }
}

void blinken(){
  if(anistate==0){
    anistate=1;
    vordergrunderstellen(vf1, vf2);
    hintergrunderstellen(hf1, hf2);
  }else{
    anistate=0;
    vordergrunderstellen(vf2, vf1);
    hintergrunderstellen(hf2, hf1);
  }
  setanzeige();
  showmystrip();
}

void vordergrundblinken(){
  if(anistate==0){
    anistate=1;
    vordergrunderstellen(vf1, vf2);
  }else{
    anistate=0;
    vordergrunderstellen(vf2, vf1);
  }
  setanzeige();
  showmystrip();
}

void pulsieren(){
  int grenze = 0;
  if (anidepth==0){
    grenze=20;
  }
  if (anidepth==1){
    grenze=30;
  }
  if (anidepth==2){
    grenze=50;
  }

  int faktor = 0;
  if(anistate<grenze){
    anistate++;
    faktor = 50-anistate;

  }else{
    anistate++;
    faktor= 50-grenze+(anistate-grenze);
  }
  if(anistate==grenze*2){
    anistate=0;
  }
    

   for(int row=0;row<MATRIX_SIZE; row++){
    for(int col=0;col<MATRIX_SIZE; col++){
        int r = anzeige[row][col][0]*0.02*faktor;
        int g = anzeige[row][col][1]*0.02*faktor;
        int b = anzeige[row][col][2]*0.02*faktor;
        strip.setPixelColor(matrix[row][col], strip.Color(r , g, b));
    }
    yield(); // Prevent watchdog reset
  }
  strip.show();
}

void anifade(){
  int grenze = 50;
  
  int faktor = 0;
  if(anistate<grenze){
    anistate++;
    faktor = anistate;

  }else{
    anistate++;
    faktor= grenze-(anistate-grenze);
  }
  if(anistate==grenze*2){
    anistate=0;
  }
    int r = vf1[0]+(vf2[0]-vf1[0])*0.02*faktor;
    int g = vf1[1]+(vf2[1]-vf1[1])*0.02*faktor;
    int b = vf1[2]+(vf2[2]-vf1[2])*0.02*faktor;

    int tempvf1[3]={r,g,b};

    r = vf2[0]+(vf1[0]-vf2[0])*0.02*faktor;
    g = vf2[1]+(vf1[1]-vf2[1])*0.02*faktor;
    b = vf2[2]+(vf1[2]-vf2[2])*0.02*faktor;

    int tempvf2[3]={r,g,b};

    vordergrunderstellen(tempvf1, tempvf2);

     r = hf1[0]+(hf2[0]-hf1[0])*0.02*faktor;
    g = hf1[1]+(hf2[1]-hf1[1])*0.02*faktor;
    b = hf1[2]+(hf2[2]-hf1[2])*0.02*faktor;

    int temphf1[3]={r,g,b};

     r = hf2[0]+(hf1[0]-hf2[0])*0.02*faktor;
    g = hf2[1]+(hf1[1]-hf2[1])*0.02*faktor;
    b = hf2[2]+(hf1[2]-hf2[2])*0.02*faktor;

    int temphf2[3]={r,g,b};

    hintergrunderstellen(temphf1, temphf2);
  setanzeige();
  showmystrip();
}

void fly(){

  for(int i=0;i<anidepth+1;i++){

  int c = random(-1,2);
  int r = random(-1,2);
  int ralt = flypos[i][0];
  int calt = flypos[i][1];

  if(matrixanzeige[ralt][calt]==1){
      strip.setPixelColor(matrix[ralt][calt], strip.Color(vordergrund[ralt][calt][0], vordergrund[ralt][calt][1], vordergrund[ralt][calt][2]));
    }else{
      strip.setPixelColor(matrix[ralt][calt], strip.Color(hintergrund[ralt][calt][0], hintergrund[ralt][calt][1], hintergrund[ralt][calt][2]));}//alt auf normal
  
  int rneu=ralt+r;
  int cneu=calt+c;

  rneu=max(0, rneu);
  rneu=min(10, rneu);
  
  cneu=max(0, cneu);
  cneu=min(10, cneu);


    strip.setPixelColor(matrix[rneu][cneu], strip.Color(125,125,125)); //fliege gesetzt
flypos[i][1]=cneu;
  flypos[i][0]=rneu;
  yield(); // Prevent watchdog reset
  }
    strip.show();

  
  
};

void glitter(){
  // Reduced counts for schwach/mittel/stark and sequential activation
  int targetCount;
  if (anidepth == 0) targetCount = 1; // schwach
  else if (anidepth == 1) targetCount = 2; // mittel
  else targetCount = 4; // stark

  // persistent state across calls
  static bool initialized = false;
  static int poolR[LED_COUNT];
  static int poolC[LED_COUNT];
  static int poolSize = 0;
  static int fade[LED_COUNT];
  static int origR[LED_COUNT];
  static int origG[LED_COUNT];
  static int origB[LED_COUNT];

  const int MAX_FADE = 120; // larger => slower fade
  const int FADE_STEP = 6;  // decrement per animation step

  // initialize pool of foreground pixels once per activation of this animation
  if (!initialized) {
    poolSize = 0;
    for (int row = 0; row < MATRIX_SIZE; row++){
      for (int col = 0; col < MATRIX_SIZE; col++){
        if (matrixanzeige[row][col] == 1) {
          poolR[poolSize] = row;
          poolC[poolSize] = col;
          poolSize++;
        }
      }
    }
    // shuffle pool
    for (int i = 0; i < poolSize; i++){
      int j = random(i, poolSize);
      int tr = poolR[i]; int tc = poolC[i];
      poolR[i] = poolR[j]; poolC[i] = poolC[j];
      poolR[j] = tr; poolC[j] = tc;
    }
    for (int i=0;i<poolSize;i++){
      fade[i] = 0;
      origR[i] = vordergrund[poolR[i]][poolC[i]][0];
      origG[i] = vordergrund[poolR[i]][poolC[i]][1];
      origB[i] = vordergrund[poolR[i]][poolC[i]][2];
    }
    initialized = true;
  }

  if (poolSize == 0) return; // nothing to do

  // ensure targetCount doesn't exceed available pool
  if (targetCount > poolSize) targetCount = poolSize;

  // count currently active (fade>0)
  int activeNow = 0;
  for (int i=0;i<poolSize;i++) if (fade[i] > 0) activeNow++;

  // If we can add one more active pixel, pick a random pool entry with fade==0
  if (activeNow < targetCount) {
    int candidates[LED_COUNT]; int cand = 0;
    for (int i=0;i<poolSize;i++) if (fade[i] == 0) { candidates[cand++] = i; }
    if (cand > 0) {
      int pick = candidates[random(0, cand)];
      fade[pick] = MAX_FADE; // start fully white
      // refresh original color from current vordergrund (in case it changed)
      origR[pick] = vordergrund[poolR[pick]][poolC[pick]][0];
      origG[pick] = vordergrund[poolR[pick]][poolC[pick]][1];
      origB[pick] = vordergrund[poolR[pick]][poolC[pick]][2];
      strip.setPixelColor(matrix[poolR[pick]][poolC[pick]], strip.Color(255,255,255));
    }
  }

  // update fades and draw
  for (int i=0;i<poolSize;i++){
    if (fade[i] > 0){
      float factor = (float)fade[i] / (float)MAX_FADE; // 1.0 -> white, 0 -> original
      int rr = origR[i] + (int)((255 - origR[i]) * factor);
      int gg = origG[i] + (int)((255 - origG[i]) * factor);
      int bb = origB[i] + (int)((255 - origB[i]) * factor);
      strip.setPixelColor(matrix[poolR[i]][poolC[i]], strip.Color(rr, gg, bb));
      fade[i] -= FADE_STEP;
      if (fade[i] <= 0){
        fade[i] = 0;
        strip.setPixelColor(matrix[poolR[i]][poolC[i]], strip.Color(origR[i], origG[i], origB[i]));
      }
    }
  }
  strip.show();
  // if animation is left (aniMode changed), next call will reinit - reset when leaving handled elsewhere
}
void startup(){
#if MATRIX_SIZE == 11
  for(int s=0;s<9;s++){
  for(int i=0;i<11;i++){
    strip.setPixelColor(t1[i], strip.Color(startcolors[8-s][0], startcolors[8-s][1], startcolors[8-s][2] ));
  } //t1
  for(int i=0;i<28;i++){
    strip.setPixelColor(t2[i], strip.Color(startcolors[9-s][0], startcolors[9-s][1], startcolors[9-s][2] ));
  } //t2
  for(int i=0;i<36;i++){
    strip.setPixelColor(t3[i], strip.Color(startcolors[10-s][0], startcolors[10-s][1], startcolors[10-s][2] ));
  } //t1
  for(int i=0;i<26;i++){
    strip.setPixelColor(t4[i], strip.Color(startcolors[11-s][0], startcolors[11-s][1], startcolors[11-s][2] ));
  } //t1
  for(int i=0;i<12;i++){
    strip.setPixelColor(t5[i], strip.Color(startcolors[12-s][0], startcolors[12-s][1], startcolors[12-s][2] ));
  } //t1
  strip.show();
  delay(500);
  yield(); // Prevent watchdog reset during startup animation
  }
#else
  // Simple startup animation for 8x8 matrix
  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }
  strip.show();
  delay(500);
  for(int i=0;i<LED_COUNT;i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.show();
#endif
}