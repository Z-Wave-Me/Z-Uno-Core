//
//    FILE: dht.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.21
// PURPOSE: DHT Temperature & Humidity Sensor library for Arduino
//     URL: http://arduino.cc/playground/Main/DHTLib
//
// HISTORY:
// 0.1.21 replace delay with delayMicroseconds() + small fix
// 0.1.20 Reduce footprint by using uint8_t as error codes. (thanks to chaveiro)
// 0.1.19 masking error for DHT11 - FIXED (thanks Richard for noticing)
// 0.1.18 version 1.16/17 broke the DHT11 - FIXED
// 0.1.17 replaced micros() with adaptive loopcount
//        removed DHTLIB_INVALID_VALUE
//        added  DHTLIB_ERROR_CONNECT
//        added  DHTLIB_ERROR_ACK_L  DHTLIB_ERROR_ACK_H
// 0.1.16 masking unused bits (less errors); refactored bits[]
// 0.1.15 reduced # micros calls 2->1 in inner loop.
// 0.1.14 replace digital read with faster (~3x) code => more robust low MHz machines.
//
// 0.1.13 fix negative temperature
// 0.1.12 support DHT33 and DHT44 initial version
// 0.1.11 renamed DHTLIB_TIMEOUT
// 0.1.10 optimized faster WAKEUP + TIMEOUT
// 0.1.09 optimize size: timeout check + use of mask
// 0.1.08 added formula for timeout based upon clockspeed
// 0.1.07 added support for DHT21
// 0.1.06 minimize footprint (2012-12-27)
// 0.1.05 fixed negative temperature bug (thanks to Roseman)
// 0.1.04 improved readability of code using DHTLIB_OK in code
// 0.1.03 added error values for temp and humidity when read failed
// 0.1.02 added error codes
// 0.1.01 added support for Arduino 1.0, fixed typos (31/12/2011)
// 0.1.00 by Rob Tillaart (01/04/2011)
//
// inspired by DHT11 library
//
// Released to the public domain
//


//adopted for Z-Uno


#include "dht.h"

/////////////////////////////////////////////////////
//
// PUBLIC
//

int8_t dht::read11(uint8_t pin)
{
    // READ VALUES
    int8_t result = _readSensor(pin, DHTLIB_DHT11_WAKEUP, DHTLIB_DHT11_LEADING_ZEROS);

    // these bits are always zero, masking them reduces errors.
    bits[0] &= 0x7F;
    bits[2] &= 0x7F;

    // CONVERT AND STORE
    humidity    = bits[0];  // bits[1] == 0;
    temperature = bits[2];  // bits[3] == 0;

    // TEST CHECKSUM
    // bits[1] && bits[3] both 0
    uint8_t sum = bits[0] + bits[2];
    if (bits[4] != sum)
    {
        return DHTLIB_ERROR_CHECKSUM;
    }
    return result;
}

int8_t dht::read(uint8_t pin)
{
    // READ VALUES
    int8_t result = _readSensor(pin, DHTLIB_DHT_WAKEUP, DHTLIB_DHT_LEADING_ZEROS);

    // these bits are always zero, masking them reduces errors.
    bits[0] &= 0x03;
    bits[2] &= 0x83;

    // CONVERT AND STORE
    humidity = (bits[0]*256 + bits[1]) * 0.1;
    temperature = ((bits[2] & 0x7F)*256 + bits[3]) * 0.1;
    if (bits[2] & 0x80)  // negative temperature
    {
        temperature = -temperature;
    }

    // TEST CHECKSUM
    uint8_t sum = bits[0] + bits[1] + bits[2] + bits[3];
    if (bits[4] != sum)
    {
        return DHTLIB_ERROR_CHECKSUM;
    }
    return result;
}

/////////////////////////////////////////////////////
//
// PRIVATE
//

int8_t dht::_readSensor(uint8_t pin, uint8_t wakeupDelay, uint8_t leadingZeroBits)
{
    return zunoDHTreadSensor(bits, pin, wakeupDelay, leadingZeroBits);
}
//
// END OF FILE
//