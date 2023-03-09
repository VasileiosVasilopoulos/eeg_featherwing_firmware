#include <board_functions.h>
#include <board_definitions.h>
#include <Arduino.h>
#include <ads1299.h>

// BUTTON INTERRUPTS

volatile int button_pressed = 0;

void IRAM_ATTR btn_1_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 1 pressed");
    }
};

void IRAM_ATTR btn_2_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 2 pressed");
    }
};

void IRAM_ATTR btn_3_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 3 pressed");
    }
};

void IRAM_ATTR btn_4_isr(){
    int now = millis();
    if (now - button_pressed > 400){
        button_pressed = now;
        Serial.println("Button 4 pressed");
    }
};



void GEENIE::set_buttons(){
  pinMode(BTN_1, INPUT_PULLUP);
  attachInterrupt(BTN_1, btn_1_isr, FALLING);

  pinMode(BTN_2, INPUT_PULLUP);
  attachInterrupt(BTN_2, btn_2_isr, FALLING);

  pinMode(BTN_3, INPUT_PULLUP);
  attachInterrupt(BTN_3, btn_3_isr, FALLING);

  pinMode(BTN_4, INPUT_PULLUP);
  attachInterrupt(BTN_4, btn_4_isr, FALLING);
}

void GEENIE::initialize(boolean isDaisy){
    // Serial.begin(SERIAL_BAUDRATE);
    ADS1299::initialize(BOARD_ADS, isDaisy, true);

    delay(100);
    verbose = true;

    setSRB();

    reset();

    n_chan_all_boards = CHANNELS_PER_BOARD;
    if (isDaisy) n_chan_all_boards = 2*CHANNELS_PER_BOARD;

    //set default state for internal test signal
    //ADS1299::WREG(CONFIG2,0b11010000);delay(1);   //set internal test signal, default amplitude, default speed, datasheet PDF Page 41
    //ADS1299::WREG(CONFIG2,0b11010001);delay(1);   //set internal test signal, default amplitude, 2x speed, datasheet PDF Page 41
    configureInternalTestSignal(ADSTESTSIG_AMP_1X,ADSTESTSIG_PULSE_FAST); //set internal test signal, default amplitude, 2x speed, datasheet PDF Page 41

    //set default state for lead off detection
    configureLeadOffDetection(LOFF_MAG_6NA,LOFF_FREQ_31p2HZ);
}

void GEENIE::reset(){
    ADS1299::RESET();             // send RESET command to default all registers
    ADS1299::SDATAC();            // exit Read Data Continuous mode to communicate with ADS

    delay(100);

    // turn off all channels
    for (int chan=1; chan <= CHANNELS_PER_BOARD; chan++) {
        deactivateChannel(chan);  //turn off the channel
        changeChannelLeadOffDetection(chan,OFF,BOTHCHAN); //turn off any impedance monitoring
    }

    setSRB1(use_SRB1());  //set whether SRB1 is active or not
    setAutoBiasGeneration(true); //configure ADS1299 so that bias is generated based on channel state
}

//deactivate the given channel...note: stops data colleciton to issue its commands
//  N is the channel number: 1-8
// 
void GEENIE::deactivateChannel(int N)
{
  byte reg, config;
	
  //check the inputs
  if ((N < 1) || (N > CHANNELS_PER_BOARD)) return;
  
  //proceed...first, disable any data collection
  ADS1299::SDATAC(); delay(1);      // exit Read Data Continuous mode to communicate with ADS

  //shut down the channel
  int N_zeroRef = constrain(N-1,0,CHANNELS_PER_BOARD-1);  //subtracts 1 so that we're counting from 0, not 1
  reg = CH1SET+(byte)N_zeroRef;
  config = ADS1299::RREG(reg); delay(1);
  bitSet(config,7);  //left-most bit (bit 7) = 1, so this shuts down the channel
  if (use_neg_inputs) bitClear(config,3);  //bit 3 = 0 disconnects SRB2
  ADS1299::WREG(reg,config); delay(1);
  
  //set how this channel affects the bias generation...
  alterBiasBasedOnChannelState(N);
}; 

//Active a channel in single-ended mode  
//  N is 1 through 8
//  gainCode is defined in the macros in the header file
//  inputCode is defined in the macros in the header file
void GEENIE::activateChannel(int N,byte gainCode,byte inputCode) 
{
  byte reg, config;
	
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
  ADS1299::WREG(CH1SET+(byte)N,configByte); delay(1);

  //add this channel to the bias generation
  alterBiasBasedOnChannelState(N);
  
  // // Now, these actions are necessary whenever there is at least one active channel
  // // though they don't strictly need to be done EVERY time we activate a channel.
  // // just once after the reset.
  
  //activate SRB1 as the Negative input for all channels, if needed
  setSRB1(use_SRB1());

  //Finalize the bias setup...activate buffer and use internal reference for center of bias creation, datasheet PDF p42
  ADS1299::WREG(CONFIG3,0b11101100); delay(1); 
};

//note that N here one-referenced (ie [1...N]), not [0...N-1]
void GEENIE::alterBiasBasedOnChannelState(int N_oneRef) {
	 int N_zeroRef = constrain(N_oneRef-1,0,CHANNELS_PER_BOARD-1);  //subtracts 1 so that we're counting from 0, not 1
	
	 boolean activateBias = false;
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
	 byte reg = CH1SET+(byte)N_zeroRef;
	 byte config = ADS1299::RREG(reg); delay(1);
	 boolean chanState = bitRead(config,7);
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
		config = ADS1299::RREG(reg); delay(1);//get the current bias settings
		bitClear(config,N_zeroRef);          //clear this channel's bit to remove from bias generation
		ADS1299::WREG(reg,config); delay(1);  //send the modified byte back to the ADS
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
		config = ADS1299::RREG(reg); //get the current bias settings
		bitSet(config,N_zeroRef);                   //set this channel's bit
		ADS1299::WREG(reg,config); delay(1);  //send the modified byte back to the ADS
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
  	  config = ADS1299::RREG(reg); //get the current lead-off settings
  	  if (code_OFF_ON == OFF) {
  	  	  bitClear(config,N);                   //clear this channel's bit
  	  } else {
  	  	  bitSet(config,N); 			  //clear this channel's bit
  	  }
  	  ADS1299::WREG(reg,config); delay(1);  //send the modified byte back to the ADS
  }
  
  if ((code_P_N_Both == NCHAN) || (code_P_N_Both == BOTHCHAN)) {
  	  //shut down the lead-off signal on the negative side
  	  reg = LOFF_SENSN;  //are we using the P inptus or the N inputs?
  	  config = ADS1299::RREG(reg); //get the current lead-off settings
  	  if (code_OFF_ON == OFF) {
  	  	  bitClear(config,N);                   //clear this channel's bit
  	  } else {
  	  	  bitSet(config,N); 			  //clear this channel's bit
  	  }           //set this channel's bit
  	  ADS1299::WREG(reg,config); delay(1);  //send the modified byte back to the ADS
  }
}; 

void GEENIE::setSRB1(boolean desired_state) {
	if (desired_state) {
		ADS1299::WREG(MISC1,0b00100000); delay(1);  //ADS1299 datasheet, PDF p46
	} else {
		ADS1299::WREG(MISC1,0b00000000); delay(1);  //ADS1299 datasheet, PDF p46
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


byte GEENIE::read_ads(){
    return ADS1299::getDeviceID();
}

 
//Start continuous data acquisition
void GEENIE::start()
{
    ADS1299::RDATAC(); 
    delay(1);           // enter Read Data Continuous mode
    ADS1299::START();    //start the data acquisition
}

//Stop the continuous data acquisition
void GEENIE::stop()
{
    ADS1299::STOP(); delay(1);   //start the data acquisition
    ADS1299::SDATAC(); delay(1);      // exit Read Data Continuous mode to communicate with ADS
}
  
//Query to see if data is available from the ADS1299...return TRUE is data is available
int GEENIE::isDataAvailable(void)
{
  return (!(digitalRead(ADS_DRDY)));
}

//Configure the test signals that can be inernally generated by the ADS1299
void GEENIE::configureInternalTestSignal(byte amplitudeCode, byte freqCode)
{
	if (amplitudeCode == ADSTESTSIG_NOCHANGE) amplitudeCode = (ADS1299::RREG(CONFIG2) & (0b00000100));
	if (freqCode == ADSTESTSIG_NOCHANGE) freqCode = (ADS1299::RREG(CONFIG2) & (0b00000011));
	freqCode &= 0b00000011;  //only the last two bits should be used
	amplitudeCode &= 0b00000100;  //only this bit should be used
	byte message = 0b11010000 | freqCode | amplitudeCode;  //compose the code
	
	ADS1299::WREG(CONFIG2,message); delay(1);
	
       //ADS1299::WREG(CONFIG2,0b11010000);delay(1);   //set internal test signal, default amplitude, default speed, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG2,0b11010001);delay(1);   //set internal test signal, default amplitude, 2x speed, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG2,0b11010101);delay(1);   //set internal test signal, 2x amplitude, 2x speed, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG2,0b11010011); delay(1);  //set internal test signal, default amplitude, at DC, datasheet PDF Page 41
      //ADS1299::WREG(CONFIG3,0b01101100); delay(1);  //use internal reference for center of bias creation, datasheet PDF p42 
}

void GEENIE::configureLeadOffDetection(byte amplitudeCode, byte freqCode)
{
	amplitudeCode &= 0b00001100;  //only these two bits should be used
	freqCode &= 0b00000011;  //only these two bits should be used
	
	//get the current configuration of he byte
	byte reg, config;
	reg = LOFF;
	config = ADS1299::RREG(reg); //get the current bias settings
	
	//reconfigure the byte to get what we want
	config &= 0b11110000;  //clear out the last four bits
	config |= amplitudeCode;  //set the amplitude
	config |= freqCode;    //set the frequency
	
	//send the config byte back to the hardware
	ADS1299::WREG(reg,config); delay(1);  //send the modified byte back to the ADS
	
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

    ADS1299::WREG(LOFF_FLIP,0b00000000);delay(1);  //set all channels to zero
  
}

//print out the state of all the control registers
void GEENIE::printAllRegisters(void)   
{
	boolean prevVerboseState = verbose;
	
        verbose = true;
        ADS1299::RREGS(0x00,0x10);     // write the first registers
        delay(100);  //stall to let all that data get read by the PC
        ADS1299::RREGS(0x11,0x17-0x11);     // write the rest
        verbose = prevVerboseState;
}