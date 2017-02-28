/*
 * That is a Simple Blink Sketch. It just blinks the LED
 */
 
#include <ZUNO_PN532.h>

// создаём объект для работы со сканером
PN532 pn532;

// LED pin number
#define LED_PIN 13

#define MY_SERIAL Serial0


// some globals variables
byte g_start = 0; // we have too initialize something in the first "loop"
byte g_data[20];  // data from PN532

void initPN532()
{
  // We have to wait a little
  delay(2000);
  // Request firware version of PN532
  pn532.getFWVersion(g_data);

  if(pn532.getLastError() != PN532_TRANSFER_OK) 
  {
       MY_SERIAL.print("Can't connect to RFID/NFC reader. Error:");
       MY_SERIAL.println(pn532.getLastError());
  }  
  else
  {
       // Transfer ok.
       // Print raw data of version
       MY_SERIAL.print("FW_Version:");
       byte i;
       for(i=0;i<4;i++)
       {
         MY_SERIAL.print(g_data[i], HEX);
         MY_SERIAL.print(" ");
       }
       MY_SERIAL.println();    
   }
  
   //  Configure 
   if(!pn532.SAMConfig())
   {
       MY_SERIAL.print("SAM config FAILED!"); 
   }
}

void setup() {
  
  MY_SERIAL.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pn532.begin();


}


void loop() {
  
  MY_SERIAL.print("loop:");
  MY_SERIAL.println(millis());

  if(!g_start)
  {
     initPN532();  
     g_start = 1;
  }

  // Read ID of the Card  
  byte id_len = pn532.readPassiveTargetID(g_data);
  
  // Is there any card?
  if(id_len != 0)
  {
       // blink with LED
       digitalWrite(LED_PIN, HIGH);
       delay(200);
       digitalWrite(LED_PIN, LOW);
       
       // We get a card, print its UID
       MY_SERIAL.print("UID:");
       byte i;
       for(i=0;i<id_len;i++)
       {
         MY_SERIAL.print(g_data[i], HEX);
         MY_SERIAL.print(" ");
       }
       MY_SERIAL.println();
       
  }

  delay(500);
  
}
