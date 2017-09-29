#include "SPI.h"

#include "ZUNO_Definitions.h"
//#include "ZUNO_call_proto.h"
#include "ZUNO_Channels.h"

#define SPI_BEGIN_FUNC_VEC(n) (n + 0)
#define SPI_ENABLE_FUNC_VEC(n) (n + 1)
#define SPI_TRANSFER_FUNC_VEC(n) (n + 2)


void SPISettings::init_it(uint32_t clock, uint8_t bitOrder, uint8_t dataMode){
    // Clock settings are defined as follows. Note that this shows SPI2X
    // inverted, so the bits form increasing numbers. Also note that
    // fosc/64 appears twice
    // SPR1 SPR0 ~SPI2X Freq
    //   0    0     0   fosc/2
    //   0    0     1   fosc/4
    //   0    1     0   fosc/8
    //   0    1     1   fosc/16
    //   1    0     0   fosc/32
    //   1    0     1   fosc/64
    //   1    1     0   fosc/64
    //   1    1     1   fosc/128

    // We find the fastest clock that is less than or equal to the
    // given clock rate. The clock divider that results in clock_setting
    // is 2 ^^ (clock_div + 1). If nothing is slow enough, we'll use the
    // slowest (128 == 2 ^^ 7, so clock_div = 6).
    uint8_t clockDiv;
    uint32_t clockSetting = F_CPU / 2;
    clockDiv = 0;
    while (clockDiv < 6 && clock < clockSetting) {
        clockSetting /= 2;
        clockDiv++;
    }
    

    // Compensate for the duplicate fosc/64
    if (clockDiv == 6)
      clockDiv = 7;

    m_clock       =   clockDiv;
    m_bitorder    =   bitOrder;
    m_mode        =   dataMode;
    
  }

// Initialize the SPI library
SPIClass::SPIClass(uint8_t begin_func_vec)
{
   this->begin_func_vec = begin_func_vec;
}
void SPIClass::begin()
{
   
}
void SPIClass::beginTransaction(SPISettings * settings)
{
  uint8_t speed = settings->getClock();
  uint8_t mode = settings->getMode();
  uint8_t border = settings->getBitOrder();
  if  (  (speed > SPI_SPEED_1_MHZ) || (mode > SPI_MODE3) || (border > MSBFIRST)) {
    // unsupported values
    return;
  }
  zunoSysCall(SPI_BEGIN_FUNC_VEC(begin_func_vec), byte(speed | (mode << 2) | (border << 4)));
  zunoSysCall(SPI_ENABLE_FUNC_VEC(begin_func_vec), byte(TRUE));
}
uint8_t SPIClass::transfer(uint8_t data)
{
   data_tmp = data;
   zunoSysCall(SPI_TRANSFER_FUNC_VEC(begin_func_vec), byte(1), &data_tmp);
   return data_tmp;
}
uint16_t SPIClass::transfer16(uint16_t data)
{
    uint8_t in_out[2]; 
    in_out[0] = data >> 8;
    in_out[1] = data & 0xFF;
    zunoSysCall(SPI_TRANSFER_FUNC_VEC(begin_func_vec), byte(2), &in_out); 
    return *((word*)in_out);  
}
void SPIClass::transfer(void *buf, size_t count)
{
    zunoSysCall(SPI_TRANSFER_FUNC_VEC(begin_func_vec), byte(count), buf); 
}
void SPIClass::endTransaction(void)
{
    zunoSysCall(SPI_ENABLE_FUNC_VEC(begin_func_vec), byte(FALSE));
}
void SPIClass::end()
{ 
    zunoSysCall(SPI_ENABLE_FUNC_VEC(begin_func_vec), byte(FALSE));
}
// Глобальная переменнная для SPI0
SPIClass SPI0(ZUNO_FUNC_SPI0_INIT);



