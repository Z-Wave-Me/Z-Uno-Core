/*
 * Copyright (c) 2016 by Z-WAVE>ME (www.z-wave.me) for Z-Uno Project (z-uno.z-wave.me)
 * 		
 *					Hardware IR-controller 
 *
 * 			Adds ability to use built in IR controller in your Z-Uno projects
 *
 *			All vendor-specific code was ported from IRremote library for Arduino
 *   
 *     INPUT 		of IR-Controller is PIN#7
 *     OUTPUTS 		of IR-Controller are PIN#6,PIN#5,PIN#4 (You can use pins together to increase power, 8mA is max for one pin)	
 *								
 */

#include "IRController.h"
#include "ZUNO_call_proto.h"

#include "Arduino.h"

#define DEBUG_IR_MATCH		0
#define DEBUG_IR_DETECT  	0

// Vendor specific contants

// AIWA
#define AIWA_RC_T501_HZ            38
#define AIWA_RC_T501_BITS          15
#define AIWA_RC_T501_PRE_BITS      26
#define AIWA_RC_T501_POST_BITS      1
#define AIWA_RC_T501_SUM_BITS    (AIWA_RC_T501_PRE_BITS + AIWA_RC_T501_BITS + AIWA_RC_T501_POST_BITS)
#define AIWA_RC_T501_HDR_MARK    8800
#define AIWA_RC_T501_HDR_SPACE   4500
#define AIWA_RC_T501_BIT_MARK     500
#define AIWA_RC_T501_ONE_SPACE    600
#define AIWA_RC_T501_ZERO_SPACE  1700


// LG
#define LG_BITS 28
#define LG_HDR_MARK 8000
#define LG_HDR_SPACE 4000
#define LG_BIT_MARK 600
#define LG_ONE_SPACE 1600
#define LG_ZERO_SPACE 550
#define LG_RPT_LENGTH 60000

// Mitsubishi
// Not implemented yet
#define MITSUBISHI_BITS 16
// #define MITSUBISHI_HDR_MARK	250  // seen range 3500
#define MITSUBISHI_HDR_SPACE	350 //  7*50+100
#define MITSUBISHI_ONE_MARK	1950 // 41*50-100
#define MITSUBISHI_ZERO_MARK  750 // 17*50-100

// NEC
#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

// Panasonic 
#define PANASONIC_ADDR_BITS     16
#define PANASONIC_DATA_BITS     32 
#define PANASONIC_HDR_MARK    3502
#define PANASONIC_HDR_SPACE   1750
#define PANASONIC_BIT_MARK     502
#define PANASONIC_ONE_SPACE   1244
#define PANASONIC_ZERO_SPACE   400


// RC5
#define MIN_RC5_SAMPLES     11
#define RC5_T1             889
#define RC5_RPT_LENGTH   46000

// RC6
#define MIN_RC6_SAMPLES      1
#define RC6_HDR_MARK      2666
#define RC6_HDR_SPACE      889
#define RC6_T1             444
#define RC6_2T1            889
 
#define RC6_RPT_LENGTH   46000


// SAMSUNG
#define SAMSUNG_BITS          32
#define SAMSUNG_HDR_MARK    5000
#define SAMSUNG_HDR_SPACE   5000
#define SAMSUNG_BIT_MARK     560
#define SAMSUNG_ONE_SPACE   1600
#define SAMSUNG_ZERO_SPACE   560
#define SAMSUNG_RPT_SPACE   2250

// SONY
#define SONY_BITS                   12
#define SONY_HDR_MARK             2400
#define SONY_HDR_SPACE             600
#define SONY_ONE_MARK             1200
#define SONY_ZERO_MARK             600
#define SONY_RPT_LENGTH          45000
#define SONY_DOUBLE_SPACE_USECS    500  // usually ssee 713 - not using ticks as get number wrapround


#define mark(V)  ir_fill_width = V; ir_fill_mark = 0x80; fill_interval();
#define space(V) ir_fill_width = V; ir_fill_mark = 0x00; fill_interval();

#define MAX_USEC_ERROR 150
#define DF_ABS(A,B) (A>B ? A - B : B - A)
#define MATCH_MARK(V) ((DF_ABS(V, ir_current_width) < MAX_USEC_ERROR) &&  (ir_current_mark))
#define MATCH_SPACE(V) ((DF_ABS(V, ir_current_width) < MAX_USEC_ERROR) &&  (!ir_current_mark))
#define MATCH_MARKSPACE(V, M) ((DF_ABS(V, ir_current_width) < MAX_USEC_ERROR) &&  (M == ir_current_mark))

#define nextBitC0(V) nextBitC(V, 0)
#define nextBitC1(V) nextBitC(V, 1)

#define IR_ENCODER_FLAG_CONVERT_US 1
#define IR_ENCODER_FLAG_DIVIDE 	   2

#define LEVEL_SPACE 	0x00
#define LEVEL_MARK 		0x80
#define LEVEL_UNDEFINED 0xFF



// Global varriables for inner sub-routine
// We have to minimize usage of stack 
extern byte * ir_buff;
extern byte ir_m_prescaller;
extern byte ir_detected_nbits;
// encoder varriables
extern word ir_offset;
extern word ir_length;
extern word ir_current_width;
extern word ir_current_divider;
extern byte ir_current_mark;
extern byte ir_match_flags;
extern word ir_fill_width;
extern byte ir_fill_mark;
extern word ir_fill_last_offset;

// detected carrier periods
extern byte ir_carrier_low;
extern byte ir_carrier_hi;
extern unsigned long ir_mask;




// Some auxilary C-style functions
// ! We have to decrease stack size
// So, we implement this outside the IRController class 
// -------------------------------------------
void cleanCommandData(IRCommand_t * ptr)
{
	ptr->data[0] = 0;
	ptr->data[1] = 0;
}
void rollCommandData0(IRCommand_t * ptr)
{
	ptr->data[0] <<= 1;
}
void appendCommandData0(IRCommand_t * ptr)
{
	ptr->data[0] |= 1;
}

void rollCommandData1(IRCommand_t * ptr)
{
	ptr->data[1] <<= 1;
}
void appendCommandData1(IRCommand_t * ptr)
{
	ptr->data[1] |= 1;
}

void initMask(byte bits)
{
	ir_mask = 0x01;
	ir_mask <<= (unsigned long)bits;
}

bool nextBitC(IRCommand_t * ptr, byte pt)
{
	bool v = (ir_mask & ptr->data[pt]) > 0;
	ir_mask >>= 1;
	return v;
}

void roll_interval()
{
	byte cdata =  ir_buff[ir_offset];
	ir_current_mark = cdata & 0x80;

	while((cdata & 0x80) == ir_current_mark)
	{
		ir_current_width <<= 7;
		cdata &=  0x7F;
		ir_current_width |= cdata;
		ir_offset++;
		if(ir_offset >= ir_length)
			break;
		cdata = ir_buff[ir_offset];
		
	}	

}
void ticks2us()
{
	// convert ticks to microseconds
	// time_of_tick = (2^msp/(32*10^6)) seconds
	// so in microseconds we have:
	// time_of_tick = 10^6*(2^msp/((2^5)*10^6)) microseconds
	// and finaly
	// time_of_tick = 2^(msp - 5)
	// replace *2^(msp - 5) with shifts we have:
	if(ir_m_prescaller > 5)
		ir_current_width <<= (ir_m_prescaller - 5);
	else
		ir_current_width >>= (5 - ir_m_prescaller);	
}
bool divide_interval()
{
	if(ir_current_width < MAX_USEC_ERROR)
		return false;
	
	if(ir_current_width < ir_current_divider)
	{
		if((ir_current_divider - ir_current_width) > MAX_USEC_ERROR)
		{
			ir_current_mark = LEVEL_UNDEFINED;
			return true; // STOP it! - undefined interval 
		}	
		ir_current_width = 0; 
		return true;

	}
	ir_current_width -= ir_current_divider;
	return true;
	
}

void next_interval()
{
	
	if(ir_offset >= ir_length)
	{
		ir_current_mark  = 0; // SPACE by default
		return;

	}	
	if(ir_match_flags & IR_ENCODER_FLAG_DIVIDE)	
		if(divide_interval()) // We can feed a quant of data from the previous interval 
			return;
		
	ir_current_width = 0;
	roll_interval();
	
	if(ir_match_flags & IR_ENCODER_FLAG_CONVERT_US)
		ticks2us();

	if(ir_match_flags & IR_ENCODER_FLAG_DIVIDE)	
	{
		divide_interval();
	}
}

void us2ticks()
{
	// convert value from microseconds to ticks
	// time_of_tick = (2^msp/(32*10^6)) seconds
	// so in microseconds we have:
	// time_of_tick = 10^6*(2^msp/((2^5)*10^6)) microseconds
	// and finaly
	// time_of_tick = 2^(msp - 5)
	// replace /2^(msp - 5) with shifts we have:
	if(ir_m_prescaller > 5)
		ir_fill_width >>= (ir_m_prescaller - 5);
	else
		ir_fill_width <<= ( 5 - ir_m_prescaller);
}

void add_last_interval()
{
	// the last interval was the same with current interval
	// just implement additive rule 
	if((ir_offset > 0) && (ir_fill_mark == ir_current_mark))
	{
		ir_offset -= ir_fill_last_offset;
		ir_current_width +=  ir_fill_width;
		return;
	}
	ir_current_width =  ir_fill_width;

}
void fillMSBByte()
{
	if(ir_current_width > 0x3FFF)
	{
		ir_buff[ir_offset++] 	=  (ir_current_width >> 14)  | ir_current_mark ; 
		ir_fill_last_offset++;
	}
}
void fillMIDByte()
{
	if(ir_current_width > 0x7F)
	{
		ir_buff[ir_offset++] =  ((ir_current_width >> 7) & 0x7F) | ir_current_mark;
		ir_fill_last_offset++; 
	}
}
void fillLSBByte()
{
	ir_buff[ir_offset++] =  (ir_current_width & 0x7F) | ir_current_mark; 
	ir_fill_last_offset++;
}
void fill_interval()
{
		
	us2ticks();

	// Additive rule
	add_last_interval();
	// Save current interval parameters
	ir_current_mark  = ir_fill_mark;
	ir_fill_last_offset = 0;

	// Hardware controller uses variable bitrate for marks/spaces
	// from 8 to 16 bits (1-3byte). 7th bit is always MARK flag
	// Convert data to this format 
	fillMSBByte();
	fillMIDByte();
	fillLSBByte();	

}
void ir_read_data()
{
	zunoPushWord(0);
	zunoPushByte(ZUNO_FUNC_IR_IO);
    zunoCall();	
    ir_carrier_hi  	= zunoPopByte();
    ir_carrier_low 	= zunoPopByte();
	ir_length    	= zunoPopWord();
}
void ir_write_data()
{	
	zunoPushWord(ir_offset);
	zunoPushByte(ZUNO_FUNC_IR_IO);
    zunoCall();	
   
}
//----------------------------------------------------	
byte IRReceiverParams::getFlags()
{
	return flags;
}
byte IRTransmitterParams::getFlags()
{
	return flags;
}



IRReceiverParams::IRReceiverParams(int vendor, byte fl):
									flags(fl),
									ms_prescaller(IR_MS_PRESCALLER_2MHZ),
									carrier_prescaller(IR_MS_PRESCALLER_8MHZ),
									carrier_averagger(IR_AVERAGER_8P),
									glitch_remover(IR_GLITCH_REMOVER_500NS),
									trail_space(IR_TRAILSPACE_16K)	
{
	if(vendor == IR_VENDOR_RC6)
		ms_prescaller = IR_MS_PRESCALLER_4MHZ;

}
void IRTransmitterParams::setupVendor(int vendor_id)
{
	byte freq = 38;	
	switch(vendor_id)
	{
		case IR_VENDOR_AIWA:
		case IR_VENDOR_NEC:
		case IR_VENDOR_LG:
		case IR_VENDOR_SAMSUNG:
			freq = 38;
			break;
		case IR_VENDOR_SONY:
			freq = 40;
			break;
		case IR_VENDOR_RC5:
		case IR_VENDOR_RC6:
			freq = 36;
			break;
		case IR_VENDOR_PANASONIC:
			freq = 35;
			break;
	
	}
	setup_kHz(freq);
}
void IRTransmitterParams::setup_kHz(byte freq)
{
	word freq_interv = 32000;
	freq_interv /= (carrier_prescaller + 1); //>>= carrier_prescaller;
	freq_interv /= freq;
	freq_interv /= 3;
	carrier_high = freq_interv;
	carrier_high--;
	freq_interv <<= 1;
	carrier_low = freq_interv;
	carrier_low--;
}

IRController::IRController()
{
	ir_buff = (BYTE *) 3064; // DMA Adress of IR. Do not modify!
}
void IRController::begin(IRReceiverParams * receiver)
{
  	ir_m_prescaller = receiver->getMSPrescaller();
 
	zunoPushWord(reinterpPOINTER((byte*)receiver));
	zunoPushByte(ZUNO_FUNC_IR_SETUP);
    zunoCall();

	   
}
void IRController::begin(IRTransmitterParams * transmitter)
{
	
  	ir_m_prescaller = transmitter->getMSPrescaller();
 
	zunoPushWord(reinterpPOINTER((byte*)transmitter));
	zunoPushByte(ZUNO_FUNC_IR_SETUP);
    zunoCall();	

}
void IRController::scan()
{
	zunoPushByte(ZUNO_FUNC_IR_LEARN_RESET);
    zunoCall();	
    
}
byte IRController::getState()
{
	zunoPushByte(ZUNO_FUNC_IR_STATUS);
    zunoCall();	
    return zunoPopByte();
}


void IRController::end()
{
	zunoPushWord(0);
	zunoPushByte(ZUNO_FUNC_IR_SETUP);
    zunoCall();	
}





byte IRController::detectCommand(IRCommand_t * command)
{
	#if DEBUG_IR_DETECT
		Serial0.println("Trying AIWA: ");
	#endif
	if(readCommand_AIWA(command))
		return true;

	#if DEBUG_IR_DETECT
		Serial0.println("Trying LG: ");
	#endif
	if(readCommand_LG(command))
		return true;

	#if DEBUG_IR_DETECT
		Serial0.println("Trying NEC: ");
	#endif
	if(readCommand_NEC(command))
		return true;	

	#if DEBUG_IR_DETECT
		Serial0.println("Trying PANASONIC: ");
	#endif
	if(readCommand_PANASONIC(command))
		return true;
	
	#if DEBUG_IR_DETECT
		Serial0.println("Trying RC5: ");
	#endif
	if(readCommand_RC5(command))
		return true;

	#if DEBUG_IR_DETECT
		Serial0.println("Trying RC6: ");
	#endif
	if(readCommand_RC6(command))
		return true;

	#if DEBUG_IR_DETECT
		Serial0.println("Trying SAMSUNG: ");
	#endif
	if(readCommand_SAMSUNG(command))
		return true;
	
	#if DEBUG_IR_DETECT
		Serial0.println("Trying SONY: ");
	#endif
	if(readCommand_SONY(command))
		return true;

	command->n_bits = 0;
	command->vendor = IR_VENDOR_UNKNOWN;
	
	
	return false;
}
// Specific vendor version of previous method (helps to reduce memory if you know your vendor)
bool IRController::readCommand_AIWA(IRCommand_t* command)
{
	cleanCommandData(command);

	ir_offset = 0;
	ir_current_width = 0;
	
	ir_match_flags = IR_ENCODER_FLAG_CONVERT_US;
	
	// Read data from ir-controller
	ir_read_data();
	next_interval();
	// Check HDR Mark/Space
	if (!MATCH_MARK (AIWA_RC_T501_HDR_MARK ))  
		return false;
	next_interval();
	if (!MATCH_SPACE(AIWA_RC_T501_HDR_SPACE))  
		return false;
	next_interval();

	byte i = 26;
	while(i--)
	{
		if (!MATCH_MARK(AIWA_RC_T501_BIT_MARK))  
			return false;
		next_interval();
		rollCommandData0(command);
		if(MATCH_SPACE(AIWA_RC_T501_ONE_SPACE))	
			appendCommandData0(command);
		else
		if(!MATCH_SPACE(AIWA_RC_T501_ZERO_SPACE))
			return false;
		next_interval();

	}
	i = 15;
	while(i--)
	{

		if (!MATCH_MARK(AIWA_RC_T501_BIT_MARK))  
			return false;
		next_interval();
		rollCommandData1(command);
		if(MATCH_SPACE(AIWA_RC_T501_ONE_SPACE))	
			appendCommandData1(command);
		else
		if(!MATCH_SPACE(AIWA_RC_T501_ZERO_SPACE))
			return false;
		next_interval();

	}
	command->n_bits = 26 + 15;
	command->vendor = IR_VENDOR_AIWA;
	return true; 
}
bool IRController::readCommand_LG(IRCommand_t * command)
{
	ir_offset = 0;
	ir_current_width = 0;
	
	cleanCommandData(command);

	ir_match_flags = IR_ENCODER_FLAG_CONVERT_US;
	
	// Read data from ir-controller
	ir_read_data();
	// start from  the first interval
	next_interval();

	 // Initial mark/space
    if (!MATCH_MARK(LG_HDR_MARK))  
    	return false;
    next_interval();
    if (!MATCH_SPACE(LG_HDR_SPACE))  
    	return false;
    next_interval();
    byte i = LG_BITS;
    while(i--)
    {
        if (!MATCH_MARK(LG_BIT_MARK))  
        	return false;
        next_interval();
        rollCommandData0(command);
        if(MATCH_SPACE(LG_ONE_SPACE))   
        	appendCommandData0(command);
        else 
        if (!MATCH_SPACE(LG_ZERO_SPACE))  
    		return false;
    	next_interval();
    }

    // Stop bit
    if (!MATCH_MARK(LG_BIT_MARK))   
    	return false;

    command->n_bits = LG_BITS;
	command->vendor = IR_VENDOR_LG;
	return true;
}	

bool IRController::readCommand_NEC(IRCommand_t* command)
{
	cleanCommandData(command);
	ir_offset 	= 0;
	ir_current_width = 0;
	
	ir_match_flags = IR_ENCODER_FLAG_CONVERT_US;
	
	// Read data from ir-controller
	ir_read_data();

	// start from  the first interval
	next_interval();
	// Check header "mark"
	if (!MATCH_MARK(NEC_HDR_MARK))  
		return false;
	
	next_interval();
	// Check header "space"
	if (!MATCH_SPACE(NEC_HDR_SPACE))  
		return false;
	next_interval();
	// Build the data
	byte i = NEC_BITS;
	while(i--)
	{
		// Check data "mark"
		if (!MATCH_MARK(NEC_BIT_MARK))  
			return false;
		next_interval();
		rollCommandData0(command);
		// Suppend this bit
		if (MATCH_SPACE(NEC_ONE_SPACE ))  
			appendCommandData0(command);
		else 
		if (!MATCH_SPACE(NEC_ZERO_SPACE))  
			return false;  
		next_interval();                                                 
	}

	command->n_bits = NEC_BITS;
	command->vendor = IR_VENDOR_NEC;

	return true;
}
bool IRController::readCommand_PANASONIC(IRCommand_t* command)
{
	cleanCommandData(command);
	ir_offset 	= 0;
	ir_current_width = 0;
	
	ir_match_flags = IR_ENCODER_FLAG_CONVERT_US;
	
	// Read data from ir-controller
	ir_read_data();

	// start from  the first interval
	next_interval();
	if (!MATCH_MARK(PANASONIC_HDR_MARK ))  
		return false;

	next_interval();
    if (!MATCH_SPACE(PANASONIC_HDR_SPACE))  
    	return false;

    next_interval();

    byte i = PANASONIC_ADDR_BITS;
    while(i--)
    {
    	if (!MATCH_MARK(PANASONIC_BIT_MARK))  
    		return false;
    	next_interval();
    	rollCommandData0(command);
		if(MATCH_SPACE(PANASONIC_ONE_SPACE))
			appendCommandData0(command);
		else 
		if(!MATCH_SPACE(PANASONIC_ZERO_SPACE))
			return false;
		next_interval();
    	
	
    }
    i = PANASONIC_DATA_BITS;
    while(i--)
    {
    	if (!MATCH_MARK(PANASONIC_BIT_MARK))  
    		return false;
    	next_interval();
    	rollCommandData1(command);
		if(MATCH_SPACE(PANASONIC_ONE_SPACE))
			appendCommandData1(command);
		else 
		if(!MATCH_SPACE(PANASONIC_ZERO_SPACE))
			return false;
		next_interval();
	
    }
    
    command->n_bits = PANASONIC_ADDR_BITS + PANASONIC_DATA_BITS;
	command->vendor = IR_VENDOR_PANASONIC;
   
	return true;
}
bool IRController::readCommand_RC5(IRCommand_t* command)
{

	
	cleanCommandData(command);
	ir_offset 		= 0;
	ir_current_width = 0;
	
	ir_detected_nbits 	= 0;

	ir_match_flags 	= IR_ENCODER_FLAG_CONVERT_US | IR_ENCODER_FLAG_DIVIDE;
	ir_current_divider = RC5_T1;
	
	// Read data from ir-controller
	ir_read_data();

	// start from  the first interval
	next_interval();
	// Start sequence
	if(ir_current_mark != LEVEL_MARK) // 1 MARK
		return false;

	next_interval();
	if(ir_current_mark != LEVEL_SPACE) // 1 SPACE
		return false;

	next_interval();
	if(ir_current_mark != LEVEL_MARK) // 1 MARK
		return false;
	

	while(ir_offset < ir_length)
	{

			next_interval();
			byte prev = ir_current_mark;
			if(prev == LEVEL_UNDEFINED)
				return false;
			next_interval();
			if(ir_current_mark == prev)
				return false;
			rollCommandData0(command);
			if(prev == LEVEL_SPACE)
			{
				// __^^ => 1
				appendCommandData0(command);
			}
			ir_detected_nbits++;	

	}

	command->n_bits = ir_detected_nbits;
	command->vendor = IR_VENDOR_RC5;

	return true; 
}
bool IRController::readCommand_RC6(IRCommand_t* command)
{
	//command->data[0] = 0; 	
	cleanCommandData(command);
	ir_offset 		= 0;
	ir_current_width = 0;
	
	ir_detected_nbits 	= 0;

	ir_match_flags 		= IR_ENCODER_FLAG_CONVERT_US;
	ir_current_divider 	= RC6_T1;
	
	// Read data from ir-controller
	ir_read_data();

	// start from  the first interval
	next_interval();

	// Initial mark
	if(!MATCH_MARK(RC6_HDR_MARK))
		return false;
	// Initial space
	next_interval();
	if(!MATCH_SPACE(RC6_HDR_SPACE))
		return false;

	// The Fun begun...
	ir_current_width = 0;
	ir_match_flags 	|=  IR_ENCODER_FLAG_DIVIDE;
	next_interval();
	// Start bit
	if(ir_current_mark != LEVEL_MARK) // 1 MARK
		return false;

	next_interval();
	if(ir_current_mark != LEVEL_SPACE) // 1 SPACE+
		return false;
	
  
    while(ir_offset < ir_length)
	{

		next_interval();
		byte prev = ir_current_mark;
		if(prev == LEVEL_UNDEFINED)
			return false;
		if(ir_detected_nbits == 3)
		{
			// "T" bit 2x length
			next_interval();
			if(prev != ir_current_mark)
				return false;
			prev = ir_current_mark;
		}

		next_interval();
		if(ir_current_mark == prev)
			return false;
		rollCommandData0(command);
		if(prev == LEVEL_MARK)
		{
			//^^__ => 1
			appendCommandData0(command);
		}	
		if(ir_detected_nbits == 3)
		{
			prev = ir_current_mark;
			// "T" bit 2x length
			next_interval();
			if(prev != ir_current_mark)
				return false;
			
		}
		ir_detected_nbits++;

	}
	command->n_bits = ir_detected_nbits;
	command->vendor = IR_VENDOR_RC6;
	return true;
} 
bool IRController::readCommand_SAMSUNG(IRCommand_t* command)
{
	cleanCommandData(command);
	//command->data[0] = 0; 	
	ir_offset 		 = 0;
	ir_current_width = 0;
	
	
	ir_match_flags 	= IR_ENCODER_FLAG_CONVERT_US;
	
	// Read data from ir-controller
	ir_read_data();

	// start from  the first interval
	next_interval();

	if(!MATCH_MARK(SAMSUNG_HDR_MARK))
		return false;
	next_interval();

	if(!MATCH_SPACE(SAMSUNG_HDR_SPACE))
		return false;
	next_interval();


	byte i = SAMSUNG_BITS;
	while(i--)
	{
		if (!MATCH_MARK(SAMSUNG_BIT_MARK))  
			return false;
		next_interval();

		rollCommandData0(command);

		if(MATCH_SPACE(SAMSUNG_ONE_SPACE))
		{
			appendCommandData0(command);
		} 
		else
		{
			if(!MATCH_SPACE(SAMSUNG_ZERO_SPACE))
				return false;
		}
		next_interval();
	}
	command->n_bits = SAMSUNG_BITS;
	command->vendor = IR_VENDOR_SAMSUNG;
	return true; 
}
bool IRController::readCommand_SONY(IRCommand_t*  command)
{
	cleanCommandData(command);
	ir_offset 			= 0;
	ir_current_width 	= 0;
	
	ir_detected_nbits  	= 0;
	
	ir_match_flags 	= IR_ENCODER_FLAG_CONVERT_US;
	
	// Read data from ir-controller
	ir_read_data();

	// start from  the first interval
	next_interval();


	// Initial mark
	if (!MATCH_MARK(SONY_HDR_MARK))  
		return false;
	next_interval();
	while(ir_offset < ir_length)
	{
		if(!MATCH_SPACE(SONY_HDR_SPACE))
			return false;
		next_interval();
		rollCommandData0(command);
		if(MATCH_MARK(SONY_ONE_MARK))
		{
			appendCommandData0(command);
		}
		else
		{
			if(!MATCH_MARK(SONY_ZERO_MARK))
				return false;	
		}
		next_interval();
		ir_detected_nbits++;
	}
	command->n_bits = ir_detected_nbits;
	command->vendor = IR_VENDOR_SONY;
	return true; 
}
	
// sends ir-command using specified vendor encoding
void IRController::sendCommand(IRCommand_t * command)
{
	switch(command->vendor)
	{
		case IR_VENDOR_AIWA:
			sendCommand_AIWA(command);
			break;
		case IR_VENDOR_NEC:
			sendCommand_NEC(command);
			break;
		case IR_VENDOR_LG:
			sendCommand_LG(command);
			break;
		case IR_VENDOR_PANASONIC:
			sendCommand_PANASONIC(command);
			break;
		case IR_VENDOR_RC5:
			sendCommand_RC5(command);	
			break;
		case IR_VENDOR_RC6:
			sendCommand_RC6(command);
			break;
		case IR_VENDOR_SAMSUNG:
			sendCommand_SAMSUNG(command);
			break;
		case IR_VENDOR_SONY:
			sendCommand_SONY(command);
			break;
		default:
			break;

	}

}	
// Specific vendor version of previous method (helps to reduce memory if you know your vendor)
void IRController::sendCommand_AIWA(IRCommand_t * command)
{

	ir_offset = 0;

	// Address
	initMask(26 - 1);
	// Header
	mark(AIWA_RC_T501_HDR_MARK);
	space(AIWA_RC_T501_HDR_SPACE);

	// Send "pre" data
	while(ir_mask)
	{
		mark(AIWA_RC_T501_BIT_MARK);
		if (nextBitC0(command))  
		{
			space(AIWA_RC_T501_ONE_SPACE);
		}
		else             
		{
			space(AIWA_RC_T501_ZERO_SPACE);
		}
	}

//-v- THIS CODE LOOKS LIKE IT MIGHT BE WRONG - CHECK!
//    it only send 15bits and ignores the top bit
//    then uses TOPBIT which is 0x80000000 to check the bit code
//    I suspect TOPBIT should be changed to 0x00008000

	// Skip first code bit
	// Address
	//ir_mask = 0x01;
	//ir_mask <<= (16 - 1);
	initMask(15 - 1);
	// Send code

	while(ir_mask)
	{
		mark(AIWA_RC_T501_BIT_MARK);
		if (nextBitC1(command)) 
		{ 
			space(AIWA_RC_T501_ONE_SPACE);
		}
		else             
		{
			space(AIWA_RC_T501_ZERO_SPACE);
		}
	}
	
//-^- THIS CODE LOOKS LIKE IT MIGHT BE WRONG - CHECK!

	// POST-DATA, 1 bit, 0x0
	mark(AIWA_RC_T501_BIT_MARK);
	space(AIWA_RC_T501_ZERO_SPACE);
	mark(AIWA_RC_T501_BIT_MARK);

	// send data via IR-Controller
	ir_write_data();
	
}
void IRController::sendCommand_LG(IRCommand_t * command)
{
	ir_offset = 0;

	// Header
    mark(LG_HDR_MARK);
    space(LG_HDR_SPACE);
    mark(LG_BIT_MARK);

    initMask(LG_BITS - 1);
    // Data
    while(ir_mask) 
    {
        if (nextBitC0(command)) 
        {
            space(LG_ONE_SPACE);
        }
        else
        { 
        	space(LG_ZERO_SPACE);
        }
        mark(LG_BIT_MARK);

    }	
   // send data via IR-Controller
	ir_write_data();
}
void IRController::sendCommand_NEC(IRCommand_t * command)
{
	
	ir_offset = 0;
	
	// Header
	mark(NEC_HDR_MARK);
	space(NEC_HDR_SPACE);

	initMask(NEC_BITS - 1);
	// Data
	while(ir_mask)
	{
		mark(NEC_BIT_MARK);
		if (nextBitC0(command))
		{ 
			space(NEC_ONE_SPACE);
		}
		else
		{ 
			space(NEC_ZERO_SPACE);
		}
	}
	// Footer
	mark(NEC_BIT_MARK);

	// send data via IR-Controller
	ir_write_data();
}
void IRController::sendCommand_PANASONIC(IRCommand_t * command)
{

	ir_offset = 0;
	
	// Header
	mark(PANASONIC_HDR_MARK);
	space(PANASONIC_HDR_SPACE);

	// Address
	initMask(PANASONIC_ADDR_BITS - 1);
	while(ir_mask)
	{
		mark(PANASONIC_BIT_MARK);
        if (nextBitC0(command))  
        {
        	space(PANASONIC_ONE_SPACE);
        }
        else      
        {    
        	space(PANASONIC_ZERO_SPACE);
        }
	}
	
	// Data
	initMask(PANASONIC_DATA_BITS - 1);
	while(ir_mask)
	{
		mark(PANASONIC_BIT_MARK);
        if (nextBitC1(command))
        {  
        	space(PANASONIC_ONE_SPACE);
        }
        else          
        {
        	space(PANASONIC_ZERO_SPACE);
        }
	}
	// Footer
    mark(PANASONIC_BIT_MARK);

   // send data via IR-Controller
	ir_write_data();

}
void IRController::sendCommand_RC5(IRCommand_t * command)
{
	
	ir_offset = 0;
	initMask(command->n_bits - 1);
	
	// Start
	mark(RC5_T1);
	space(RC5_T1);
	mark(RC5_T1);

	// Data
	while (ir_mask) {
		if (nextBitC0(command)) {
			space(RC5_T1); // 1 is space, then mark
			mark(RC5_T1);
		} else {
			mark(RC5_T1);
			space(RC5_T1);
		}
	}
	// send data via IR-Controller
	ir_write_data();

}
void IRController::sendCommand_RC6(IRCommand_t * command)
{

	ir_offset = 0;
	//ir_mask = 0x01;
	//ir_mask <<= (command->n_bits - 1);
	initMask(command->n_bits - 1);

	// Header
	mark(RC6_HDR_MARK);
	space(RC6_HDR_SPACE);

	// Start bit
	mark(RC6_T1);
	space(RC6_T1);

	// Data
	byte i = 0;
	while (ir_mask) 
	{
		// The fourth bit we send is a "double width trailer bit"
		word width = (i == 3) ? (RC6_2T1) : (RC6_T1);
		if (nextBitC0(command)) {
			mark(width);
			space(width);
		} else {
			space(width);
			mark(width);
		}
		i++;
	}
	// send data via IR-Controller
	ir_write_data();

}

void IRController::sendCommand_SAMSUNG(IRCommand_t * command)
{
	byte i;

	ir_offset = 0;

	initMask(SAMSUNG_BITS - 1);
	// Header
	mark(SAMSUNG_HDR_MARK);
	space(SAMSUNG_HDR_SPACE);
	// Data
	while(ir_mask)
	{
		mark(SAMSUNG_BIT_MARK);
		if(nextBitC0(command))
		{
			space(SAMSUNG_ONE_SPACE);
		}
		else
		{
			space(SAMSUNG_ZERO_SPACE);
		}
	}
	// Footer
	mark(SAMSUNG_BIT_MARK);

	// send data via IR-Controller
	ir_write_data();

}

void IRController::sendCommand_SONY(IRCommand_t * command)
{

	ir_offset = 0;
	initMask(command->n_bits - 1);
	// Header
	mark(SONY_HDR_MARK);
	// Data
	while(ir_mask)
	{
	    space(SONY_HDR_SPACE);
	    if(nextBitC0(command))
	    {
			mark(SONY_ONE_MARK);
		}
		else
		{
			mark(SONY_ZERO_MARK);
		}
	}

	// send data via IR-Controller
	ir_write_data();

}

void IRController::send_raw16(word * data)
{

	
	word i;
	ir_offset = 0;

	for(i=1;i<=data[0];i++)
	{
		ir_fill_mark = 0;
		if(i&0x01)
			ir_fill_mark = 0x80;
		ir_fill_width = data[i];
		fill_interval();
	}
	// send data via IR-Controller
	ir_write_data();

}
bool IRController::equals_raw16(word * data)
{

	word i;
	ir_offset = 0;
	ir_match_flags 	= IR_ENCODER_FLAG_CONVERT_US;
	

	ir_read_data();

	for(i=1;i<=data[0];i++)
	{
		next_interval();
		ir_fill_mark = 0;
		if(i&0x01)
			ir_fill_mark = 0x80;
		if(!MATCH_MARKSPACE(data[i], ir_fill_mark))
			return false;
		
	}
	return true;
}
void IRController::recv_raw16(word * data)
{
	word raw16_count   	= 0;	
	ir_offset 			= 0;
	ir_match_flags 		= IR_ENCODER_FLAG_CONVERT_US;

	ir_read_data();
	
	raw16_count = 0;	

	
	while(ir_offset < ir_length)
	{
		next_interval();
		data[raw16_count+1] = ir_current_width;
		raw16_count++;	
		
	}
	data[0] = raw16_count;

}



IRController IR;

byte * ir_buff;
byte ir_m_prescaller;
byte ir_detected_nbits;
word ir_offset;
word ir_length;
word ir_current_width;
word ir_current_divider;
byte ir_current_mark;
byte ir_match_flags;
word ir_fill_width;
byte ir_fill_mark;
word ir_fill_last_offset;
byte ir_carrier_low;
byte ir_carrier_hi;
unsigned long ir_mask;
