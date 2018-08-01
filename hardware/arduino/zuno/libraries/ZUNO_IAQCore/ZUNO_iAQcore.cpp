/**
 * @file IAQCore.cpp
 * @author Hannes Fuchs
 * @date 2017-07-20
 * @copyright MIT
 * @brief Main Class file for iAQ-Core C/P Sensor library
 * 
 * Modified for Z-Uno by Michael Pruefer 07/2018
 *
 * This is a simple libray for the indoor air quality sensors iAQ-core C and
 * iAQ-core P from AMS. More information about these sensors could be found
 * under http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors
 *
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-C
 * @see http://ams.com/eng/Products/Environmental-Sensors/Air-Quality-Sensors/iAQ-core-P
 */

#include "ZUNO_IAQCore.h"

#include <Wire.h>
/**
 * @breif The main Class for the iAQ-core sensor library
 */
IAQCore::IAQCore() {
}


/**
 * @brief Initalizates the I2C Bus and sets the sensor type
 * @param type The sensor type C or P, see IAQCore.h
 */
void IAQCore::begin(uint8_t type) {
    // We need the clock stretching here...
    Wire.enableTS(true);
    Wire.begin();
    _type = type;

    // atm there are only two types
    if (type == IAQCORE_TYPE_C) {
        _lastreadtime = -IAQCORE_MEASUREMENT_INTERVAL_TYPE_C;
        _measurementinterval = IAQCORE_MEASUREMENT_INTERVAL_TYPE_C;
    } else {
        _lastreadtime = -IAQCORE_MEASUREMENT_INTERVAL_TYPE_P;
        _measurementinterval = IAQCORE_MEASUREMENT_INTERVAL_TYPE_P;
    }

    // set unofficial status
    _data[IAQCORE_STATUS_OFFSET] = IAQCORE_STATUS_NOT_UPDATED_AT_ALL;
}

/**
 * @brief The main method to request values from the sensor
 * @param num defines how many values to update. Possible values: 
 *          IAQCORE_READ_CO2_AND_STATUS, IAQCORE_READ_ALL 
 * Updates sensor values
 *
 * @return Sensor status, see IAQCore.h for values
 */
uint8_t IAQCore::update(byte num) {
    // return quickly if we read to fast
    uint32_t currenttime = millis();
    if ((currenttime - _lastreadtime) < _measurementinterval) {
        IAQCORE_DEBUG_PRINTLN(F("Measurement interval to short, return last status"));
        return _data[IAQCORE_STATUS_OFFSET];
    }
    // set measurement time
    _lastreadtime = currenttime;
    memset(_data, 0x00, 9);
    // to be safe, this status should be overwritten
    _data[IAQCORE_STATUS_OFFSET] = IAQCORE_STATUS_UPDATING;

    IAQCORE_DEBUG_PRINTLN(F("Request data from sensor"));
    Wire.requestFrom(IAQCORE_DEF_I2C_ADDR, num, true);
    // read into buffer
    IAQCORE_DEBUG_PRINT(F("Received: "));
    for (byte i=0; i<num; i++) {
        _data[i] = Wire.read();
        IAQCORE_DEBUG_PRINT(_data[i], HEX); IAQCORE_DEBUG_PRINT(F(" "));
    }
    IAQCORE_DEBUG_PRINTLN(F(""));

    return _data[IAQCORE_STATUS_OFFSET];
}


/**
 * @brief Calculates the CO2 prediction value and returns it.
 *
 * @return Calculated CO2 prediction value
 */
uint16_t IAQCore::getCO2PredictionPPM(void) {
    uint16_t tmp = _data[IAQCORE_CO2_PREDICTION_MSB_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_CO2_PREDICTION_LSB_OFFSET];
    return tmp;
}

/**
 * @brief Just returns the status of the sensor
 *
 * @return Sensor status, see IAQCore.h for values
 */
uint8_t IAQCore::getStatus(void) {
    return _data[IAQCORE_STATUS_OFFSET];
}

/**
 * @brief Calculates the sensors resistance value and returns it.
 *
 * @return Calculated sensor resistance value
 */
uint32_t IAQCore::getSensorResistanceOhm(void) {
    uint32_t tmp = _data[IAQCORE_RESISTANCE_MSB_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_RESISTANCE_MID_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_RESISTANCE_LSB_OFFSET];
    return tmp;
}

/**
 * @brief Calculates the TVOC prediction value and returns it.
 *
 * @return Calculated TVOC prediction value
 */
uint16_t IAQCore::getTVOCPredictionPPB(void) {
    uint16_t tmp = _data[IAQCORE_TVOC_PREDICTION_MSB_OFFSET];
    tmp <<= 8;
    tmp += _data[IAQCORE_TVOC_PREDICTION_LSB_OFFSET];
    return tmp;
}

