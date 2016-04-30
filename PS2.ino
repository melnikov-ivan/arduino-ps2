#define ps2_timeout_inhibit 100
#define ps2_timeout_write 5 // write timeout
#define ps2_timeout_read 5 // read timeout

class PS2 {

int pinCL;
int pinDT;

public:

  PS2(int cl, int dt) {
    pinCL = cl;
    pinDT = dt;
    pinMode(pinCL, OUTPUT);
    pinMode(pinDT, OUTPUT);
    digitalWrite(pinCL, HIGH);
    digitalWrite(pinDT, HIGH);
  }

  void sendData(unsigned char data) {
    // inhibit
    pinMode(pinCL, OUTPUT);
    pinMode(pinDT, OUTPUT);
    digitalWrite(pinCL, LOW);
    delayMicroseconds(ps2_timeout_inhibit);

    // request-to-send
    digitalWrite(pinDT, LOW);
    digitalWrite(pinCL, HIGH);

    // now read clock
    pinMode(pinCL, INPUT);
    // wait start bit clock
    until(pinCL, LOW);

    // write data
    unsigned char parity = 1;
    for (unsigned char i = 0; i < 8; i++) {
      unsigned char b = (data >> i) & 0x01;
      writeBit(b);
      parity = parity ^ b;
    }

    // write parity bit
    writeBit(parity);

    // stop bit
    writeBit(1);
    
    // read ACK bit
    pinMode(pinDT, INPUT_PULLUP);
    until(pinCL, LOW);
    if (digitalRead(pinDT) != 0) {
      Serial.println("ERROR ack bit");
    }
    until(pinCL, HIGH);

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
    for (unsigned char i = 0; i < 8; i++) {
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
    until(pinCL, LOW);
    delayMicroseconds(ps2_timeout_write);
    digitalWrite(pinDT, b);
    until(pinCL, HIGH);
  }

  // Device-to-Host
  // read one bit
  unsigned char readBit() {
    until(pinCL, LOW);
    delayMicroseconds(ps2_timeout_read);
    unsigned char c = digitalRead(pinDT);
    until(pinCL, HIGH);
    return c;
  }

  // wait for signal
  void until(int pin, bool level) {
    while(digitalRead(pin) != level) { 
      delayMicroseconds(1);
    }
  }

};
