#include "Mercury206Meter.h"

#define MERCURY_SERIAL Serial1
#define DEBUG_SERIAL   Serial0
#define MAX_MODBUS_BUFF     52

//#define MODBUS_DBG

/*
Active HIGH for Driver Input Enable; 
Active LOW for Receiver Output Enable. 
It can be controlled by a common IO interface.
*/
#define MODBUS_TRANSMIT    HIGH
#define MODBUS_RECEIVE     LOW

extern word g_curr_crc;
extern byte g_modbus_buff[MAX_MODBUS_BUFF];
extern byte g_modbus_len;

byte bcd2dec(byte c) {
  byte dec;

  dec = (c>>4);
  dec *= 10;
  dec += (c&0x0F);
  return dec;
}

#define BCD2WORD2(W, offset)  W=bcd2dec(g_modbus_buff[offset]); W*=100; W+=bcd2dec(g_modbus_buff[offset+1])  
#define BCD2INT2(W, offset)  W=bcd2dec((g_modbus_buff[offset]) & 0x7F); W*=100; W+=bcd2dec(g_modbus_buff[offset+1]); W = (g_modbus_buff[offset] & 0x08) ?  -W : W
#define BCD2WORD3(W, offset)  BCD2WORD2(W, offset);  W*=100; W+=bcd2dec(g_modbus_buff[offset+2])  
#define BCD2DWORD(W, offset)  BCD2WORD2(W, offset); W*=100; W+=bcd2dec(g_modbus_buff[offset+2]); W*=100; W+=bcd2dec(g_modbus_buff[offset+3])   
#define SEND_GET_DATA(CMD) fillCmd(CMD, 5)
#define SEND_GET_DATA_LEN(CMD,LEN) fillCmd(CMD, LEN)
#define FILL_DATETIME(DT,offset)\
  DT hour    = bcd2dec(g_modbus_buff[offset]);\
  DT minutes = bcd2dec(g_modbus_buff[offset+1]);\
  DT seconds = bcd2dec(g_modbus_buff[offset+2]);\
  DT day     = bcd2dec(g_modbus_buff[offset+3]);\
  DT month   = bcd2dec(g_modbus_buff[offset+4]);\
  DT year    = bcd2dec(g_modbus_buff[offset+5])


void crc16_modbus() {
  byte i,j;
  g_curr_crc = 0xFFFF;
  for(i=0;i<g_modbus_len;i++){
    g_curr_crc ^= word(g_modbus_buff[i]);  
    j=8;
    while(j--){
        if(g_curr_crc & 0x01) {
          g_curr_crc >>= 1;
          g_curr_crc ^= word(0xA001);  
        } else {
          g_curr_crc >>= 1;
        }

    }
  }
}

void send_modbus_cmd(byte dir_pin){
  BYTE i;
  crc16_modbus();
  g_modbus_buff[g_modbus_len] = g_curr_crc & 0xFF;
  g_modbus_buff[g_modbus_len+1] = g_curr_crc >> 8;
  g_modbus_len += 2; 
  #ifdef MODBUS_DBG
  DEBUG_SERIAL.print("\n>>> ");
  DEBUG_SERIAL.dumpPrint(g_modbus_buff, g_modbus_len);
  #endif
  digitalWrite(dir_pin, MODBUS_TRANSMIT);
  delay(5);
  for(i=0;i<g_modbus_len;i++) {
    MERCURY_SERIAL.write(g_modbus_buff[i]);
  }
  delayMicroseconds(5000);
  digitalWrite(dir_pin, MODBUS_RECEIVE);
  delay(4+MAX_MODBUS_BUFF+5);
  g_modbus_len = 0;
  while(MERCURY_SERIAL.available()){
    g_modbus_buff[g_modbus_len] = MERCURY_SERIAL.read();
    g_modbus_len++;
    if(g_modbus_len >= MAX_MODBUS_BUFF)
      return; // to avoid an overflow
  }
  #ifdef MODBUS_DBG
  DEBUG_SERIAL.print("\n<<< ");
  DEBUG_SERIAL.dumpPrint(g_modbus_buff, g_modbus_len);
  DEBUG_SERIAL.println("");
  #endif
  g_modbus_len -= 2;
  crc16_modbus();
  if(g_modbus_buff[g_modbus_len] != (g_curr_crc & 0xFF)){
    g_modbus_len = 0; 
    return;
  }
  g_curr_crc >>= 8;
  if(g_modbus_buff[g_modbus_len+1] != (g_curr_crc)) {
    g_modbus_len = 0; 
    return;
  }
  #ifdef MODBUS_DBG
  DEBUG_SERIAL.println("CRC_OK");
  #endif
  
}
void Mercury206Meter::begin() {
    MERCURY_SERIAL.begin(9600);
    pinMode(m_dir_pin, OUTPUT);
}
void Mercury206Meter::setAddress(dword serialnumber){
    sn[3] =  serialnumber & 0xFF;
    serialnumber >>= 8;
    sn[2] =  serialnumber & 0xFF;
    serialnumber >>= 8;
    sn[1] =  serialnumber & 0xFF;
    serialnumber >>= 8;
    sn[0] =  serialnumber & 0xFF;
}
void Mercury206Meter::fillCmd(byte cmd, byte len) {
    memcpy(g_modbus_buff,sn,4);
    g_modbus_buff[4] = cmd;
    g_modbus_len = len;
    send_modbus_cmd(m_dir_pin);
}
bool Mercury206Meter::getVersion(MercuryFWVersion * p_fw_version) {
    SEND_GET_DATA(COMMAND_MERCURY_GETFWVERSION);
    if(!g_modbus_len)
        return false;
    memcpy(p_fw_version, g_modbus_buff + 5, sizeof(MercuryFWVersion));
    return true;

}

bool Mercury206Meter::getCurrentVAW(MercuryVAW * vaw){
  word tmp;
  SEND_GET_DATA(COMMAND_MERCURY_GETVAW);
  if(!g_modbus_len)
      return false;
   BCD2WORD2(tmp, 5);
   vaw->volts = tmp;
   BCD2WORD2(tmp, 7);
   vaw->amps = tmp;
   BCD2WORD3(tmp, 9);
   vaw->watts = tmp;
   return true;
      
}
bool Mercury206Meter::getCurrentLoadPower(word * p_power){
    SEND_GET_DATA(COMMAND_MERCURY_GETPOWER);
    if(!g_modbus_len)
        return false;
    BCD2WORD2(*p_power, 5);
    return true;

}
word Mercury206Meter::getBatteryVoltage() {
  // COMMAND_MERCURY_GETBATTERYVOLTAGE
  word volts;
  SEND_GET_DATA(COMMAND_MERCURY_GETBATTERYVOLTAGE);
   if(!g_modbus_len)
        return MERCURY_BADDATA_W;
  BCD2WORD2(volts, 5);
  return volts;
}
bool Mercury206Meter::getKWhCounter(byte ci, dword * counter) {
  SEND_GET_DATA(COMMAND_MERCURY_GETKWHCOUNTERS);
  if(!g_modbus_len)
        return false;
  ci <<= 2;
  ci += 5;
  //DEBUG_SERIAL.println(ci);
  BCD2DWORD((*counter), ci);
  return true;
}
byte Mercury206Meter::getFrequencyTarif(word * freq){
  SEND_GET_DATA(COMMAND_MERCURY_GETFREQ);
  if(!g_modbus_len)
    return MERCURY_BADDATA_B;
  BCD2WORD2(*freq, 5);
  return g_modbus_buff[7];
}
word Mercury206Meter::getReactivePower(){
   word res;
   g_modbus_buff[5] = 0; // Reactive power
   SEND_GET_DATA_LEN(COMMAND_MERCURY_GETPARAM, 6);
   if(!g_modbus_len)
    return MERCURY_BADDATA_W;
   BCD2WORD3(res, 6);
   return res;
}
int  Mercury206Meter::getCosFi(){
  int res;
  g_modbus_buff[5] = 2; // COS(Fi)
  SEND_GET_DATA_LEN(COMMAND_MERCURY_GETPARAM, 6);
  if(!g_modbus_len)
    return MERCURY_BADDATA_W;
  BCD2INT2(res, 5);
  return res;
}

byte Mercury206Meter::getRelayMode() {
  SEND_GET_DATA(COMMAND_MERCURY_GETRELAYMODE);
  if(!g_modbus_len)
    return MERCURY_BADDATA_B;
  return g_modbus_buff[5];
}

bool Mercury206Meter::getMaximums(byte type, MercuryMaximums * maxs){
  word tmp;
  g_modbus_buff[5] = type; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_GETMAXIMUMS, 6);
  if(!g_modbus_len)
    return false;
  BCD2WORD2(tmp, 5);
  maxs->system_value = tmp;
  FILL_DATETIME(maxs->system_datetime. ,7);
  BCD2WORD2(tmp, 13);
  maxs->custom_value = tmp;
  FILL_DATETIME(maxs->custom_datetime. ,15);
  return true;
}
word Mercury206Meter::getEnergyLimit() {
  word res;
  SEND_GET_DATA(COMMAND_MERCURY_GETENERGYLIMIT);
  if(!g_modbus_len)
    return MERCURY_BADDATA_B;
  BCD2WORD2(res, 5);
  return res;

}
word Mercury206Meter::getPowerLimit() {
  word res;
  SEND_GET_DATA(COMMAND_MERCURY_GETPOWERLIMIT);
  if(!g_modbus_len)
    return MERCURY_BADDATA_B;
  BCD2WORD2(res, 5);
  return res;
}
bool Mercury206Meter::getCurrentDateTime(MercuryTimeDate * dt){
  SEND_GET_DATA(COMMAND_MERCURY_GETDATETIME);
  if(!g_modbus_len)
    return false;
  FILL_DATETIME(dt-> , 6);
  return true;
}
bool Mercury206Meter::getLastEnergyFail(MercuryTimeDate * dt){
  SEND_GET_DATA(COMMAND_MERCURY_GETOFFDATETIME);
  if(!g_modbus_len)
    return false;
  FILL_DATETIME(dt-> , 6);
  return true;
}
bool Mercury206Meter::getLastEnergyRecover(MercuryTimeDate * dt){
  SEND_GET_DATA(COMMAND_MERCURY_GETONDATETIME);
  if(!g_modbus_len)
    return false;
  FILL_DATETIME(dt-> , 6);
  return true;
}
bool Mercury206Meter::getRelayState(){
   g_modbus_buff[5] = 1; // Relay
   SEND_GET_DATA_LEN(COMMAND_MERCURY_GETPARAM, 6);
   if(!g_modbus_len)
    return MERCURY_BADDATA_B;
   return g_modbus_buff[6];
} 
bool Mercury206Meter::getMercuryTarifTable(byte ii2, MercuryTarifTimeTable * tt){
   g_modbus_buff[5] = ii2; // Relay
   SEND_GET_DATA_LEN(COMMAND_MERCURY_GETTARIFTABLE, 6);
   if(!g_modbus_len)
      return false;
   memcpy(tt, g_modbus_buff+5, sizeof(MercuryTarifTimeTable));
   return true;
}
// SET
bool Mercury206Meter::setTarifCount(byte tc){
  g_modbus_buff[5] = tc; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETTARIFCOUNT, 6);
  if(!g_modbus_len)
    return false;
  return true;
}
bool Mercury206Meter::setRelayMode(byte mode){
  g_modbus_buff[5] = mode; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETRELAYMODE, 6);
  if(!g_modbus_len)
    return false;
  return true;
}
bool Mercury206Meter::setEnergyLimit(word energy){ 
  g_modbus_buff[5] = energy/word(100);
  g_modbus_buff[6] = energy%word(100);
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETENERGYLIMIT, 7);
  if(!g_modbus_len)
    return false;
  return true;
}
bool Mercury206Meter::setPowerLimit(word power){
  g_modbus_buff[5] = power/word(100);
  g_modbus_buff[6] = power%word(100);
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETPOWERLIMIT, 7);
  if(!g_modbus_len)
    return false;
  return true;
}
bool Mercury206Meter::resetMaximums(byte type){
  memset(g_modbus_buff+5,0,8);
  g_modbus_buff[5+8] = type;
  SEND_GET_DATA_LEN(COMMAND_MERCURY_RESETMAXIMUMS, 5+9);
  if(!g_modbus_len)
    return false;
  return true;
}
bool Mercury206Meter::setMainDisplay(byte main){
  g_modbus_buff[5] = main; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETDISPLAYMODE, 6);
  if(!g_modbus_len)
    return false;
  return true;

}
bool Mercury206Meter::setAuxDisplay(byte aux){
  g_modbus_buff[5] = aux; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETDISPLAYAUX, 6);
  if(!g_modbus_len)
    return false;
  return true;
}
bool Mercury206Meter::setTarif(byte tarif) {
  g_modbus_buff[5] = tarif; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETTARIF, 6);
  if(!g_modbus_len)
    return false;
  return true;

}
bool Mercury206Meter::setTarifTable(byte ii2, MercuryTarifTimeTable * tt){
 
  memcpy(g_modbus_buff+5, tt, sizeof(MercuryTarifTimeTable));
  g_modbus_buff[5+sizeof(MercuryTarifTimeTable)] = ii2; 
  SEND_GET_DATA_LEN(COMMAND_MERCURY_SETTARIFTABLE, 5+sizeof(MercuryTarifTimeTable)+1);
  if(!g_modbus_len)
      return false;
  return true;
}
word g_curr_crc = 0xFFFF;
byte g_modbus_buff[MAX_MODBUS_BUFF];
byte g_modbus_len = 0;