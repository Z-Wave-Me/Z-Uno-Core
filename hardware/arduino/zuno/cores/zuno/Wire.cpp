 

#include "Wire.h"

#include "ZUNO_Definitions.h"
#include "ZUNO_call_proto.h"
#include "ArduinoCAPI.h"
#include "HLCore.h" 
#include "HardwareSerial.h" 


#define STATE_TRANSMITION_INIT      0
#define STATE_TRANSMITION_WRITEMODE 1
#define STATE_TRANSMITION_READMODE  2
#define STATE_TRANSMITION_STOPPED   3

#define NACK_ON_ADDRESS             2
#define NACK_ON_DATA                4

#define FLAGS_REQSTOP               1




// I2C Driver to use it with spins
// AUXILARY CLASS
I2CDriver::I2CDriver(s_pin scl, s_pin sda): _scl(scl), _sda(sda), twi_start(false)
{

}
void I2CDriver::bind(void)
{
    pinMode(_scl, OUTPUT);
    digitalWrite(_scl, HIGH);
    pinMode(_sda, OUTPUT);
    digitalWrite(_sda, HIGH);
}
void I2CDriver::start(void)
{
     twi_start = true;
}   
void I2CDriver::stop(void)
{
    noInterrupts();   
    
    pinMode(_sda, OUTPUT);
    digitalWrite(_sda, LOW);
    NOPS(8);    
    digitalWrite(_scl, HIGH);
    delayMicroseconds(5);
    digitalWrite(_sda, HIGH);

    interrupts();
}  

byte I2CDriver::write(byte b)
{
    byte i = 8;    
    noInterrupts();
    
    if(twi_start)
    {
        // We have to setup start sequence
        // Cause we are the master of this bus...
        pinMode(_sda, OUTPUT);
        digitalWrite(_sda, LOW);
        delayMicroseconds(5);
        digitalWrite(_scl, LOW);
        delayMicroseconds(5); 
        // Set flag to control _sda 
        // right way at the and of this routine
        twi_start = b&0x01;
    }
    else
    {
        digitalWrite(_scl, LOW);
        pinMode(_sda, OUTPUT);
    }
     
    // Sending bits of the byte       
    while(i)
    {  
        if(b & 0x80)
        {
            digitalWrite(_scl, LOW);
            digitalWrite(_sda, HIGH);
        }
        else
        {
            NOPS(1);
            digitalWrite(_scl, LOW);
            digitalWrite(_sda, LOW);
        }
        NOPS(30); 
        digitalWrite(_scl, HIGH);
        NOPS(5);
        b <<= 1;
        i--;
    }
            
    // Receiving ACK
    NOPS(12);
    digitalWrite(_scl, LOW);
    pinMode(_sda, INPUT_PULLUP);
    NOPS(23);
    digitalWrite(_scl, HIGH);
    i = (digitalRead(_sda) == 0);
    NOPS(12);
    digitalWrite(_scl, LOW);
    if(!twi_start)
    {
        pinMode(_sda, OUTPUT);
        digitalWrite(_sda, LOW);
    }  

    interrupts();
    twi_start = false;
    
    return i;
}

byte I2CDriver::read(byte ack)
{
    byte i = 8, input = 0;

    noInterrupts();
    digitalWrite(_scl, LOW);

    // reading input bits
    while(i)
    {

        digitalWrite(_scl, LOW);
        NOPS(30);
        digitalWrite(_scl, HIGH);
        input <<= 1;
        input |= digitalRead(_sda) > 0;
        NOPS(5);
        i--;
    }
   
    // Writing ACK/NACK
    digitalWrite(_scl, LOW);
    NOPS(3); 
    pinMode(_sda, OUTPUT);
    digitalWrite(_sda, LOW);
    if(!ack)
        digitalWrite(_sda, HIGH); 
    NOPS(15);
    digitalWrite(_scl, HIGH);
    NOPS(18);
    digitalWrite(_scl, LOW); 
    pinMode(_sda, INPUT_PULLUP); 
    interrupts();

    return input;    
}

// MAIN I2C CLASS
TwoWire::TwoWire(I2CDriver * driver):p_driver(driver)
{
    txAddress = 0;
    state = STATE_TRANSMITION_STOPPED;

}
void TwoWire::begin(I2CDriver * driver)
{
    if(driver)
    {
        p_driver = driver;
    }

    p_driver->bind();
    state = STATE_TRANSMITION_STOPPED;

}
void  TwoWire::bindDriver(I2CDriver * driver)
{   
   p_driver = driver;     
}
void TwoWire::beginTransmission(uint8_t addr, uint8_t forced_write)
{
    txAddress = addr;
    txAddress <<= 1;
        

    available_bytes = 0;
    state = STATE_TRANSMITION_INIT;
    b_flags = 0;
    sucess_code = 0;

    // In some cases we need to setup connection here...
    if(forced_write)
    {
        state = STATE_TRANSMITION_WRITEMODE;
        p_driver->start();
        if(!p_driver->write(txAddress))
            sucess_code |= NACK_ON_ADDRESS;

    }
   
}
uint8_t TwoWire::endTransmission(uint8_t stop)
{
    // We already stopped I2C
    if(state == STATE_TRANSMITION_STOPPED)
        return sucess_code;
    // We have to send one byte if user dind't do it...
    if(state == STATE_TRANSMITION_INIT)
    {
        p_driver->start();
        if(!p_driver->write(txAddress))
        {
            sucess_code |= NACK_ON_ADDRESS;
        }   
    }
    // We have open connection for reading bytes
    // Some devices will have bad inner state if we don't say "NACK" 
    // at the end.
    if((state == STATE_TRANSMITION_READMODE) && (sucess_code == 0) && available_bytes)
    {
        // Send NACK
        p_driver->read(0x00);
    }    
    // Send stop sequence
    p_driver->stop();

    state = STATE_TRANSMITION_STOPPED;

    return sucess_code;
}
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count, bool stop)
{
    byte ret;
    
    txAddress = addr;
    txAddress <<= 1;
    txAddress |= 0x01;
    

    available_bytes = count;

    if(stop)
    {
        b_flags |= FLAGS_REQSTOP;
    }
   
    sucess_code = 0;
        
    // Arduino uses this function to start I2C connection sometimes
    if(state == STATE_TRANSMITION_STOPPED)
    {
         p_driver->start();
         ret = p_driver->write(txAddress);

    }
    state = STATE_TRANSMITION_READMODE;


    if(!ret)
    {
        sucess_code |= NACK_ON_ADDRESS;
        p_driver->stop();
        state = STATE_TRANSMITION_STOPPED;
        available_bytes =0;
        return 0;  
    }
   
   

    return available_bytes;
}
size_t TwoWire::write(uint8_t data)
{
    byte res;

    if(state == STATE_TRANSMITION_INIT)
    {
        p_driver->start();
        res = p_driver->write(txAddress);
    }
    res = p_driver->write(data);
    state = STATE_TRANSMITION_WRITEMODE;
    if(!res)
    {
        sucess_code |= NACK_ON_DATA;
    }
    return res; 
}
int TwoWire::available(void)
{
    return available_bytes;
}
int TwoWire::read(void)
{
    BYTE ret;
    if(state == STATE_TRANSMITION_STOPPED)
        return 0xFF;
    if(state == STATE_TRANSMITION_INIT)
    {
        txAddress |= 0x01;
        p_driver->start();
        ret = p_driver->write(txAddress);
        state = STATE_TRANSMITION_READMODE;
        if(!ret)
        {
            sucess_code |= NACK_ON_ADDRESS;
            p_driver->stop();
            state = STATE_TRANSMITION_STOPPED;
            available_bytes =0;
            return 0;  
        }

    }
    available_bytes--; 
    ret = 1;
    if(!available_bytes)
        ret = 0;
    ret = p_driver->read(ret);
    
    if((!available_bytes) && (b_flags & FLAGS_REQSTOP)) 
    {
            p_driver->stop();
            state = STATE_TRANSMITION_STOPPED;
    }
    return ret;
}
int TwoWire::peek(void)
{   
    return 0;
}
void TwoWire::flush(void)
{
}
I2CDriver defaultI2CDriver(SCL_PIN, SDA_PIN);
TwoWire Wire(&defaultI2CDriver);
