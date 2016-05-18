# arduino-ps2
Arduino library to connect with optical mouse over PS/2 protocol

Blog post about process http://imelnikov.ru/model/radio/arduino-rc

Based on
- http://www.computer-engineering.org/ps2protocol
- http://computer-engineering.org/ps2mouse 

## Usage
Download PS2.ino and Mouse.ino files and put them in the same folder.

```cpp
Mouse m(5, 6); // cl, dt
m.init();

int data[2];
m.read(data);

Serial.print(" dx ");
Serial.print(data[0]);
Serial.print(" dy ");
Serial.println(data[1]);
```

For op720 mouse
* red - VCC
* black - GND
* green - CLK
* white - DT

