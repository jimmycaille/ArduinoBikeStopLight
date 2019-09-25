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
bool packetWaiting;

unsigned long lastSend = 0;
unsigned int sendDelay = 2000;
byte incr=0;
void messageReceived() {
    packetWaiting = true;
}

void setup() {
    radio.init();
    radio.setSyncWord(syncWord);
    radio.setDevAddress(0x00);//test
    radio.setChannel(0x00);//test
    radio.setCarrierFreq(CFREQ_433); //CFREQ_868-915-433-918
    //radio.disableAddressCheck();
    //radio.setTxPowerAmp(PA_LongDistance);
    radio.setTxPowerAmp(PA_LowPower);

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
            Serial.print(F("Received packet :"));
            if (!packet.crc_ok) {
                Serial.print(F(" crc NOT ok"));
            }
            Serial.print(F(" lqi: "));
            Serial.print(lqi(packet.lqi));
            Serial.print(F(" rssi: "));
            Serial.print(rssi(packet.rssi));
            Serial.print(F("dBm"));

            if (packet.crc_ok && packet.length > 0) {
                Serial.print(F(" packet: len "));
                Serial.print(packet.length);
                Serial.print(F(" data: "));
                Serial.print((const char *) packet.data);
            }
            Serial.println("");
        }

        attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
    }
    unsigned long now = millis();
    if (now > lastSend + sendDelay) {
        detachInterrupt(CC1101Interrupt);

        lastSend = now;
        //const char *message = "hello world_";
        char message[15] = "hello world_";
        itoa(incr,message+strlen(message),10);incr++;
        //Serial.println(incr);
        //Serial.println((const char *)message);
        
        CCPACKET packet;
        // We also need to include the 0 byte at the end of the string
        packet.length = strlen(message)  + 1;
        strncpy((char *) packet.data, message, packet.length);

        packet.data[0] = 0x22;

        radio.sendData(packet);
        Serial.println(F("Sent packet..."));

        attachInterrupt(CC1101Interrupt, messageReceived, FALLING);
    }
}
