#include "ZUNO_Buttons.h"
extern word g_btn_now;
extern word g_btn_diff;
extern byte g_btn_next;

#define EVENT_SINGLE_CLICK 0x01
#define EVENT_LONG_CLICK   0x02
#define EVENT_DOUBLE_CLICK 0x04
#define EVENT_TRIPLE_CLICK 0x08

#define EVENT_RELEASE      0x80



SimpleButton::SimpleButton() {
    _lastTransition = 0;
    _state = STATE_IDLE;
    _debounce_time = BTN_DEFAULT_DEBOUNCE_DELAY;
    _single_time   = BTN_DEFAULT_SINGLECLICK_DELAY;
    _long_time     = BTN_DEFAULT_LONGCLICK_DELAY;
    _free_time     = BTN_DEFAULT_FREE_DELAY;
    _release_counter = _free_time;
    _event_map     =  0; 
}
void SimpleButton::update() {
    byte pressed = isPressed();
    g_btn_now =  millis()/10;
    g_btn_diff = g_btn_now - _lastTransition;

    if (!pressed && _state == STATE_IDLE) {
        if(_release_counter)
          _release_counter--;
        return;
    }

  switch (_state) {
    case STATE_IDLE:
        g_btn_next = pressed ? STATE_DEBOUNCE : STATE_IDLE;
        break;
    case STATE_DEBOUNCE:
        if(pressed){
            if(g_btn_diff > _debounce_time){
                g_btn_next = STATE_PRESSED;
            }
        } else {
            g_btn_next = STATE_IDLE;
        }
        break;
    case STATE_PRESSED:
        if(pressed){
            if(g_btn_diff > _long_time){
                g_btn_next = STATE_LONGCLICK;
                _event_map |= EVENT_LONG_CLICK;
            }
        } else {
            g_btn_next = STATE_CLICKUP;
        }
        break;
    case STATE_CLICKUP:           
        g_btn_next = STATE_CLICKIDLE;
        break;
    case STATE_CLICKIDLE: 
        if(pressed){
            g_btn_next = STATE_DOUBLECLICKDEBOUNCE;
        } else {
            if (g_btn_diff >= _single_time){
                g_btn_next = STATE_SINGLECLICK;
                _event_map |= EVENT_SINGLE_CLICK;
            }
        }
        break;
    case STATE_SINGLECLICK:        
        g_btn_next =  STATE_IDLE;
        break;
    case STATE_DOUBLECLICKDEBOUNCE:
        if(pressed){
            if (g_btn_diff >= _debounce_time){
                g_btn_next = STATE_DOUBLECLICK;
            }
        } else {
            g_btn_next = STATE_CLICKUP;
            //g_btn_next =  STATE_CLICKIDLE;
        }
        break;
    case STATE_DOUBLECLICK: 
        if(!pressed){
            g_btn_next =  STATE_TRIPLECLICKIDLE; 
        } 
        break; 
    case STATE_TRIPLECLICKIDLE:
        if(pressed){
            g_btn_next =  STATE_TRIPLECLICKDEBOUNCE; 
            break;
        } 
        if (g_btn_diff >= _single_time){
            g_btn_next = STATE_IDLE;
            _event_map |= EVENT_DOUBLE_CLICK;
        }
        break;
    case STATE_TRIPLECLICKDEBOUNCE:
        if(pressed){
            if(g_btn_diff > _debounce_time){
                g_btn_next = STATE_TRIPLECLICK;
                _event_map |= EVENT_TRIPLE_CLICK;
            }
        } else {
            g_btn_next = STATE_IDLE;
            _event_map |= EVENT_DOUBLE_CLICK;
        }
        break;
    case STATE_TRIPLECLICK:    
    case STATE_LONGCLICK:       
        if(!pressed){
            g_btn_next =  STATE_OTHERUP; 
            _event_map |= EVENT_RELEASE;
        }
        break;
    case STATE_OTHERUP:              
        g_btn_next = STATE_IDLE;
        break;
  }
  
  if (g_btn_next != _state) {
    if(g_btn_next == STATE_IDLE)
       _release_counter = _free_time;
    _lastTransition = g_btn_now;
    _state = g_btn_next;
  }
  
}

bool SimpleButton::isClick(void){
    return (_event_map & (EVENT_SINGLE_CLICK | EVENT_DOUBLE_CLICK)) != 0;

}
bool SimpleButton::isSingleClick(void){
    return (_event_map & (EVENT_SINGLE_CLICK)) != 0;

}
bool SimpleButton::isDoubleClick(void){
     return (_event_map & (EVENT_DOUBLE_CLICK)) != 0;
}
bool SimpleButton::isLongClick(void){
    return (_event_map & (EVENT_LONG_CLICK)) != 0;
}
bool SimpleButton::isReleased(void){
    return (_event_map & (EVENT_RELEASE)) != 0;
}

bool SimpleButton::isFree(void){
    return (_state == STATE_IDLE) && (!_release_counter);
}
bool SimpleButton::isTripleClick(void){
    return (_event_map & (EVENT_TRIPLE_CLICK)) != 0;
}
bool PinButton::isPressed() { 
    byte val = digitalRead(_pin_num);
    if(_inverted) 
        val = !val; 
    return val;
}

word g_btn_now;
word g_btn_diff;
byte g_btn_next;
