#include "ZUNO_SERVO.h"

// global data
word g_servo_open;
word g_servo_close;
byte g_servo_state;
s_pin  g_servo_pin_ch1 = 11;



#define SERVO_STATE_CLOSED 		0
#define SERVO_STATE_OPENED 		1

#define SERVO_DEG000_INTERVAL	2180
#define SERVO_DEG180_INTERVAL	9560

#define SERVO_OFFWHOLE_INTERVAL 40000  
#define SERVO_OFFREST_INTERVAL  40000 // 10ms in the end of interval  

ZUNO_SETUP_ISR_GPTIMER(servo_gpt_handler);

ServoController::ServoController(s_pin p1)
{
	g_servo_pin_ch1 = p1;

	g_servo_open = SERVO_DEG000_INTERVAL;
	g_servo_close = SERVO_OFFWHOLE_INTERVAL - SERVO_DEG000_INTERVAL;
	g_servo_state = 0;

}

void ServoController::begin()
{

	pinMode(g_servo_pin_ch1, OUTPUT);
	digitalWrite(g_servo_pin_ch1, LOW);
	
	g_servo_state = 2;

	zunoGPTInit(ZUNO_GPT_CYCLIC);	
  	zunoGPTSet(800); 
  	zunoGPTEnable(1); 
  	zunoGPTSet(g_servo_open); 


}
void ServoController::end()
{
	zunoGPTEnable(0); 
}
void ServoController::setValue(word value)
{
	value *= ((SERVO_DEG180_INTERVAL - SERVO_DEG000_INTERVAL) / 180);
	value += SERVO_DEG000_INTERVAL;
	g_servo_open = value;
	g_servo_close = SERVO_OFFWHOLE_INTERVAL - value;
	
}
void ServoController::setValueRAW(word value)
{
	value /= 4;
	g_servo_open = value;
	g_servo_close = SERVO_OFFWHOLE_INTERVAL - value;
}

void servo_gpt_handler()
{
	g_servo_state ++;
	switch((g_servo_state % 3))
	{
		case 0:
			digitalWrite(g_servo_pin_ch1, HIGH);
			zunoGPTSet(g_servo_close); 
			break;
		case 1:
			digitalWrite(g_servo_pin_ch1, LOW);
			zunoGPTSet(SERVO_OFFREST_INTERVAL);
			break;
		case 2:
			zunoGPTSet(g_servo_open); 
			break;
	}	

}