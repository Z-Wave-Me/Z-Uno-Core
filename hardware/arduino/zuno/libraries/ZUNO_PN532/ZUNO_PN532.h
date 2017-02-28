/**************************************************************************/
/*! 
    @file     Adafruit_NFCShield_I2C.cpp
    @author   Adafruit Industries
  @license  BSD (see license.txt)
  
  I2C Driver for NXP's PN532 NFC/13.56MHz RFID Transceiver

  This is a library for the Adafruit PN532 NFC/RFID shields
  This library works with the Adafruit NFC breakout 
  ----> https://www.adafruit.com/products/364
  
  Check out the links above for our tutorials and wiring diagrams 
  These chips use I2C to communicate
  
  Rewrited fo Z-Uno project by Z-Wave>ME
*/
/**************************************************************************/

#ifndef ZUNO_PN532
#define ZUNO_PN532


#include "Arduino.h"


#include <Wire.h>



enum
{
    PN532_TRANSFER_OK,
    PN532_TRANSFER_NOT_RESP,
    PN532_TRANSFER_CMD_NACK,
    PN532_TRANSFER_TIMEOUT,
    PN532_TRANSFER_BAD_ACK,
    PN532_TRANSFER_BAD_FRAME,
    PN532_TRANSFER_NO_SPACE,
    PN532_TRANSFER_BAD_DATA_CRC
      
    
      
};



class PN532
{
 public:
  PN532();
  void begin(void);
  

  void wakeup();
  void writeCommand(byte * buff, byte len);
  void readResponse(byte * resp, byte len);

  byte getLastError(){return last_error; };

  void  getFWVersion(byte * version_ptr);

  bool SAMConfig(void);
  byte readPassiveTargetID(byte *uid);

  void setPassiveCardBaudrate(byte br) { passive_baudrate = br; };

private:
  
  byte last_error; 
  byte passive_baudrate;
  void readAckFrame();
  



};

#endif
