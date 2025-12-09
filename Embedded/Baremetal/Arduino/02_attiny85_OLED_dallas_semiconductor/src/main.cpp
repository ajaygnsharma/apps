#include <Arduino.h>
#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A2
//#define analogPin  4 //the thermistor attach to 
//#define beta       3950 //the beta of the thermistor
//#define resistance 10 //the value of the pull-up resistor
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup() {
  TinyWireM.begin();
  sensors.begin();

  oled.begin(); // Optional: address for OLED

  oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 0);
  oled.print(F("ATtiny85 OLED!"));
  oled.setCursor(0, 1);
  oled.print(F("Tiny4kOLED :)"));
  oled.on();
}

void loop() {
  sensors.requestTemperatures();
  delay(50); // wait for conversion

  int tempC = (int)sensors.getTempCByIndex(0);

  char buf[10]{};
  snprintf(buf, sizeof(buf), "%d", tempC);

  oled.clear();
  oled.setFont(FONT6X8);
  oled.setCursor(0, 0);
  oled.print(F("Temp:"));
  oled.setCursor(0, 1);
  oled.print(buf);

  if (tempC == -127) {
    oled.setCursor(0, 2);
    oled.print(F("Sensor ERR"));
  }

  delay(200);
}
