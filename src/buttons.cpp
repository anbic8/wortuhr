#include "buttons.h"
#include "globals.h"
#include "show.h"
#include "effects.h"



void bt1click(){
  if(mode==1){
    //Dimmfaktordurchspielen
    dimm=dimm+50;
    if(dimm>255){
      dimm=50;
    }
    strip.setBrightness(dimm);
    strip.show();
    /* //RTC-Modus
    if(settimemode==0){
      dimm=dimm+50;
    if(dimm>255){
      dimm=0;
    }
    }
    if(settimemode==1){
      stunden=stunden-1;
      showwhilesetting();
    }
    if(settimemode==2){
      minutes = minutes - 1;
      if(minutes==-1){
        minutes==59;
      }
      showwhilesetting();
    }
    */

    
  }else{
    stunden=stunden+1;
    stunden=stunden%24;
    showClock();
  }
}
void bt1double(){
 if(mode==1){
  
}
}
void bt1longs(){
  if(mode==1){
  //nachmodus aktivieren
  mqttonset=1;
  if(on==0){
    mqtton=1;
    }else{
      mqtton=0;
    }
}
readTime();
showClock();
}
void bt2click(){
  if(mode==1){

    /* //RTC-Modus
    if(settimemode==0){
      // Sommer/ Winterzeit hin und her
      if(sommerzeit==0){
        stunden = stunden +1;
        sommerzeit=1;
          
  }else{
      stunden = stunden-1;
      sommerzeit=0;
  }
  void setDate(seconds, minutes, stunden, day, month, year );

      design customDesign = {
    server.arg("db").toInt(),
    server.arg("dv").toInt(),
    server.arg("uv").toInt(),
    anp,
    ausp,
    server.arg("nacht").toInt(),
    sommerzeit,
    dimm
  };

  EEPROM.put(sizeof(settings)+sizeof(MyColor), customDesign);
  EEPROM.commit();
  }

    }
    if(settimemode==1){
      stunden = stunden + 1;
      showwhilesetting();
    }
    if(settimemode==2){
      minutes = minutes+1;
      showwhilesetting();
    }
    */
     
  }else{
    minutes=minutes+5;
    minutes=minutes%60;
    showClock();
  }
}
void bt2double(){
  if(mode==1){
  
  }
}
void bt2longs(){
  if(mode==1){
    /*RTC Mode
    if(settimemode<3){
    settimemode++;
    }
    if(settimemode==3){
      settimemode=0;
      void setDate(seconds, minutes, stunden, day, month, year );
      }
    */
  }
}

void showwhilesetting(){
  hours = stunden%12;
  minutes=minutes%60;
      mb = int(minutes/5);
       h = hours;
     m = minutes%5;
  setmatrixanzeige();
      showmystrip();
}