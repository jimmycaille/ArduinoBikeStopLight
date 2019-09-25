#include <Arduino.h>
#include "cc1101.h"
#include "ccpacket.h"

// Attach CC1101 pins to their corresponding SPI pins
// Uno pins:
// CSN (SS) => 10
// MOSI => 11
// MISO => 12
// SCK => 13
// GD0 => A valid interrupt pin for your platform (defined below this)

/*
 * ARDUINO /  CC-1101  /    ARDUINO
 *             _____
 *       VDD -|1   2|- VDD  <- Vcc 3v3
 * 11 -> SI  -|3   4|- SCK  <- 13
 * 12 <- SO  -|5   6|- GDO2
 * 10 -> CSn -|7   8|- GDO0 <-> 2 (ISR+?)
 *       GND -|9  10|- GND  -> GND
 *             ¯¯¯¯¯
 */

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
#define CC1101Interrupt 4 // Pin 19
#define CC1101_GDO0 19
#elif defined(__MK64FX512__)
// Teensy 3.5
#define CC1101Interrupt 9 // Pin 9
#define CC1101_GDO0 9
#else
#define CC1101Interrupt 0 // Pin 2
#define CC1101_GDO0 2
#endif

CC1101 radio;

byte syncWord[2] = {199, 10};
byte rcvAddr = 0x11;
byte sndAddr = 0x22;
byte channel = 0x00;
byte freq    = CFREQ_433;//868-915-433-918
byte mode    = 0;//0 or MODE_LOW_SPEED (38 or 4.8kbps)
byte power   = PA_LowPower;//PA_LowPower-PA_LongDistance

byte incr=0;
bool packetWaiting;

void messageReceived() {
    packetWaiting = true;
}

void setup() {
    radio.init(freq, mode);
    radio.setSyncWord(syncWord);
    radio.setDevAddress(rcvAddr);
    radio.setChannel(channel);//or radio.disableAddressCheck();
    //radio.setCarrierFreq(freq);
    radio.setTxPowerAmp(power);

    Serial.begin(9600);
    Serial.print(F("CC1101_PARTNUM "));
    Serial.println(radio.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
    Serial.print(F("CC1101_VERSION "));
    Serial.println(radio.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
    Serial.print(F("CC1101_MARCSTATE "));
    Serial.println(radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

    Serial.println(F("CC1101 radio initialized."));
    attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
}

// Get signal strength indicator in dBm.
// See: http://www.ti.com/lit/an/swra114d/swra114d.pdf
int rssi(char raw) {
    uint8_t rssi_dec;
    // TODO: This rssi_offset is dependent on baud and MHz; this is for 38.4kbps and 433 MHz.
    uint8_t rssi_offset = 74;
    rssi_dec = (uint8_t) raw;
    if (rssi_dec >= 128)
        return ((int)( rssi_dec - 256) / 2) - rssi_offset;
    else
        return (rssi_dec / 2) - rssi_offset;
}

// Get link quality indicator.
int lqi(char raw) {
    return 0x3F - raw;
}

void loop() {
    if (packetWaiting) {
        detachInterrupt(CC1101Interrupt);
        packetWaiting = false;
        CCPACKET packet;
        if (radio.receiveData(&packet) > 0) {
            if (!packet.crc_ok) {
                Serial.print(F(" crc NOT ok"));
            }
            //if (packet.crc_ok && packet.length > 0) {
            if (packet.length > 0) {
                //respond to ping
                char message[59];
                strncpy(message,packet.data+1, 25);
                strcpy(message+strlen(message)," dst:0x");
                String(packet.data[0],HEX).toCharArray(message+strlen(message),5);
                strcpy(message+strlen(message)," len:");
                itoa(packet.length,message+strlen(message),10);
                strcpy(message+strlen(message)," lqi:");
                itoa(lqi(packet.lqi),message+strlen(message),10);
                strcpy(message+strlen(message)," rssi:");
                itoa(rssi(packet.rssi),message+strlen(message),10);
                strcpy(message+strlen(message),"dBm SND:PONG_");
                itoa(incr,message+strlen(message),10);incr++;
                
                Serial.print("RCV:");
                Serial.print(message);
                
                CCPACKET packet;
                // We also need to include the 0 byte at the end of the string and addr
                packet.length = strlen(message)  + 2;
                packet.data[0] = sndAddr;
                strncpy((char *) packet.data+1, message, packet.length);
        
                if(radio.sendData(packet)){
                  Serial.print(" SUCCESS responding !");
                }else{
                  Serial.print(" ERROR during response...");
                }
            }else{

              
            }
            Serial.println("");
        }
        attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
    }
}
