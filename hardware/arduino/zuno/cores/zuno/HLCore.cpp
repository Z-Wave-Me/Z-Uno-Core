#include "Arduino.h"

#include "ZUNO_call_proto.h"

DWORD g_pulse_timeout_us = 0; // in microseconds
DWORD g_pulse_timeout_loops = 0; // in cycles == 1s


// calculate micros
// 
// time = c  + cycles * f
// where c & f are constants
// We can calculate it by means of logic analyzer 
// We have at least 2 measures and make system of equaluation
// 1)  measured_timeout1 = c + n1 * f
// 2)  measured_timeout2 = c + n2 * f
// Just take a table by means of logic analyzer
// #loops	#us
//	14		80.31us
//	425		2156 us
//	901    4573 us
//	144		738.4 us
// 
// 	s1 = n1*f + c
//	s2 = n2*f + c
//	80 		=	14*f + c
//	4573	=	901*f + c  we have c = 4573 - 901*f
//	887*f = 4493
//	f = 5.06538 us
//	c = 9.09262 us
// We have to use fixed-point math. All values are in nano seconds (1e-9)

#define F_CONTANT_NANOS 3069
#define C_CONTANT_NANOS 3200
 

#define LOOPS_TO_US(L) L *= F_CONTANT_NANOS; L += C_CONTANT_NANOS; L /= 1000;    
#define US_TO_LOOPS(U) U *= 1000; U -= C_CONTANT_NANOS; U /= F_CONTANT_NANOS;


void setPulseTimeout(DWORD timeout)
{
	
	g_pulse_timeout_us = timeout;

	US_TO_LOOPS(timeout);

	g_pulse_timeout_loops = timeout;
}
// Be careful. Don't use values < 500uS for timeout
DWORD pulseIn(s_pin pin, byte level, DWORD timeout)
{
	DWORD width = 0;
	//DWORD timeout;
	byte  port_mask = 1 << (pin - 9);
	

	// If you change timeout...
	if(timeout != g_pulse_timeout_us)
	{
		// ... it takes about 500uS for 8051
		// don't do it every time
		setPulseTimeout(timeout); 
	}	
	// use precaculated value of timeout in "loops"
	timeout = g_pulse_timeout_loops;
	if(!level)
		port_mask = 0;	
	
	
	noInterrupts();
	// wait for any previous pulse to end
	while(digitalRead(pin) == port_mask)
	{
		if(!timeout) 
		{	
			interrupts();
			return 0;
		}
		timeout--;
	}
	// wait for the pulse to start
	while(digitalRead(pin) != port_mask)
	{
		if(!timeout) 
		{	
			interrupts();
			return 0;
		}	
		timeout--;
	}
	
	width = timeout;
	// wait for the pulse to stop
	while(digitalRead(pin) == port_mask)
	{
		width--;
		if(!width)
		{	
			interrupts();
			return 0;
		}	
	}
	interrupts();

	timeout -= width;


	// time = c  + cycles * f
	// see below
	// calculate micros
	
	LOOPS_TO_US(timeout);	
	return timeout;	
}

/// math
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#if 0
void testZunoCall()
{
	word  w1 = 0xAABB;
	DWORD dw1 = 0xCCDDEEF0;

	zunoPushDword(dw1);
	zunoPushWord(w1);
	zunoPushByte(ZUNO_FUNC_TEST);
	zunoCall();

	w1 = zunoPopWord();
	dw1 = zunoPopDWORD();

	#if 1
	// DBG
	Serial0.println("TEST_FUNC Result");
	Serial0.println(w1, HEX);
	Serial0.println(dw1, HEX);
	Serial0.println("----------------");
	
	#endif
	
}
#endif