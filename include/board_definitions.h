#ifndef _____BOARD_DEFINITIONS_H
#define _____BOARD_DEFINITIONS_H


// BOARD PINOUTS
#define BTN_1 26
#define BTN_2 25
#define BTN_3 34
#define BTN_4 39

#define DRDY_ADS_1 27
#define DRDY_ADS_2 36
#define CS_ADS_1 12
#define CS_ADS_2 4
#define CS_SD 33

#define SCLK 5
#define DIN 18
#define DOUT 19

#define SCRN_BTN_A 15
#define SCRN_BTN_B 32
#define SCRN_BTN_C 14
#define I2C_SCL 22
#define I2C_SDA 23

//BOARD OTHER
#define CHANNELS_PER_ADC (4)  // number of EEG channels
#define CHANNELS_PER_BOARD (8)  // number of EEG channels

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
#define NUMBER_CHANNELS_PER_ADS_SAMPLE 24

#define PCKT_START 0xA0	// prefix for data packet error checking
#define PCKT_END 0xC0	// postfix for data packet error checking



// GEENIE_COMMANDS
/** Turning channels off */
#define GEENIE_CHANNEL_OFF_1 '!'
#define GEENIE_CHANNEL_OFF_2 '@'
#define GEENIE_CHANNEL_OFF_3 '#'
#define GEENIE_CHANNEL_OFF_4 '$'
#define GEENIE_CHANNEL_OFF_5 '%'
#define GEENIE_CHANNEL_OFF_6 '^'
#define GEENIE_CHANNEL_OFF_7 '&'
#define GEENIE_CHANNEL_OFF_8 '*'

/** Turn channels on */
#define GEENIE_CHANNEL_ON_1 '1'
#define GEENIE_CHANNEL_ON_2 '2'
#define GEENIE_CHANNEL_ON_3 '3'
#define GEENIE_CHANNEL_ON_4 '4'
#define GEENIE_CHANNEL_ON_5 '5'
#define GEENIE_CHANNEL_ON_6 '6'
#define GEENIE_CHANNEL_ON_7 '7'
#define GEENIE_CHANNEL_ON_8 '8'

/** Start stop commands */
#define GEENIE_START 'm'
#define GEENIE_STOP 'n'
#define GEENIE_START_SD 's'
#define GEENIE_STOP_SD 'w'

/** Test Signal Control Commands
 * 1x - Voltage will be 1 * (VREFP - VREFN) / 2.4 mV
 * 2x - Voltage will be 2 * (VREFP - VREFN) / 2.4 mV
 */
#define GEENIE_TEST_SIGNAL_CONNECT_TO_DC            'p'
#define GEENIE_TEST_SIGNAL_CONNECT_TO_GROUND        '0'
#define GEENIE_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAmST '='
#define GEENIE_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW '-'
#define GEENIE_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST ']'
#define GEENIE_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW '['

/** Channel Setting Commands */
#define GEENIE_CHANNEL_CMD_ADC_Normal      '0'
#define GEENIE_CHANNEL_CMD_ADC_Shorted     '1'
#define GEENIE_CHANNEL_CMD_ADC_BiasDRP     '6'
#define GEENIE_CHANNEL_CMD_ADC_BiasDRN     '7'
#define GEENIE_CHANNEL_CMD_ADC_BiasMethod  '2'
#define GEENIE_CHANNEL_CMD_ADC_MVDD        '3'
#define GEENIE_CHANNEL_CMD_ADC_Temp        '4'
#define GEENIE_CHANNEL_CMD_ADC_TestSig     '5'
#define GEENIE_CHANNEL_CMD_BIAS_INCLUDE    '1'
#define GEENIE_CHANNEL_CMD_BIAS_REMOVE     '0'
#define GEENIE_CHANNEL_CMD_CHANNEL_1       '1'
#define GEENIE_CHANNEL_CMD_CHANNEL_2       '2'
#define GEENIE_CHANNEL_CMD_CHANNEL_3       '3'
#define GEENIE_CHANNEL_CMD_CHANNEL_4       '4'
#define GEENIE_CHANNEL_CMD_CHANNEL_5       '5'
#define GEENIE_CHANNEL_CMD_CHANNEL_6       '6'
#define GEENIE_CHANNEL_CMD_CHANNEL_7       '7'
#define GEENIE_CHANNEL_CMD_CHANNEL_8       '8'
#define GEENIE_CHANNEL_CMD_CHANNEL_9       'Q'
#define GEENIE_CHANNEL_CMD_CHANNEL_10      'W'
#define GEENIE_CHANNEL_CMD_CHANNEL_11      'E'
#define GEENIE_CHANNEL_CMD_CHANNEL_12      'R'
#define GEENIE_CHANNEL_CMD_CHANNEL_13      'T'
#define GEENIE_CHANNEL_CMD_CHANNEL_14      'Y'
#define GEENIE_CHANNEL_CMD_CHANNEL_15      'U'
#define GEENIE_CHANNEL_CMD_CHANNEL_16      'I'
#define GEENIE_CHANNEL_CMD_GAIN_1          '0'
#define GEENIE_CHANNEL_CMD_GAIN_2          '1'
#define GEENIE_CHANNEL_CMD_GAIN_4          '2'
#define GEENIE_CHANNEL_CMD_GAIN_6          '3'
#define GEENIE_CHANNEL_CMD_GAIN_8          '4'
#define GEENIE_CHANNEL_CMD_GAIN_12         '5'
#define GEENIE_CHANNEL_CMD_GAIN_24         '6'
#define GEENIE_CHANNEL_CMD_LATCH           'X'
#define GEENIE_CHANNEL_CMD_POWER_OFF       '1'
#define GEENIE_CHANNEL_CMD_POWER_ON        '0'
#define GEENIE_CHANNEL_CMD_SET             'x'
#define GEENIE_CHANNEL_CMD_SRB1_CONNECT    '1'
#define GEENIE_CHANNEL_CMD_SRB1_DISCONNECT '0'
#define GEENIE_CHANNEL_CMD_SRB2_CONNECT    '1'
#define GEENIE_CHANNEL_CMD_SRB2_DISCONNECT '0'

/** Default Channel Settings */
#define GEENIE_CHANNEL_DEFAULT_ALL_SET 'd'
#define GEENIE_CHANNEL_DEFAULT_ALL_REPORT 'D'

/** LeadOff Impedance Commands */
#define GEENIE_CHANNEL_IMPEDANCE_LATCH                'Z'
#define GEENIE_CHANNEL_IMPEDANCE_SET                  'z'
#define GEENIE_CHANNEL_IMPEDANCE_TEST_SIGNAL_APPLIED    '1'
#define GEENIE_CHANNEL_IMPEDANCE_TEST_SIGNAL_APPLIED_NOT '0'

/** SD card Commands */
#define GEENIE_SD_LOG_FOR_HOUR_1    'G'
#define GEENIE_SD_LOG_FOR_HOUR_2    'H'
#define GEENIE_SD_LOG_FOR_HOUR_4    'J'
#define GEENIE_SD_LOG_FOR_HOUR_12   'K'
#define GEENIE_SD_LOG_FOR_HOUR_24   'L'
#define GEENIE_SD_LOG_FOR_MIN_5     'A'
#define GEENIE_SD_LOG_FOR_MIN_15    'S'
#define GEENIE_SD_LOG_FOR_MIN_30    'F'
#define GEENIE_SD_LOG_FOR_SEC_14    'a'
#define GEENIE_SD_LOG_STOP        'j'

/** Stream Data Commands */
#define GEENIE_STREAM_START  'b'
#define GEENIE_STREAM_STOP   's'

/** Miscellaneous */
#define GEENIE_MISC_QUERY_REGISTER_SETTINGS '?'
#define GEENIE_MISC_SOFT_RESET              'v'

/** 16 Channel Commands */
#define GEENIE_CHANNEL_MAX_NUMBER_8    'c'
#define GEENIE_CHANNEL_MAX_NUMBER_16   'C'

#define GEENIE_BOARD_MODE_SET '/'

#define GEENIE_GET_VERSION 'V'

/** Set sample rate */
#define GEENIE_SAMPLE_RATE_SET '~'

/** Insert marker into the stream */
#define GEENIE_INSERT_MARKER '`'

/** Sync Clocks */
#define GEENIE_TIME_SET '<'
#define GEENIE_TIME_STOP '>'

/** Wifi Stuff */
#define GEENIE_WIFI_ATTACH '{'
#define GEENIE_WIFI_REMOVE '}'
#define GEENIE_WIFI_STATUS ':'
#define GEENIE_WIFI_RESET ';'

#endif