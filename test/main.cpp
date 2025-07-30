#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "utils.h"

struct selector_pins motor_pins {
	D3, D2, D1
};

struct selector_pins switch_pins {
	D4, D5, D6
};

struct switch_state state_switch {
	D0, LOW, 0
};

void rotate_coil();

//ESP8266WebServer server(80);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	pinMode(D0, INPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);
	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);
	pinMode(D7, OUTPUT);
	pinMode(D8, INPUT);
	digitalWrite(D7, HIGH);
	state_switch.time = millis();
	select_channel(motor_pins, 0);
	select_channel(switch_pins, 1);
}

void loop() {
	rotate_coil();
	delay(2000);
	// put your main code here, to run repeatedly:
}

void rotate_coil() {
	int c_state = 1;
	digitalWrite(D7, LOW);
	while (1) {
		state_switch = monitor_switch(state_switch);
		if (c_state == 0 && read_switch(state_switch) == ON) {
			c_state = 1;
			digitalWrite(D7,  HIGH);
			break;
		}else if (c_state == 1 && read_switch(state_switch) == OFF) {
			c_state = 0;
		}
		yield();
	}
}