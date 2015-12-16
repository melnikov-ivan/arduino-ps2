#define inhibit 100
#define d1 5  // write timeout
#define d2 5 // read timeout

#include "Arduino.h"

class PS2 {

private:
  int pinCL;
  int pinDT;

public:

  PS2(int clock, int data) {
    pinCL = clock;
    pinDT = data;
  }

  void sendData(unsigned char data) {
    // inhibit
    pinMode(pinCL, OUTPUT);
    pinMode(pinDT, OUTPUT);
    digitalWrite(pinCL, LOW);
    delayMicroseconds(inhibit);
    
    
    // request-to-send
    digitalWrite(pinDT, LOW);
    delayMicroseconds(inhibit);
    digitalWrite(pinCL, HIGH);
    pinMode(pinCL, INPUT_PULLUP);

    // wait start bit clock
    get(pinCL, HIGH);

    unsigned char parity = 1;
    for (unsigned char i=0; i<8; i++) {
      char bit = (data >> i) & 0x01;
      writeBit(bit);
      parity = parity ^ bit;
    }

    // write parity bit
    writeBit(parity);

    // stop bit
    writeBit(1);
    
    // read ACK bit
    pinMode(pinDT, INPUT_PULLUP);
    get(pinCL, LOW);
    if (digitalRead(pinDT) != 0) {
      Serial.println("ERROR ack bit");
    }
    get(pinCL, HIGH);

    // hold line
    digitalWrite(pinCL, LOW);
  }

  unsigned char readData() {
    unsigned char data = 0x00;

    pinMode(pinCL, OUTPUT);
    pinMode(pinDT, OUTPUT);
    digitalWrite(pinCL, HIGH);
    digitalWrite(pinDT, HIGH);
    delayMicroseconds(50);

    pinMode(pinCL, INPUT_PULLUP);
    pinMode(pinDT, INPUT_PULLUP);

    // start bit
    while (digitalRead(pinDT) != LOW) ;
    if (0 != readBit()) {
      Serial.println("ERROR start bit");
    }

    // data 
    unsigned char parity = 1;
    for (unsigned char i=0; i<8; i++) {
      char bit = readBit();
      data = data | (bit << i);
      parity = parity ^ bit;
    }

    // parity bit
    if (parity != readBit()) {
      Serial.println("ERROR parity bit");
    }

    // stop bit
    if (1 != readBit()) {
      Serial.println("ERROR stop bit");
    }

    pinMode(pinCL, OUTPUT);
    digitalWrite(pinCL, LOW);
    delayMicroseconds(50);
    
    return data;
  }


private:

  // Host-to-Device
  // write one bit
  void writeBit(unsigned char b) {
    while(digitalRead(pinCL) != LOW) { }
    delayMicroseconds(d1);
    digitalWrite(pinDT, b);
    while(digitalRead(pinCL) != HIGH) { }
  }

  // Device-to-Host
  // read one bit
  unsigned char readBit() {
    get(pinCL, LOW);
    delayMicroseconds(d2);
    unsigned char c = digitalRead(pinDT);
    get(pinCL, HIGH);
    return c;
  }

  // wait for signal
  void get(unsigned pin, unsigned level) {
    while(digitalRead(pin) != level) { }
  }

};
