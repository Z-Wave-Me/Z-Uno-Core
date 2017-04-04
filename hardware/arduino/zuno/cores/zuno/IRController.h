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
 *     OUTPUTS 		of IR-Controller are PIN#6,PIN#5,PIN#4 (You can use some pins to increase power, 8mA is max for one pin)	
 *								
 */

#pragma once

#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"


enum
{
	IR_VENDOR_AIWA,
	IR_VENDOR_DENON,
	IR_VENDOR_JVC,
	IR_VENDOR_LEGO,
	IR_VENDOR_LG,
	IR_VENDOR_MITSUBISHI,
	IR_VENDOR_NEC,
	IR_VENDOR_PANASONIC,
	IR_VENDOR_RC5,
	IR_VENDOR_RC6,
	IR_VENDOR_SAMSUNG,
	IR_VENDOR_SANYO,
	IR_VENDOR_SHARP,
	IR_VENDOR_SONY,
	IR_VENDOR_WHYNTER,


	IR_VENDOR_UNKNOWN = 0xFF	
	
};
// IR Receiver Settings and Values

enum
{
	IR_FLAGS_IO_INVERTED 			= 	0x01,
	IR_FLAGS_OUTPUT_HIGHDRIVE 		=	0x02,
	IR_FLAGS_OUTPUT_MSCARRIER		=	0x04,
	IR_FLAGS_OUTPUT_HIGHIDLE 		=	0x08,
	IR_FLAGS_INPUT_DETECTCARRIER 	=	0x10,
	IR_FLAGS_IO_OUTPUT				=	0x80
};
enum
{
	 IR_STATUS_BUF_OVERFLOW   	=    	0x20,
	 IR_STATUS_MARK_OVERFLOW  	= 		0x10,
	 IR_STATUS_CARRIER_FAILED	=		0x08,
	 IR_STATUS_DMA_FAILED		=		0x02,
	 IR_STATUS_BUSY				=		0x01
};
enum
{
	IR_TRANSMITTER_OUTPUT_PIN6	= 0x1,
	IR_TRANSMITTER_OUTPUT_PIN5	= 0x2,
	IR_TRANSMITTER_OUTPUT_PIN4	= 0x4

};
enum
{
	IR_MS_PRESCALLER_32MHZ,  // 0
	IR_MS_PRESCALLER_16MHZ,  // 1
	IR_MS_PRESCALLER_8MHZ,   // 2
	IR_MS_PRESCALLER_4MHZ,   // 3
	IR_MS_PRESCALLER_2MHZ,   // 4
	IR_MS_PRESCALLER_1MHZ,   // 5
	IR_MS_PRESCALLER_500KHZ,
	IR_MS_PRESCALLER_250KHZ
};

enum
{
	IR_CARRIER_PRESCALLER_32MHZ,  		// 32	// 0
	IR_CARRIER_PRESCALLER_16MHZ,  		// 16	// 1
	IR_CARRIER_PRESCALLER_10P6MHZ,   	// 10.6 // 2
	IR_CARRIER_PRESCALLER_8MHZ,   		// 8    // 3
	IR_CARRIER_PRESCALLER_6P4MHZ,   	// 6.4  // 4
	IR_CARRIER_PRESCALLER_5P3MHZ,   	// 5.3  // 5 
	IR_CARRIER_PRESCALLER_4P57MHZ,		// 4.57 // 6
	IR_CARRIER_PRESCALLER_4MHZ			// 4    // 7
};


enum
{
	IR_TRAILSPACE_05K,
	IR_TRAILSPACE_1K,
	IR_TRAILSPACE_2K,
	IR_TRAILSPACE_4K,
	IR_TRAILSPACE_8K,
	IR_TRAILSPACE_16K,
	IR_TRAILSPACE_32K,
	IR_TRAILSPACE_64K
};

enum
{
	IR_AVERAGER_1P,
	IR_AVERAGER_2P,
	IR_AVERAGER_4P,
	IR_AVERAGER_8P
	
	
};

enum
{
	IR_GLITCH_REMOVER_DISABLE,
	IR_GLITCH_REMOVER_125NS,
	IR_GLITCH_REMOVER_250NS,
	IR_GLITCH_REMOVER_500NS
	
};

extern word g_ir_df;
// IR-receiver settings
class IRReceiverParams
{
	public:

		IRReceiverParams(byte fl = 0,
						 byte msp = IR_MS_PRESCALLER_4MHZ, 
						 byte crp = IR_MS_PRESCALLER_8MHZ,
						 byte ts = IR_TRAILSPACE_32K, 
						 byte av = IR_AVERAGER_8P, 
						 byte gr =  IR_GLITCH_REMOVER_500NS):
										flags(fl),
										ms_prescaller(msp),
										carrier_prescaller(crp),
										carrier_averagger(av),
										glitch_remover(gr),
										trail_space(ts)	
										{};
		// Vendor-specific constructor
		IRReceiverParams(int vendor, byte fl);

	public:
		byte getFlags();
		byte getMSPrescaller(){return ms_prescaller;};
		
	private:

		byte flags;
		byte ms_prescaller;
		byte carrier_prescaller;
		byte carrier_averagger;
		byte glitch_remover;
		byte trail_space;





};

// IR-trasmitter settings
class IRTransmitterParams
{
	public:

		IRTransmitterParams(byte outputs =  IR_TRANSMITTER_OUTPUT_PIN6,
							byte fl = 0,
						 	byte msp = IR_MS_PRESCALLER_1MHZ, 
						 	byte crp = IR_MS_PRESCALLER_8MHZ,
						 	byte low = 74, byte high = 148):    
										flags(fl | IR_FLAGS_IO_OUTPUT),
										ms_prescaller(msp),
										carrier_prescaller(crp),
										carrier_low(low),
										carrier_high(high),
										main_outputs(outputs)	
										{};

	public:
		byte getFlags();
		byte getOutputs(){return main_outputs;};
		byte getMSPrescaller(){return ms_prescaller;};

		void setupVendor(int vendor);
		void setup_kHz(byte freq);
		
	private:

		byte flags;
		byte ms_prescaller;
		byte carrier_prescaller;
		byte carrier_low;
		byte carrier_high;
		byte main_outputs;

		

};

#define MAX_IRCOMMAND_DATA 2

typedef struct IRCommand_s
{

	byte 			vendor;
	byte 			n_bits;
	unsigned long   data[MAX_IRCOMMAND_DATA];	
}IRCommand_t;

// Main IR-Controller class
class IRController
{
	public:
	IRController();
	// Start hardware IR controller as receiver
	void begin(IRReceiverParams * transmitter);
	void begin(IRTransmitterParams * receiver);

	// RAW16 format:
	// word #   1    			2   		3   		4         5 		...   N+1
	//        NUMBER_OF_WORDS   MARK#1      SPACE#1     MARK#2    SPACE#2         SPACE#N
	// all intervals in microseconds
	// the sequence always ends with mark 

	// sends raw16-data via IR controller
	void send_raw16(word * data);
	// checks if raw16 data equals to current data received by ir
	bool equals_raw16(word * data);
	// receives raw16-data from IR controller
	void recv_raw16(word * data);

	// start/restart ir-scanning process
	void scan();


	byte getState();

	// try do detect IR VENDOR and  parse command 
	// return vedor_code see IR_VENDOR_* 
	bool detectCommand(IRCommand_t * command);
	// Specific vendor version of previous method (helps to reduce memory if you know your vendor)
	bool readCommand_AIWA(IRCommand_t * command);
	bool readCommand_LG(IRCommand_t * command);
	bool readCommand_NEC(IRCommand_t * command);
	bool readCommand_PANASONIC(IRCommand_t * command);
	bool readCommand_RC5(IRCommand_t * command);
	bool readCommand_RC6(IRCommand_t* command);
	bool readCommand_SAMSUNG(IRCommand_t * command);
	bool readCommand_SONY(IRCommand_t * command);
	

	// sends ir-command using specified vendor encoding
	void sendCommand(IRCommand_t * command);	
	// Specific vendor version of previous method (helps to reduce memory if you know your vendor)
	void sendCommand_AIWA(IRCommand_t * command);
	void sendCommand_LG(IRCommand_t * command);
	void sendCommand_NEC(IRCommand_t * command);
	void sendCommand_PANASONIC(IRCommand_t * command);
	void sendCommand_RC5(IRCommand_t * command);
	void sendCommand_RC6(IRCommand_t * command);
	void sendCommand_SAMSUNG(IRCommand_t * command);
	void sendCommand_SONY(IRCommand_t * command);
	
	// stops hardware IR-controller
	void end();

	private:
		byte my_data;	



};


extern IRController IR;



