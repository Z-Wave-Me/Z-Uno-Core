#pragma once

#include <Arduino.h>

#define LED_MODE_ALWAYS_OFF 0
#define LED_MODE_ALWAYS_ON  1

struct LedMode {

    byte    divider;
    dword   pattern;
};
class ZUnoLED{
public:
    ZUnoLED(byte led_num, LedMode * led_modes, byte num_modes);
    void tick();
    void setCustomMode(byte mode_num);
    void off();
    void on();
    void setInverted(byte inv){_inverted = inv; };
protected:
    void setLED(byte on_off);
private:
    byte _led_pin;
    byte _inverted;
    LedMode * _led_modes;
    byte      _modes_count;
    byte _current_mode;
    word _patt_index;
    byte _cashed_pattern[4];
    word _cashed_divider;
};