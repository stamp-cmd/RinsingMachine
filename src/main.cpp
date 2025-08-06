#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "utils.h"

#define MOTOR_PIN D7
#define SWITCH_PIN D0

//#define STOCK

struct selector_pins motor_pins { D3, D2, D1 };
struct selector_pins switch_pins { D4, D5, D6 };

struct switch_state state_switch { D0, LOW, 0 };

int gen_time(int type);
void dispense_time(int type);
void dispense_switch();
void unit_select(int n);
void move(int n);
void man_int(int n);

int inventory[5] = {3, 3, 3, 3, 3};
int times[5] = {1200, 1200, 1200, 1200, 1200};
int dhappy = 0;
int dsad = 0;
int dangry = 0;
int danxious = 0;

static const char cont[] PROGMEM = "<a href=\"/move?n=0\">Motor 1</a> <a href=\"/move?n=1\">Motor 2</a> <a href=\"/move?n=2\">Motor 3</a> <a href=\"/move?n=3\">Motor 4</a> <a href=\"/move?n=4\">Motor 5</a> <a href=/stop>Stop All</a>";
static const char sets[] PROGMEM = "<a href=\"fill?n=0\">Snack 1</a> <a href=\"fill?n=1\">Snack 1</a> <a href=\"fill?n=2\">Snack 1</a> <a href=\"fill?n=3\">Snack 1</a> <a href=\"fill?n=4\">Snack 1</a>";
static const char page[] PROGMEM = "<title>Website</title><h1>Moodi</h1><h2>Select your emotion</h2><div><hr><form action=http://192.168.4.1/m_one><input type=submit value=\"Happy :3\"></form><hr><form action=http://192.168.4.1/m_two><input type=submit value=\"Angry >:3\"></form><hr><form action=http://192.168.4.1/m_three><input type=submit value=\"Sad :c\"></form><hr><form action=http://192.168.4.1/m_four><input type=submit value=\"Anxious :/\"></form><hr></div><style>h1{text-align:center;font-family:Verdana,sans-serif;color:#8b4513;margin-bottom:0}h2{text-align:center;margin-bottom:0}body{display:grid;align-items:center;justify-content:center;background-color:#ffebcd;height:100vh}input{color:#8b4513;font-size:30px;width:100%;background-color:#ffebcd;border:0}hr{width:80%;height:0}div{width:800px;height:70vh;display:grid}</style>";
static const char tim[] PROGMEM = "<form action=time><input type=number id=happy placeholder=Happy value=1200> <input type=number id=angry placeholder=Angry value=1200> <input type=number id=sad placeholder=Sad value=1200> <input type=number id=anxious placeholder=Anxious value=1200> <input type=number id=dhappy placeholder=\"Happy Decrement\"value=0> <input type=number id=dangry placeholder=\"Angry Decrement\"value=0> <input type=number id=dsad placeholder=\"Sad Decrement\"value=0> <input type=number id=danxious placeholder=\"Anxious Decrement\"value=0> <input type=submit></form>";
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

    server.on("/m_one", []() { 
        move(0);
        Serial.println("Motor 1");
    });
    server.on("/m_two", []() {
        move(1);
        Serial.println("Motor 2");
    });
    server.on("/m_three", []() {
        move(2);
        Serial.println("Motor 3");
    });
    server.on("/m_four", []() {
        move(3);
        Serial.println("Motor 4");
    });
    server.on("/m_five", []() {
        move(4);
        Serial.println("Motor 5");
    });

    server.on("/fill", []() {
        char buf[32];
        int n = server.arg("n").charAt(0) - (char)48;
        inventory[n]++;
        snprintf(buf, 32, "%d %d %d %d %d", inventory[0], inventory[1], inventory[2], inventory[3], inventory[4]);
        server.send(200, "text/plain", buf);
    });

    server.on("/stock", []() {
        char buf[32];
        snprintf(buf, 32, "%d %d %d %d %d", inventory[0], inventory[1], inventory[2], inventory[3], inventory[4]);
        server.send(200, "text/plain", buf);
    });
    
    server.on("/delay", []() {
        server.send(200, "text/html", FPSTR(tim));
    });

    server.on("/time", []() {
        times[0] = server.arg("happy").toInt();
        times[1] = server.arg("angry").toInt();
        times[2] = server.arg("sad").toInt();
        times[3] = server.arg("anxious").toInt();
        dhappy = server.arg("dhappy").toInt();
        dangry = server.arg("dangry").toInt();
        dsad = server.arg("dsad").toInt();
        danxious = server.arg("danxious").toInt();
    });
    
    server.on("/set", []() { server.send(200, "text/html", sets); });
    
    server.on("/man", []() {
        server.send(200, "text/html", FPSTR(cont));
    });

    server.on("/move", []() {
        int n = server.arg("n").charAt(0) - (char)48;
        select_channel(motor_pins, n);
        MOTOR_ON(MOTOR_PIN);
        server.send(204, emptyString, emptyString);
    });


    server.on("/stop", []() {
        MOTOR_OFF(MOTOR_PIN);
    });

    server.begin();
}

void move(int n) {
    #ifdef STOCK
        man_int(n);
    #endif
    select_channel(motor_pins, n);
    dispense_time(n);
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

// rotate motor for n milliseconds
void dispense_time(int type) {
    int start_time = millis();
    int delay = gen_time(type);
    MOTOR_ON(MOTOR_PIN);
    while (1) {
        if (millis() - start_time >= (unsigned long)(delay)) {
            MOTOR_OFF(MOTOR_PIN);
            break;
        }
        yield();
    }
}

// use for calculating time, unit in ms
// time to complete 1 revolution - no load 800ms
int gen_time(int type) {
    #ifdef STOCK
        times[0] -= dhappy;
        times[1] -= dangry;
        times[2] -= dsad;
        times[3] -= dhappy;
    #endif
    return times[type];
}

void man_int(int n) {
    if (inventory == 0) {
        server.send(418, "text/plain", "Out of Order :(");
        Serial.println("Out");
    }else {
        inventory[n]--;
        server.send(200, "text/html", FPSTR(page));
    }
}