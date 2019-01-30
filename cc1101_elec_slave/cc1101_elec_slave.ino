
#include "EEPROM.h"
#include "src/cc1101.h"
//src:https://www.electrodragon.com/w/CC1101
//replaced by other library (instead of panstamp not for nano)

#define LEDOUTPUT 7

// The connection to the hardware chip CC1101 the RF Chip
CC1101 cc1101;

byte b;
byte syncWord = 199; 

// a flag that a wireless packet has been received 
boolean packetAvailable = false;         

void blinker(){
     digitalWrite(LEDOUTPUT, HIGH);
     delay(100);
     digitalWrite(LEDOUTPUT, LOW);
     delay(100);
}

/**
* Handle interrupt from CC1101 (INT0)
*/
void cc1101signalsInterrupt(void){
 // set the flag that a package is available
 packetAvailable = true;
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
 //cc1101.reset();
 // initialize the RF Chip
 cc1101.init();
 cc1101.reset();
 cc1101.setSyncWord(&syncWord, false);
 cc1101.setCarrierFreq(CFREQ_868);//433
 cc1101.disableAddressCheck();
 
  Serial.print("CC1101_PARTNUM ");
 b=cc1101.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER);
 Serial.println(b);
 b=cc1101.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER);
 Serial.print("CC1101_VERSION ");
 Serial.println(b);  
 Serial.print("CC1101_MARCSTATE ");
 Serial.println(cc1101.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER)& 0x1f);

 attachInterrupt(0, cc1101signalsInterrupt, FALLING);
 Serial.println("device initialized");

 Serial.println("setup done");
}

void loop()
{
 if(packetAvailable){
   Serial.print("packet received");
   // Disable wireless reception interrupt
   detachInterrupt(0);

   // clear the flag
   packetAvailable = false;
   
   CCPACKET packet;

    //TESTTTESTTEST sssssssssssssssssssssssssssssssss
   cc1101.receiveData(&packet);//&buffer
       Serial.print("crc: ");
       Serial.print(packet.crc_ok);
       Serial.print("packet: len");
       Serial.print(packet.length);
       Serial.print(" data0: ");
       Serial.println(packet.data[0]);
    //TESTTTESTTEST sssssssssssssssssssssssssssssssss


  
   if(cc1101.receiveData(&packet) > 0){
     
     if(!packet.crc_ok) {
       Serial.println("crc not ok");
     }
     
     if(packet.crc_ok && packet.length > 0){
      
       Serial.print("packet: len");
       Serial.print(packet.length);
       Serial.print(" data0: ");
       Serial.println(packet.data[0]);
       
     }
   }
   
   // Enable wireless reception interrupt
   attachInterrupt(0, cc1101signalsInterrupt, FALLING);
 }
}
