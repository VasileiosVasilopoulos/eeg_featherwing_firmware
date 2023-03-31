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

//BOARD OTHER
#define CHANNELS_PER_BOARD (4)  // number of EEG channels
#define MAX_ADC_CHANNELS (8)  // number of EEG channels


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

//gainCode choices
#define ADS_GAIN01 (0b00000000)
#define ADS_GAIN02 (0b00010000)
#define ADS_GAIN04 (0b00100000)
#define ADS_GAIN06 (0b00110000)
#define ADS_GAIN08 (0b01000000)
#define ADS_GAIN12 (0b01010000)
#define ADS_GAIN24 (0b01100000)

//inputCode choices
#define ADSINPUT_NORMAL (0b00000000)
#define ADSINPUT_SHORTED (0b00000001)
#define ADSINPUT_TESTSIG (0b00000101)
#define ADSINPUT_BIAS_DRP (0b00000110)

//test signal choices...ADS1299 datasheet page 41
#define ADSTESTSIG_AMP_1X (0b00000000)
#define ADSTESTSIG_AMP_2X (0b00000100)
#define ADSTESTSIG_PULSE_SLOW (0b00000000)
#define ADSTESTSIG_PULSE_FAST (0b00000001)
#define ADSTESTSIG_DCSIG (0b00000011)
#define ADSTESTSIG_NOCHANGE (0b11111111)

//Lead-off signal choices
#define LOFF_MAG_6NA (0b00000000)
#define LOFF_MAG_24NA (0b00000100)
#define LOFF_MAG_6UA (0b00001000)
#define LOFF_MAG_24UA (0b00001100)
#define LOFF_FREQ_DC (0b00000000)
#define LOFF_FREQ_7p8HZ (0b00000001)
#define LOFF_FREQ_31p2HZ (0b00000010)
#define LOFF_FREQ_FS_4 (0b00000011)
#define PCHAN (1)
#define NCHAN (2)
#define BOTHCHAN (3)

#define OFF (0)
#define ON (1)

// Baud rates
#define BAUD_RATE 115200
#define BAUD_RATE_BLE 9600
#define BAUD_RATE_MIN_NO_AVG 200000

// File transmissions
#define BOP 'A' // Begining of stream packet

#define NUMBER_BYTES_PER_ADS_SAMPLE 24

#define PCKT_START 0xA0	// prefix for data packet error checking
#define PCKT_END 0xC0	// postfix for data packet error checking


#endif