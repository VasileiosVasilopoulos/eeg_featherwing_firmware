#include <ads1299.h>
#include <board_definitions.h>



void ADS1299::initialize(int _CS, boolean _isDaisy, boolean _verbose){
	verbose = _verbose;
    isDaisy = _isDaisy;
	// int FREQ = _FREQ;
    CS = _CS;
	
	// vspi->begin(SCLK, DOUT, DIN, CS);
    vspi = new SPIClass(VSPI);

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
    // pinMode(SS, OUTPUT);
	
    
    digitalWrite(SCLK, LOW);
    digitalWrite(DIN, LOW);
    // digitalWrite(SS, HIGH);
    
    // // set as master and enable SPI
    // SPCR |= _BV(MSTR);
    // SPCR |= _BV(SPE);

    // set bit order
    // SPCR &= ~(_BV(DORD)); ////SPI data format is MSB (pg. 25) 
    vspi->setBitOrder(SPI_BYTEORDER);
	
    // set data mode
    // SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_DATA_MODE; //clock polarity = 0; clock phase = 1 (pg. 8)
    vspi->setDataMode(SPI_MODE);
    vspi->setFrequency(SPI_SPEED);
    
    vspi->begin(SCLK, DOUT, DIN, CS);
    // set clock divider
	// switch (FREQ){
	// 	case 8:
	// 		DIVIDER = SPI_CLOCK_DIV_2;
	// 		break;
	// 	case 4:
	// 		DIVIDER = SPI_CLOCK_DIV_4;
	// 		break;
	// 	case 1:
	// 		DIVIDER = SPI_CLOCK_DIV_16;
	// 		break;
	// 	default:
	// 		break;
	// }
    // SPCR = (SPCR & ~SPI_CLOCK_MASK) | (DIVIDER);  // set SCK frequency  
    // SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | (DIVIDER); // by dividing 16MHz system clock
    
    
    
    
    
    // **** ----- End of SPI Setup ----- **** //
    
    // initalize the  data ready chip select and reset pins:
    pinMode(ADS_DRDY, INPUT);
    pinMode(CS, OUTPUT);
	
	digitalWrite(CS,HIGH); 	
	// digitalWrite(ADS_RST,HIGH);
}


//System Commands
void ADS1299::WAKEUP() {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW); 
    vspi->transfer(_WAKEUP);
    digitalWrite(CS, HIGH); 
    delayMicroseconds(3);  		//must wait 4 tCLK cycles before sending another command (Datasheet, pg. 35)
    vspi->endTransaction();
}

void ADS1299::STANDBY() {		// only allowed to send WAKEUP after sending STANDBY
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW);
    vspi->transfer(_STANDBY);
    digitalWrite(CS, HIGH);
    vspi->endTransaction();
}

void ADS1299::RESET() {			// reset all the registers to default settings
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW);
    vspi->transfer(_RESET);
    delayMicroseconds(12);   	//must wait 18 tCLK cycles to execute this command (Datasheet, pg. 35)
    digitalWrite(CS, HIGH);
    vspi->endTransaction();
}

void ADS1299::START() {			//start data conversion 
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW);
    vspi->transfer(_START);
    digitalWrite(CS, HIGH);
    vspi->endTransaction();
}

void ADS1299::STOP() {			//stop data conversion
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW);
    vspi->transfer(_STOP);
    digitalWrite(CS, HIGH);
    vspi->endTransaction();
}

void ADS1299::RDATAC() {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW);
    vspi->transfer(_RDATAC);
    digitalWrite(CS, HIGH);
	delayMicroseconds(3);   
    vspi->endTransaction();
}
void ADS1299::SDATAC() {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    digitalWrite(CS, LOW);
    vspi->transfer(_SDATAC);
    digitalWrite(CS, HIGH);
	delayMicroseconds(3);   //must wait 4 tCLK cycles after executing this command (Datasheet, pg. 37)
    vspi->endTransaction();
}

// Register Read/Write Commands
byte ADS1299::getDeviceID() {			// simple hello world com check
	byte data = RREG(0x00);
	if(verbose){						// verbose otuput
		Serial.print(F("Device ID "));
		printHex(data);	
	}
	return data;
}

byte ADS1299::RREG(byte _address) {		//  reads ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    digitalWrite(CS, LOW); 				//  open SPI
    vspi->transfer(opcode1); 					//  opcode1
    vspi->transfer(0x00); 					//  opcode2
    regData[_address] = vspi->transfer(0x00);//  update mirror location with returned byte
    digitalWrite(CS, HIGH); 			//  close SPI	
    vspi->endTransaction();
	if (verbose){						//  verbose output
		printRegisterName(_address);
		printHex(_address);
		Serial.print(", ");
		printHex(regData[_address]);
		Serial.print(", ");
		for(byte j = 0; j<8; j++){
			Serial.print(bitRead(regData[_address], 7-j));
			if(j!=7) Serial.print(", ");
		}
		
		Serial.println();
	}
	return regData[_address];			// return requested register value
}

// Read more than one register starting at _address
void ADS1299::RREGS(byte _address, byte _numRegistersMinusOne) {
//	for(byte i = 0; i < 0x17; i++){
//		regData[i] = 0;					//  reset the regData array
//	}
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    vspi->beginTransaction(SPISettings());
    byte opcode1 = _address + 0x20; 	//  RREG expects 001rrrrr where rrrrr = _address
    digitalWrite(CS, LOW); 				//  open SPI
    vspi->transfer(opcode1); 					//  opcode1
    vspi->transfer(_numRegistersMinusOne);	//  opcode2
    for(int i = 0; i <= _numRegistersMinusOne; i++){
        regData[_address + i] = vspi->transfer(0x00); 	//  add register byte to mirror array
		}
    digitalWrite(CS, HIGH); 			//  close SPI
    vspi->endTransaction();
	if(verbose){						//  verbose output
		for(int i = 0; i<= _numRegistersMinusOne; i++){
			printRegisterName(_address + i);
			printHex(_address + i);
			Serial.print(", ");
			printHex(regData[_address + i]);
			Serial.print(", ");
			for(int j = 0; j<8; j++){
				Serial.print(bitRead(regData[_address + i], 7-j));
				if(j!=7) Serial.print(", ");
			}
			Serial.println();
		}
    }
}

void ADS1299::WREG(byte _address, byte _value) {	//  Write ONE register at _address
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    vspi->beginTransaction(SPISettings());
    byte opcode1 = _address + 0x40; 	//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS, LOW); 				//  open SPI
    vspi->transfer(opcode1);					//  Send WREG command & address
    vspi->transfer(0x00);						//	Send number of registers to read -1
    vspi->transfer(_value);					//  Write the value to the register
    digitalWrite(CS, HIGH); 			//  close SPI
    vspi->endTransaction();
	regData[_address] = _value;			//  update the mirror array
	if(verbose){						//  verbose output
		Serial.print(F("Register "));
		printHex(_address);
		Serial.println(F(" modified."));
	}
}

void ADS1299::WREGS(byte _address, byte _numRegistersMinusOne) {
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));  
    vspi->beginTransaction(SPISettings());
    byte opcode1 = _address + 0x40;		//  WREG expects 010rrrrr where rrrrr = _address
    digitalWrite(CS, LOW); 				//  open SPI
    vspi->transfer(opcode1);					//  Send WREG command & address
    vspi->transfer(_numRegistersMinusOne);	//	Send number of registers to read -1	
	for (int i=_address; i <=(_address + _numRegistersMinusOne); i++){
		vspi->transfer(regData[i]);			//  Write to the registers
	}	
	digitalWrite(CS,HIGH);				//  close SPI
    vspi->endTransaction();
	if(verbose){
		Serial.print(F("Registers "));
		printHex(_address); Serial.print(F(" to "));
		printHex(_address + _numRegistersMinusOne);
		Serial.println(F(" modified"));
	}
}

void ADS1299::updateChannelData(){
	byte inByte;
	int nchan=8;  //assume 8 channel.  If needed, it automatically changes to 16 automatically in a later block.
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
	digitalWrite(CS, LOW);				//  open SPI
	
	// READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
	for(int i=0; i<3; i++){			//  read 3 byte status register from ADS 1 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi->transfer(0x00);
		stat_1 = (stat_1<<8) | inByte;				
	}
	
	for(int i = 0; i<8; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi->transfer(0x00);
			channelData[i] = (channelData[i]<<8) | inByte;
		}
	}
	
	if (isDaisy) {
		nchan = 16;
		// READ CHANNEL DATA FROM SECOND ADS IN DAISY LINE
		for(int i=0; i<3; i++){			//  read 3 byte status register from ADS 2 (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
			inByte = vspi->transfer(0x00);
			stat_2 = (stat_1<<8) | inByte;				
		}
		
		for(int i = 8; i<16; i++){
			for(int j=0; j<3; j++){		//  read 24 bits of channel data from 2nd ADS in 8 3 byte chunks
				inByte = vspi->transfer(0x00);
				channelData[i] = (channelData[i]<<8) | inByte;
			}
		}
	}
	
	digitalWrite(CS, HIGH);				//  close SPI
    vspi->endTransaction();
	
	//reformat the numbers
	for(int i=0; i<nchan; i++){			// convert 3 byte 2's compliment to 4 byte 2's compliment	
		if(bitRead(channelData[i],23) == 1){	
			channelData[i] |= 0xFF000000;
		}else{
			channelData[i] &= 0x00FFFFFF;
		}
	}
}

//read data
void ADS1299::RDATA() {				//  use in Stop Read Continuous mode when DRDY goes low
	byte inByte;
	stat_1 = 0;							//  clear the status registers
	stat_2 = 0;	
	int nchan = 8;	//assume 8 channel.  If needed, it automatically changes to 16 automatically in a later block.
    // vspi->beginTransaction(SPISettings(SPI_SPEED, SPI_BYTEORDER, SPI_MODE));
    vspi->beginTransaction(SPISettings());
    
    //TODO begin transaction does not end somewhere
	

    digitalWrite(CS, LOW);				//  open SPI
	vspi->transfer(_RDATA);
	
	// READ CHANNEL DATA FROM FIRST ADS IN DAISY LINE
	for(int i=0; i<3; i++){			//  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
		inByte = vspi->transfer(0x00);
		stat_1 = (stat_1<<8) | inByte;				
	}
	
	for(int i = 0; i<8; i++){
		for(int j=0; j<3; j++){		//  read 24 bits of channel data from 1st ADS in 8 3 byte chunks
			inByte = vspi->transfer(0x00);
			channelData[i] = (channelData[i]<<8) | inByte;
		}
	}
	
	if (isDaisy) {
		nchan = 16;
		
		// READ CHANNEL DATA FROM SECOND ADS IN DAISY LINE
		for(int i=0; i<3; i++){			//  read 3 byte status register (1100+LOFF_STATP+LOFF_STATN+GPIO[7:4])
			inByte = vspi->transfer(0x00);
			stat_2 = (stat_1<<8) | inByte;				
		}
		
		for(int i = 8; i<16; i++){
			for(int j=0; j<3; j++){		//  read 24 bits of channel data from 2nd ADS in 8 3 byte chunks
				inByte = vspi->transfer(0x00);
				channelData[i] = (channelData[i]<<8) | inByte;
			}
		}
	}
	
	for(int i=0; i<nchan; i++){			// convert 3 byte 2's compliment to 4 byte 2's compliment	
		if(bitRead(channelData[i],23) == 1){	
			channelData[i] |= 0xFF000000;
		}else{
			channelData[i] &= 0x00FFFFFF;
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