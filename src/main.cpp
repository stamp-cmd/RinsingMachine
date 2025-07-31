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

char page[478] = "<h2>Motor Control</h2><ul style=list-style-type:none><li><form action=/m_one><input type=submit value=\"Motor one\"></form><li><form action=/m_two><input type=submit value=\"Motor two\"></form><li><form action=/m_three><input type=submit value=\"Motor three\"></form><li><form action=/m_four><input type=submit value=\"Motor four\"></form><li><form action=/m_five><input type=submit value=\"Motor five\"></form><li><form action=/stop><input type=submit value=\"Stop Previous\"></form></ul>";
ESP8266WebServer server(80);

void setup() {
    // REQUIRED
    Serial.begin(9600);
    pinMode(SWITCH_PIN, INPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(D3, OUTPUT);
    MOTOR_OFF(MOTOR_PIN);
    state_switch.time = millis();

    //TODO: Temporary code

    bool wifi_res = WiFi.softAP("RinsingMachine", "", 1, 0, 2);
    if (wifi_res) {
        Serial.println("WIFI READY");
    }else {
        Serial.println("WIFI FAILED");
    }

    server.on("/", []() {
        server.send(200, "text/html", page);
    });

    server.on("/m_one", []() {
        select_channel(motor_pins, 0);
        MOTOR_ON(MOTOR_PIN);
    });

    server.on("/m_two", []() {
        select_channel(motor_pins, 1);
        MOTOR_ON(MOTOR_PIN);
    });

    server.on("/m_three", []() {
        select_channel(motor_pins, 2);
        MOTOR_ON(MOTOR_PIN);
    });

    server.on("/m_four", []() {
        select_channel(motor_pins, 3);
        MOTOR_ON(MOTOR_PIN);
    });

    server.on("/m_five", []() {
        select_channel(motor_pins, 4);
        MOTOR_ON(MOTOR_PIN);
    });

    server.on("/stop", []() {
        MOTOR_OFF(MOTOR_PIN);
    });

    server.begin();
}

void loop() {
    server.handleClient();
}

void unit_select(int n) {
    select_channel(motor_pins, n);
    select_channel(switch_pins, n);
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
