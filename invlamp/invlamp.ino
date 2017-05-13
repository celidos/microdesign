
char t = 1;

void setup()
{
  pinMode(22, OUTPUT);
  pinMode(30, INPUT);
  digitalWrite(22, HIGH);
  Serial.begin(9600);

}

void loop()
{
  //if (t == 1)
  //  digitalWrite(22, HIGH);
  //else
  //  digitalWrite(22, LOW);
  //delay(40);
  //t = 1 - t;
  char t1 = digitalRead(25);
  //char t2 = digitalRead(24);
  //Serial.print(char(t1*2 + t2 + '0')); 
  Serial.print(t1); 
}
