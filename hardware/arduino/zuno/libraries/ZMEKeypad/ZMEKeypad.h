#pragma once

#include "Arduino.h"


// Lightwieght realisation of Keypad controller 


#define MAX_SCAN_KEYS 3
struct KeyState_s
{
	BYTE 	key_num;
	BYTE 	key_state;
	DWORD	time;
};

enum 
{
		KEYPAD_KEYSTATE_IDLE,
		KEYPAD_KEYSTATE_DEBOUNCE,
		KEYPAD_KEYSTATE_PRESSED,
		KEYPAD_KEYSTATE_HOLDED

};


#define DEBOUNCE_DEFAULT_TIME 10
#define HOLD_DEFAULT_TIME 200

#define KEYPAD_KEYINDEX(A) (A & ~(0x80))
#define KEYPAD_IS_HOLDING(A) (A & (0x80))



class ZMEKeypad
{

	public:
		ZMEKeypad(BYTE * colums, BYTE numcols, BYTE * rows, BYTE numrows);

		// You have to poll this function in the loop
		// Returns number of pressed keys
		// OUT: key_actions 
		BYTE scanKeys(BYTE * key_actions);		

		// You can use this as superclass and overrite these functions
		// for another hartware. 
		virtual void m_pinMode(BYTE pin, BYTE mode) { pinMode(pin, mode);};
		virtual BYTE m_pinGet(BYTE pin) { return digitalRead(pin); };
		virtual void m_pinSet(BYTE pin, BYTE value) { digitalWrite(pin, value); };

		// Initializes keyscanner
		void begin();
		void clearKeys(){mon_keys = 0;};

		// You can setup hold/debounce intervals in the 10th of microseconds
		void setHoldTime(BYTE tm){hold_time = tm;};
		void setDebounceTime(BYTE tm){debounce_time = tm;};

		// Checks if all the buttons was released
		BYTE isIdled(){return mon_keys == 0;};

	private:

		BYTE * column_vec; 
		BYTE * row_vec;
		BYTE   n_col, n_row;
		
		KeyState_s key_states[MAX_SCAN_KEYS];
		BYTE       mon_keys;

		BYTE   debounce_time;
		BYTE   hold_time;



};