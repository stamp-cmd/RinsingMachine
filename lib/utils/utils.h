#ifndef _UTILS_H_
#define _UTILS_H_

#define MOTOR_ON(x) digitalWrite(x, LOW);
#define MOTOR_OFF(x) digitalWrite(x, HIGH);

struct selector_pins {
    int a;
    int b;
    int c;
};

// select n channel `from selector_pins` provided
void select_channel(struct selector_pins pin, int channel);

struct switch_state {
    int pin;
    int state;
    unsigned long time;
};

// get switch's state in time and value
struct switch_state monitor_switch(struct switch_state state);
// get definite switch value after n interval
int read_switch(struct switch_state state);

enum val {
    OFF,
    ON,
    WAIT
};

#endif