/**************************************************************************/
/*! 
    @file     ZUNO_PN532.cpp (ported from Adafruit_NFCShield_I2C.cpp/PN532.cpp)
    @author   Z-Wave.Me, Adafruit Industries, Seed Studio
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

#include "Arduino.h"
#include <string.h>


#include <Wire.h>


#include "ZUNO_PN532.h"

#define PN532_I2C_ADDRESS       0x24
#define PN532_PACKBUFFSIZ       0x80




// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              0x00
#define PN532_COMMAND_GETFIRMWAREVERSION    0x02
#define PN532_COMMAND_GETGENERALSTATUS      0x04
#define PN532_COMMAND_READREGISTER          0x06
#define PN532_COMMAND_WRITEREGISTER         0x08
#define PN532_COMMAND_READGPIO              0x0C
#define PN532_COMMAND_WRITEGPIO             0x0E
#define PN532_COMMAND_SETSERIALBAUDRATE     0x10
#define PN532_COMMAND_SETPARAMETERS         0x12
#define PN532_COMMAND_SAMCONFIGURATION      0x14
#define PN532_COMMAND_POWERDOWN             0x16
#define PN532_COMMAND_RFCONFIGURATION       0x32
#define PN532_COMMAND_RFREGULATIONTEST      0x58
#define PN532_COMMAND_INJUMPFORDEP          0x56
#define PN532_COMMAND_INJUMPFORPSL          0x46
#define PN532_COMMAND_INLISTPASSIVETARGET   0x4A
#define PN532_COMMAND_INATR                 0x50
#define PN532_COMMAND_INPSL                 0x4E
#define PN532_COMMAND_INDATAEXCHANGE        0x40
#define PN532_COMMAND_INCOMMUNICATETHRU     0x42
#define PN532_COMMAND_INDESELECT            0x44
#define PN532_COMMAND_INRELEASE             0x52
#define PN532_COMMAND_INSELECT              0x54
#define PN532_COMMAND_INAUTOPOLL            0x60
#define PN532_COMMAND_TGINITASTARGET        0x8C
#define PN532_COMMAND_TGSETGENERALBYTES     0x92
#define PN532_COMMAND_TGGETDATA             0x86
#define PN532_COMMAND_TGSETDATA             0x8E
#define PN532_COMMAND_TGSETMETADATA         0x94
#define PN532_COMMAND_TGGETINITIATORCOMMAND 0x88
#define PN532_COMMAND_TGRESPONSETOINITIATOR 0x90
#define PN532_COMMAND_TGGETTARGETSTATUS     0x8A

#define PN532_RESPONSE_INDATAEXCHANGE       0x41
#define PN532_RESPONSE_INLISTPASSIVETARGET  0x4B

#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK
#define PN532_MIFARE_ISO14443A        (0x00)



#define REVERSE_BITS_ORDER(b)         b = (b & 0xF0) >> 4 | (b & 0x0F) << 4; \
                                      b = (b & 0xCC) >> 2 | (b & 0x33) << 2; \
                                      b = (b & 0xAA) >> 1 | (b & 0x55) << 1


// GLOBALS extrns
extern byte g_nfc_len;
extern byte g_nfc_tmp;
extern byte g_nfc_sum;
extern word g_nfc_waittime;
extern byte g_nfc_lastcmd;
extern byte g_pn532_packetbuffer[PN532_PACKBUFFSIZ];
extern byte g_c_byte;


// GLOBALS
byte g_nfc_len;
byte g_nfc_sum;
word g_nfc_waittime;
byte g_nfc_lastcmd;
byte g_pn532_packetbuffer[PN532_PACKBUFFSIZ];
byte g_c_byte;
byte g_nfc_tmp;


/**************************************************************************/
/*! 
    @brief  Instantiates a new PN532 class

    @param  irq       Location of the IRQ pin
    @param  reset     Location of the RSTPD_N pin
*/
/**************************************************************************/
PN532::PN532() 
{
   
    last_error = PN532_TRANSFER_OK;
    passive_baudrate = PN532_MIFARE_ISO14443A;
}
void PN532::begin(void)
{
     Wire.begin();
}
void PN532::wakeup()
{
    Wire.beginTransmission(PN532_I2C_ADDRESS, true); // I2C start
    delay(25);
    Wire.endTransmission();                    // I2C end
}


#define ACK_FRAME_LEN 0x06
#define ACK_TIMEOUT   0x20
#define RESP_TIMEOUT   0x20


byte PN532_ACK[] = {0x01, 0, 0, 0xFF, 0, 0xFF, 0};

void PN532::readAckFrame()
{
    g_nfc_sum = ACK_TIMEOUT;
    delay(2);

    while(1)
    {
       Wire.requestFrom(PN532_I2C_ADDRESS,  sizeof(PN532_ACK));
       if(Wire.getStatus() == 0)
          break;
       Wire.endTransmission();
       delay(1);
       g_nfc_sum--;
       if(!g_nfc_sum)
       {
          last_error = PN532_TRANSFER_TIMEOUT;
          return;
       }

    } 
    g_nfc_len = 0;
    //Wire.requestFrom(PN532_I2C_ADDRESS,  sizeof(PN532_ACK) + 1);
    while(g_nfc_len < sizeof(PN532_ACK))
    {
        //Wire.endTransmission();
        g_c_byte = Wire.read();
        //Serial0.print(g_c_byte, HEX);
        if(g_c_byte != PN532_ACK[g_nfc_len])
        {
            //Wire.endTransmission();
            last_error = PN532_TRANSFER_BAD_ACK;
            //return;
        }
        g_nfc_len++; 
    }
    Wire.endTransmission();
}
void PN532::writeCommand(byte * buff, byte len)
{

    byte i; 
    g_nfc_lastcmd = buff[0];

    last_error = PN532_TRANSFER_OK;


    g_nfc_len = len + 1;// length of data field: TFI + DATA
    g_nfc_sum = PN532_HOSTTOPN532; // sum of TFI + DATA

    Wire.beginTransmission(PN532_I2C_ADDRESS, true);
    delayMicroseconds(500);
    Wire.write(PN532_PREAMBLE);
    Wire.write(PN532_STARTCODE1);
    Wire.write(PN532_STARTCODE2);  
    Wire.write(g_nfc_len);
    Wire.write(~g_nfc_len + 1);                 // checksum of length
    Wire.write(PN532_HOSTTOPN532);
       
    for (i = 0; i < len; i++) 
    {

        Wire.write(buff[i]);
        g_nfc_sum += buff[i];
    }

    g_nfc_sum = ~g_nfc_sum;
    g_nfc_sum++;
    Wire.write(g_nfc_sum);
    Wire.write(PN532_POSTAMBLE);
    
    last_error = Wire.endTransmission(); 
    if(last_error != 0)
    {
        if(last_error == 2)
        {
          last_error = PN532_TRANSFER_NOT_RESP;  
        }
        else
        {
          last_error = PN532_TRANSFER_CMD_NACK;
        }
   
    }
    //Serial0.println(last_error);

    readAckFrame();
}
void PN532::readResponse(byte * resp, byte len)
{
    byte i;

    g_nfc_sum = RESP_TIMEOUT;
    

    while(1)
    {
       Wire.requestFrom(PN532_I2C_ADDRESS,  len + 10);
       if(Wire.getStatus() == 0)
       {
            if(Wire.read() & 1)
                break;
       }
       Wire.endTransmission();
       delay(10);
       g_nfc_sum--;
       if(!g_nfc_sum)
       {
          last_error = PN532_TRANSFER_TIMEOUT;
          return;
       }

    } 

    last_error = PN532_TRANSFER_BAD_FRAME;
    if(Wire.read() != PN532_PREAMBLE)
    {
         Wire.endTransmission();
         return;
    }
    if(Wire.read() != PN532_STARTCODE1)
    {
         Wire.endTransmission();
         return;
    }
    if(Wire.read() != PN532_STARTCODE2)
    {
         Wire.endTransmission();
         return;
    }

    g_nfc_len = Wire.read();
    g_nfc_tmp = Wire.read();
    g_nfc_tmp += g_nfc_len; 
    if(g_nfc_tmp != 0)
    {
         //Serial0.println(g_nfc_tmp);
         Wire.endTransmission();
         return;
    }  

    if(Wire.read()  != PN532_PN532TOHOST)
    {
         Wire.endTransmission();
         return;
    }  

    if(Wire.read()  != (g_nfc_lastcmd + 1))
    {
         Wire.endTransmission();
         return;
    }  

    g_nfc_len -= 2;

    if(g_nfc_len > len)
    {
        last_error = PN532_TRANSFER_NO_SPACE;
        Wire.endTransmission();
        return;
    }
    
    last_error = PN532_TRANSFER_BAD_DATA_CRC;
    g_nfc_sum = PN532_PN532TOHOST;
    g_nfc_sum += g_nfc_lastcmd;
    g_nfc_sum ++;

    for (i = 0; i < g_nfc_len; i++) 
    {
        resp[i] = Wire.read();
        g_nfc_sum += resp[i];
    }
    

    g_nfc_tmp = Wire.read();
    g_nfc_tmp += g_nfc_sum;  

    if (g_nfc_tmp != 0) 
    {
        Wire.endTransmission();
        //Serial0.print();
        return;
    } 
    last_error = PN532_TRANSFER_OK;
    Wire.endTransmission();
}

void PN532::getFWVersion(byte * version_ptr)
{

    g_pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

    writeCommand(g_pn532_packetbuffer, 1);

    if(last_error != PN532_TRANSFER_OK)
      return;
    
    readResponse(g_pn532_packetbuffer, sizeof(g_pn532_packetbuffer));
    
    if(last_error != PN532_TRANSFER_OK)
      return;    
    version_ptr[0] = g_pn532_packetbuffer[0];
    version_ptr[1] = g_pn532_packetbuffer[1];
    version_ptr[2] = g_pn532_packetbuffer[2];
    version_ptr[3] = g_pn532_packetbuffer[3];
    
}
bool PN532::SAMConfig(void)
{
    g_pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    g_pn532_packetbuffer[1] = 0x01; // normal mode;
    g_pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
    g_pn532_packetbuffer[3] = 0x01; // use IRQ pin!

    writeCommand(g_pn532_packetbuffer, 4);

    if(last_error != PN532_TRANSFER_OK)
      return false;

    readResponse(g_pn532_packetbuffer, sizeof(g_pn532_packetbuffer));

    return (last_error == PN532_TRANSFER_OK);
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*!
    Waits for an ISO14443A target to enter the field

    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @returns length of UID in bytes,  if everything executed properly, 0 for an error
*/
/**************************************************************************/
byte PN532::readPassiveTargetID(byte *uid)//uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout, bool inlist)
{
    byte len_ret = 0;
    g_pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    g_pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    g_pn532_packetbuffer[2] = passive_baudrate;

    writeCommand(g_pn532_packetbuffer, 3);
    if(last_error != PN532_TRANSFER_OK)
      return 0;

    
    readResponse(g_pn532_packetbuffer, sizeof(g_pn532_packetbuffer));


    // check some basic stuff
    /* ISO14443A card response should be in the following format:

      byte            Description
      -------------   ------------------------------------------
      b0              Tags Found
      b1              Tag Number (only one used in this example)
      b2..3           SENS_RES
      b4              SEL_RES
      b5              NFCID Length
      b6..NFCIDLen    NFCID
    */

    if (g_pn532_packetbuffer[0] != 1)
        return 0;

    /* Card appears to be Mifare Classic */
    len_ret = g_pn532_packetbuffer[5];

    for (uint8_t i = 0; i < len_ret; i++) {
        uid[i] = g_pn532_packetbuffer[6 + i];
    }

    return len_ret;
}
 


