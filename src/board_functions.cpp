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

void GEENIE::initialize_ads(){
    // Serial.begin(SERIAL_BAUDRATE);
    Ads1299.initialize(BOARD_ADS, false, true);
    delay(100);
    verbose = true;
    reset_ads();
}

void GEENIE::reset_ads(){
    Ads1299.RESET();             // send RESET command to default all registers
    Ads1299.SDATAC();            // exit Read Data Continuous mode to communicate with ADS

    delay(100);

    // turn off all channels
    // for (int chan=1; chan <= OPENBCI_NCHAN_PER_BOARD; chan++) {
    // deactivateChannel(chan);  //turn off the channel
    // changeChannelLeadOffDetection(chan,OFF,BOTHCHAN); //turn off any impedance monitoring
    // }

    // setSRB1(use_SRB1());  //set whether SRB1 is active or not
    // setAutoBiasGeneration(true); //configure ADS1299 so that bias is generated based on channel state
}

byte GEENIE::read_ads(){
    return Ads1299.getDeviceID();
}