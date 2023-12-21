#ifndef ____ADS1299_H____
#define ____ADS1299_H____

#include <board_definitions.h>
#include <stdio.h>
#include <Arduino.h>
#include <SPI.h>

class ADS1299 {
public:
    void initialize(int _CS_1, int _DRDY_1, int CS_2, int _DRDY_2, boolean _verbose);
    
    //ADS1299 SPI Command Definitions (Datasheet, p35)
    //System Commands
    void WAKEUP();
    void STANDBY();
    void RESET();
    void START();
    void STOP();
    
    //Data Read Commands
    void RDATAC();
    void SDATAC();
    void RDATA();
    
    //Register Read/Write Commands
    byte getDeviceID_1();
    byte getDeviceID_2();
    void ads_drdy_1();
    void ads_drdy_2();
    byte RREG_1(byte _address);
    byte RREG_2(byte _address);
    void RREGS_1(byte _address, byte _numRegistersMinusOne);     
    void RREGS_2(byte _address, byte _numRegistersMinusOne);
    void printRegisterName(byte _address);
    void WREG_1(byte _address, byte _value); 
    void WREG_2(byte _address, byte _value); 
    void WREG_BOTH(byte _address, byte _value); 
    void WREGS_1(byte _address, byte _numRegistersMinusOne); 
    void WREGS_2(byte _address, byte _numRegistersMinusOne); 
    void WREGS_BOTH(byte _address, byte _numRegistersMinusOne); 
    void printHex(byte _data);
    void updateChannelData();
    void updateChannelData_1();
    void updateChannelData_2();

    void printChannelDataAsText(int N, long int sampleNumber);
    

    //SPI Transfer function
    // byte transfer(byte _data);

    //configuration
    int DRDY_1, CS_1, DRDY_2, CS_2; 		// pin numbers for DRDY and CS 
    // int DIVIDER;		// select SPI SCK frequency
    int stat1_1,stat1_2, stat2_1, stat2_2;    // used to hold the status register for boards 1 and 2
    byte regData_1 [24];	// array is used to mirror register data
    byte regData_2 [24];	// array is used to mirror register data
    byte boardChannelDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];    // array to hold raw channel data
    int boardChannelDataInt[NUMBER_CHANNELS_PER_ADS_SAMPLE];    // array used when reading channel data as ints
    int meanBoardChannelDataInt[NUMBER_CHANNELS_PER_ADS_SAMPLE];
    int lastBoardChannelDataInt[NUMBER_CHANNELS_PER_ADS_SAMPLE];

    byte daisyChannelDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte lastBoardDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte lastDaisyDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte meanBoardDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte meanDaisyDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte sampleCounter;
    byte sampleCounterBLE;

    boolean firstDataPacket1;
    boolean firstDataPacket2;
    // long channelData [16];	// array used when reading channel data board 1+2
    boolean verbose;		// turn on/off Serial feedback
    
    int n_chan_all_boards;

    static const int spiClk = 1000000; // 1 MHz

    SPIClass * vspi_1 = NULL;
    // SPIClass * vspi_2 = NULL;

    volatile boolean channelDataAvailable_1, channelDataAvailable_2;

    boolean streaming;

    short auxData[3]; // This is user faceing

    
    unsigned long lastSampleTime;
};


#endif