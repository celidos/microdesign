#include "TimerOne.h"

const int adc_0 = A0;
int val; 

void setup() {
  Timer1.initialize(100);
  Timer1.attachInterrupt(sendData);

  Serial.begin(9600);
}

void sendData() {
  Serial1.write("A0");
  val = map(analogRead(adc_0), 0, 1023, 0, 255); //конвертация из 10бит в байт
  Serial1.write(val);
}


void loop() {  
}
