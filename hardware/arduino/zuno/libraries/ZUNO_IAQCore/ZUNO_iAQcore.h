/**
 * @file IAQCore.h
 * @author Hannes Fuchs
 * @date 2017-07-20
 * @copyright MIT
 * @brief Header file for iAQ-Core C/P Sensor library
 *  
 *  Modified for Z-Uno by Michael Pruefer 07/2018
 *
 * This is a simple libray for the indoor air quality sensors iAQ-core C and
 * iAQ-core P from AMS. Mor information about these sensors could be found
 * under http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors
 *
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-C
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-P
 */

#ifndef _IAQCORE_H
#define _IAQCORE_H

#include <Arduino.h>
// uncomment to enable debugging output
//#define IAQCORE_DEBUG

// output for debugging output
#define IAQCORE_DEBUG_PRINTER Serial

// debug macros
#ifdef IAQCORE_DEBUG
    #define IAQCORE_DEBUG_PRINT(...) { IAQCORE_DEBUG_PRINTER.print(__VA_ARGS__); }
    #define IAQCORE_DEBUG_PRINTLN(...) { IAQCORE_DEBUG_PRINTER.println(__VA_ARGS__); }
#else
    #define IAQCORE_DEBUG_PRINT(...) {}
    #define IAQCORE_DEBUG_PRINTLN(...) {}
#endif


#define IAQCORE_DEF_I2C_ADDR                (0x5A)
/**
 * Do we need this? From Datasheet:
 * "The addressing byte includes the read/write bit at the lowest significant 
 *  bit. The communication with the iAQ-Core starts with 0xB5 for reading 
 *  data."
 */
#define IAQCORE_START_READING               (0xB5)

// atm there are two types: continuous (C) and pulsed (P)
#define IAQCORE_TYPE_C                      (0x00)
#define IAQCORE_TYPE_P                      (0x01)

#define IAQCORE_MEASUREMENT_INTERVAL_TYPE_C (1000)
#define IAQCORE_MEASUREMENT_INTERVAL_TYPE_P (11000)

// Bytes to read
#define IAQCORE_READ_CO2_AND_STATUS         (3)
#define IAQCORE_READ_ALL                    (9)

// Status codes
#define IAQCORE_STATUS_OK                   (0x00)
#define IAQCORE_STATUS_RUNIN                (0x10)
#define IAQCORE_STATUS_BUSY                 (0x01)
#define IAQCORE_STATUS_ERROR                (0x80)
// inofficial status
#define IAQCORE_STATUS_NOT_UPDATED_AT_ALL   (0xf0)
#define IAQCORE_STATUS_UPDATING             (0xf1)
#define IAQCORE_STATUS_I2C_REQ_FAILED       (0xf2)

// Memory addressing
#define IAQCORE_CO2_PREDICTION_MSB_OFFSET   (0x00)
#define IAQCORE_CO2_PREDICTION_LSB_OFFSET   (0x01)
#define IAQCORE_STATUS_OFFSET               (0x02)
// Note: Byte 0x03 is always 0x00
#define IAQCORE_RESISTANCE_NULL_BYTE        (0x03)
#define IAQCORE_RESISTANCE_MSB_OFFSET       (0x04)  
#define IAQCORE_RESISTANCE_MID_OFFSET       (0x05)
#define IAQCORE_RESISTANCE_LSB_OFFSET       (0x06)
#define IAQCORE_TVOC_PREDICTION_MSB_OFFSET  (0x07)
#define IAQCORE_TVOC_PREDICTION_LSB_OFFSET  (0x08)


class IAQCore {
    public:
        IAQCore();
        void begin(uint8_t type=IAQCORE_TYPE_C);
        uint8_t update(byte type = IAQCORE_READ_CO2_AND_STATUS);
        uint16_t getCO2PredictionPPM(void);
        uint8_t getStatus(void);
        uint32_t getSensorResistanceOhm(void);
        uint16_t getTVOCPredictionPPB(void);
    private:
        uint8_t _data[9];
        uint8_t _type;
        uint32_t _lastreadtime;
        uint32_t _measurementinterval;
};

#endif
