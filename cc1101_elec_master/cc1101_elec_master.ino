
#include "EEPROM.h"
#include "src/cc1101.h"
//src: https://www.electrodragon.com/w/CC1101
//replaced by other library (instead of panstamp not for nano)

CC1101 cc1101;

#define LEDOUTPUT 7

// counter to get increment in each loop
byte counter;
byte b;
byte syncWord = 199; 

void blinker(){
     digitalWrite(LEDOUTPUT, HIGH);
     delay(100);
     digitalWrite(LEDOUTPUT, LOW);
     delay(100);
}

void setup()
{
 Serial.begin(38400);
 Serial.println("start");

 // setup the blinker output
 pinMode(LEDOUTPUT, OUTPUT);
 digitalWrite(LEDOUTPUT, LOW);

 // blink once to signal the setup
 blinker();
 
 // reset the counter
 counter=0;
 Serial.println("initializing...");
 // initialize the RF Chip
 cc1101.init();
 cc1101.reset();
 cc1101.setSyncWord(&syncWord, false);
 cc1101.setCarrierFreq(CFREQ_868);//433
 cc1101.disableAddressCheck();
 
 Serial.print("CC1101_PARTNUM "); //cc1101=0
 b=cc1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER);
 Serial.println(b);
 b=cc1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER);
 Serial.print("CC1101_VERSION "); //cc1101=4
 Serial.println(b);  
 Serial.print("CC1101_MARCSTATE ");
 Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);
 
 Serial.println("device initialized");
}

void send_data() {
CCPACKET data;
 data.length=1;
 byte blinkCount=counter++;
 data.data[0]=blinkCount;
 Serial.print("CC1101_MARCSTATE ");
 Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);
 if(cc1101.sendData(data)){
   Serial.print("sent ok :)");
   Serial.print(" packet:");
   Serial.print(data.data[0]);
   Serial.print(" len:");
   Serial.print(data.length);
   Serial.print(" ");
   blinker();
 }else{
   Serial.print("sent failed :(");
   blinker();
   blinker();
 }   
}

void loop()
{
 send_data();
 Serial.println("loop done"); 
 delay(1000);
}
