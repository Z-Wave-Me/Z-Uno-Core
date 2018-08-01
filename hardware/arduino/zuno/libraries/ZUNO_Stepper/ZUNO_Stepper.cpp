#include <ZUNO_Stepper.h>

dword g_stepper_tmpl;
word g_stepper_tmpw;
byte g_stepper_tmpb;
byte g_stepper_dir;
byte g_stepper_pos;
byte g_stepper_period;
word g_stepper_delay;

byte g_stepper_pincount;
byte * g_stepper_pins;
byte g_stepper_pat;

#define STEPPER_CONSTANT_DELAYTIMESHIFT 176
//                              2 - 4  5
CCBYTE STEPPER_PATTERN_COUNT[]={4,4,4,10};
CCBYTE STEPPER_PATTERN_SHIFT[]={0,0,4,8};
CCBYTE STEPPER_PATTERN_DATA[]={ 0x01,0x03,0x02,0x00, // 2 pins
                                0x0A,0x06,0x05,0x09, // 4 pins
                                0x0D,0x09,0x0B,0x0A,0x1A,0x12,0x16,0x14,0x15,0x05 // 5pins
                               };


// Auxilary
void aux_stp_exractpat(byte pos);
void aux_stp_writepat();
void aux_stp_increment();
// -------------------------- 

Stepper::Stepper(byte number_of_steps, byte number_of_pins, byte * pins){
    byte i;
    this->number_of_steps = number_of_steps;
    pin_count = number_of_pins;
    _motor_pins = pins;
    step_number = 0;    // which step the motor is on
    for(i=0;i<pin_count;i++){
        pinMode(_motor_pins[i], OUTPUT);
    }
 
}   
void Stepper::setSpeed(word rpms) {
    g_stepper_tmpl = 60000000L; // uSeconds
    g_stepper_tmpl /= rpms;
    g_stepper_tmpl /= number_of_steps;
    step_delay = g_stepper_tmpl;
    step_delay -= STEPPER_CONSTANT_DELAYTIMESHIFT;
    Serial0.println(step_delay);
}
void Stepper::step(int steps) {
    g_stepper_dir  =  steps<0 ? 0 : 1;
    g_stepper_tmpw = abs(steps);
    g_stepper_pincount = pin_count;
    g_stepper_pins = _motor_pins;
    g_stepper_pos  = step_number;
    g_stepper_period = number_of_steps;
    while(g_stepper_tmpw--) {
        aux_stp_exractpat(g_stepper_pos);
        aux_stp_writepat();
        delayMicroseconds(step_delay);
        aux_stp_increment();
    }
    step_number = g_stepper_pos;    
}
void aux_stp_increment(){
    if(g_stepper_dir){
        g_stepper_pos++;    
        g_stepper_pos %= g_stepper_period;     
        return;
    }
    if(!g_stepper_pos)
        g_stepper_pos = g_stepper_period;
     g_stepper_pos--;

}
void aux_stp_exractpat(byte pos) {
    byte i = g_stepper_pincount-2;
    pos %= STEPPER_PATTERN_COUNT[i];
    pos += STEPPER_PATTERN_SHIFT[i];
    i = pos;
    g_stepper_pat = STEPPER_PATTERN_DATA[i];
}
void aux_stp_writepat() {
    byte i = g_stepper_pincount;
    byte mask = g_stepper_pat;
    while(i) {
        i--;
        digitalWrite(g_stepper_pins[i], mask & 0x01);
        mask >>= 1;
    }
}