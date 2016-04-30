#define MOUSE_READ 0xEB
#define MOUSE_RESET 0xFF
#define MOUSE_MODE_REMOTE 0xF0
#define MOUSE_RESPONSE_OK 0xFA


class Mouse: public PS2 {

public: 

  Mouse(int cl, int dt) : PS2(cl, dt) {  }

  void init() {
    sendData(MOUSE_RESET);  // reset
    readData();  // ack byte
    readData();  // blank */
    readData();  // blank */
    sendData(MOUSE_MODE_REMOTE);  // remote mode
    readData();  // ack
  }

  void read(int data[2]) {
    sendData(MOUSE_READ);
    if (MOUSE_RESPONSE_OK != readData()) {
      Serial.println("error");
      return;
    }

    char status = readData();
    data[0] = get_x(status, readData());
    data[1] = get_y(status, readData());
  }

private:

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

};