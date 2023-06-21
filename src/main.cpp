#include <Arduino.h>
#include <board_definitions.h>
#include <board_functions.h>

GEENIE Geenie;


void setup() {
  // put your setup code here, to run once:
  delay(5000);

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Ready");
  Geenie.set_buttons();
  Geenie.initialize();
  // byte ads_addr = Geenie.read_ads();

  // Geenie.activateChannel(1, ADS_GAIN24, ADSINPUT_NORMAL);
  // Geenie.activateChannel(2, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(3, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(4, ADS_GAIN24, ADSINPUT_NORMAL);
  
  delay(4000);
  Geenie.RREGS(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4

  Geenie.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Geenie.streaming) {
    if (Geenie.channelDataAvailable) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      Geenie.updateChannelData();
      Geenie.sendChannelDataSerial(Geenie.PACKET_TYPE_ACCEL);
    }
  }

    // Check serial 0 for new data
  // if (Geenie.hasDataSerial()) {
    // Read one char from the serial 0 port
    // char newChar = Geenie.getCharSerial();

    // Send to the sd library for processing
    // sdProcessChar(newChar);

    // Send to the board library
    // Geenie.processChar(newChar);

  // }
  
  // Call the loop function on the board
  // board.loop();
}
