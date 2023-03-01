#include <board_functions.h>
#include <board_definitions.h>
#include <Arduino.h>

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

void GEENIE::initialize(){
    // Serial.begin(SERIAL_BAUDRATE);
}