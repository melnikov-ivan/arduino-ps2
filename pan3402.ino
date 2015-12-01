#define inhibit 100
#define d1 5  // write timeout
#define d2 5 // read timeout
#define READ_DATA 0xEB

#define pinCL 5
#define pinDT 6
#define pinBT 7


void mouse_init()
{
  sendData(0xff);  // reset
  readData();  // ack byte
  readData();  // blank */
  readData();  // blank */
  sendData(0xf0);  // remote mode
  readData();  // ack
  delayMicroseconds(100);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(250000);
  pinMode(pinBT, INPUT_PULLUP); // button to start
//  mouse_init();
}

void loop() {

  if (digitalRead(pinBT)) {
    delay(500);
    return;
  }
  unsigned long time = micros();
  sendData(READ_DATA);
//  Serial.println(micros() - time);
  if (0xFA != readData()) {
    Serial.println("error");
    return;
  }
  readData(); // stats
  Serial.print(micros());
  Serial.print(" dx ");
  Serial.print(readData());
  Serial.print(" dy ");
  Serial.println(readData());

  
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
    writeBit(data & 0x01);
    
    parity = parity ^ (data & 0x01);
    data = data >> 1;
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
  delayMicroseconds(60);

  pinMode(pinCL, INPUT_PULLUP);
  pinMode(pinDT, INPUT_PULLUP);

  // start bit
  while (digitalRead(pinDT) != LOW) ;
  if (0 != readBit()) {
    Serial.println("ERROR start bit");
  }

  // data 
  unsigned char bit = 0x01;
  unsigned char parity = 1;
  for (unsigned char i=0; i<8; i++) {
    unsigned c = readBit();
    data = data | (bit * c);

    parity = parity ^ c;
    bit = bit << 1;
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

