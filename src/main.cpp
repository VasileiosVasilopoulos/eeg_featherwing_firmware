#include <Arduino.h>
#include <board_definitions.h>
#include <board_functions.h>

GEENIE Geenie;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Ready");
  Geenie.set_buttons();
}

void loop() {
  // put your main code here, to run repeatedly:
}