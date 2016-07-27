//
//    FILE: dht.cpp
//  AUTHOR: Z-Wave.Me
// VERSION: 0.1
// PURPOSE: DHT Temperature & Humidity Sensor library for Z-Uno
//
// Based on the library by Rob Tillaart
// https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib
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
    int8_t result = _readSensor(pin, DHTLIB_DHT11_WAKEUP);

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
    int8_t result = _readSensor(pin, DHTLIB_DHT_WAKEUP);

    // these bits are always zero, masking them reduces errors.
    bits[0] &= 0x03;
    bits[2] &= 0x83;

    // CONVERT AND STORE
    humidity = (bits[0]*256 + bits[1])/10;
    temperature = ((bits[2] & 0x7F)*256 + bits[3])/10;
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

int8_t dht::_readSensor(uint8_t pin, uint8_t wakeupDelay)
{
    return zunoDHTreadSensor(bits, pin, wakeupDelay);
}
//
// END OF FILE
//