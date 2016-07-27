 
#include "Wire.h"
#include "ZUNO_Definitions.h"
#include "ZUNO_call_proto.h"
#include "ArduinoCAPI.h"

#define I2C_BEGIN_FUNC_VEC(n)   (n + 0)
#define I2C_END_FUNC_VEC(n)     (n + 1)
#define I2C_WRITE_FUNC_VEC(n)   (n + 2)
#define I2C_READ_FUNC_VEC(n)    (n + 3)

#define SCL_PIN  9
#define SDA_PIN  10

#define STATE_TRANSMITION_INIT      0
#define STATE_TRANSMITION_WRITEMODE 1
#define STATE_TRANSMITION_READMODE  2
#define STATE_TRANSMITION_STOPPED   3

#define NACK_ON_ADDRESS             2
#define NACK_ON_DATA                4



#define FLAGS_REQSTOP               1





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
void TwoWire::beginTransmission(uint8_t addr)
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

}
uint8_t TwoWire::endTransmission(uint8_t stop)
{
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