#include <ZUNO_LEDS.h>


byte  g_led_j;
ZUnoLED::ZUnoLED(byte led_num, LedMode * led_modes, byte num_modes): _led_pin(led_num), _led_modes(led_modes), _modes_count(num_modes) {
    _current_mode = LED_MODE_ALWAYS_OFF;
    _inverted = FALSE;
    _patt_index = 0;
    pinMode(_led_pin, OUTPUT);
}
void ZUnoLED::setLED(byte on_off){
    if(_inverted){
        on_off = !on_off;
    }
    digitalWrite(_led_pin, on_off);
}
void ZUnoLED::tick(){
    if(_current_mode <= LED_MODE_ALWAYS_ON)
        return;
    if((_patt_index % _cashed_divider) ==0){
        byte i = word(_patt_index / _cashed_divider) % 32;
        i = _cashed_pattern[(i >> 3)] & (1 << (i&0x07));
        i = (i != 0);
        if(_inverted){
            i = !i;
        }
        digitalWrite(_led_pin, i);
    }
    _patt_index++; 
}
void ZUnoLED::setCustomMode(byte mode_num){
     if((mode_num + 2) == _current_mode)
        return;
     memcpy(_cashed_pattern, &(_led_modes[mode_num].pattern), 4);
     _cashed_divider = _led_modes[mode_num].divider;
     _current_mode = mode_num + 2;
     _patt_index = 0; // Always start from first stick in case of the new pattern
}
void ZUnoLED::off(){
    _current_mode = LED_MODE_ALWAYS_OFF;
    setLED(_current_mode);
}
void ZUnoLED::on(){
    _current_mode = LED_MODE_ALWAYS_ON;
    setLED(_current_mode);
}