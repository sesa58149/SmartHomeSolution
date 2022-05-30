#define INPUT_PIR D1
#define OUTPUT_LED D2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(INPUT_PIR, INPUT);
  pinMode(OUTPUT_LED, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  while(1)
  {
    int pinVal = digitalRead(INPUT_PIR);
    Serial.print("Input: ");
    Serial.println( pinVal, HEX);
    digitalWrite(OUTPUT_LED, pinVal);
    //delay(1000);
    //digitalWrite(OUTPUT_LED, 1);
    delay(100);
  }

}
