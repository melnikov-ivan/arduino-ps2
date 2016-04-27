# arduino-ps2
Arduino library to connect with optical mouse over PS/2 protocol

Based on
- http://www.computer-engineering.org/ps2protocol
- http://computer-engineering.org/ps2mouse 

## Usage 
```cpp
PAN3402 mouse(5,6); // cl, dt
mouse.sendData();
mouse.readData()
```

For op720 mouse
* red - VCC
* black - GND
* green - CLK
* white - DT

