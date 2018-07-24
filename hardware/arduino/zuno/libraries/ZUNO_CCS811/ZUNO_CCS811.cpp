#include "ZUNO_CCS811.h"
#include <Wire.h>
extern byte g_ccs_addr;
extern word g_ccs_vref;
extern word g_ccs_ntc;

extern dword ccs811_calctemp;
extern byte  ccs811_tdata[5];
extern CCBYTE ccs811_rst_seq[4];
void aux_ccs_writen(byte reg, byte n);
void aux_ccs_readn(byte n);
#define CCS_READ8(REG)  aux_ccs_writen(REG, 0);aux_ccs_readn(1)
#define CCS_WRITECMD(CMD) aux_ccs_writen(CMD, 0)
#define CCS_WRITE8(REG, VAL) ccs811_tdata[0] = VAL;aux_ccs_writen(REG, 1)

#define DBG_CCS 0
bool ZUNO_CCS811::begin(uint8_t addr){
    g_ccs_addr = _i2caddr = addr;
    Wire.enableTS(true);
    Wire.begin();
    SWReset();
    delay(50);
    CCS_READ8(CCS811_HW_ID);
    if(ccs811_tdata[0] != CCS811_HW_ID_CODE)
        return FALSE; // The device is not avaliable
    CCS_WRITECMD(CCS811_BOOTLOADER_APP_START);
    delay(50);
    CCS_READ8(CCS811_STATUS);
    _status = ccs811_tdata[0];
    if(_status & CCS811_STATUS_ERROR)
        return FALSE;
    if(!(_status & CCS811_STATUS_FW))
        return FALSE;
    // default mode & disable INTs
    _mode = CCS811_DRIVE_MODE_1SEC;
    CCS_WRITE8(CCS811_MEAS_MODE,CCS811_DRIVE_MODE_1SEC);
    return true;
}
void ZUNO_CCS811::setEnvironmentalData(CCS811_Environment * env){

    byte i;
    ccs811_calctemp = env->humidity;
    ccs811_calctemp <<= 9;
    ccs811_calctemp /= 10;

    ccs811_tdata[0] = (ccs811_calctemp >> 8) & 0xFF;
    ccs811_tdata[1] = (ccs811_calctemp) & 0xFF; 

    ccs811_calctemp = env->temperature;
    ccs811_calctemp += 2500; // SHIFT -25C == 0
    ccs811_calctemp <<= 9;
    ccs811_calctemp /= 100;

    ccs811_tdata[2] = (ccs811_calctemp >> 8) & 0xFF;
    ccs811_tdata[3] = (ccs811_calctemp) & 0xFF; 

    aux_ccs_writen(CCS811_ENV_DATA, 4);

}
void ZUNO_CCS811::setThresholds(CCS811_Threshold * th){

}
void ZUNO_CCS811::SWReset(){
    memcpy(ccs811_tdata, ccs811_rst_seq, sizeof(ccs811_rst_seq));
    aux_ccs_writen(CCS811_SW_RESET, sizeof(ccs811_rst_seq));
}
void ZUNO_CCS811::setDriveMode(uint8_t mode){
    _mode = mode;
    CCS_WRITE8(CCS811_MEAS_MODE,_mode);
}
bool ZUNO_CCS811::available(){
    CCS_READ8(CCS811_STATUS);
    _status = ccs811_tdata[0];
    return _status & CCS811_STATUS_DATAREADY;

}
byte ZUNO_CCS811::readData(){
    aux_ccs_writen(CCS811_ALG_RESULT_DATA, 0);
    aux_ccs_readn(5);
    _eCO2 = ccs811_tdata[0];
    _eCO2 <<= 8;
    _eCO2 |= ccs811_tdata[1];
    _TVOC = ccs811_tdata[2];
    _TVOC <<= 8;
    _TVOC |= ccs811_tdata[3];
    _status = ccs811_tdata[4];
    return _status;
}
bool ZUNO_CCS811::checkError(){
    return FALSE;

}
// AUX
byte g_ccs_addr;
word g_ccs_vref;
word g_ccs_ntc;
void aux_ccs_writen(byte reg, byte n){
  byte i;
  Wire.beginTransmission(g_ccs_addr); // start transmission to device 
  Wire.write(reg);
  for(i=0;i<n;i++){
    delayMicroseconds(30);
    Wire.write(ccs811_tdata[i]); // sends register address to read from
  }
  Wire.endTransmission(); // end transmission
  #if DBG_CCS
  Serial0.print(">> ");
  Serial0.print(reg, HEX);
  Serial0.print(" ");
  Serial0.dumpPrint(ccs811_tdata,n);
  Serial0.println("");
  #endif
}
void aux_ccs_readn(byte n) {
  byte i;
  Wire.requestFrom(g_ccs_addr, n);// send data n-bytes read
  for(i=0;i<n;i++){
      delayMicroseconds(30);
      ccs811_tdata[i] = Wire.read(); // receive DATA
  }
  #if DBG_CCS
  Serial0.print("<< ");
  Serial0.dumpPrint(ccs811_tdata,n);
  Serial0.println("");
  #endif
}

CCBYTE ccs811_rst_seq[4] = {0x11, 0xE5, 0x72, 0x8A};

dword ccs811_calctemp;
byte  ccs811_tdata[5];
