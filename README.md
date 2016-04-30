# arduino-ps2
Arduino library to connect with optical mouse over PS/2 protocol

Based on
- http://www.computer-engineering.org/ps2protocol
- http://computer-engineering.org/ps2mouse 

## Usage
Download PS2.ino file and put it somewhere nearby. Include it in your scketch. That's all.

```cpp
<<<<<<< HEAD
PAN3402 mouse(5,6); // cl, dt
mouse.sendData();
mouse.readData()
=======
#include <../ps2/PS2.ino>

PS2 mouse(5, 6);
mouse.sendData(0xEB); // get x/y command
if (0xFA != m.readData()) {
    Serial.println("error");
    return;
}
>>>>>>> master
```

For op720 mouse
* red - VCC
* black - GND
* green - CLK
* white - DT

