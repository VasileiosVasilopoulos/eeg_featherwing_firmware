#ifndef ____ADS1299_H____
#define ____ADS1299_H____

#include <board_definitions.h>
#include <stdio.h>
#include <Arduino.h>
#include <SPI.h>

class ADS1299 {
public:
    void initialize(int _CS, boolean _verbose);
    
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
    byte getDeviceID();
    void ads_drdy();
    byte RREG(byte _address);
    void RREGS(byte _address, byte _numRegistersMinusOne);     
    void printRegisterName(byte _address);
    void WREG(byte _address, byte _value); 
    void WREGS(byte _address, byte _numRegistersMinusOne); 
    void printHex(byte _data);
    void updateChannelData();
    

    void printChannelDataAsText(int N, long int sampleNumber);
    

    //SPI Transfer function
    // byte transfer(byte _data);

    //configuration
    int DRDY, CS; 		// pin numbers for DRDY and CS 
    // int DIVIDER;		// select SPI SCK frequency
    int stat1_1,stat1_2, stat2_1, stat2_2;    // used to hold the status register for boards 1 and 2
    byte regData [24];	// array is used to mirror register data
    byte boardChannelDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];    // array to hold raw channel data
    byte daisyChannelDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte lastBoardDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte lastDaisyDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte meanBoardDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte meanDaisyDataRaw[NUMBER_BYTES_PER_ADS_SAMPLE];
    byte sampleCounter;
    byte sampleCounterBLE;

    // long channelData [16];	// array used when reading channel data board 1+2
    boolean verbose;		// turn on/off Serial feedback
    
    int n_chan_all_boards;

    static const int spiClk = 1000000; // 1 MHz

    SPIClass * vspi = NULL;

    volatile boolean channelDataAvailable;

    boolean streaming;

    short auxData[3]; // This is user faceing

    
    unsigned long lastSampleTime;
};


#endif