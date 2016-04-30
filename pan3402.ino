#define MOUSE_READ 0xEB
#define MOUSE_RESET 0xFF
#define MOUSE_MODE_REMOTE 0xF0
#define MOUSE_RESPONSE_OK 0xFA

PS2 m(5, 6);

void mouse_init()
{
  m.sendData(MOUSE_RESET);  // reset
  m.readData();  // ack byte
  m.readData();  // blank */
  m.readData();  // blank */
  m.sendData(MOUSE_MODE_REMOTE);  // remote mode
  m.readData();  // ack
}

int last = millis();

void setup() {
  Serial.begin(9600);
  mouse_init();
}

void loop() {

  m.sendData(MOUSE_READ);
  if (MOUSE_RESPONSE_OK != m.readData()) {
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
