#include <board_functions.h>
#include <board_definitions.h>
#include <Arduino.h>
#include <ads1299.h>
#include <FunctionalInterrupt.h>
#include "BluetoothSerial.h"
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>

Adafruit_SH1107 display = Adafruit_SH1107(64, 128, &Wire);


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

// const char *pin = "1234"; // Change this to more secure PIN.
String device_name = "Geenie";

// BUTTON INTERRUPTS

volatile int button_pressed = 0;

void IRAM_ATTR GEENIE::btn_1_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 1 pressed");
        start();
        streaming = true;
    }
};

void IRAM_ATTR GEENIE::btn_2_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 2 pressed");
        stop();
        streaming = false;
    }
};

void IRAM_ATTR GEENIE::btn_3_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 3 pressed");
    }
};

void IRAM_ATTR GEENIE::btn_4_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 4 pressed");
    }
};



void GEENIE::set_buttons(){
  pinMode(BTN_1, INPUT_PULLUP);
//   attachInterrupt(BTN_1, btn_1_isr, FALLING);
  attachInterrupt(BTN_1, std::bind(&GEENIE::btn_1_isr,this), FALLING);

  pinMode(BTN_2, INPUT_PULLUP);
//   attachInterrupt(BTN_2, btn_2_isr, FALLING);
  attachInterrupt(BTN_2, std::bind(&GEENIE::btn_2_isr,this), FALLING);

  pinMode(BTN_3, INPUT_PULLUP);
//   attachInterrupt(BTN_3, btn_3_isr, FALLING);
  attachInterrupt(BTN_3, std::bind(&GEENIE::btn_3_isr,this), FALLING);

  pinMode(BTN_4, INPUT_PULLUP);
//   attachInterrupt(BTN_4, btn_4_isr, FALLING);
  attachInterrupt(BTN_4, std::bind(&GEENIE::btn_4_isr,this), FALLING);
}

void GEENIE::initialize(){
    // Serial.begin(SERIAL_BAUDRATE);
    bt_connected = false;
    
    ADS1299::initialize(CS_ADS_1, DRDY_ADS_1, CS_ADS_2, DRDY_ADS_2, true);
    delay(100);

    verbose = true;
    streaming = false;
    lastSampleTime = 0;

    setSRB();
    delay(500);

    reset();

    n_chan_all_boards = CHANNELS_PER_BOARD;

    
    //Finalize the bias setup...activate buffer and use internal reference for center of bias creation, datasheet PDF p42
    ADS1299::WREG_1(CONFIG3,0b11111100); delay(1);
    ADS1299::WREG_2(CONFIG3,0b11101000); delay(1);
    //set default state for internal test signal
    //ADS1299::WREG(CONFIG2,0b11010000);delay(1);   //set internal test signal, default amplitude, default speed, datasheet PDF Page 41
    //ADS1299::WREG(CONFIG2,0b11010001);delay(1);   //set internal test signal, default amplitude, 2x speed, datasheet PDF Page 41
    // configureInternalTestSignal(ADSTESTSIG_AMP_1X,ADSTESTSIG_PULSE_FAST); //set internal test signal, default amplitude, 2x speed, datasheet PDF Page 41

    //set default state for lead off detection
    // configureLeadOffDetection(LOFF_MAG_6NA,LOFF_FREQ_31p2HZ);

}

void GEENIE::initialize_oled(){
  if(!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println(F("SSD1306 allocation failed"));
    oled_available = false;
  } else {
    oled_available = true;
    line1 = (char*)"";
    line2 = (char*)"";
    line3 = (char*)"";
    line4 = (char*)"";
    line5 = (char*)"";
    line6 = (char*)"";

    display.clearDisplay();
    display.display();
    display.setRotation(1);
    delay(1000);

    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,0);
    line1 = "Geenie v1.0";
    line6 = "1|2|3|4|5|6|7|8|";
    drawLines();
  }
}

void GEENIE::drawLines(){
  if (oled_available){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(line1);
    display.println(line2);
    display.println(line3);
    display.println(line4);
    display.println(line5);
    display.println(line6);
    for (int i=1;i<=8;i++){
      display.print(line7[i-1]);
      display.print('|');
    }
    display.display();
  }
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("Client Connected");
  }
}

// void GEENIE::callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
// // Callback function implementation
//     bt_connected = true;
// }

void GEENIE::initialize_bluetooth(){
    SerialBT.register_callback(callback);

    bt_connected = true;
    if(!SerialBT.begin(device_name)){
      Serial.println("An error occurred initializing Bluetooth");
    }else{
      Serial.println("Bluetooth initialized");
    }
    // SerialBT.setPin(pin);
    Serial.println("Bluetooth Started! Ready to pair...");
}



void GEENIE::reset(){
    ADS1299::RESET();             // send RESET command to default all registers
    ADS1299::WREG_1(CONFIG1, 0b11010110);delay(100);
    ADS1299::WREG_2(CONFIG1, 0b11010110);delay(100);
    ADS1299::SDATAC();            // exit Read Data Continuous mode to communicate with ADS

    // delay(1000);

    ADS1299::WREG_1(BIAS_SENSP, 0b00001111);delay(100);
    ADS1299::WREG_2(BIAS_SENSP, 0b00000000);delay(100);
    ADS1299::WREG_BOTH(_GPIO, 0b00000000);delay(100);
    // ADS1299::WREG(CONFIG1, 0x96);delay(100);
    // ADS1299::WREG(CONFIG2, 0xC0);delay(100);
    // ADS1299::WREG(CH1SET, 0x01);delay(100);
    // ADS1299::WREG(CH2SET, 0x01);delay(100);
    // ADS1299::WREG(CH3SET, 0x01);delay(100);
    // ADS1299::WREG(CH4SET, 0x01);delay(100);

    // ADS1299::RREGS(0x00,0x17);     // read all registers starting at ID and ending at CONFIG4

    // ADS1299::START();delay(100);
    // ADS1299::RDATAC();delay(100);
     
    // turn off all channels
    for (int chan=1; chan <= CHANNELS_PER_BOARD; chan++) {
        deactivateChannel(chan);  //turn off the channel
        changeChannelLeadOffDetection(chan,OFF,BOTHCHAN); //turn off any impedance monitoring
    }

    setSRB1(use_SRB1());  //set whether SRB1 is active or not
    // setAutoBiasGeneration(true); //configure ADS1299 so that bias is generated based on channel state
}

//deactivate the given channel...note: stops data colleciton to issue its commands
//  N is the channel number: 1-8
// 
void GEENIE::deactivateChannel(int N)
{
  byte reg, config;

  line7[N-1] = 'X';
	
  //check the inputs
  if ((N < 1) || (N > CHANNELS_PER_BOARD)) return;
  
  ADS1299::SDATAC(); delay(1);      // exit Read Data Continuous mode to communicate with ADS
  int N_zeroRef = constrain(N-1,0,CHANNELS_PER_BOARD-1);  //subtracts 1 so that we're counting from 0, not 1
  if (N_zeroRef>=CHANNELS_PER_ADC) {
    N_zeroRef = N_zeroRef - 4;
    reg = CH1SET+(byte)N_zeroRef;
    config = ADS1299::RREG_2(reg); delay(1);
    bitSet(config,7);  //left-most bit (bit 7) = 1, so this shuts down the channel
    if (use_neg_inputs) bitClear(config,3);  //bit 3 = 0 disconnects SRB2
    ADS1299::WREG_2(reg,config); delay(1);
  } else {
    reg = CH1SET+(byte)N_zeroRef;
    config = ADS1299::RREG_1(reg); delay(1);
    bitSet(config,7);  //left-most bit (bit 7) = 1, so this shuts down the channel
    if (use_neg_inputs) bitClear(config,3);  //bit 3 = 0 disconnects SRB2
    ADS1299::WREG_1(reg,config); delay(1);
  } 
  //set how this channel affects the bias generation...
  // alterBiasBasedOnChannelState(N);
}; 

//Active a channel in single-ended mode  
//  N is 1 through 8
//  gainCode is defined in the macros in the header file
//  inputCode is defined in the macros in the header file
void GEENIE::activateChannel(int N,byte gainCode,byte inputCode) 
{
  // byte reg, config;

  line7[N-1] = 'O';
	
   //check the inputs
  if ((N < 1) || (N > CHANNELS_PER_BOARD)) return;
  
  //proceed...first, disable any data collection
  ADS1299::SDATAC(); delay(1);      // exit Read Data Continuous mode to communicate with ADS
  //active the channel using the given gain.  Set MUX for normal operation
  //see ADS1299 datasheet, PDF p44
  N = constrain(N-1,0,CHANNELS_PER_BOARD-1);  //shift down by one
  byte configByte = 0b00000000;  //left-most zero (bit 7) is to activate the channel
  gainCode = gainCode & 0b01110000;  //bitwise AND to get just the bits we want and set the rest to zero
  configByte = configByte | gainCode; //bitwise OR to set just the gain bits high or low and leave the rest alone
  inputCode = inputCode & 0b00000111;  //bitwise AND to get just the bits we want and set the rest to zero
  configByte = configByte | inputCode; //bitwise OR to set just the gain bits high or low and leave the rest alone
  if (use_SRB2[N]) configByte |= 0b00001000;  //set the SRB2 flag...p44 in the data sheet
  Serial.println(configByte, HEX);
  if (N>=CHANNELS_PER_ADC) {
    N = N-4;
    ADS1299::WREG_2(CH1SET+(byte)N,configByte); delay(1);
    // alterBiasBasedOnChannelState(N);
  } else {
    ADS1299::WREG_1(CH1SET+(byte)N,configByte); delay(1);
    // alterBiasBasedOnChannelState(N);
  }
  //add this channel to the bias generation
  
  // activate SRB1 as the Negative input for all channels, if needed
  setSRB1(use_SRB1());


};

//note that N here one-referenced (ie [1...N]), not [0...N-1]
void GEENIE::alterBiasBasedOnChannelState(int N_oneRef) {
	//  int N_zeroRef = constrain(N_oneRef-1,0,CHANNELS_PER_BOARD-1);  //subtracts 1 so that we're counting from 0, not 1
	
	//  boolean activateBias = false;
	 if ((use_channels_for_bias==true) && (isChannelActive(N_oneRef))) {
	 	 //activate this channel's bias
	 	 activateBiasForChannel(N_oneRef);
	 } else {
	 	 deactivateBiasForChannel(N_oneRef);
	 }
}

//note that N here one-referenced (ie [1...N]), not [0...N-1]
boolean GEENIE::isChannelActive(int N_oneRef) {
	 int N_zeroRef = constrain(N_oneRef-1,0,CHANNELS_PER_BOARD-1);  //subtracts 1 so that we're counting from 0, not 1
	 //get whether channel is active or not
  boolean chanState;
  if (N_zeroRef>=CHANNELS_PER_ADC) {
    N_zeroRef = N_zeroRef-4;
    byte reg = CH1SET+(byte)N_zeroRef;
    byte config = ADS1299::RREG_2(reg); delay(1);
    chanState = bitRead(config,7);
  } else {
    byte reg = CH1SET+(byte)N_zeroRef;
    byte config = ADS1299::RREG_1(reg); delay(1);
    chanState = bitRead(config,7);
  }
	 return chanState;
}


void GEENIE::deactivateBiasForChannel(int N_oneRef) {
	int N_zeroRef = constrain(N_oneRef-1,0,CHANNELS_PER_BOARD-1); //subtracts 1 so that we're counting from 0, not 1
 	
	//deactivate this channel's bias...both positive and negative
	//see ADS1299 datasheet, PDF p44.
	byte reg, config;
	for (int I=0;I<2;I++) {
		if (I==0) {
			reg = BIAS_SENSP;
		} else {
			reg = BIAS_SENSN;
		}
    if (N_zeroRef>=CHANNELS_PER_ADC) {
      N_zeroRef = N_zeroRef-4;
      config = ADS1299::RREG_2(reg); delay(1);//get the current bias settings
      bitClear(config,N_zeroRef);          //clear this channel's bit to remove from bias generation
      ADS1299::WREG_2(reg,config); delay(1);  //send the modified byte back to the ADS
    } else {
      config = ADS1299::RREG_1(reg); delay(1);//get the current bias settings
      bitClear(config,N_zeroRef);          //clear this channel's bit to remove from bias generation
      ADS1299::WREG_1(reg,config); delay(1);  //send the modified byte back to the ADS
    }
	}
}
void GEENIE::activateBiasForChannel(int N_oneRef) {
	int N_zeroRef = constrain(N_oneRef-1,0,CHANNELS_PER_BOARD-1); //subtracts 1 so that we're counting from 0, not 1
 	
	//see ADS1299 datasheet, PDF p44.
	//per Chip's experiments, if using the P inputs, just include the P inputs
	//per Joel's experiements, if using the N inputs, include both P and N inputs
	byte reg, config;
	int nLoop = 1;  if (use_neg_inputs) nLoop=2;
	for (int i=0; i < nLoop; i++) {
		reg = BIAS_SENSP;
		if (i > 0) reg = BIAS_SENSN;
    if (N_zeroRef>=CHANNELS_PER_ADC) {
      N_zeroRef = N_zeroRef-4;
      config = ADS1299::RREG_2(reg); //get the current bias settings
      bitSet(config,N_zeroRef);                   //set this channel's bit
      ADS1299::WREG_2(reg,config); delay(1);  //send the modified byte back to the ADS
    } else {
      config = ADS1299::RREG_1(reg); //get the current bias settings
      bitSet(config,N_zeroRef);                   //set this channel's bit
      ADS1299::WREG_1(reg,config); delay(1);  //send the modified byte back to the ADS  
    }

	}
}	

//change the given channel's lead-off detection state...note: stops data colleciton to issue its commands
//  N is the channel number: 1-8
// 
void GEENIE::changeChannelLeadOffDetection(int N, int code_OFF_ON, int code_P_N_Both)
{
  byte reg, config;
	
  //check the inputs
  if ((N < 1) || (N > CHANNELS_PER_BOARD)) return;
  N = constrain(N-1,0,CHANNELS_PER_BOARD-1);  //shift down by one
  
  //proceed...first, disable any data collection
  ADS1299::SDATAC(); delay(1);      // exit Read Data Continuous mode to communicate with ADS

  if ((code_P_N_Both == PCHAN) || (code_P_N_Both == BOTHCHAN)) {
      //shut down the lead-off signal on the positive side
  	  reg = LOFF_SENSP;  //are we using the P inptus or the N inputs?
      if (N>=CHANNELS_PER_ADC) {
        N = N-4;
        config = ADS1299::RREG_2(reg); //get the current lead-off settings
        if (code_OFF_ON == OFF) {
            bitClear(config,N);                   //clear this channel's bit
        } else {
            bitSet(config,N); 			  //clear this channel's bit
        }
        ADS1299::WREG_2(reg,config); delay(1);  //send the modified byte back to the ADS
      } else {
        config = ADS1299::RREG_1(reg); //get the current lead-off settings
        if (code_OFF_ON == OFF) {
            bitClear(config,N);                   //clear this channel's bit
        } else {
            bitSet(config,N); 			  //clear this channel's bit
        }
        ADS1299::WREG_1(reg,config); delay(1);  //send the modified byte back to the ADS
      }
  }
  
  if ((code_P_N_Both == NCHAN) || (code_P_N_Both == BOTHCHAN)) {
  	  //shut down the lead-off signal on the negative side
  	  reg = LOFF_SENSN;  //are we using the P inptus or the N inputs?
      if (N>=CHANNELS_PER_ADC) {
        N = N-4;
        config = ADS1299::RREG_2(reg); //get the current lead-off settings
        if (code_OFF_ON == OFF) {
            bitClear(config,N);                   //clear this channel's bit
        } else {
            bitSet(config,N); 			  //clear this channel's bit
        }           //set this channel's bit
        ADS1299::WREG_2(reg,config); delay(1);  //send the modified byte back to the ADS
      } else {
        config = ADS1299::RREG_1(reg); //get the current lead-off settings
        if (code_OFF_ON == OFF) {
            bitClear(config,N);                   //clear this channel's bit
        } else {
            bitSet(config,N); 			  //clear this channel's bit
        }           //set this channel's bit
        ADS1299::WREG_1(reg,config); delay(1);  //send the modified byte back to the ADS
      }
  }
}; 

void GEENIE::setSRB1(boolean desired_state) {
	if (desired_state) {
		ADS1299::WREG_BOTH(MISC1,0b00100000); delay(1);  //ADS1299 datasheet, PDF p46
	} else {
		ADS1299::WREG_BOTH(MISC1,0b00000000); delay(1);  //ADS1299 datasheet, PDF p46
	}
}

void GEENIE::setAutoBiasGeneration(boolean state) {
	use_channels_for_bias = state;
	
	//step through the channels are recompute the bias state
	for (int Ichan=1; Ichan<CHANNELS_PER_BOARD;Ichan++) {
		alterBiasBasedOnChannelState(Ichan);
	}
}

//only use SRB1 if all use_SRB2 are set to false
boolean GEENIE::use_SRB1(void) {
	for (int Ichan=0; Ichan < CHANNELS_PER_BOARD; Ichan++) {
		if (use_SRB2[Ichan]) {
			return false;
		}
	}
	return true;
}


byte GEENIE::read_ads_1(){
    return ADS1299::getDeviceID_1();
}

byte GEENIE::read_ads_2(){
    return ADS1299::getDeviceID_2();
}

 
//Start continuous data acquisition
void GEENIE::start()
{
    ADS1299::RDATAC(); 
    delay(10);           // enter Read Data Continuous mode
    ADS1299::START();    //start the data acquisition
    streaming = true;

    // display.clearDisplay();
    // display.setCursor(0, 10);
    // display.println("Measurement Started");

    // display.display();

    line2 = "Measurement Started";
    drawLines();

}

//Stop the continuous data acquisition
void GEENIE::stop()
{
    ADS1299::STOP(); delay(1);   //start the data acquisition
    ADS1299::SDATAC(); delay(1);      // exit Read Data Continuous mode to communicate with ADS

    line2 = "Measurement Stoped";
    drawLines();
}
  
//Query to see if data is available from the ADS1299...return TRUE is data is available
int GEENIE::isDataAvailable(void)
{
  return (!(digitalRead(DRDY_ADS_1)));
}

//Configure the test signals that can be inernally generated by the ADS1299
void GEENIE::configureInternalTestSignal(byte amplitudeCode, byte freqCode)
{
  byte _amplitudeCode = amplitudeCode;
  byte _freqCode = freqCode;
	if (_amplitudeCode == ADSTESTSIG_NOCHANGE) _amplitudeCode = (ADS1299::RREG_1(CONFIG2) & (0b00000100));
	if (_freqCode == ADSTESTSIG_NOCHANGE) _freqCode = (ADS1299::RREG_1(CONFIG2) & (0b00000011));
	_freqCode &= 0b00000011;  //only the last two bits should be used
	_amplitudeCode &= 0b00000100;  //only this bit should be used
	byte message = 0b11010000 | _freqCode | _amplitudeCode;  //compose the code
	ADS1299::WREG_1(CONFIG2,message); delay(1);
	
  _amplitudeCode = amplitudeCode;
  _freqCode = freqCode;
	if (_amplitudeCode == ADSTESTSIG_NOCHANGE) _amplitudeCode = (ADS1299::RREG_2(CONFIG2) & (0b00000100));
	if (_freqCode == ADSTESTSIG_NOCHANGE) _freqCode = (ADS1299::RREG_2(CONFIG2) & (0b00000011));
	_freqCode &= 0b00000011;  //only the last two bits should be used
	_amplitudeCode &= 0b00000100;  //only this bit should be used
	message = 0b11010000 | _freqCode | _amplitudeCode;  //compose the code
	ADS1299::WREG_2(CONFIG2,message); delay(1);


       //ADS1299::WREG(CONFIG2,0b11010000);delay(1);   //set internal test signal, default amplitude, default speed, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG2,0b11010001);delay(1);   //set internal test signal, default amplitude, 2x speed, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG2,0b11010101);delay(1);   //set internal test signal, 2x amplitude, 2x speed, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG2,0b11010011); delay(1);  //set internal test signal, default amplitude, at DC, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG3,0b01101100); delay(1);  //use internal reference for center of bias creation, datasheet PDF p42 
}

void GEENIE::configureLeadOffDetection(byte amplitudeCode, byte freqCode)
{
  byte _amplitudeCode = amplitudeCode;
  byte _freqCode = freqCode;
	_amplitudeCode &= 0b00001100;  //only these two bits should be used
	_freqCode &= 0b00000011;  //only these two bits should be used
	
	//get the current configuration of he byte
	byte reg, config;
	reg = LOFF;
	config = ADS1299::RREG_1(reg); //get the current bias settings
	
	//reconfigure the byte to get what we want
	config &= 0b11110000;  //clear out the last four bits
	config |= _amplitudeCode;  //set the amplitude
	config |= _freqCode;    //set the frequency
	
	//send the config byte back to the hardware
	ADS1299::WREG_1(reg,config); delay(1);  //send the modified byte back to the ADS
	

  _amplitudeCode = amplitudeCode;
  _freqCode = freqCode;
	_amplitudeCode &= 0b00001100;  //only these two bits should be used
	_freqCode &= 0b00000011;  //only these two bits should be used
	
	//get the current configuration of he byte
	reg = LOFF;
	config = ADS1299::RREG_2(reg); //get the current bias settings
	
	//reconfigure the byte to get what we want
	config &= 0b11110000;  //clear out the last four bits
	config |= _amplitudeCode;  //set the amplitude
	config |= _freqCode;    //set the frequency
	
	//send the config byte back to the hardware
	ADS1299::WREG_2(reg,config); delay(1);  //send the modified byte back to the ADS
}

//set which version of OpenBCI we're using.  This affects whether we use the 
//positive or negative inputs.  It affects whether we use SRB1 or SRB2 for the
//referenece signal.  Finally, it affects whether the lead_off signals are
//flipped or not.
void GEENIE::setSRB()
{
    //set whether to use positive or negative inputs
    use_neg_inputs = false;

    //set SRB2
    for (int i=0; i < CHANNELS_PER_BOARD; i++) {
        use_SRB2[i] = false;
    }

    ADS1299::WREG_BOTH(LOFF_FLIP,0b00000000);delay(1);  //set all channels to zero
}

//print out the state of all the control registers
void GEENIE::printAllRegisters(void)   
{
	boolean prevVerboseState = verbose;
	
        verbose = true;
        ADS1299::RREGS_1(0x00,0x10);
        delay(100);  //stall to let all that data get read by the PC
        ADS1299::RREGS_1(0x11,0x17-0x11);

        delay(100); 

        ADS1299::RREGS_2(0x00,0x10);
        delay(100);  //stall to let all that data get read by the PC
        ADS1299::RREGS_2(0x11,0x17-0x11);
        verbose = prevVerboseState;
}

void GEENIE::sendChannelDataSerial(PACKET_TYPE packetType)
{

  writeSerial(BOP);   // 1 byte - 0x41
  writeSerial(sampleCounter); // 1 byte
  ADS_writeChannelData();     // 24 bytes
  // Write  Timestamp
    // serialize the number, placing the MSB in lower packets
  for (int j = 3; j >= 0; j--)
  {
    writeSerial((uint8_t)(lastSampleTime >> (j * 8)));
  }
  writeSerial((uint8_t)(PCKT_END | packetType)); // 1 byte

  sampleCounter += 1;
}

void GEENIE::sendChannelDataSerialBt(PACKET_TYPE packetType)
{
  if (bt_connected){
    SerialBT.write(BOP);   // 1 byte - 0x41
    SerialBT.write(sampleCounter); // 1 byte
    ADS_writeChannelDataBt();     // 24 bytes
    // Write  Timestamp
      // serialize the number, placing the MSB in lower packets
    for (int j = 3; j >= 0; j--)
    {
      SerialBT.write((uint8_t)(lastSampleTime >> (j * 8)));
    }
    SerialBT.write((uint8_t)(PCKT_END | packetType)); // 1 byte
    sampleCounter += 1;
  }
}

void GEENIE::writeSerial(uint8_t c)
{
  if (Serial)
  {
    Serial.write(c);
  }
}

void GEENIE::ADS_writeChannelData()
{
  ADS_writeChannelDataAvgDaisy();
}

void GEENIE::ADS_writeChannelDataBt()
{
  for (int i = 0; i < 24; i++)
  {
    SerialBT.write(meanBoardDataRaw[i]);
  }
}


void GEENIE::ADS_writeChannelDataAvgDaisy()
{
  if (Serial)
  {
    for (int i = 0; i < 24; i++)
    {
      writeSerial(meanBoardDataRaw[i]);
    }
  }
}

void GEENIE::writeAuxDataSerial(void)
{
  for (int i = 0; i < 3; i++)
  {
    writeSerial((uint8_t)highByte(auxData[i])); // write 16 bit axis data MSB first
    writeSerial((uint8_t)lowByte(auxData[i]));  // axisData is array of type short (16bit)
  }
}

void GEENIE::checkForCommands(void){
  if (SerialBT.available()) {                                                             // BT: Checks if there are data from the bluetooth available
    int ret = SerialBT.read();
    switch (ret)
      {
      case GEENIE_START:
        start();
        Serial.write("GEENIE_START\n");
        break;
      case GEENIE_STOP:
        stop();
        Serial.write("GEENIE_STOP\n");
        break;
      case GEENIE_START_SD:
        Serial.write("GEENIE_START_SD\n");
        break;
      case GEENIE_STOP_SD:
        Serial.write("GEENIE_STOP_SD\n");
        break;
      case GEENIE_CHANNEL_ON_1:
        activateChannel(1, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_1\n");
        break;
      case GEENIE_CHANNEL_ON_2:
        activateChannel(2, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_2\n");
        break;
      case GEENIE_CHANNEL_ON_3:
        activateChannel(3, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_3\n");
        break;
      case GEENIE_CHANNEL_ON_4:
        activateChannel(4, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_4\n");
        break;
      case GEENIE_CHANNEL_ON_5:
        activateChannel(5, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_5\n");
        break;
      case GEENIE_CHANNEL_ON_6:
        activateChannel(6, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_6\n");
        break;
      case GEENIE_CHANNEL_ON_7:
        activateChannel(7, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_7\n");
        break;
      case GEENIE_CHANNEL_ON_8:
        activateChannel(8, ADS_GAIN24, ADSINPUT_NORMAL);
        Serial.write("GEENIE_CHANNEL_ON_8\n");
        break;
      case GEENIE_CHANNEL_OFF_1:
        deactivateChannel(1);
        Serial.write("GEENIE_CHANNEL_OFF_1\n");
        break;
      case GEENIE_CHANNEL_OFF_2:
        deactivateChannel(2);
        Serial.write("GEENIE_CHANNEL_OFF_2\n");
        break;
      case GEENIE_CHANNEL_OFF_3:
        deactivateChannel(3);
        Serial.write("GEENIE_CHANNEL_OFF_3\n");
        break;
      case GEENIE_CHANNEL_OFF_4:
        deactivateChannel(4);
        Serial.write("GEENIE_CHANNEL_OFF_4\n");
        break;
      case GEENIE_CHANNEL_OFF_5:
        deactivateChannel(5);
        Serial.write("GEENIE_CHANNEL_OFF_5\n");
        break;
      case GEENIE_CHANNEL_OFF_6:
        deactivateChannel(6);
        Serial.write("GEENIE_CHANNEL_OFF_6\n");
        break;
      case GEENIE_CHANNEL_OFF_7:
        deactivateChannel(7);
        Serial.write("GEENIE_CHANNEL_OFF_7\n");
        break;
      case GEENIE_CHANNEL_OFF_8:
        deactivateChannel(8);
        Serial.write("GEENIE_CHANNEL_OFF_8\n");
        break;
      default:
        break;
      }
    }
  }

/**
* @description Called in every `loop()` and checks `Serial0`
* @returns {boolean} - `true` if there is data ready to be read
*/
boolean GEENIE::hasDataSerial(void)
{
  // TODO: Need to undo this comment out
  // if (!Serial0) return false;
  // if (!iSerial0.rx) return false;
  if (Serial.available())
  {
    return true;
  }
  else
  {
    return false;
  }
}

char GEENIE::getCharSerial(void)
{
  return Serial.read();
}

float GEENIE::getBatteryLevel(){
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  // measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage

  return measuredvbat;
}

void GEENIE::displayBattery(){
  float bat = getBatteryLevel();
  line3 = F("Battery: ");
  line3 += String(bat, 1);
  line3 += F("V");
  drawLines();
}

// void GEENIE::loop(void)
// {
//   if (isMultiCharCmd)
//   {
//     checkMultiCharCmdTimer();
//   }
// }

/**
* @description Process one char at a time from serial port. This is the main
*  command processor for the OpenBCI system. Considered mission critical for
*  normal operation.
* @param `character` {char} - The character to process.
* @return {boolean} - `true` if the command was recognized, `false` if not
*/
// boolean GEENIE::processChar(char character)
// {
//   // if (curBoardMode == BOARD_MODE_DEBUG || curDebugMode == DEBUG_MODE_ON)
//   // {
//   Serial.print("pC: ");
//   Serial.println(character);
//   // }

//   // if (checkMultiCharCmdTimer())
//   // { // we are in a multi char command
//   //   switch (getMultiCharCommand())
//   //   {
//   //   case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL:
//   //     processIncomingChannelSettings(character);
//   //     break;
//   //   case MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF:
//   //     processIncomingLeadOffSettings(character);
//   //     break;
//   //   case MULTI_CHAR_CMD_SETTINGS_BOARD_MODE:
//   //     processIncomingBoardMode(character);
//   //     break;
//   //   case MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE:
//   //     processIncomingSampleRate(character);
//   //     break;
//   //   case MULTI_CHAR_CMD_INSERT_MARKER:
//   //     processInsertMarker(character);
//   //     break;
//   //   default:
//   //     break;
//   //   }
//   // }
//   // else
//   // { // Normal...
//   switch (character)
//   {
//   //TURN CHANNELS ON/OFF COMMANDS
//   case OPENBCI_CHANNEL_OFF_1:
//     streamSafeChannelDeactivate(1);
//     break;
//   case OPENBCI_CHANNEL_OFF_2:
//     streamSafeChannelDeactivate(2);
//     break;
//   case OPENBCI_CHANNEL_OFF_3:
//     streamSafeChannelDeactivate(3);
//     break;
//   case OPENBCI_CHANNEL_OFF_4:
//     streamSafeChannelDeactivate(4);
//     break;
//   case OPENBCI_CHANNEL_OFF_5:
//     streamSafeChannelDeactivate(5);
//     break;
//   case OPENBCI_CHANNEL_OFF_6:
//     streamSafeChannelDeactivate(6);
//     break;
//   case OPENBCI_CHANNEL_OFF_7:
//     streamSafeChannelDeactivate(7);
//     break;
//   case OPENBCI_CHANNEL_OFF_8:
//     streamSafeChannelDeactivate(8);
//     break;
//   case OPENBCI_CHANNEL_OFF_9:
//     streamSafeChannelDeactivate(9);
//     break;
//   case OPENBCI_CHANNEL_OFF_10:
//     streamSafeChannelDeactivate(10);
//     break;
//   case OPENBCI_CHANNEL_OFF_11:
//     streamSafeChannelDeactivate(11);
//     break;
//   case OPENBCI_CHANNEL_OFF_12:
//     streamSafeChannelDeactivate(12);
//     break;
//   case OPENBCI_CHANNEL_OFF_13:
//     streamSafeChannelDeactivate(13);
//     break;
//   case OPENBCI_CHANNEL_OFF_14:
//     streamSafeChannelDeactivate(14);
//     break;
//   case OPENBCI_CHANNEL_OFF_15:
//     streamSafeChannelDeactivate(15);
//     break;
//   case OPENBCI_CHANNEL_OFF_16:
//     streamSafeChannelDeactivate(16);
//     break;

//   case OPENBCI_CHANNEL_ON_1:
//     streamSafeChannelActivate(1);
//     break;
//   case OPENBCI_CHANNEL_ON_2:
//     streamSafeChannelActivate(2);
//     break;
//   case OPENBCI_CHANNEL_ON_3:
//     streamSafeChannelActivate(3);
//     break;
//   case OPENBCI_CHANNEL_ON_4:
//     streamSafeChannelActivate(4);
//     break;
//   case OPENBCI_CHANNEL_ON_5:
//     streamSafeChannelActivate(5);
//     break;
//   case OPENBCI_CHANNEL_ON_6:
//     streamSafeChannelActivate(6);
//     break;
//   case OPENBCI_CHANNEL_ON_7:
//     streamSafeChannelActivate(7);
//     break;
//   case OPENBCI_CHANNEL_ON_8:
//     streamSafeChannelActivate(8);
//     break;
//   case OPENBCI_CHANNEL_ON_9:
//     streamSafeChannelActivate(9);
//     break;
//   case OPENBCI_CHANNEL_ON_10:
//     streamSafeChannelActivate(10);
//     break;
//   case OPENBCI_CHANNEL_ON_11:
//     streamSafeChannelActivate(11);
//     break;
//   case OPENBCI_CHANNEL_ON_12:
//     streamSafeChannelActivate(12);
//     break;
//   case OPENBCI_CHANNEL_ON_13:
//     streamSafeChannelActivate(13);
//     break;
//   case OPENBCI_CHANNEL_ON_14:
//     streamSafeChannelActivate(14);
//     break;
//   case OPENBCI_CHANNEL_ON_15:
//     streamSafeChannelActivate(15);
//     break;
//   case OPENBCI_CHANNEL_ON_16:
//     streamSafeChannelActivate(16);
//     break;

//   // TEST SIGNAL CONTROL COMMANDS
//   case OPENBCI_TEST_SIGNAL_CONNECT_TO_GROUND:
//     activateAllChannelsToTestCondition(ADSINPUT_SHORTED, ADSTESTSIG_NOCHANGE, ADSTESTSIG_NOCHANGE);
//     break;
//   case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_SLOW:
//     activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_SLOW);
//     break;
//   case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_1X_FAST:
//     activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_1X, ADSTESTSIG_PULSE_FAST);
//     break;
//   case OPENBCI_TEST_SIGNAL_CONNECT_TO_DC:
//     activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_DCSIG);
//     break;
//   case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_SLOW:
//     activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_SLOW);
//     break;
//   case OPENBCI_TEST_SIGNAL_CONNECT_TO_PULSE_2X_FAST:
//     activateAllChannelsToTestCondition(ADSINPUT_TESTSIG, ADSTESTSIG_AMP_2X, ADSTESTSIG_PULSE_FAST);
//     break;

//   // CHANNEL SETTING COMMANDS
//   case OPENBCI_CHANNEL_CMD_SET: // This is a multi char command with a timeout
//     startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_CHANNEL);
//     numberOfIncomingSettingsProcessedChannel = 1;
//     break;

//   // LEAD OFF IMPEDANCE DETECTION COMMANDS
//   case OPENBCI_CHANNEL_IMPEDANCE_SET:
//     startMultiCharCmdTimer(MULTI_CHAR_CMD_PROCESSING_INCOMING_SETTINGS_LEADOFF);
//     numberOfIncomingSettingsProcessedLeadOff = 1;
//     break;

//   case OPENBCI_CHANNEL_DEFAULT_ALL_SET: // reset all channel settings to default
//     if (!streaming)
//     {
//       printAll("updating channel settings to");
//       printAll(" default");
//       sendEOT();
//     }
//     streamSafeSetAllChannelsToDefault();
//     break;
//   case OPENBCI_CHANNEL_DEFAULT_ALL_REPORT: // report the default settings
//     reportDefaultChannelSettings();
//     break;

//   // DAISY MODULE COMMANDS
//   case OPENBCI_CHANNEL_MAX_NUMBER_8: // use 8 channel mode
//     if (daisyPresent)
//     {
//       removeDaisy();
//     }
//     else if (wifi.present && wifi.tx)
//     {
//       wifi.sendStringLast("No daisy to remove");
//     }
//     break;
//   case OPENBCI_CHANNEL_MAX_NUMBER_16: // use 16 channel mode
//     if (daisyPresent == false)
//     {
//       attachDaisy();
//     }
//     if (daisyPresent)
//     {
//       printAll("16");
//     }
//     else
//     {
//       printAll("8");
//     }
//     sendEOT();
//     break;

//   // STREAM DATA AND FILTER COMMANDS
//   case OPENBCI_STREAM_START: // stream data
//     if (curAccelMode == ACCEL_MODE_ON)
//     {
//       enable_accel(RATE_25HZ);
//     } // fire up the accelerometer if you want it
//     wifi.tx = commandFromSPI;
//     if (wifi.present && wifi.tx)
//     {
//       wifi.sendStringLast("Stream started");
//       iSerial0.tx = false;
//     }
//     // Reads if the command is not from the SPI port and we are not in debug mode
//     if (!commandFromSPI && !iSerial1.tx)
//     {
//       // If the sample rate is higher than 250, we need to drop down to 250Hz
//       //  to not break the RFduino system that can't handle above 250SPS.
//       if (curSampleRate != SAMPLE_RATE_250)
//       {
//         streamSafeSetSampleRate(SAMPLE_RATE_250);
//         delay(50);
//       }
//     }
//     streamStart(); // turn on the fire hose
//     break;
//   case OPENBCI_STREAM_STOP: // stop streaming data
//     if (curAccelMode == ACCEL_MODE_ON)
//     {
//       disable_accel();
//     } // shut down the accelerometer if you're using it
//     wifi.tx = true;
//     streamStop();
//     if (wifi.present && wifi.tx)
//     {
//       wifi.sendStringLast("Stream stopped");
//     }
//     break;

//   //  INITIALIZE AND VERIFY
//   case OPENBCI_MISC_SOFT_RESET:
//     boardReset(); // initialize ADS and read device IDs
//     break;
//   //  QUERY THE ADS AND ACCEL REGITSTERS
//   case OPENBCI_MISC_QUERY_REGISTER_SETTINGS:
//     if (!streaming)
//     {
//       printAllRegisters(); // print the ADS and accelerometer register values
//     }
//     break;

//   // TIME SYNC
//   case OPENBCI_TIME_SET:
//     // Set flag to send time packet
//     if (!streaming)
//     {
//       printAll("Time stamp ON");
//       sendEOT();
//     }
//     curTimeSyncMode = TIME_SYNC_MODE_ON;
//     setCurPacketType();
//     break;

//   case OPENBCI_TIME_STOP:
//     // Stop the Sync
//     if (!streaming)
//     {
//       printAll("Time stamp OFF");
//       sendEOT();
//     }
//     curTimeSyncMode = TIME_SYNC_MODE_OFF;
//     setCurPacketType();
//     break;

//   // BOARD TYPE SET TYPE
//   case OPENBCI_BOARD_MODE_SET:
//     startMultiCharCmdTimer(MULTI_CHAR_CMD_SETTINGS_BOARD_MODE);
//     optionalArgCounter = 0;
//     break;

//   // Sample rate set
//   case OPENBCI_SAMPLE_RATE_SET:
//     startMultiCharCmdTimer(MULTI_CHAR_CMD_SETTINGS_SAMPLE_RATE);
//     break;

//   // Insert Marker into the EEG data stream
//   case OPENBCI_INSERT_MARKER:
//     startMultiCharCmdTimer(MULTI_CHAR_CMD_INSERT_MARKER);
//     break;

//   case OPENBCI_WIFI_ATTACH:
//     if (wifi.attach())
//     {
//       printSuccess();
//       printSerial("Wifi attached");
//       sendEOT();
//     }
//     else
//     {
//       printFailure();
//       printSerial("Wifi not attached");
//       sendEOT();
//     }
//     break;
//   case OPENBCI_WIFI_REMOVE:
//     if (wifi.remove())
//     {
//       printSuccess();
//       printSerial("Wifi removed");
//     }
//     else
//     {
//       printFailure();
//       printSerial("Wifi not removed");
//     }
//     sendEOT();
//     break;
//   case OPENBCI_WIFI_STATUS:
//     if (wifi.present)
//     {
//       printAll("Wifi present");
//     }
//     else
//     {
//       printAll("Wifi not present, send {");
//       printAll(" to attach the shield");
//     }
//     sendEOT();
//     break;
//   case OPENBCI_WIFI_RESET:
//     wifi.reset();
//     printSerial("Wifi soft reset");
//     sendEOT();
//     break;
//   case OPENBCI_GET_VERSION:
//     printAll("v3.1.2");
//     sendEOT();
//     break;
//   default:
//     return false;
//   }
//   // }
//   return true;
// }