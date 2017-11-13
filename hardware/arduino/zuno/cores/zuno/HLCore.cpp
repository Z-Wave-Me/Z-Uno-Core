#include "Arduino.h"

#include "ZUNO_Channels.h"

DWORD g_pulse_timeout_us = 0; 		// in microseconds
DWORD g_pulse_timeout_loops = 0;	 // in cycles == 1s
extern XBYTE stack_pointer_outside;
extern XBYTE user_stack_pointer_delta;

/* ----------------------------------------------------------------------------
							Arduiino-like functions
-------------------------------------------------------------------------------*/

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

void setPulseTimeout(DWORD timeout) {
	g_pulse_timeout_us = timeout;
	US_TO_LOOPS(timeout);
	g_pulse_timeout_loops = timeout;
}
// Be careful. Don't use values < 500uS for timeout
DWORD pulseIn(s_pin pin, byte level, DWORD timeout) {
	DWORD width = 0;
	//DWORD timeout;
	byte  port_mask = 1 << (pin - 9);
	// If you change timeout...
	if(timeout != g_pulse_timeout_us) {
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
	while(digitalRead(pin) == port_mask) {
		if(!timeout) {	
			interrupts();
			return 0;
		}
		timeout--;
	}
	// wait for the pulse to start
	while(digitalRead(pin) != port_mask) {
		if(!timeout) {	
			interrupts();
			return 0;
		}	
		timeout--;
	}
	
	width = timeout;
	// wait for the pulse to stop
	while(digitalRead(pin) == port_mask) {
		width--;
		if(!width) {	
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

#define DELAYUS_STATIC_LOOP_CYCLES 14
#define DELAYUS_STATIC_LOOP_SUB    9

// Just a simple busy-waiting loop
void delayLoops(byte v) {
	zunoASM("\nmov r6,dpl\n"
			"delayLoops_LOOP:\n"    
    		"djnz r6,delayLoops_LOOP\n"
    		"ret\n");
}

// The minimum delay is 9 uS
// The precision is about 4 us
// Function more precise for intervals 
// interval = 9 + n*4
// 9 13 17 21 25 29 33 37 41 45 49 ... 97 ... 101 ... 16009
// Any other interval will be moved to nearest low value of this raw  
void delayMicroseconds(word us) {
	NOPS(1);
	if(us<DELAYUS_STATIC_LOOP_SUB)
	{
		us = DELAYUS_STATIC_LOOP_SUB;
	}
	else
	{
		NOPS(5);
	}
	us -= DELAYUS_STATIC_LOOP_SUB;
	us >>= 2; // every loop tuned to 4 useconds
	while(us--)
	{
		NOPS(DELAYUS_STATIC_LOOP_CYCLES);
	}
}
DWORD millis(void) {
	zunoSysCall(ZUNO_FUNC_MILLIS);
	return SYSRET_DW;
}
void delay(dword value) {
	zunoLCALL(SAVE_SKETCHCONTENT_SUBROUTINE);
	zunoSysCall(ZUNO_FUNC_DELAY_MS, value); // We have to setup value of before we call this function to avoid stack loss
	if(SYSRET_B == 0xFF)
		return;
	user_stack_pointer_delta = zunoGI("SP");
	user_stack_pointer_delta -= stack_pointer_outside;
	zunoLCALL(SAVE_USERSTACK_SUBROUTINE);
	zunoSI("SP", stack_pointer_outside); //is the next we store before entering
	return;
}
/// math

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/* ----------------------------------------------------------------------------
							Z-Uno Z-Wave network configuration
-------------------------------------------------------------------------------*/
void zunoAddChannel(byte type, byte st, byte p) {
	if(g_user_sketch.n_channels > ZUNO_MAX_MULTI_CHANNEL_NUMBER)
		return;
	g_user_sketch.channels[g_user_sketch.n_channels].main_type = type;
	g_user_sketch.channels[g_user_sketch.n_channels].sub_type = st;
	g_user_sketch.channels[g_user_sketch.n_channels].prop = p;
	g_user_sketch.n_channels++;
}
void zunoAddAssociation(byte t) {
	if(g_user_sketch.n_assocs > ZUNO_MAX_ASSOC_NUMBER)
		return;
	g_user_sketch.asociations[g_user_sketch.n_assocs] = t;
	g_user_sketch.n_assocs++;
}
/* ----------------------------------------------------------------------------
							Z-Uno config parameters
-------------------------------------------------------------------------------*/
void zunoLoadCFGParam(byte param_number, dword * value) {
	param_number -= 64;
	param_number <<= 2;
	zunoSysCall(ZUNO_FUNC_EEPROM_READ, dword(START_CONFIGPARAM_EEPROM_ADDR + param_number), word(4), value);
}
void zunoSaveCFGParam(byte param_number, dword * value) {
	param_number -= 64;
	param_number <<= 2;
	zunoSysCall(ZUNO_FUNC_EEPROM_WRITE, dword(START_CONFIGPARAM_EEPROM_ADDR + param_number), word(4), value); 
}
/* ----------------------------------------------------------------------------
									GPIO
-------------------------------------------------------------------------------*/
BYTE digitalRead(BYTE pin) {
	zunoSysCall(ZUNO_FUNC_DIGITAL_READ, byte(pin));
	return SYSRET_B;
}
WORD analogRead(BYTE pin) {
	zunoSysCall(ZUNO_FUNC_ANALOG_READ, byte(pin));
	return SYSRET_W;
}
// Broken subroutine
// Memset was broken in stdlibrary of SDCC
void _zme_memset(byte * ptr, byte val, int count)
{
	while(count--)
	{
		*ptr = val;
		ptr++;
	}
}
