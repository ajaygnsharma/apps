#include <Arduino.h>
#include <TinyWireM.h>
#include <Tiny4kOLED.h>

#define analogPin  A2 //the thermistor attach to 
#define beta 3950     //the beta of the thermistor
#define resistance 10 //the value of the pull-up resistor
#define BUZZER_PIN 1

void setup()
{
  TinyWireM.begin();
  oled.begin(); // Optional: address for OLED
  oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 0);
  oled.print(F("ATtiny85 OLED!"));
  oled.on();
}


void loop()
{
  tone(BUZZER_PIN, 1000);
  long a = 1023 - analogRead(analogPin);  //read thermistor value 
  
  char buf[128]{};
  snprintf(buf, sizeof(buf), "Raw reading %ld", a);
  //oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 0);
  oled.print(buf);

  //the calculating formula of temperature
  int tempC = beta /(log((1025.0 * 10 / a - 10) / 10) + beta / 298.0) - 273.0;
  //float tempF = 1.8*tempC + 32.0;
  
  memset(buf, '\0', sizeof(buf));
  snprintf(buf, sizeof(buf), "Centigrade %d", tempC);
  oled.setCursor(0, 1);
  oled.print(buf);
  delay(10);
  noTone(BUZZER_PIN);
  delay(200); //wait for 200 milliseconds
}
