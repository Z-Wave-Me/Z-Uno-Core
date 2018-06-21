#pragma once

#include <Arduino.h>
struct MercuryFWVersion{
    byte version_major;
    byte version_minor;
    byte separator;
    byte hex_day;
    byte hex_month;
    byte hex_year;

};
struct MercuryVAW {
    word volts;
    word amps;
    word watts;
};
struct MercuryTimeDate {
    byte hour;
    byte minutes;
    byte seconds;
    byte day;
    byte month;
    byte year;
};
struct MercuryMaximums {
    word                system_value;
    MercuryTimeDate     system_datetime;
    word                custom_value;
    MercuryTimeDate     custom_datetime;
};
struct MercuryRelayParams {
    byte tries;
    byte delay;
    byte timeout;
};
enum {
    MERCURY_MAX_VOLTAGE,
    MERCURY_MAX_CURRENT,
    MERCURY_MAX_POWER
};

struct MercuryTarifH{
    byte nH;
    byte mm;
};
struct MercuryTarifTimeTable {
    MercuryTarifH entries[16];
};
#define MERCURY_BADDATA_W 0xFFFF
#define MERCURY_BADDATA_B 0xFF

#define COMMAND_MERCURY_SETPOWERLIMIT              0x03
#define COMMAND_MERCURY_SETENERGYLIMIT             0x04
#define COMMAND_MERCURY_SETDISPLAYMODE             0x09
#define COMMAND_MERCURY_SETTARIFCOUNT              0x0A
#define COMMAND_MERCURY_SETTARIF                   0x0B
#define COMMAND_MERCURY_SETTARIFTABLE              0x11
#define COMMAND_MERCURY_RESETMAXIMUMS              0x12  
#define COMMAND_MERCURY_GETDATETIME                0x21
#define COMMAND_MERCURY_GETPOWERLIMIT              0x22
#define COMMAND_MERCURY_GETENERGYLIMIT             0x23
#define COMMAND_MERCURY_GETPOWER                   0x25
#define COMMAND_MERCURY_GETKWHCOUNTERS             0x27 
#define COMMAND_MERCURY_GETFWVERSION               0x28
#define COMMAND_MERCURY_GETBATTERYVOLTAGE          0x29
#define COMMAND_MERCURY_GETOFFDATETIME             0x2B
#define COMMAND_MERCURY_GETONDATETIME              0x2C
#define COMMAND_MERCURY_GETTARIFTABLE              0x31
#define COMMAND_MERCURY_GETMAXIMUMS                0x33 
#define COMMAND_MERCURY_GETVAW                     0x63
#define COMMAND_MERCURY_GETRELAYMODE               0x6D
#define COMMAND_MERCURY_SETDISPLAYAUX              0x70
#define COMMAND_MERCURY_SETRELAYMODE               0x71
#define COMMAND_MERCURY_GETFREQ                    0x81
#define COMMAND_MERCURY_GETPARAM                   0x86





class Mercury206Meter{
  public:
    Mercury206Meter(dword serialnumber=0, byte dir_pin=0): m_dir_pin(dir_pin){
        setAddress(serialnumber);     
    };
    void begin();
    void setAddress(dword serialnumber);
    bool getVersion(MercuryFWVersion * p_fw_version);
    bool getCurrentLoadPower(word * p_power);
    bool getCurrentVAW(MercuryVAW * vaw);
    word getBatteryVoltage();
    bool getKWhCounter(byte ci, dword * counter);
    byte getRelayMode();
    byte getFrequencyTarif(word * freq);
    word getReactivePower();
    int  getCosFi();
    bool getMaximums(byte type, MercuryMaximums * maxs);
    word getEnergyLimit();
    word getPowerLimit();
    bool getCurrentDateTime(MercuryTimeDate * dt);
    bool getLastEnergyFail(MercuryTimeDate * dt);
    bool getLastEnergyRecover(MercuryTimeDate * dt);
    byte getRelayState();
    bool getMercuryTarifTable(byte ii2, MercuryTarifTimeTable * tt);
    bool setTarifCount(byte tc);
    bool setRelayMode(byte mode);
    bool setEnergyLimit(word energy);
    bool setPowerLimit(word power);
    bool resetMaximums(byte type);
    bool setMainDisplay(byte main);
    bool setAuxDisplay(byte aux);
    bool setTarif(byte tarif);
    bool setTarifTable(byte ii2, MercuryTarifTimeTable * tt);
    
    

private:
    void fillCmd(byte cmd, byte len);
    byte sn[4];
    byte m_dir_pin;
};