//src
//https://github.com/veonik/arduino-cc1101

#include <Arduino.h>
#include "src\cc1101.h"
#include "src\ccpacket.h"

/*  PINOUT WIRING
 * ARDUINO CC1101
 *     GND GND
 * 3.3 VCC VCC
 *      10 CSN
 *      11 (MO)SI
 *      12 (MI)SO
 *      13 SCK
 *      02 GD0 -> should be a valid interrupt pin (see below)
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
unsigned int sendDelay = 5000;

void messageReceived() {
    packetWaiting = true;
}

void setup() {
    radio.init();
    radio.setSyncWord(0x01);//syncWord
    radio.setCarrierFreq(CFREQ_868);//433
    radio.disableAddressCheck();
    radio.setTxPowerAmp(PA_LowPower);

    Serial.begin(9600);
    Serial.print(F("CC1101_PARTNUM "));
    Serial.println(radio.readReg(CC1101_PARTNUM, CC1101_STATUS_REGISTER));
    Serial.print(F("CC1101_VERSION "));
    Serial.println(radio.readReg(CC1101_VERSION, CC1101_STATUS_REGISTER));
    Serial.print(F("CC1101_MARCSTATE "));
    Serial.println(radio.readReg(CC1101_MARCSTATE, CC1101_STATUS_REGISTER) & 0x1f);

    Serial.println(F("CC1101 radio initialized."));
    attachInterrupt(digitalPinToInterrupt(CC1101Interrupt), messageReceived, FALLING);
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
        detachInterrupt(digitalPinToInterrupt(CC1101Interrupt));
        packetWaiting = false;
        CCPACKET packet;
        //if (radio.receiveData(&packet) > 0) {
        radio.receiveData(&packet);
            Serial.println(F("Received packet..."));
            if (!packet.crc_ok) {
                Serial.println(F("crc not ok"));
            }
            Serial.print(F("lqi: "));
            Serial.println(lqi(packet.lqi));
            Serial.print(F("rssi: "));
            Serial.print(rssi(packet.rssi));
            Serial.println(F("dBm"));

            if (packet.crc_ok && packet.length > 0) {
                Serial.print(F("packet: len "));
                Serial.println(packet.length);
                Serial.println(F("data: "));
                Serial.println((const char *) packet.data);
            }
        //}

        attachInterrupt(digitalPinToInterrupt(CC1101Interrupt), messageReceived, FALLING);
    }
    unsigned long now = millis();
    if (now > lastSend + sendDelay) {
        detachInterrupt(digitalPinToInterrupt(CC1101Interrupt));

        lastSend = now;
        const char *message = "hello world";
        CCPACKET packet;
        // We also need to include the 0 byte at the end of the string
        packet.length = strlen(message)  + 1;
        strncpy((char *) packet.data, message, packet.length);

        radio.sendData(packet);
        Serial.println(F("Sent packet..."));

        attachInterrupt(digitalPinToInterrupt(CC1101Interrupt), messageReceived, FALLING);
    }
}
