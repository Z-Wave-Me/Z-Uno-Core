#pragma once
#include "Arduino.h"
// 
// NOTE:
// by default the library uses 1 second poll mode
/*=========================================================================
    I2C ADDRESS/BITS
 -----------------------------------------------------------------------*/
    #define CCS811_ADDRESS                (0x5A)
/*=========================================================================*/

/*=========================================================================
    REGISTERS
-----------------------------------------------------------------------*/
enum
{
        CCS811_STATUS = 0x00,
        CCS811_MEAS_MODE = 0x01,
        CCS811_ALG_RESULT_DATA = 0x02,
        CCS811_RAW_DATA = 0x03,
        CCS811_ENV_DATA = 0x05,
        CCS811_NTC = 0x06,
        CCS811_THRESHOLDS = 0x10,
        CCS811_BASELINE = 0x11,
        CCS811_HW_ID = 0x20,
        CCS811_HW_VERSION = 0x21,
        CCS811_FW_BOOT_VERSION = 0x23,
        CCS811_FW_APP_VERSION = 0x24,
        CCS811_ERROR_ID = 0xE0,
        CCS811_SW_RESET = 0xFF,
};
    
//bootloader registers
enum
{
        CCS811_BOOTLOADER_APP_ERASE = 0xF1,
        CCS811_BOOTLOADER_APP_DATA = 0xF2,
        CCS811_BOOTLOADER_APP_VERIFY = 0xF3,
        CCS811_BOOTLOADER_APP_START = 0xF4
};
    
enum
{
        CCS811_DRIVE_MODE_IDLE = 0x00,
        CCS811_DRIVE_MODE_1SEC = 0x10,
        CCS811_DRIVE_MODE_10SEC = 0x20,
        CCS811_DRIVE_MODE_60SEC = 0x30,
        CCS811_DRIVE_MODE_250MS = 0x40,
};

enum {
    CCS811_ERR_WRITE_REG_INVALID=0x01,
    CCS811_ERR_READ_REG_INVALID=0x02,
    CCS811_ERR_MEASMODE_INVALID=0x04,
    CCS811_ERR_MAX_RESISTANCE=0x08,
    CCS811_ERR_HEATER_FAULT=0x10,
    CCS811_ERR_HEATER_SUPPLY=0x20
};
enum {
    CCS811_STATUS_ERROR=0x01,
    CCS811_STATUS_DATAREADY=0x08,
    CCS811_STATUS_APPVALID=0x10,
    CCS811_STATUS_FW=0x80
};
enum {
    CCS811_MEAS_INT_THR=0x04,
    CCS811_MEAS_INT_DATA=0x08,
    CCS811_MEAS_MODEMASK=0x70,
};
/*=========================================================================*/

#define CCS811_HW_ID_CODE           0x81

#define CCS811_REF_RESISTOR         100000
struct CCS811_Threshold{
    word low_med;
    word hi_med;
    byte hyst;
};
struct CCS811_Environment{
    word humidity;
    int  temperature;
};
class ZUNO_CCS811 {

    public:
        ZUNO_CCS811() {

        };
        bool begin(uint8_t addr = CCS811_ADDRESS);

        void setEnvironmentalData(CCS811_Environment * env);

        //calculate temperature based on the NTC register
        int calculateTemperature();
        
        void setThresholds(CCS811_Threshold * th);

        void SWReset();
        
        void setDriveMode(uint8_t mode);
        
        /**************************************************************************/
        /*! 
            @brief  returns the stored total volatile organic compounds measurement. This does does not read the sensor. To do so, call readData()
            @returns TVOC measurement as 16 bit integer
        */
        /**************************************************************************/
        word getTVOC() { return _TVOC; };

        /**************************************************************************/
        /*! 
            @brief  returns the stored estimated carbon dioxide measurement. This does does not read the sensor. To do so, call readData()
            @returns eCO2 measurement as 16 bit integer
        */
        /**************************************************************************/
        word geteCO2() { return _eCO2; };
        
        byte getMode() { return _mode; };

        //check if data is available to be read
        bool available();
        byte readData();
        
        bool checkError();
        
        private:
            uint8_t _i2caddr;
            int _tempOffset;
        
            word _TVOC;
            word _eCO2;
            byte _status;
            byte _mode;

};