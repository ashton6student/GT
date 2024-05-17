#define button 34

void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT);
  Serial.begin(115200);
}

void loop() {
  // read the input on analog pin 0:
  int sensorValue = digitalRead(button);
  // print out the value you read:
  Serial.println(sensorValue);
  delay(1);        // delay in between reads for stability
}
