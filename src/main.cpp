#include <Arduino.h>
#include <board_definitions.h>
#include <board_functions.h>

GEENIE Geenie;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUDRATE);
  while(!Serial){}

  Serial.println("Ready");
  Geenie.set_buttons();
    if(!Geenie.initialize_ads()){
      Serial.println("Card Mount Failed");
      return;
  }
  
  byte ads_addr = Geenie.read_ads();
}

void loop() {
  // put your main code here, to run repeatedly:
}

// void setup() {
//   // initialize digital pin LED_BUILTIN as an output.
//   pinMode(LED_BUILTIN, OUTPUT);
// }

// // the loop function runs over and over again forever
// void loop() {
//   digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//   delay(1000);                       // wait for a second
//   digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//   delay(1000);                       // wait for a second
// }