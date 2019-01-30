//lib import
#include <FastLED.h>

// <------- START OF CONFIGURATION ------->

//PIN CONFIGURATION
#define DATA_PIN 5
#define STOP_PIN 2 //should be INT pin !
#define LEFT_PIN 3
#define RGHT_PIN 4

//TOTAL LEDs and START/END each fct
#define NUM_LEDS 90
#define L_start 0
#define L_end   3
#define S_start 4
#define S_end   5
#define R_start 6
#define R_end   9

//Animations
#define LR_TimeOut 1000 //ms
#define INTV_ROLL 32    //interval between led
#define STEP_ROLL 4     //higher == quicker
#define STEP_FADE 16    //higher == quicker

//LED COLORS (R,G,B)
const int color_led_stop[] = {200,0,0};
const int color_led_turn[] = {255,100,0};
const int color_led_norm[] = {40,0,0};

// <------- END OF CONFIGURATION ------->

// Define the array of leds
CRGB leds[NUM_LEDS];

//Used in isr
volatile bool btnStopState = false;

bool LanimeOn=false;
bool LanimeOff=true;
bool Lavailable=true;
bool RanimeOn=false;
bool RanimeOff=true;
bool Ravailable=true;

long Ltimer=0;
long Rtimer=0;

void setup() {
  //setup pins
  pinMode(STOP_PIN, INPUT_PULLUP);
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RGHT_PIN, INPUT_PULLUP);
  
  //isr for stop (prioritized)
  attachInterrupt(digitalPinToInterrupt(STOP_PIN), stopBtn, CHANGE);
  
  //add leds
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  
  //safety wait
  delay(1000);
  
  //turn all leds off
  for(int i=0;i<NUM_LEDS;i++){
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  delay(500);
  
  //turn left on
  for(int i=L_start;i<=L_end;i++){
    leds[i] = CRGB(color_led_turn[0],color_led_turn[1],color_led_turn[2]);
  }
  FastLED.show();
  delay(500);
  
  //turn left off, right on
  for(int i=L_start;i<=L_end;i++){
    leds[i] = CRGB(0,0,0);
  }
  for(int i=R_start;i<=R_end;i++){
    leds[i] = CRGB(color_led_turn[0],color_led_turn[1],color_led_turn[2]);
  }
  FastLED.show();
  delay(500);
  
  //turn right off, stop on
  for(int i=R_start;i<=R_end;i++){
    leds[i] = CRGB(0,0,0);
  }
  for(int i=S_start;i<=S_end;i++){
    leds[i] = CRGB(color_led_stop[0],color_led_stop[1],color_led_stop[2]);
  }
  FastLED.show();
  delay(500);
  
  //turn stop off
  for(int i=S_start;i<=S_end;i++){
    leds[i] = CRGB(0,0,0);
  }
  FastLED.show();
  delay(500);
}

void loop() {
  //check stop
  manageStop();
  
  //check left
  if(!digitalRead(LEFT_PIN) && !LanimeOn){
    Lavailable=false;
    LanimeOn =true;
    LanimeOff=false;
    roll(L_start,L_end,0,STEP_ROLL,INTV_ROLL,color_led_turn,true);
  }
  if(digitalRead(LEFT_PIN) && !LanimeOff){
    Lavailable=false;
    LanimeOff=true;
    LanimeOn =false;
    fade(L_start,L_end,0,STEP_FADE,false,color_led_turn);
    Ltimer=millis();
  }
  if(LanimeOff && millis()-Ltimer>LR_TimeOut){
    //turn(L_start,L_end,color_led_norm,true);
    Lavailable=true;
  }
  
  //check right
  if(!digitalRead(RGHT_PIN) && !RanimeOn){
    Ravailable=false;
    RanimeOn =true;
    RanimeOff=false;
    roll(R_start,R_end,0,STEP_ROLL,INTV_ROLL,color_led_turn,false);
  }
  if(digitalRead(RGHT_PIN) && !RanimeOff){
    Ravailable=false;
    RanimeOff=true;
    RanimeOn =false;
    fade(R_start,R_end,0,STEP_FADE,false,color_led_turn);
    Rtimer=millis();
  }
  if(RanimeOff && millis()-Rtimer>LR_TimeOut){
    //turn(R_start,R_end,color_led_norm,true);
    Ravailable=true;
  }
}
void manageStop(){
  if(btnStopState){
    turn(S_start,S_end,color_led_stop,true);
    if(Lavailable){
      turn(L_start,L_end,color_led_stop,true);
    }
    if(Ravailable){
      turn(R_start,R_end,color_led_stop,true);
    }
  }else{
    turn(S_start,S_end,color_led_norm,true);
    if(Lavailable){
      turn(L_start,L_end,color_led_norm,true);
    }
    if(Ravailable){
      turn(R_start,R_end,color_led_norm,true);
    }
  }
}
void stopBtn(){
  //turn(6,7,color_led_stop,digitalRead(STOP_PIN));
  btnStopState = !digitalRead(STOP_PIN);
}
void turn(int startLED, int endLED, int rgb[], bool on){
  if(on){
    for(int i=startLED;i<=endLED;i++){
      leds[i] = CRGB(rgb[0],rgb[1],rgb[2]);
    }
  }else{
    for(int i=startLED;i<=endLED;i++){
      leds[i] = CRGB(0,0,0);
    }
  }
  FastLED.show();
}

void roll(int startLED, int endLED, int delayT, int stepS, int delayLED, int rgb[], bool dir){
  int newMax = 255+(delayLED*(endLED-startLED+1));
  
  for(int i=0;i<=newMax;i+=stepS){
    for(int j=startLED;j<=endLED;j++){
      int val = i - (j-startLED)*delayLED;
      if(val<0){
        val=0;
      }
      if(val>255){
        val=255;
      }
      if(!dir){
        leds[j] = CRGB(rgb[0]/255.0*val,rgb[1]/255.0*val,rgb[2]/255.0*val);
      }else{
        leds[endLED-j] = CRGB(rgb[0]/255.0*val,rgb[1]/255.0*val,rgb[2]/255.0*val);
      }
    }
    //check if stop
    manageStop();
    
    FastLED.show();
    //delay(delayT);
  }
  for(int j=startLED;j<=endLED;j++){
    leds[j] = CRGB(rgb[0],rgb[1],rgb[2]);
  }
  FastLED.show();
}
void fade(int startLED, int endLED, int delayT, int stepS, bool in, int rgb[]){
  if(in){
    for(int i=0;i<=255;i+=stepS){
      for(int j=startLED;j<=endLED;j++){
        leds[j] = CRGB(rgb[0]/255.0*i,rgb[1]/255.0*i,rgb[2]/255.0*i);
      }
      //check if stop
      manageStop();
    
      FastLED.show();
      //delay(delayT);
    }
    //turn on all
    for(int j=startLED;j<=endLED;j++){
      leds[j] = CRGB(rgb[0],rgb[1],rgb[2]);
    }
    FastLED.show();
  }else{
    for(int i=255;i>=0;i-=stepS){
      for(int j=startLED;j<=endLED;j++){
        leds[j] = CRGB(rgb[0]/255.0*i,rgb[1]/255.0*i,rgb[2]/255.0*i);
      }
      //check if stop
      manageStop();
    
      FastLED.show();
      //delay(delayT);
    }
    //turn off all
    for(int j=startLED;j<=endLED;j++){
      leds[j] = CRGB(0,0,0);
    }
    FastLED.show();
  }
}
