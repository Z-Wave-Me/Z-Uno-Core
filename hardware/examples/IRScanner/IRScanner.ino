/*
 * Receives data from built-in IR-controller of Z-Uno and 
 * tries to detect command and vendor
 * (c) Z-Wave.Me 2016
 */
 

#include <IRController.h>

// You have to connect your IR-receiver to PIN#7 of your Z-Uno 
// In most cases receiver ICs have built-in demodulator.
// If you have one of them, please check that your receiver frequency 
// and fraquency of your IR-controller matches.
// The most popular is 38 KHz. SAMSUNG/NEC/LG/...
// SONY uses 40KHz, but in most cases its commands can be detected on 38KHz too. 
// Traditionally IR-receivers like VISHAY TSOP31238/TSOP312* 
// have inverted output. 
// It means that received MARK has LOW level and
// SPACE has HIGH level.

IRReceiverParams ir_receiver(IR_FLAGS_IO_INVERTED);
IRCommand_t ir_cmd;

// LED pin number
#define LED_PIN 13

// For some cases use UART (Serial0/Serial1)
// It's a most comfortable way for debugging
// By default we use built-in USB CDC
#define MY_SERIAL Serial

void setup() {
  
  pinMode(LED_PIN, OUTPUT);      // set LED pin as output
  digitalWrite(LED_PIN, LOW);

  MY_SERIAL.begin(115200);
  
  MY_SERIAL.println("\n **** Sketch is starting... ****\n");
  
  // Setup IR-controller as receiver
  IR.begin(&ir_receiver);
  IR.scan();

}

byte ir_state;
word raw_data[200];

void loop() {

  // Check state of IR-controller
  ir_state = IR.getState();

  if(!(ir_state & IR_STATUS_BUSY))
  {
      

      // IR-Controller received a data 
      digitalWrite(LED_PIN, HIGH);
      
      word i; 
      
      // Extract data in RAW16 format
      // see IRController.h for detailed descriptions
      IR.recv_raw16(raw_data);

      MY_SERIAL.print("RAW16_data[");
      MY_SERIAL.print(raw_data[0]+1);
      MY_SERIAL.print("]= {");
      // Length of packet stored in the first word
      for(i=0;i<=raw_data[0];i++)
      {
           if(i != 0)
               MY_SERIAL.print(", "); 
           MY_SERIAL.print("0x");     
           MY_SERIAL.print(raw_data[i], HEX);

      }
      MY_SERIAL.println("}");

      // In some cases we can decode command
      // from MARK/SPACES to binary code and 
      // now we will try it...
      MY_SERIAL.println("Trying to decode command: ");
      if(IR.detectCommand(&ir_cmd))
      {
          MY_SERIAL.print("Vendor: ");
          MY_SERIAL.print(ir_cmd.vendor);
          MY_SERIAL.print(" Bits: ");
          MY_SERIAL.print(ir_cmd.n_bits);
          // For some protocols commands have 
          // 64 bit length
          if(ir_cmd.n_bits > 32)
          {  
            MY_SERIAL.print(" Command: \n HW:");
            MY_SERIAL.print(ir_cmd.data[0], HEX);
            MY_SERIAL.print(" LW:"); 
            MY_SERIAL.println(ir_cmd.data[1], HEX);
          }  
          else
          {
              MY_SERIAL.print(" Command: ");
              MY_SERIAL.println(ir_cmd.data[0], HEX);
          }
               
       }

       // restart scan process
       IR.scan();
       digitalWrite(LED_PIN, LOW);  
  }


  delay(500);
  
}
