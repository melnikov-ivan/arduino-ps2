#define inhibit 100
#define t1 5 // write timeout
#define t2 5 // read timeout
#define READ_DATA 0xEB


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
  // Serial.println("sendData()");
  // inhibit
  pinMode(pinCL, OUTPUT);
  pinMode(pinDT, OUTPUT);
  digitalWrite(pinCL, LOW);
  delayMicroseconds(inhibit);

  // request-to-send
  digitalWrite(pinDT, LOW);
  delayMicroseconds(inhibit);
  digitalWrite(pinCL, HIGH);

  // now read clock
  pinMode(pinCL, INPUT); // _PULLUP
  // wait start bit clock
  get(pinCL, LOW); // HIGH

  // write data
  unsigned char parity = 1;
  for (unsigned char i=0; i<8; i++) {
    unsigned char b = (data >> i) & 0x01;
    // Serial.print(b);
    writeBit(b);
    parity = parity ^ b;
  }

  // write parity bit
  writeBit(parity);
  // Serial.println("pa");

  // stop bit
  writeBit(1);
  // Serial.println("st");
  
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

// private:

// Host-to-Device
// write one bit
void writeBit(unsigned char b) {
  get(pinCL, LOW);
  // Serial.println(b);
  delayMicroseconds(t1);
  digitalWrite(pinDT, b);
  get(pinCL, HIGH);
}

// Device-to-Host
// read one bit
unsigned char readBit() {
  get(pinCL, LOW);
  delayMicroseconds(t2);
  unsigned char c = digitalRead(pinDT);
  get(pinCL, HIGH);
  return c;
}

// wait for signal
void get(int pin, bool level) {
  while(digitalRead(pin) != level) { 
    delayMicroseconds(1);
  }
}

};



PS2 m(5, 6);

void mouse_init()
{
  m.sendData(0xff);  // reset
  m.readData();  // ack byte
  m.readData();  // blank */
  m.readData();  // blank */
  m.sendData(0xf0);  // remote mode
  m.readData();  // ack
  delayMicroseconds(100);
}

int last = millis();

void setup() {
  Serial.begin(9600);
  mouse_init();
}

void loop() {

  m.sendData(READ_DATA);
  if (0xFA != m.readData()) {
    Serial.println("error");
    return;
  }

  char status = m.readData();
  int x = get_x(status, m.readData());
  int y = get_y(status, m.readData());

  if (millis() - last < 100) {
    return;
  }
  last = millis();
  Serial.print(" dx ");
  Serial.print(x);
  Serial.print(" dy ");
  Serial.println(y);
  
}

int get_x(char status, int x) {
  // 4 bit in status is sign for x
  if (bitRead(status, 4)) {
    for(int i = 8; i < 16; ++i) {
      x |= (1<<i);
    }
  }
  return x;
}

int get_y(char status, int x) {
  // 5 bit in status is sign for y
  if (bitRead(status, 5)) {
    for(int i = 8; i < 16; ++i) {
      x |= (1<<i);
    }
  }
  return x;
}
