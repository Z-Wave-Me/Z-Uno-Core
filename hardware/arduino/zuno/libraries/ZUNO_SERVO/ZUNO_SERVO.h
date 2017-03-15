#ifndef ZUNO_SERVO_LIB
#define ZUNO_SERVO_LIB

#include "Arduino.h"

class ServoController
{

	public:
		ServoController(s_pin p1);
		void begin();
		void end();
		void setValue(word value);
		void setValueRAW(word value);
	private:
		
		byte my_data;
};

#endif // ZUNO_SERVO_LIB