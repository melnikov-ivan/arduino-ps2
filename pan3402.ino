// Library to connect to optical mouse over PS/2 protocol
// Supports commands for PAN3402, PAN3401, etc

Mouse m(5, 6);

void setup() {
  Serial.begin(9600);
  m.init();
}

void loop() {

  int data[2];
  m.read(data);

  Serial.print(" dx ");
  Serial.print(data[0]);
  Serial.print(" dy ");
  Serial.println(data[1]);
  
  delay(50);
}
