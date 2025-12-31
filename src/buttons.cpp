#include "buttons.h"
#include "globals.h"
#include "show.h"
#include "effects.h"
#ifdef USE_RCT
  #include "rct.h"
#endif


void bt1click(){
  if(mode==1){
    #ifdef USE_RCT
      // RTC-Modus: Zeit manuell einstellen
      if(settimemode==0){
        dimm=dimm+50;
        if(dimm>255){
          dimm=50;
        }
        strip.setBrightness(dimm);
        strip.show();
      }
      if(settimemode==1){
        stunden=stunden-1;
        if(stunden<0){
          stunden=23;
        }
        showwhilesetting();
      }
      if(settimemode==2){
        minutes = minutes - 1;
        if(minutes<0){
          minutes=59;
        }
        showwhilesetting();
      }
    #else
      // NTP-Modus: Nur Helligkeit anpassen
      dimm=dimm+50;
      if(dimm>255){
        dimm=50;
      }
      strip.setBrightness(dimm);
      strip.show();
    #endif

    
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
    #ifdef USE_RCT
      // RTC-Modus: Zeit einstellen
      if(settimemode==0){
        // Sommer/ Winterzeit hin und her
        if(sommerzeit==0){
          stunden = stunden +1;
          sommerzeit=1;
        }else{
          stunden = stunden-1;
          sommerzeit=0;
        }
        setDate(seconds, minutes, stunden, day, month, year);

        design customDesign = {
          dbv,
          dvv,
          uvv,
          an,
          aus,
          nacht,
          sommerzeit,
          dimm
        };

        EEPROM.put(sizeof(settings)+sizeof(MyColor), customDesign);
        EEPROM.commit();
      }
      if(settimemode==1){
        stunden = stunden + 1;
        if(stunden>23){
          stunden=0;
        }
        showwhilesetting();
      }
      if(settimemode==2){
        minutes = minutes+1;
        if(minutes>59){
          minutes=0;
        }
        showwhilesetting();
      }
    #else
      // NTP-Modus: Demo - Minuten ändern
      minutes=minutes+5;
      minutes=minutes%60;
      showClock();
    #endif
     
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
    #ifdef USE_RCT
      // RTC Mode: Zyklus durch Einstellungsmodi
      if(settimemode<3){
        settimemode++;
      }
      if(settimemode==3){
        settimemode=0;
        setDate(seconds, minutes, stunden, day, month, year);
      }
    #endif
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