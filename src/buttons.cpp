#include "buttons.h"
#include "globals.h"
#include "eeprom_layout.h"
#include "show.h"
#include "effects.h"
#include "pomodoro.h"
#include <EEPROM.h>
#ifdef USE_RCT
  #include "rct.h"
#endif

// Fuehrt die unter /setting zugewiesene Funktion einer Taste aus (nur
// NTP-Builds - siehe globals_buttons.h fuer die Optionsliste).
void applyButtonFunction(uint8_t fn) {
  switch (fn) {
    case 1: // Helligkeit erhoehen
      dimm += 50;
      if (dimm > 255) dimm = 50;
      strip.setBrightness(dimm);
      strip.show();
      break;
    case 2: // Nachtmodus umschalten - identische Logik wie die bisherige bt1longs()
      mqttonset = 1;
      mqtton = (on == 0) ? 1 : 0;
      readTime();
      showClock();
      break;
    case 3: // Pomodoro umschalten
      if (pomodoroModeActive) stopPomodoro(); else startPomodoro();
      break;
    case 4: // Effekte-Modus umschalten (gegenseitiger Ausschluss mit Pomodoro)
      effectsModeActive = !effectsModeActive;
      if (effectsModeActive && pomodoroModeActive) stopPomodoro();
      if (!effectsModeActive) threshold = 0;
      EEPROM.write(EepromLayout::LIGHT_EFFECTS_ENABLED_OFFSET, effectsModeActive ? 1 : 0);
      EEPROM.commit();
      break;
    case 5: // Naechster Uebergangseffekt
      effectMode = (effectMode + 1) % EFFECT_OPTIONS_COUNT;
      threshold = 0;
      readTime();
      showClock();
      break;
    default:
      break; // 0 = Keine Funktion
  }
}


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
      // NTP-Modus: konfigurierbare Funktion (siehe /setting)
      applyButtonFunction(btn1ClickFunction);
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
    #ifdef USE_RCT
      // RTC-Modus: Taste 1 lang bleibt Nachtmodus-Umschalter, ist nicht
      // Teil des Uhreinstell-Ablaufs (siehe /setting-Info-Karte).
      mqttonset=1;
      if(on==0){
        mqtton=1;
      }else{
        mqtton=0;
      }
    #else
      // NTP-Modus: konfigurierbare Funktion (siehe /setting)
      applyButtonFunction(btn1LongFunction);
    #endif
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
          dimm,
          mqttenable
        };

        EEPROM.put(EepromLayout::DESIGN_OFFSET, customDesign);
        EEPROM.commit();
      }
      if(settimemode==1){
        stunden = stunden + 1;
        if(stunden>23){
          stunden=0;
        }
        showwhilesetting();
        LOG("Stunden: "); LOGLN(stunden);
      }
      if(settimemode==2){
        minutes = minutes+1;
        if(minutes>59){
          minutes=0;
        }
        showwhilesetting();
        LOG("Minuten: "); LOGLN(minutes);
      }
    #else
      // NTP-Modus: konfigurierbare Funktion (siehe /setting)
      applyButtonFunction(btn2ClickFunction);
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
        LOG("Settimemode: "); LOGLN(settimemode);
      }
      if(settimemode==3){
        settimemode=0;
        setDate(seconds, minutes, stunden, day, month, year);
        LOG("Zeit eingestellt");
      }
    #else
      // NTP-Modus: konfigurierbare Funktion (siehe /setting)
      applyButtonFunction(btn2LongFunction);
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
  setanzeige();
      showmystrip();
}