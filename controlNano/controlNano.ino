
int tonePin = 6;
int LecPin1 = 2;

void setup() {
   Serial.begin(115200);
   //Serial2.begin(115200);
   pinMode(tonePin, OUTPUT);
   pinMode(LecPin1,INPUT);
   pinMode(LecPin2,INPUT);
   pinMode(LecPin3,INPUT);
}



void tokyo_drift() {
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 130, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 130, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 130, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    tone(tonePin, 184, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 184, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 220, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 195, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 184, 220.3125);
    delay(234.375);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 220, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 195, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 184, 220.3125);
    delay(234.375);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 130, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 130, 220.3125);
    delay(234.375);
    delay(117.1875);
    tone(tonePin, 164, 220.3125);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);
    delay(234.375);
    tone(tonePin, 123, 220.3125);
    delay(234.375);

}

void loop() {
    // Play midi
        
    while(digitalRead(LecPin1) == 1 ){
      tokyo_drift();
    }
}
