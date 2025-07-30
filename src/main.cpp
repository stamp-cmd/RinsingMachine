#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "utils.h"

#define MOTOR_PIN D7
#define SWITCH_PIN D0

struct selector_pins motor_pins { D3, D2, D1 };
struct selector_pins switch_pins { D4, D5, D6 };

struct switch_state state_switch { D0, LOW, 0 };

void dispense_time(int time);
void dispense_switch();
void unit_select(int n);

void setup() {
    // REQUIRED
    Serial.begin(9600);
    pinMode(SWITCH_PIN, INPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(D3, OUTPUT);
    MOTOR_OFF(MOTOR_PIN);
    state_switch.time = millis();

    select_channel(switch_pins, 0);
    select_channel(motor_pins, 0);
}

int switch_map[] = {0, 1, 2, 3, 4, 5};
int motor_map[] = {0, 1, 2, 3, 4, 5};

void loop() {
    state_switch = monitor_switch(state_switch);
    if (read_switch(state_switch) != WAIT) {
        MOTOR_ON(MOTOR_PIN)
        Serial.println(read_switch(state_switch));
    }else {

    }
}

void unit_select(int n) {
    select_channel(motor_pins, motor_map[n]);
    select_channel(switch_pins, switch_map[n]);
}

// "detect if switch is pressed" - algorithm
void dispense_switch() {
    int current = 0;
    MOTOR_ON(MOTOR_PIN);
    while (1) {
        state_switch = monitor_switch(state_switch);
        if (current == 0 && read_switch(state_switch) == ON) {
            current = 1;
            MOTOR_OFF(MOTOR_PIN);
            break;
        }else if (current == 1 && read_switch(state_switch) == OFF) {
            current = 0;
        }
        yield();
    }
}

// time to complete 1 revolution - no load 800ms
void dispense_time(int time) {
    int start_time = millis();
    MOTOR_ON(MOTOR_PIN);
    while (1) {
        if (millis() - start_time >= (unsigned long)(time)) {
            MOTOR_OFF(MOTOR_PIN);
            break;
        }
    }
}