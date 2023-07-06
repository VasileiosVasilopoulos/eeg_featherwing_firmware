#include <ads1299.h>
#include <board_definitions.h>

// using namespace std;

#include <FunctionalInterrupt.h>


void ADS1299::initialize(int _CS_1 ,int _DRDY_1, int _CS_2, int _DRDY_2, boolean _verbose){ 
	verbose = _verbose;
    firstDataPacket = true;
    n_chan_all_boards = CHANNELS_PER_BOARD;

    // WREG(CONFIG1,0b11101100); delay(1);

	// int FREQ = _FREQ;
    CS_1 = _CS_1;
    CS_2 = _CS_2;
    DRDY_1 = _DRDY_1;
    DRDY_2 = _DRDY_2;
    pinMode(CS_1, OUTPUT);
    digitalWrite(CS_1,HIGH);
    pinMode(CS_2, OUTPUT);
    digitalWrite(CS_2,HIGH); 	
	// vspi->begin(SCLK, DOUT, DIN, CS);
    vspi_1 = new SPIClass(VSPI);
    vspi_1->begin(SCLK, DOUT, DIN, CS_1);
    // vspi_2 = new SPIClass(VSPI);
    // vspi_2->begin(SCLK, DOUT, DIN, CS_2);
	delay(50);				// recommended power up sequence requiers Tpor (~32mS)	
	pinMode(ADS_RST,OUTPUT);
	pinMode(ADS_RST,LOW);
	delayMicroseconds(4);	// toggle reset pin
	pinMode(ADS_RST,HIGH);
	delayMicroseconds(20);	// recommended to wait 18 Tclk before using device (~8uS);

    // **** ----- SPI Setup ----- **** //
    
    // Set direction register for SCK and MOSI pin.
    // MISO pin automatically overrides to INPUT.
    // When the SS pin is set as OUTPUT, it can be used as
    // a general purpose output port (it doesn't influence
    // SPI operations).
    
    pinMode(SCLK, OUTPUT);
    pinMode(DIN, OUTPUT);
    digitalWrite(SCLK, LOW);
    digitalWrite(DIN, LOW);
    // **** ----- End of SPI Setup ----- **** //	

    vspi_1->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    // vspi_2->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));

    pinMode(DRDY_1, INPUT_PULLUP);
    pinMode(DRDY_2, INPUT_PULLUP);
    attachInterrupt(DRDY_1, std::bind(&ADS1299::ads_drdy_1,this), FALLING);
    attachInterrupt(DRDY_2, std::bind(&ADS1299::ads_drdy_2,this), FALLING);
	// digitalWrite(ADS_RST,HIGH);
}

void IRAM_ATTR ADS1299::ads_drdy_1(){
    // RDATA();
    channelDataAvailable_1 = true;
    // updateChannelData();
    // printChannelDataAsText(8, 0);
};

void IRAM_ATTR ADS1299::ads_drdy_2(){
    // Serial.println("DRDY Low 2");
    // RDATA();
    channelDataAvailable_2 = true;
    // updateChannelData();
    // printChannelDataAsText(8, 0);
};

//System Commands
void ADS1299::WAKEUP() {
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_WAKEUP);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_WAKEUP);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
}

void ADS1299::STANDBY() {		// only allowed to send WAKEUP after sending STANDBY
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_STANDBY);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_STANDBY);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
}

void ADS1299::RESET() {			// reset all the registers to default settings
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_RESET);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_RESET);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
}

void ADS1299::START() {			//start data conversion 
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_START);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_START);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
}

void ADS1299::STOP() {			//stop data conversion
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_STOP);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_STOP);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
}

void ADS1299::RDATAC() {
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_RDATAC);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_RDATAC);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
}
void ADS1299::SDATAC() {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    digitalWrite(CS_1, LOW); 
    vspi_1->transfer(_SDATAC);
    digitalWrite(CS_1, HIGH); 
    delayMicroseconds(3);  

    digitalWrite(CS_2, LOW); 
    vspi_1->transfer(_SDATAC);
    digitalWrite(CS_2, HIGH); 
    delayMicroseconds(3);  
    // vspi->endTransaction();
}

// Register Read/Write Commands
byte ADS1299::getDeviceID_1() {			// simple hello world com check
	byte data = RREG_1(0x00);
	if(verbose){						// verbose otuput
		Serial.print(F("Device ID "));
		printHex(data);	
	}
	return data;
}

// Register Read/Write Commands
byte ADS1299::getDeviceID_2() {			// simple hello world com check
	byte data = RREG_2(0x00);
	if(verbose){						// verbose otuput
		Serial.print(F("Device ID "));
		printHex(data);	
	}
	return data;
}

byte ADS1299::RREG_1(byte _address) {		//  reads ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    digitalWrite(CS_1, LOW); 				//  open SPI
    vspi_1->transfer(opcode1); 					//  opcode1
    vspi_1->transfer(0x00); 					//  opcode2
    regData_1[_address] = vspi_1->transfer(0x00);//  update mirror location with returned byte
    digitalWrite(CS_1, HIGH); 			//  close SPI	
    // vspi->endTransaction();
	if (verbose){						//  verbose output
		printRegisterName(_address);
		printHex(_address);
		Serial.print(", ");
		printHex(regData_1[_address]);
		Serial.print(", ");
		for(byte j = 0; j<8; j++){
			Serial.print(bitRead(regData_1[_address], 7-j));
			if(j!=7) Serial.print(", ");
		}
		
		Serial.println();
	}
	return regData_1[_address];			// return requested register value
}

byte ADS1299::RREG_2(byte _address) {		//  reads ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    digitalWrite(CS_2, LOW); 				//  open SPI
    vspi_1->transfer(opcode1); 					//  opcode1
    vspi_1->transfer(0x00); 					//  opcode2
    regData_2[_address] = vspi_1->transfer(0x00);//  update mirror location with returned byte
    digitalWrite(CS_2, HIGH); 			//  close SPI	
    // vspi->endTransaction();
	if (verbose){						//  verbose output
		printRegisterName(_address);
		printHex(_address);
		Serial.print(", ");
		printHex(regData_2[_address]);
		Serial.print(", ");
		for(byte j = 0; j<8; j++){
			Serial.print(bitRead(regData_2[_address], 7-j));
			if(j!=7) Serial.print(", ");
		}
		
		Serial.println();
	}
	return regData_2[_address];			// return requested register value
}

// Read more than one register starting at _address
void ADS1299::RREGS_1(byte _address, byte _numRegistersMinusOne) {
//	for(byte i = 0; i < 0x17; i++){
//		regData[i] = 0;					//  reset the regData array
//	}
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    digitalWrite(CS_1, LOW); 				//  open SPI
    vspi_1->transfer(opcode1); 					//  opcode1
    vspi_1->transfer(_numRegistersMinusOne);	//  opcode2
    for(int i = 0; i <= _numRegistersMinusOne; i++){
        regData_1[_address + i] = vspi_1->transfer(0x00); 	//  add register byte to mirror array
		}
    digitalWrite(CS_1, HIGH); 			//  close SPI
    // vspi->endTransaction();
	if(verbose){						//  verbose output
		for(int i = 0; i<= _numRegistersMinusOne; i++){
			printRegisterName(_address + i);
			printHex(_address + i);
			Serial.print(", ");
			printHex(regData_1[_address + i]);
			Serial.print(", ");
			for(int j = 0; j<8; j++){
				Serial.print(bitRead(regData_1[_address + i], 7-j));
				if(j!=7) Serial.print(", ");
			}
			Serial.println();
		}
    }
}

// Read more than one register starting at _address
void ADS1299::RREGS_2(byte _address, byte _numRegistersMinusOne) {
//	for(byte i = 0; i < 0x17; i++){
//		regData[i] = 0;					//  reset the regData array
//	}
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    digitalWrite(CS_2, LOW); 				//  open SPI
    vspi_1->transfer(opcode1); 					//  opcode1
    vspi_1->transfer(_numRegistersMinusOne);	//  opcode2
    for(int i = 0; i <= _numRegistersMinusOne; i++){
        regData_1[_address + i] = vspi_1->transfer(0x00); 	//  add register byte to mirror array
		}
    digitalWrite(CS_2, HIGH); 			//  close SPI
    // vspi->endTransaction();
	if(verbose){						//  verbose output
		for(int i = 0; i<= _numRegistersMinusOne; i++){
			printRegisterName(_address + i);
			printHex(_address + i);
			Serial.print(", ");
			printHex(regData_2[_address + i]);
			Serial.print(", ");
			for(int j = 0; j<8; j++){
				Serial.print(bitRead(regData_2[_address + i], 7-j));
				if(j!=7) Serial.print(", ");
			}
			Serial.println();
		}
    }
}


void ADS1299::WREG_1(byte _address, byte _value) {	//  Write ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x40; 	//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_1, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(0x00);						//	Send number of registers to read -1
    vspi_1->transfer(_value);					//  Write the value to the register
    digitalWrite(CS_1, HIGH); 			//  close SPI
    // vspi->endTransaction();
	regData_1[_address] = _value;			//  update the mirror array
	if(verbose){						//  verbose output
		Serial.print(F("Register "));
		printHex(_address);
		Serial.println(F(" modified."));
	}
}

void ADS1299::WREG_2(byte _address, byte _value) {	//  Write ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x40; 	//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_2, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(0x00);						//	Send number of registers to read -1
    vspi_1->transfer(_value);					//  Write the value to the register
    digitalWrite(CS_2, HIGH); 			//  close SPI
    // vspi->endTransaction();
	regData_2[_address] = _value;			//  update the mirror array
	if(verbose){						//  verbose output
		Serial.print(F("Register "));
		printHex(_address);
		Serial.println(F(" modified."));
	}
}

void ADS1299::WREG_BOTH(byte _address, byte _value) {	//  Write ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x40; 	//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_1, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(0x00);						//	Send number of registers to read -1
    vspi_1->transfer(_value);					//  Write the value to the register
    digitalWrite(CS_1, HIGH); 			//  close SPI
	regData_1[_address] = _value;			//  update the mirror array

    digitalWrite(CS_2, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(0x00);						//	Send number of registers to read -1
    vspi_1->transfer(_value);					//  Write the value to the register
    digitalWrite(CS_2, HIGH); 			//  close SPI
    // vspi->endTransaction();
	regData_2[_address] = _value;			//  update the mirror array
	if(verbose){						//  verbose output
		Serial.print(F("Register "));
		printHex(_address);
		Serial.println(F(" modified."));
	}
}

void ADS1299::WREGS_1(byte _address, byte _numRegistersMinusOne) {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x40;		//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_1, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(_numRegistersMinusOne);	//	Send number of registers to read -1	
	for (int i=_address; i <=(_address + _numRegistersMinusOne); i++){
		vspi_1->transfer(regData_1[i]);			//  Write to the registers
	}	
	digitalWrite(CS_1,HIGH);				//  close SPI
    // vspi->endTransaction();
	if(verbose){
		Serial.print(F("Registers "));
		printHex(_address); Serial.print(F(" to "));
		printHex(_address + _numRegistersMinusOne);
		Serial.println(F(" modified"));
	}
}

void ADS1299::WREGS_2(byte _address, byte _numRegistersMinusOne) {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    byte opcode1 = _address + 0x40;		//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_2, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(_numRegistersMinusOne);	//	Send number of registers to read -1	
	for (int i=_address; i <=(_address + _numRegistersMinusOne); i++){
		vspi_1->transfer(regData_2[i]);			//  Write to the registers
	}	
	digitalWrite(CS_2,HIGH);				//  close SPI
    // vspi->endTransaction();
	if(verbose){
		Serial.print(F("Registers "));
		printHex(_address); Serial.print(F(" to "));
		printHex(_address + _numRegistersMinusOne);
		Serial.println(F(" modified"));
	}
}

void ADS1299::WREGS_BOTH(byte _address, byte _numRegistersMinusOne) {
    byte opcode1 = _address + 0x40;		//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_1, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(_numRegistersMinusOne);	//	Send number of registers to read -1	
	for (int i=_address; i <=(_address + _numRegistersMinusOne); i++){
		vspi_1->transfer(regData_1[i]);			//  Write to the registers
	}	
	digitalWrite(CS_1,HIGH);				//  close SPI
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    opcode1 = _address + 0x40;		//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS_2, LOW); 				//  open SPI
    vspi_1->transfer(opcode1);					//  Send WREG command & address
    vspi_1->transfer(_numRegistersMinusOne);	//	Send number of registers to read -1	
	for (int i=_address; i <=(_address + _numRegistersMinusOne); i++){
		vspi_1->transfer(regData_2[i]);			//  Write to the registers
	}	
	digitalWrite(CS_2,HIGH);				//  close SPI
    // vspi->endTransaction();
	if(verbose){
		Serial.print(F("Registers "));
		printHex(_address); Serial.print(F(" to "));
		printHex(_address + _numRegistersMinusOne);
		Serial.println(F(" modified"));
	}
}


void ADS1299::updateChannelData(){

    channelDataAvailable_1 = false;
    channelDataAvailable_2 = false;
    lastSampleTime = millis();
    // Serial.println(millis());

	byte inByte;
    int bytecounter = 0;


    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
	digitalWrite(CS_1, LOW);				//  open SPI
	// READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
	for(int i=0; i<3; i++){			//  read 3 byte status register from ADS 1 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi_1->transfer(0x00);
		stat1_1 = (stat1_1<<8) | inByte;				
	}
	for(int i = 0; i<CHANNELS_PER_ADC; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi_1->transfer(0x00);
			boardChannelDataRaw[bytecounter] = inByte;
            bytecounter++;
            boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte;
		}
	}
	digitalWrite(CS_1, HIGH);				//  close SPI

	digitalWrite(CS_2, LOW);				//  open SPI
    for(int i=0; i<3; i++){			//  read 3 byte status register from ADS 1 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi_1->transfer(0x00);
		stat1_2 = (stat1_2<<8) | inByte;				
	}
	
	for(int i = 4; i<4+CHANNELS_PER_ADC; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi_1->transfer(0x00);
			boardChannelDataRaw[bytecounter] = inByte;
            bytecounter++;
            boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte;
		}
	}
    digitalWrite(CS_2, HIGH);				//  close SPI

    // vspi->endTransaction();
	
	//reformat the numbers
	// for(int i=0; i<nchan; i++){			// convert 3 byte 2's compliment to 4 byte 2's compliment	
	// 	if(bitRead(boardChannelDataRaw[i],23) == 1){	
	// 		boardChannelDataRaw[i] |= 0xFF000000;
	// 	}else{
	// 		boardChannelDataRaw[i] &= 0x00FFFFFF;
	// 	}
	// }
}

void ADS1299::updateChannelData_1(){

    channelDataAvailable_1 = false;
    lastSampleTime = millis();
    // Serial.println(millis());

	byte inByte;
    int bytecounter = 0;

    if (!firstDataPacket) {
        for (int i = 0; i < CHANNELS_PER_ADC; i++){                  // shift and average the byte arrays
            lastBoardChannelDataInt[i] = boardChannelDataInt[i]; // remember the last samples
        }
    }


    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
	digitalWrite(CS_1, LOW);				//  open SPI
	// READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
	for(int i=0; i<3; i++){			//  read 3 byte status register from ADS 1 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi_1->transfer(0x00);
		stat1_1 = (stat1_1<<8) | inByte;				
	}
	for(int i = 0; i<CHANNELS_PER_ADC; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi_1->transfer(0x00);
			boardChannelDataRaw[bytecounter] = inByte;
            bytecounter++;
            boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte;
		}
	}
	digitalWrite(CS_1, HIGH);				//  close SPI

      // need to convert 24bit to 32bit if using the filter
    for (int i = 0; i < CHANNELS_PER_ADC; i++)
    { // convert 3 byte 2's compliment to 4 byte 2's compliment
        if (bitRead(boardChannelDataInt[i], 23) == 1)
        {
        boardChannelDataInt[i] |= 0xFF000000;
        }
        else
        {
        boardChannelDataInt[i] &= 0x00FFFFFF;
        }
    }

    if (!firstDataPacket) {
        bytecounter = 0;
        for (int i = 0; i < CHANNELS_PER_ADC; i++) { // take the average of this and the last sample
            meanBoardChannelDataInt[i] = (lastBoardChannelDataInt[i] + boardChannelDataInt[i]) / 2;
        }
        for (int i = 0; i < CHANNELS_PER_BOARD; i++)
        { // place the average values in the meanRaw array
            for (int b = 2; b >= 0; b--) {
                meanBoardDataRaw[bytecounter] = (meanBoardChannelDataInt[i] >> (b * 8)) & 0xFF;
                bytecounter++;
            }
        }
    }

    if (firstDataPacket == true) {
        firstDataPacket = false;
    }
}

void ADS1299::updateChannelData_2(){
    channelDataAvailable_2 = false;
    lastSampleTime = millis();
    // Serial.println(millis());

    if (!firstDataPacket) {
	for(int i = 4; i<4+CHANNELS_PER_ADC; i++){                 // shift and average the byte arrays
            lastBoardChannelDataInt[i] = boardChannelDataInt[i]; // remember the last samples
        }
    }


	byte inByte;
    int bytecounter = 12;

	digitalWrite(CS_2, LOW);				//  open SPI
    for(int i=0; i<3; i++){			//  read 3 byte status register from ADS 1 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi_1->transfer(0x00);
		stat1_2 = (stat1_2<<8) | inByte;				
	}
	
	for(int i = 4; i<4+CHANNELS_PER_ADC; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi_1->transfer(0x00);
			boardChannelDataRaw[bytecounter] = inByte;
            bytecounter++;
            boardChannelDataInt[i] = (boardChannelDataInt[i] << 8) | inByte;
		}
	}
    digitalWrite(CS_2, HIGH);				//  close SPI

      // need to convert 24bit to 32bit if using the filter
	for(int i = 4; i<4+CHANNELS_PER_ADC; i++){
        if (bitRead(boardChannelDataInt[i], 23) == 1)
        {
        boardChannelDataInt[i] |= 0xFF000000;
        }
        else
        {
        boardChannelDataInt[i] &= 0x00FFFFFF;
        }
    }

    if (!firstDataPacket) {
        bytecounter = 0;
	for(int i = 4; i<4+CHANNELS_PER_ADC; i++){
            meanBoardChannelDataInt[i] = (lastBoardChannelDataInt[i] + boardChannelDataInt[i]) / 2;
        }
        for (int i = 0; i < CHANNELS_PER_BOARD; i++)
        { // place the average values in the meanRaw array
            for (int b = 2; b >= 0; b--) {
                meanBoardDataRaw[bytecounter] = (meanBoardChannelDataInt[i] >> (b * 8)) & 0xFF;
                bytecounter++;
            }
        }
    }

    if (firstDataPacket == true) {
        firstDataPacket = false;
    }
}



//read data
void ADS1299::RDATA() {				//  use in Stop Read Continuous mode when DRDY goes low
	byte inByte;
	stat1_1 = 0;
    stat1_2 = 0;							//  clear the status registers
	stat2_1 = 0;
    stat2_2 = 0;	
	int nchan = 8;	//assume 8 channel.  If needed, it automatically changes to 16 automatically in a later block.
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    
    
    //TODO begin transaction does not end somewhere
	

    digitalWrite(CS_1, LOW);				//  open SPI
	vspi_1->transfer(_RDATA);
	
	// READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
	for(int i=0; i<3; i++){			//  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi_1->transfer(0x00);
		stat1_1 = (stat1_1<<8) | inByte;				
	}
	
	for(int i = 0; i<4; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi_1->transfer(0x00);
			boardChannelDataRaw[i] = (boardChannelDataRaw[i]<<8) | inByte;
		}
	}

    digitalWrite(CS_1, HIGH);				//  close SPI
    digitalWrite(CS_2, LOW);				//  open SPI

    for(int i=0; i<3; i++){			//  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi_1->transfer(0x00);
		stat1_2 = (stat1_2<<8) | inByte;				
	}
	
	for(int i = 4; i<8; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi_1->transfer(0x00);
			boardChannelDataRaw[i] = (boardChannelDataRaw[i]<<8) | inByte;
		}
	}
	
	for(int i=0; i<nchan; i++){			// convert 3 byte 2's compliment to 4 byte 2's compliment	
		if(bitRead(boardChannelDataRaw[i],23) == 1){	
			boardChannelDataRaw[i] |= 0xFF000000;
		}else{
			boardChannelDataRaw[i] &= 0x00FFFFFF;
		}
	}
	
    
}

// String-Byte converters for RREG and WREG
void ADS1299::printRegisterName(byte _address) {
    if(_address == ID){
        Serial.print(F("ID, ")); //the "F" macro loads the string directly from Flash memory, thereby saving RAM
    }
    else if(_address == CONFIG1){
        Serial.print(F("CONFIG1, "));
    }
    else if(_address == CONFIG2){
        Serial.print(F("CONFIG2, "));
    }
    else if(_address == CONFIG3){
        Serial.print(F("CONFIG3, "));
    }
    else if(_address == LOFF){
        Serial.print(F("LOFF, "));
    }
    else if(_address == CH1SET){
        Serial.print(F("CH1SET, "));
    }
    else if(_address == CH2SET){
        Serial.print(F("CH2SET, "));
    }
    else if(_address == CH3SET){
        Serial.print(F("CH3SET, "));
    }
    else if(_address == CH4SET){
        Serial.print(F("CH4SET, "));
    }
    else if(_address == CH5SET){
        Serial.print(F("CH5SET, "));
    }
    else if(_address == CH6SET){
        Serial.print(F("CH6SET, "));
    }
    else if(_address == CH7SET){
        Serial.print(F("CH7SET, "));
    }
    else if(_address == CH8SET){
        Serial.print(F("CH8SET, "));
    }
    else if(_address == BIAS_SENSP){
        Serial.print(F("BIAS_SENSP, "));
    }
    else if(_address == BIAS_SENSN){
        Serial.print(F("BIAS_SENSN, "));
    }
    else if(_address == LOFF_SENSP){
        Serial.print(F("LOFF_SENSP, "));
    }
    else if(_address == LOFF_SENSN){
        Serial.print(F("LOFF_SENSN, "));
    }
    else if(_address == LOFF_FLIP){
        Serial.print(F("LOFF_FLIP, "));
    }
    else if(_address == LOFF_STATP){
        Serial.print(F("LOFF_STATP, "));
    }
    else if(_address == LOFF_STATN){
        Serial.print(F("LOFF_STATN, "));
    }
    else if(_address == _GPIO){
        Serial.print(F("GPIO, "));
    }
    else if(_address == MISC1){
        Serial.print(F("MISC1, "));
    }
    else if(_address == MISC2){
        Serial.print(F("MISC2, "));
    }
    else if(_address == CONFIG4){
        Serial.print(F("CONFIG4, "));
    }
}

// Used for printing HEX in verbose feedback mode
void ADS1299::printHex(byte _data){
	Serial.print("0x");
    if(_data < 0x10) Serial.print("0");
    Serial.print(_data, HEX);
}

//print as text each channel's data
//   print channels 1-N (where N is 1-8...anything else will return with no action)
//   sampleNumber is a number that, if greater than zero, will be printed at the start of the line
void ADS1299::printChannelDataAsText(int N, long int sampleNumber)
{
	//check the inputs
	// if ((N < 1) || (N > n_chan_all_boards)) return;
	
	//print the sample number, if not disabled
	if (sampleNumber > 0) {
		Serial.print(sampleNumber);
		Serial.print(", ");
	}

	//print each channel
	for (int chan = 0; chan < N; chan++ )
	{
		Serial.print(boardChannelDataRaw[chan]);
		Serial.print(", ");
	}
	
	//print end of line
	Serial.println();
};
