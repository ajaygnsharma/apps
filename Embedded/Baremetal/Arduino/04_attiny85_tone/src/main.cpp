#include <Arduino.h>
#define BUZZER_PIN 1

void setup() {
}

void loop() {
  tone(BUZZER_PIN, 1000);
  delay(10); 
  noTone(BUZZER_PIN);
  delay(200);
}
