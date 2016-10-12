#pragma once

#include "Arduino.h"

// Light-weight library for KeyPad
// (c) Z-Wave>ME for Z-Uno project


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

		// This function have to be called in loop repeatedly
		BYTE scanKeys(BYTE * key_actions);		// Return the number of pressed keys as the name of function
												// Te first parameter is using to return state of keys	
		// To make abstraction from hardware we use virtual function
		virtual void m_pinMode(BYTE pin, BYTE mode) { pinMode(pin, mode);};
		virtual BYTE m_pinGet(BYTE pin) { return digitalRead(pin); };
		virtual void m_pinSet(BYTE pin, BYTE value) { digitalWrite(pin, value); };

		// Initializing of keyscanner
		void begin();
		void clearKeys(){mon_keys = 0;};

		// To make KeyScanner more universaly - we can customize some time intervals 
		void setHoldTime(BYTE tm){hold_time = tm;};
		void setDebounceTime(BYTE tm){debounce_time = tm;};


	private:

		BYTE * column_vec; 
		BYTE * row_vec;
		BYTE   n_col, n_row;
		
		KeyState_s key_states[MAX_SCAN_KEYS];
		BYTE       mon_keys;

		BYTE   debounce_time;
		BYTE   hold_time;



};