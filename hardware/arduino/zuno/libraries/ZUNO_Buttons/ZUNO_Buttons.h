#pragma once
/**
 * Simple, reliable button with multiple types of click detection.
 *
 * Supports debounced click, singleClick, doubleClick, longPress and
 * release events.
 *
 * This class provides a simple wrapper around the more generic MultiButton,
 * to simply use an Arduino digital pin as a button.
 *
 * Copyright (C) Martin Poelstra
 * MIT License
 * Ported and redesigned by Z-Wave.Me Team for Z-Uno project, 2018
 */
#include <Arduino.h>

#define BTN_DEFAULT_DEBOUNCE_DELAY    5
#define BTN_DEFAULT_SINGLECLICK_DELAY 10
#define BTN_DEFAULT_LONGCLICK_DELAY   100
#define BTN_DEFAULT_FREE_DELAY        120 


enum State {
      STATE_IDLE,
      STATE_DEBOUNCE,
      STATE_PRESSED,
      STATE_CLICKUP,
      STATE_CLICKIDLE,
      STATE_SINGLECLICK,
      STATE_DOUBLECLICKDEBOUNCE,
      STATE_DOUBLECLICK,
      STATE_TRIPLECLICKIDLE,
      STATE_TRIPLECLICKDEBOUNCE,
      STATE_TRIPLECLICK,
      STATE_LONGCLICK,
      STATE_OTHERUP
};
class SimpleButton {
  public:
    SimpleButton();
    void update();
    
    bool isClick(void);
    bool isSingleClick(void);
    bool isDoubleClick(void);
    bool isTripleClick(void);
    bool isLongClick(void);
    bool isReleased(void);
    bool isFree(void);

    void setDebounceTime(byte val){ _debounce_time = val; };
    void setSingleClickTime(byte val){ _single_time = val; };
    void setLongClickTime(byte val){ _long_time = val; };
    
    void startDispatch(){ _dispatched_events = _event_map; };
    void endDispatch(){ _event_map &= ~(_dispatched_events); };
    byte currState(){return _state; };
  private:
    virtual bool isPressed(){};

    unsigned int _lastTransition;
    byte _state;
    //bool _new;
    // time values in 10ms intervals
    byte _debounce_time;
    byte _single_time;
    byte _long_time;
    byte _free_time;
    byte _release_counter;
    byte _event_map;
    byte _dispatched_events;

};

class PinButton: public SimpleButton {

public:
    PinButton(byte pin_num, bool inverted = true): _pin_num(pin_num), _inverted(inverted) {};
    
private:
    virtual bool isPressed();
    byte _pin_num;
    bool _inverted;

};
