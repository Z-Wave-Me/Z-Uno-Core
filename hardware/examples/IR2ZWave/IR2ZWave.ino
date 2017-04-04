
/*
 * Receives data from built-in IR-controller of Z-Uno and 
 * sends command to specified group via Z-Wave
 * (c) Z-Wave.Me 2016
 */

#include <IRController.h>

// You have to connect your IR-receiver to PIN#7 of your Z-Uno 
// In most cases receiver ICs have built-in demodulator.
// If you have one of them, please check that your receiver frequency 
// and fraquency of your IR-controller matches.
// The most popular is 38 KHz. SAMSUNG/NEC/LG/...
// SONY uses 40KHz, but in most cases its commands can be detected on 38KHz too. 


// Setup associations - we have 2 groups for on/off behavior
ZUNO_SETUP_ASSOCIATIONS(ZUNO_ASSOCIATION_GROUP_SET_VALUE, ZUNO_ASSOCIATION_GROUP_SET_VALUE); 

// Traditionally IR-receivers like VISHAY TSOP31238/TSOP312* 
// have inverted output. 
// It means that received MARK has LOW level and
// SPACE has HIGH level.

IRReceiverParams ir_receiver(IR_FLAGS_IO_INVERTED);
IRCommand_t ir_cmd;
byte ir_state;
word raw_data[200];

// LED pin number
#define LED_PIN 13

// For some cases use UART (Serial0/Serial1)
// It's a most comfortable way for debugging
// By default we use built-in USB CDC
#define MY_SERIAL Serial

#define SWITCH_ON 0xff
#define SWITCH_OFF 0

#define CONTROL_GROUP1 1  // RAW GROUP
#define CONTROL_GROUP2 2  // VENDOR GROUP

// Use IRScanner.ino as IR-sniffer
// Sniffed raw16 commands
// This codes were shiffed from Apple IR Controller
// RAW: Up/DOWN Decoded: Left/Right
word raw_command_on[] = {0x43, 0x2392, 0x1171, 0x24E, 0x219, 0x249, 0x66F, 0x248, 0x66F, 0x249, 0x66F, 0x249, 0x21E, 0x24E, 0x66A, 0x24E, 0x66A, 0x24D, 0x66A, 0x24D, 0x66B, 0x24D, 0x66B, 0x24C, 0x66C, 0x24C, 0x21B, 0x246, 0x221, 0x24C, 0x21B, 0x247, 0x220, 0x24D, 0x66B, 0x24C, 0x66B, 0x24C, 0x66B, 0x24C, 0x21A, 0x247, 0x670, 0x247, 0x21F, 0x24D, 0x219, 0x248, 0x21E, 0x24F, 0x218, 0x24A, 0x21C, 0x245, 0x221, 0x24B, 0x66C, 0x24B, 0x21C, 0x246, 0x220, 0x24C, 0x21A, 0x247, 0x671, 0x246, 0x231, 0x247};
word raw_command_off[] = {0x43, 0x239C, 0x1165, 0x250, 0x231, 0x231, 0x66C, 0x24B, 0x66C, 0x24B, 0x66C, 0x24C, 0x235, 0x22C, 0x670, 0x247, 0x66F, 0x248, 0x66E, 0x249, 0x66D, 0x24A, 0x66C, 0x24B, 0x66B, 0x24D, 0x234, 0x22D, 0x239, 0x234, 0x232, 0x22F, 0x237, 0x236, 0x666, 0x247, 0x670, 0x247, 0x23A, 0x232, 0x66A, 0x24D, 0x669, 0x24E, 0x233, 0x22E, 0x238, 0x235, 0x232, 0x230, 0x236, 0x22B, 0x23B, 0x231, 0x235, 0x22D, 0x66F, 0x249, 0x238, 0x234, 0x232, 0x230, 0x236, 0x236, 0x666, 0x246, 0x24B, 0x22D};

// Detected vendor 
#define MY_IR_VENDOR IR_VENDOR_NEC // NEC protocol in most cases
// Decoded commands	
#define MY_IR_VENDOR_CMD_ON  0x77E1E022   
#define MY_IR_VENDOR_CMD_OFF 0x77E11022		


void setup() {
  
  pinMode(LED_PIN, OUTPUT);      // set LED pin as output
  digitalWrite(LED_PIN, LOW);

  MY_SERIAL.begin(115200);
  
  MY_SERIAL.println("\n **** Sketch is starting... ****\n");
  
  // Setup IR-controller as receiver
  IR.begin(&ir_receiver);
  IR.scan();



}


void loop() {


  // Check state of IR-controller
  ir_state = IR.getState();

  if(!(ir_state & IR_STATUS_BUSY))
  {
      // IR-Controller received a data 
      digitalWrite(LED_PIN, HIGH);
      
      // Case 1	
      // Control via RAW DATA

      if(IR.equals_raw16(raw_command_on))
      {
      	MY_SERIAL.println("\n GROUP1 ON\n");
      	zunoSendToGroupSetValueCommand(CONTROL_GROUP1, SWITCH_ON);
      }
      else
      if(IR.equals_raw16(raw_command_off))
      {
      	MY_SERIAL.println("\n GROUP1 OFF\n");
      	zunoSendToGroupSetValueCommand(CONTROL_GROUP1, SWITCH_OFF);
      }
      else
      // Case 2
      // Control via detected / known vedor specific command
      if(IR.detectCommand(&ir_cmd))
      {
      	  if(ir_cmd.vendor == MY_IR_VENDOR)
      	  {

      	  	if(ir_cmd.data[0] == MY_IR_VENDOR_CMD_ON)
      		{
      				MY_SERIAL.println("\n GROUP2 ON\n");
      				zunoSendToGroupSetValueCommand(CONTROL_GROUP2, SWITCH_ON);
      		}
      		else
			if(ir_cmd.data[0] == MY_IR_VENDOR_CMD_OFF)
      		{
      				MY_SERIAL.println("\n GROUP2 OFF\n");
      				zunoSendToGroupSetValueCommand(CONTROL_GROUP2, SWITCH_OFF);
      		}


      	  }
      }
      // restart scan process
      IR.scan();
      digitalWrite(LED_PIN, LOW); 


  }
  delay(500);
 }




