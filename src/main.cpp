#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "utils.h"

#define MOTOR_PIN D7
#define SWITCH_PIN D0

struct selector_pins motor_pins { D3, D2, D1 };
struct selector_pins switch_pins { D4, D5, D6 };

struct switch_state state_switch { D0, LOW, 0 };

int gen_time(int items, int type);
void dispense_time(int items, int type);
void dispense_switch();
void unit_select(int n);
void move(int n);

static const char page[] PROGMEM = "<h1>Moodi</h1><h2>Select your emotion</h2><div><hr><form action=m_one><input type=submit value=\"Happy :3\"></form><hr><form action=m_two><input type=submit value=\"Angry >:3\"></form><hr><form action=m_three><input type=submit value=\"Sad :c\"></form><hr><form action=m_four><input type=submit value=\"Bored :&sol;\"></form><hr></div><style>h1{text-align:center;font-family:Verdana,sans-serif;color:#8b4513;margin-bottom:0}h2{text-align:center;margin-bottom:0}body{display:grid;align-items:center;justify-content:center;background-color:#ffebcd;height:100vh}input{font-size:30px;width:100%;background-color:#ffebcd;border:0}hr{width:80%;height:0}div{width:800px;height:70vh;display:grid}</style>";
ESP8266WebServer server(80);

void setup() {
    // REQUIRED
    Serial.begin(9600);
    // Pinmode nearly all of them cuz idk
    pinMode(SWITCH_PIN, INPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    pinMode(motor_pins.a, OUTPUT);
    pinMode(motor_pins.b, OUTPUT);
    pinMode(motor_pins.c, OUTPUT);
    pinMode(switch_pins.a, OUTPUT);
    pinMode(switch_pins.b, OUTPUT);
    pinMode(switch_pins.c, OUTPUT);

    MOTOR_OFF(MOTOR_PIN);
    state_switch.time = millis();

    // Forever code now

    bool wifi_res = WiFi.softAP("RinsingMachine", "", 1, 0, 2);
    if (wifi_res) {
        Serial.println("WIFI READY");
    }else {
        Serial.println("WIFI FAILED");
    }

    server.on("/", []() {
        server.send(200, "text/html", FPSTR(page));
    });

    server.on("/m_one", []() { move(0); server.send(204, emptyString, emptyString); Serial.println("Motor 1"); });
    server.on("/m_two", []() { move(1); server.send(204, emptyString, emptyString); Serial.println("Motor 2"); });
    server.on("/m_three", []() { move(2); server.send(204, emptyString, emptyString); Serial.println("Motor 3"); });
    server.on("/m_four", []() { move(3); server.send(204, emptyString, emptyString); Serial.println("Motor 4"); });
    server.on("/m_five", []() { move(4); server.send(204, emptyString, emptyString); Serial.println("Motor 5"); });

    server.on("/stop", []() {
        MOTOR_OFF(MOTOR_PIN);
    });

    server.begin();
}

void move(int n) {
    select_channel(motor_pins, n);
    MOTOR_ON(MOTOR_PIN);
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
void dispense_time(int items, int type) {
    int start_time = millis();
    int delay = gen_time(items, type);
    MOTOR_ON(MOTOR_PIN);
    while (1) {
        if (millis() - start_time >= (unsigned long)(delay)) {
            MOTOR_OFF(MOTOR_PIN);
            break;
        }
        yield();
    }
}

// unit in ms
int gen_time(int items, int type) {
    return 800;
}