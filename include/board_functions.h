#ifndef __GEENIE_FUNCTIONS_H__
#define __GEENIE_FUNCTIONS_H__
#include <ads1299.h>


class GEENIE: public ADS1299 {
    public:
        void initialize(boolean isDaisy=false);
        void set_buttons();
        byte read_ads();
        void setSRB();			//Set which version of OpenBCI you're using.
        void start();
        void stop();
        boolean isChannelActive(int N_oneRef);
        void activateChannel(int N_oneRef, byte gainCode,byte inputCode); //setup the channel 1-8
        void deactivateChannel(int N_oneRef);                            //disable given channel 1-8
        void alterBiasBasedOnChannelState(int N_oneRef);
        int isDataAvailable(void);
        void configureInternalTestSignal(byte amplitudeCode, byte freqCode);  //configure the test signal parameters
        void configureLeadOffDetection(byte amplitudeCode, byte freqCode);  //configure the lead-off detection signal parameters
        void deactivateBiasForChannel(int N_oneRef);
        void activateBiasForChannel(int N_oneRef);
        void changeChannelLeadOffDetection(int N_oneRef, int code_OFF_ON, int code_P_N_Both);
        void setSRB1(boolean desired_state);
        void setAutoBiasGeneration(boolean state);
        void reset();
        void printAllRegisters(void);
    private:
        // ADS1299 Ads1299;
        boolean verbose;		// turn on/off Serial feedback
        boolean use_neg_inputs;
        boolean use_SRB2[CHANNELS_PER_BOARD];
        boolean use_channels_for_bias;
        boolean use_SRB1(void);
        int n_chan_all_boards;
};

#endif