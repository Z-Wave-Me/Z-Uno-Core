#pragma once

#include "Arduino.h"

// Очень простая и "легкая" библиотеа для работы с KeyPad


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

		// Должна циклтчно вызываться
		BYTE scanKeys(BYTE * key_actions);		// Возвращает количество нажатых клавиш
												// Параметр - описание нажатых клавишь		
		// Чтобы можно было наследовать и переоgределять сканирование 
		// для другого железа... работу с пинами 
		// делаем через виртуальные функции
		virtual void m_pinMode(BYTE pin, BYTE mode) { pinMode(pin, mode);};
		virtual BYTE m_pinGet(BYTE pin) { return digitalRead(pin); };
		virtual void m_pinSet(BYTE pin, BYTE value) { digitalWrite(pin, value); };

		// Инициализация пинов KeyPada
		void begin();
		void clearKeys(){mon_keys = 0;};

		// Переопределение временных констант
		// Время измеряется в 10-ках миллисекунд
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