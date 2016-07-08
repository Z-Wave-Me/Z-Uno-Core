 
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


TwoWire::TwoWire(uint8_t func_vec)
{
    this->func_vec = func_vec;
    txAddress = 0;
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

    zunoPushByte(txAddress << 1);
    zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
    zunoCall();
    // zunoPopByte() // Вернуть статус

    available_bytes = 1; // Пока так - нужно сделать нормальный интерфейс вызовов
    

}
uint8_t TwoWire::endTransmission(void)
{
    
    zunoPushByte(I2C_END_FUNC_VEC(func_vec));
    zunoCall();

}
uint8_t TwoWire::endTransmission(uint8_t nack)
{
    zunoPushByte(I2C_END_FUNC_VEC(func_vec));
    zunoCall();
}
uint8_t TwoWire::requestFrom(uint8_t addr, uint8_t count)
{
    txAddress = addr;
    zunoPushByte(txAddress << 1 | 0x01);
    zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
    zunoCall();
    available_bytes = count;
}
size_t TwoWire::write(uint8_t data)
{

    
    zunoPushByte(data);
    zunoPushByte(I2C_WRITE_FUNC_VEC(func_vec));
    zunoCall();

    // Должны проверять NACK/ACK и возвращать его
    return zunoPopByte(); 
}
int TwoWire::available(void)
{
    return available_bytes;
}
int TwoWire::read(void)
{
    BYTE ret;
    if(available_bytes)
    {
        available_bytes--;   
    }
    else
        return 0;
    zunoPushByte(nack_ack);
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