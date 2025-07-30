#include <Arduino.h>
#include "utils.h"

#define DELAY 50

void select_channel(struct selector_pins pins, int channel) {
    digitalWrite(pins.c, channel & 4);
    digitalWrite(pins.b, channel & 2);
    digitalWrite(pins.a, channel & 1);
}

struct switch_state monitor_switch(struct switch_state state) {
    struct switch_state res = state;
    if (digitalRead(state.pin) != state.state) {
        res.state = digitalRead(state.pin);
        res.time = millis();   
    }
    return res;
}

int read_switch(struct switch_state state) {
    if (millis() - state.time > DELAY) {
        return digitalRead(state.pin) == HIGH ? ON : OFF;
    }else {
        return WAIT;
    }
}