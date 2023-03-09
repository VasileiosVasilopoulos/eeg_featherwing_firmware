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
// #define SPI_SPEED 14000000
#define SPI_SPEED 1000000

#define SPI_BYTEORDER MSBFIRST
#define SPI_MODE SPI_MODE1

//SPI Command Definition Byte Assignments (Datasheet, p35)
#define _WAKEUP 0x02 // Wake-up from standby mode
#define _STANDBY 0x04 // Enter Standby mode
#define _RESET 0x06 // Reset the device registers to default
#define _START 0x08 // Start and restart (synchronize) conversions
#define _STOP 0x0A // Stop conversion
#define _RDATAC 0x10 // Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 0x11 // Stop Read Data Continuous mode
#define _RDATA 0x12 // Read data by command; supports multiple read back

//Register Addresses
#define ID 0x00
#define CONFIG1 0x01
#define CONFIG2 0x02
#define CONFIG3 0x03
#define LOFF 0x04
#define CH1SET 0x05
#define CH2SET 0x06
#define CH3SET 0x07
#define CH4SET 0x08
#define CH5SET 0x09
#define CH6SET 0x0A
#define CH7SET 0x0B
#define CH8SET 0x0C
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define _GPIO 0x14
#define MISC1 0x15
#define MISC2 0x16
#define CONFIG4 0x17


#endif