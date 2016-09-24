 

#include "Wire.h"

#include "ZUNO_Definitions.h"
#include "ZUNO_call_proto.h"
#include "ArduinoCAPI.h"


/*
#define I2C_BEGIN_FUNC_VEC(n)   (n + 0)
#define I2C_END_FUNC_VEC(n)     (n + 1)
#define I2C_WRITE_FUNC_VEC(n)   (n + 2)
#define I2C_READ_FUNC_VEC(n)    (n + 3)
*/


#define STATE_TRANSMITION_INIT      0
#define STATE_TRANSMITION_WRITEMODE 1
#define STATE_TRANSMITION_READMODE  2
#define STATE_TRANSMITION_STOPPED   3

#define NACK_ON_ADDRESS             2
#define NACK_ON_DATA                4

#define FLAGS_REQSTOP               1






// I2C Driver to use it with spins
// AUXILARY CLASS
I2CDriver::I2CDriver(s_pin scl, s_pin sda): _scl(scl), _sda(sda)
{   
}
void I2CDriver::bind(void)
{
    pinMode(_scl, OUTPUT);
    pinMode(_sda, OUTPUT);
    digitalWrite(_scl, HIGH);
    digitalWrite(_sda, HIGH);
}
void I2CDriver::start(void)
{
     noInterrupts();   
     NOPS(2);  
     digitalWrite(_sda, LOW);
     NOPS(4);  
     interrupts();
}   
void I2CDriver::stop(void)
{
    noInterrupts();   
    digitalWrite(_scl, LOW);
    NOPS(2);    
    pinMode(_sda, OUTPUT);
    digitalWrite(_sda, LOW);
    NOPS(8);
    digitalWrite(_scl, HIGH);
    NOPS(8);
    digitalWrite(_sda, HIGH);
    NOPS(8);
    interrupts();
}   
byte I2CDriver::write(byte b)
{
    byte i = 8, ack;
    noInterrupts();
    pinMode(_sda, OUTPUT);
    while(i)
    {
        NOPS(3);    
        digitalWrite(_scl, LOW);
        NOPS(3); 
        if(b & 0x80)
            digitalWrite(_sda, HIGH);
        else
            digitalWrite(_sda, LOW);
        NOPS(15);
        //delayCycles(1);
        digitalWrite(_scl, HIGH);
        NOPS(15);
        //delayCycles(1);
        b <<= 1;
        i--;
    }
            
    // Принимаем ACK
    digitalWrite(_scl, LOW);
    digitalWrite(_sda, HIGH);
    //NOPS(1);
    pinMode(_sda, INPUT_PULLUP);
    //delayCycles(10);
    NOPS(12);
    ack = digitalRead(_sda) == 0;
    //pinMode(_sda, OUTPUT);
    //digitalWrite(_sda, HIGH);
    NOPS(3);
    digitalWrite(_scl, HIGH);

    NOPS(8);
    
    pinMode(_sda, OUTPUT);
    interrupts();
    
    return ack;

}

byte I2CDriver::read(byte ack)
{
    byte i = 8, input = 0;

    ack = !ack;
    noInterrupts();
    //digitalWrite(_sda, HIGH);
    pinMode(_sda, INPUT_PULLUP);
    while(i)
    {

        digitalWrite(_scl, LOW);
        NOPS(32);
        digitalWrite(_scl, HIGH);
        input <<= 1;
        input |= digitalRead(_sda) > 0;
        NOPS(2);
        i--;
    }
   
    // Записываем ACK
    digitalWrite(_scl, LOW);
    NOPS(3); 
    pinMode(_sda, OUTPUT);
    if(ack)
        digitalWrite(_sda, HIGH);
    else
        digitalWrite(_sda, LOW);
    NOPS(15);
    digitalWrite(_scl, HIGH);
    NOPS(18);
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

    
    // Подсоединяемся к шине I2C как мастер
    p_driver->start();

    available_bytes = 1; // Пока так - нужно сделать нормальный интерфейс вызовов
    state = STATE_TRANSMITION_INIT;

    b_flags = 0; //FLAGS_REQSTOP;

    sucess_code = 0;

    if(forced_write)
    {
        sucess_code = p_driver->write(txAddress << 1);    
        state = STATE_TRANSMITION_WRITEMODE;
    }

}
uint8_t TwoWire::endTransmission(uint8_t stop)
{
    // Нужно для того, чтобы работал нормально скан шины и тд
    if(state == STATE_TRANSMITION_INIT)
    {
        if(!p_driver->write((txAddress << 1) | 0x01))
        {
            sucess_code |= NACK_ON_ADDRESS;
        }    
    }
   
    p_driver->stop();

    state = STATE_TRANSMITION_STOPPED;

    return sucess_code;
}
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count, bool stop)
{
    txAddress = addr;

    available_bytes = count;

    if(stop)
    {
        b_flags |= FLAGS_REQSTOP;
    }
    // Arduino uses this function to start I2C connection sometimes
    if(state == STATE_TRANSMITION_STOPPED)
    {
         p_driver->start();
         state = STATE_TRANSMITION_INIT;

    }
    if(state == STATE_TRANSMITION_INIT)
    {
        byte ret = p_driver->write((txAddress << 1) | (0x01));
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
   
   

    return available_bytes;
}
size_t TwoWire::write(uint8_t data)
{
    byte res;

    if(state == STATE_TRANSMITION_INIT)
    {

        res = p_driver->write(txAddress << 1);
        if(!res)
        {
            sucess_code |= NACK_ON_ADDRESS;
            return res;
        }
        state = STATE_TRANSMITION_WRITEMODE;

    }
    res = p_driver->write(data);
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
    BYTE b_ack = 1;
    if(state == STATE_TRANSMITION_STOPPED)
        return 0xFF;
    if(state == STATE_TRANSMITION_INIT)
    {
        ret = p_driver->write((txAddress << 1) | (0x01));
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
    if(!available_bytes)
        b_ack = 0;
    ret = p_driver->read(b_ack);
    
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

/*
TwoWire::TwoWire(uint8_t func_vec)
{
    this->func_vec = func_vec;
    txAddress = 0;
    state = STATE_TRANSMITION_INIT;

}
void TwoWire::begin()
{
    txAddress = 0;
    

    // Инициализируем пины
    pinMode(SCL_PIN, OUTPUT);
    pinMode(SDA_PIN, OUTPUT);
    digitalWrite(SCL_PIN, HIGH);
    digitalWrite(SDA_PIN, HIGH);



}
void TwoWire::beginTransmission(uint8_t addr, uint8_t forced_write)
{
    txAddress = addr;

    
    // Подсоединяемся к шине I2C как мастер
    zunoPushByte(I2C_BEGIN_FUNC_VEC(func_vec));
    zunoCall();

    // zunoPopByte() // Вернуть статус

    available_bytes = 1; // Пока так - нужно сделать нормальный интерфейс вызовов
    state = STATE_TRANSMITION_INIT;

    b_flags = 0; //FLAGS_REQSTOP;

    sucess_code = 0;

    if(forced_write)
    {
        zunoPushByte(txAddress << 1);
        zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
        zunoCall();
        state = STATE_TRANSMITION_WRITEMODE;
        sucess_code = zunoPopByte();
    }

}
uint8_t TwoWire::endTransmission(uint8_t stop)
{
    // Нужно для того, чтобы работал нормально скан шины и тд
    if(state == STATE_TRANSMITION_INIT)
    {
        zunoPushByte(txAddress << 1 | 0x01);
        zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
        zunoCall();
        if(!zunoPopByte())
        {
            sucess_code |= NACK_ON_ADDRESS;
        }
    }
    zunoPushByte(I2C_END_FUNC_VEC(func_vec));
    zunoCall();

    return sucess_code;
}
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count, bool stop)
{
    txAddress = addr;
   
    available_bytes = count;

    if(stop)
    {
        b_flags |= FLAGS_REQSTOP;
    }
}
size_t TwoWire::write(uint8_t data)
{
    byte res;

    if(state == STATE_TRANSMITION_INIT)
    {

        zunoPushByte(txAddress << 1);
        zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
        zunoCall();
        state = STATE_TRANSMITION_WRITEMODE;
        res = zunoPopByte();

        if(!res)
        {
            sucess_code |= NACK_ON_ADDRESS;
            return res;
        }

    }

    //output_buff[]
    
    zunoPushByte(data);
    zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
    zunoCall();
    res = zunoPopByte();
    if(!res)
    {
        sucess_code |= NACK_ON_DATA;
    }
    // Должны проверять NACK/ACK и возвращать его
    return res; 
}
int TwoWire::available(void)
{
    return available_bytes;
}
int TwoWire::read(void)
{
    BYTE ret;
    BYTE b_ack = 1;
    if(state == STATE_TRANSMITION_INIT)
    {

        zunoPushByte(txAddress << 1 | 0x01);
        zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
        zunoCall();
        state = STATE_TRANSMITION_READMODE;
        if(!zunoPopByte())
        {
            sucess_code |= NACK_ON_ADDRESS;
            state = STATE_TRANSMITION_STOPPED;
            return 0xFF;  

        }
    }
    if(state == STATE_TRANSMITION_STOPPED)
    {
        return 0xFF;
    }
    if(available_bytes)
    {
        available_bytes--;  
        
        if((!available_bytes) && (b_flags & FLAGS_REQSTOP)) 
        {
            b_ack = 0;
            state = STATE_TRANSMITION_STOPPED;
        }
    }
    else
    {
        return 0;
    }
    zunoPushByte(b_ack);
    zunoPushByte(I2C_READ_FUNC_VEC(func_vec));
    zunoCall();

    ret = zunoPopByte();
    return ret;
}
int TwoWire::peek(void)
{   
    return 0;
}
void TwoWire::flush(void)
{

}
 
TwoWire Wire(ZUNO_FUNC_I2C_BEGIN); 

*/  