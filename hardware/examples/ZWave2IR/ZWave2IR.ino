/*
 * Receives ON/OFF command via Z-Wave and
 * sends specified IR command using built-in IR-controller of Z-Uno
 * 
 * (c) Z-Wave.Me 2016
 */

#include <IRController.h>

// You have to connect your IR-LED to PIN#6 of Z-Uno
// In other hand you can use PIN#4/PIN#5 for that purpose too - just modify the next string 
IRTransmitterParams ir_transmitter(byte(IR_TRANSMITTER_OUTPUT_PIN6), 
                                   IR_FLAGS_OUTPUT_HIGHDRIVE,
                                   IR_MS_PRESCALLER_4MHZ, 
                                   IR_CARRIER_PRESCALLER_8MHZ);

// LED pin number
#define LED_PIN 13

#define MY_SERIAL Serial0
#define SWITCH_ON 0xff
#define SWITCH_OFF 0


// Use IRScanner.ino as IR-sniffer
// Sniffed raw16 commands
// This codes were shiffed from SONY TV IRController
// RAW: ON/OFF Decoded: MUTE/MUTE
word raw_command_on[] = {0x19, 0x990, 0x242, 0x26D, 0x243, 0x26C, 0x244, 0x26A, 0x246, 0x4C5, 0x243, 0x4BD, 0x24B, 0x4C0, 0x248, 0x267, 0x249, 0x4C2, 0x246, 0x269, 0x247, 0x267, 0x249, 0x265, 0x24B, 0x264};
word raw_command_off[] = {0x19, 0x987, 0x24B, 0x4C0, 0x248, 0x267, 0x249, 0x4C2, 0x247, 0x268, 0x248, 0x4C3, 0x245, 0x269, 0x246, 0x268, 0x248, 0x4C3, 0x245, 0x26A, 0x246, 0x269, 0x247, 0x267, 0x249, 0x266};

// Detected vendor 

#define MY_IR_VENDOR IR_VENDOR_SONY // Use SONY protocol. See IRController.h for more details.
// Decoded commands	
#define MY_IR_VENDOR_CMD_ON  0x290  
#define MY_IR_VENDOR_CMD_OFF 0x290
#define DECODED_COMMAND_NBITS 12

// Last saved LED value
byte currentCh1 = 0;
byte currentCh2 = 0;
byte current_cmd = 0;
IRCommand_t vendor_cmd;

ZUNO_SETUP_DEBUG_MODE(DEBUG_ON);

// next macro sets up the Z-Uno channels
// in this example we set up 2 switch binary channels
// you can read more on http://z-uno.z-wave.me/Reference/ZUNO_SWITCH_BINARY/
ZUNO_SETUP_CHANNELS(ZUNO_SWITCH_BINARY(getter1, setter1),
					ZUNO_SWITCH_BINARY(getter2, setter2));		


void setup() {
  pinMode(LED_PIN, OUTPUT); // setup pin as output
  // We have to setup IRLED pin as output
  pinMode(6, OUTPUT); 
  digitalWrite(6, LOW); 
  
  MY_SERIAL.begin(115200);
  MY_SERIAL.println("\n **** Sketch is starting... ****\n");

  // setting up IR-controller as IR-transmitter
  ir_transmitter.setupVendor(MY_IR_VENDOR);
  IR.begin(&ir_transmitter);

  vendor_cmd.vendor = MY_IR_VENDOR;
  vendor_cmd.n_bits = DECODED_COMMAND_NBITS;

}
void sendRAWCommand(WORD * raw16)
{
	// we have to repeat packet 
	// like another IR-controllers do
	byte count = 7;
	while(count--)
	{
		IR.send_raw16(raw16);
		delay(30);
	}
}
void sendVendorCommand(unsigned long cmd)
{
	// we have to repeat packet 
	// like another IR-controllers do
	byte count = 7;
	while(count--)
	{
		vendor_cmd.data[0] = cmd;
  	    IR.sendCommand(&vendor_cmd);
		delay(30);
	}
}
void loop() {

  switch(current_cmd)
  {
  		case 1:
  			MY_SERIAL.println("\n RAW ON\n");
  			sendRAWCommand(raw_command_on);
  			break;
  		case 2:
  			MY_SERIAL.println("\n RAW OFF\n");
  			sendRAWCommand(raw_command_off);
  			break;
  		case 3:
  			MY_SERIAL.println("\n VENDOR ON\n");
  			sendVendorCommand(MY_IR_VENDOR_CMD_ON);
  			break;
  		case 4:	
  			MY_SERIAL.println("\n VENDOR OFF\n");
  			sendVendorCommand(MY_IR_VENDOR_CMD_OFF);
  			break;

  		default:
  			// nothing to do
  			break;
  		
  }
  
  current_cmd = 0;
  
  delay(500);
}

// function, which sets new channel state
// this function runs only once the controller sends new value
 void setter1 (byte value) {
  // value is a variable, holding a "new value"
  // which came from the controller or other Z-Wave device
  if (value > 0) {    // if greater then zero
    	current_cmd = 1; // will be dispatched in the next loop

  } else {            // if equals zero
    	current_cmd = 2;
  } 
  // let's save our value for the situation, when the controller will ask us about it
  currentCh1 = value;
}

// function, which returns the previously saved relay value
// this function runs only once the controller asks
byte getter1 (){
  return currentCh1;
}

// function, which sets new relay state
// this function runs only once the controller sends new value
 void setter2 (byte value) {
  // value is a variable, holding a "new value"
  // which came from the controller or other Z-Wave device
  if (value > 0) {    // if greater then zero
    	current_cmd = 3; // will be dispatched in the next loop
    				 
  } else {            // if equals zero
    	current_cmd = 4;
  } 
  // let's save our value for the situation, when the controller will ask us about it
  currentCh2 = value;
}

// function, which returns the previously saved relay value
// this function runs only once the controller asks
byte getter2 (){
  return currentCh2;
}