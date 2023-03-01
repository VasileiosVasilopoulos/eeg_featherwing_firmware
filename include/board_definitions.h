#ifndef _____BOARD_DEFINITIONS_H
#define _____BOARD_DEFINITIONS_H

//FROM OPENBCI
#define BOTH_ADS 5	        // Slave Select Both ADS chips


// BOARD PINOUTS
#define BTN_1 A0
#define BTN_2 25
#define BTN_3 34
#define BTN_4 39
#define ADS_DRDY 36
#define BOARD_ADS 4
#define SCLK 5
#define DIN 18
#define DOUT 19
#define ACC_INT_1 13
#define ACC_INT_2 12
#define DAISY_ADS 27
#define SD_CS 33
#define SCRN_BTN_A 15
#define SCRN_BTN_B 32
#define SCRN_BTN_C 14
#define I2C_SCL 22
#define I2C_SDA 23

// NEED CABLE
#define ADS_RST 16

#define SERIAL_BAUDRATE 115200

//SPI Settings
#define SPI_SPEED 14000000
#define SPI_BYTEORDER MSBFIRST
#define SPI_MODE SPI_MODE0

//SPI Command Definition Byte Assignments (Datasheet, p35)
#define _WAKEUP 0x02 // Wake-up from standby mode
#define _STANDBY 0x04 // Enter Standby mode
#define _RESET 0x06 // Reset the device registers to default
#define _START 0x08 // Start and restart (synchronize) conversions
#define _STOP 0x0A // Stop conversion
#define _RDATAC 0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 0x11 // Stop Read Data Continuous mode
#define _RDATA 0x12 // Read data by command; supports multiple read back


#endif