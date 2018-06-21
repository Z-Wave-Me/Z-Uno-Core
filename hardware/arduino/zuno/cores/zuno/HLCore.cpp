#include "Arduino.h"

#include "ZUNO_Channels.h"

DWORD g_pulse_timeout_us = 0; 		// in microseconds
DWORD g_pulse_timeout_loops = 0;	 // in cycles == 1s

BYTE  g_pwm_pinmap = 0;
WORD  g_pwm_data[] = {0,0,0,0};

extern XBYTE stack_pointer_outside;
extern XBYTE user_stack_pointer_delta;
extern CCBYTE _cc_zuno_pinmap[26];

BYTE g_gpio_pin;
BYTE g_gpio_val;
WORD g_gpio_wval;



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
//	901     4573 us
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

#define F_CONTANT_NANOS 5123// 3069
#define C_CONTANT_NANOS 4900// 3200
 

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
void zunoLoadCFGParam(byte param_number, word * value) {
	param_number -= 64;
	param_number <<= 1;
	zunoSysCall(ZUNO_FUNC_EEPROM_READ, dword(START_CONFIGPARAM_EEPROM_ADDR + param_number), word(2), value);
}
void zunoSaveCFGParam(byte param_number, word * value) {
	param_number -= 64;
	param_number <<= 1;
	zunoSysCall(ZUNO_FUNC_EEPROM_WRITE, dword(START_CONFIGPARAM_EEPROM_ADDR + param_number), word(2), value); 
}
/* ----------------------------------------------------------------------------
									GPIO
-------------------------------------------------------------------------------*/
#define STRING_IT(S) #S
#define PORT_WRITE(PORT) if(g_gpio_val == 1) {zunoORI(#PORT, bpin);} else {zunoANDI(#PORT, ~(bpin));}
#define PORT_MODE(PORT) if(g_gpio_val){zunoORI(STRING_IT(PORT##DIR), bpin); PORT_WRITE(PORT) }else{zunoANDI(STRING_IT(PORT##DIR), ~(bpin));}

void rawPinMode() {
	zunoSI("PG",1);
	BYTE bpin = 1 << (_cc_zuno_pinmap[g_gpio_pin] & 0x0F);
	switch((_cc_zuno_pinmap[g_gpio_pin] & 0xF0)){
		case 0:
			PORT_MODE(P0);
			break;
		case 0x10:
			PORT_MODE(P1);
			break;
		case 0x20:
			PORT_MODE(P2);
			break;
		case 0x30:
			PORT_MODE(P3);
			break;
	}
}
void rawDigitalWrite() {
	zunoSI("PG",1);
	BYTE bpin = 1 << (_cc_zuno_pinmap[g_gpio_pin] & 0x0F);
	switch((_cc_zuno_pinmap[g_gpio_pin] & 0xF0)){
		case 0:
			PORT_WRITE(P0);
			break;
		case 0x10:
			PORT_WRITE(P1);
			break;
		case 0x20:
			PORT_WRITE(P2);
			break;
		case 0x30:
			PORT_WRITE(P3);
			break;
	}
}
BYTE rawDigitalRead(BYTE pin) {
	zunoSI("PG",1);
	BYTE bpin = 1 << (_cc_zuno_pinmap[g_gpio_pin] & 0x0F);
	switch((_cc_zuno_pinmap[g_gpio_pin] & 0xF0)){
		case 0:
			return ((zunoGI("P0") & bpin) != 0);
		case 0x10:
			return ((zunoGI("P1") & bpin) != 0);
		case 0x20:
			return ((zunoGI("P2") & bpin) != 0);
		case 0x30:
			return ((zunoGI("P3") & bpin) != 0);
	}	

	return 0;
}

WORD convertADCValue(WORD vl) {
    char tmp = g_ptr_config[ZUNO_CFG_BYTE_ADC_RES];
    if(tmp>8) {
      if(tmp > 12) {
        tmp -= 12;
        vl <<= tmp;

      }
      else
      {
        tmp = 12 - tmp;
        vl >>= tmp;
      }

    } else {
      tmp = 8-tmp;
      vl >>= tmp;
    }
    return vl;
}

WORD rawAnalogRead(BYTE pin) {
  WORD val = 0;
  zunoSI("PG",1);
  zunoANDI("ACN0",~(0x08)); // Single Mode
  if(pin == BATTERY){
    zunoORI("ACN0",0x01); // Battery monitor
  }else{
    zunoANDI("ACN0",~(0x01));  
  }
  // Upper Ref
  zunoANDI("ACN1",~(0x08)); // BGAB off
  if(g_ptr_config[ZUNO_CFG_BYTE_ADC_REF] & ADC_HIGH_VCC) {
    zunoORI("ACN2",0x20);     // VCC on
  } else {
    zunoANDI("ACN2",~(0x20));  // VCC off
    if(g_ptr_config[ZUNO_CFG_BYTE_ADC_REF] & ADC_HIGH_INTERNAL)
      zunoORI("ACN1",0x08);     // BGAB on
  }
  // Low Ref
  if(g_ptr_config[ZUNO_CFG_BYTE_ADC_REF] & ADC_LOW_GND) {
      zunoORI("ACN2",0x10); 
  } else {
      zunoANDI("ACN2",~(0x10)); 
  }
  // Buffering
  if(g_ptr_config[ZUNO_CFG_BYTE_ADC_ADV] & ZUNO_ADC_ADV_BUFFERED) {
    zunoORI("ACN1",0x10); 
  } else {
    zunoANDI("ACN1",~(0x10));
  }
  // Resolution
  if(g_ptr_config[ZUNO_CFG_BYTE_ADC_RES] <= 8) {
     zunoORI("ACN0",0x10); 
  } else {
     zunoANDI("ACN0",~(0x10));
  }
  // AZPL
  zunoORI("ACN2",g_ptr_config[ZUNO_CFG_BYTE_ADC_ADV] & 0xC0); 
  // PinSelect
  pin -= A0;
  pin &= 0x03;
  pin = (3-pin);
  zunoSI("ACN4", 1<<pin);
  zunoANDI("ACN1",~(0x03));
  zunoORI("ACN1",pin); 
  // --- Power ON ADC
  zunoORI("ACN0", 0x80);
  // --- start conversion
  zunoSI("ACD1", 0x80);
  zunoSI("ACD1", 0x40);
  zunoORI("ACN0", 0x40);
  // Waiting for ADC conversion...
  while(!(zunoGI("ACD1") & 0x80));
  // read values
  zunoSI("ACD1", 0x80);
  val = zunoGI("ACD2");
  if(!(zunoGI("ACN0") & 0x10)){
    // 12bit mode
    val <<= 4;
    val |= zunoGI("ACD1") & 0x0F; 
  }
  val = convertADCValue(val);
  // --- disable conversion
  zunoANDI("ACN0", 0x7F);
  zunoORI("ACN0", 0x80);
  zunoSI("ACD1", 0xC0);
  // --- free pins
  zunoSI("ACN4", 0x00);
  // Power OFF ADC
  zunoANDI("ACN0", 0x7F);
  return val;

}
void rawAnalogWrite() {
	BYTE tmp;
	BYTE i;
	// use the same pin number for GPIO & PWM
	g_gpio_pin -= PWM1;
	g_gpio_pin &= 0x03;
	tmp = ~(0x10 << g_gpio_pin);

	zunoSI("PG",1);
	zunoANDI("P0DIR", tmp);
	if(g_gpio_wval == 0) {
		zunoANDI("P0", tmp);
		g_pwm_pinmap &= (tmp >> 4);
		return;
	}
	g_pwm_pinmap |= (1 << g_gpio_pin);
	if(g_ptr_config[ZUNO_CFG_BYTE_PWM_RES] < 16)
		g_gpio_wval <<= (16-g_ptr_config[ZUNO_CFG_BYTE_PWM_RES]);
	g_pwm_data[g_gpio_pin] = g_gpio_wval;
	zunoSI("PG",0);
	zunoSI("LD0", g_pwm_pinmap);
	zunoSI("LD1", 0);
	for(i=0;i<4;i++){
		zunoSI("LDD",g_pwm_data[i] & 0xFF);
		zunoSI("LDD",g_pwm_data[i] >> 8);
	}
	zunoORI("LD0", 0x10);
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