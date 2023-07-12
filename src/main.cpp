#include <Arduino.h>
#include <board_definitions.h>
#include <board_functions.h>
#include "BluetoothSerial.h"

GEENIE Geenie;
boolean temp = false;


void setup() {
  // put your setup code here, to run once:
  delay(2000);

  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Ready");
  Geenie.set_buttons();
  Geenie.initialize();
  byte ads_addr = Geenie.read_ads_1();
  Serial.println(ads_addr);
  byte ads_addr2 = Geenie.read_ads_2();
  Serial.println(ads_addr2);
  Geenie.activateChannel(1, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(2, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(3, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(4, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(5, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(6, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(7, ADS_GAIN24, ADSINPUT_NORMAL);
  Geenie.activateChannel(8, ADS_GAIN24, ADSINPUT_NORMAL);
  Serial.println("___________________ADC Initialized__________________________");
  delay(400);

  Geenie.initialize_bluetooth();
  Serial.println("___________________Bluetooth Initialized__________________________");

  delay(400);

  Geenie.initialize_oled();
  Serial.println("___________________Oled Initialized__________________________");

  delay(5000);
  // Serial.println("___________________CHIP 1 REGISTERS__________________________");
  // Geenie.RREGS_1(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4
  // Serial.println("___________________CHIP 2 REGISTERS__________________________");
  // Geenie.RREGS_2(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4
  // Serial.println("_____________________________________________________________");
   
  Geenie.start();
}

void loop() {
  if (Geenie.streaming) {
    // if (Geenie.channelDataAvailable_1 && Geenie.channelDataAvailable_2) {
    //   // Read from the ADS(s), store data, set channelDataAvailable flag to false
    //   Geenie.updateChannelData();
    //   Geenie.sendChannelDataSerial(Geenie.PACKET_TYPE_ACCEL);
    // }
    if (Geenie.channelDataAvailable_2) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      Geenie.updateChannelData_2();
      temp=true;
    }
    if (Geenie.channelDataAvailable_1) {
      // Read from the ADS(s), store data, set channelDataAvailable flag to false
      Geenie.updateChannelData_1();
      temp=true;
    }

    if (temp==true) {
      Geenie.sendChannelDataSerialBt(Geenie.PACKET_TYPE_ACCEL);
      temp = false;
    }

    Geenie.displayBattery();
  }
  Geenie.checkForCommands();
}
