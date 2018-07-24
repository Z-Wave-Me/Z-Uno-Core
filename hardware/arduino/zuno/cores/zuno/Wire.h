/*
  TwoWire.h - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2016 by Z-Wave>Me (info@z-wave.me) for Z-Uno project
*/

#pragma once

#include "Stream.h"



// DEFAULT I2C PINS
#define SCL_PIN  9
#define SDA_PIN  10


#define NACK_ON_ADDRESS             2
#define NACK_ON_DATA                4

// We use auxilary class I2CDriver to give user a capability 
// to remap default I2C pins to another s_pins
// We can do it this way
// I2CDriver custom_I2C(11,12);
// ...
// void setup(){
//      ...  
//      Wire.bindDriver(&custom_I2C);   
//  }  

class I2CDriver
{

    public:
    I2CDriver(s_pin scl = SCL_PIN, s_pin sda = SDA_PIN);
    void bind(void);
    void start(void);   
    void stop(void);   
    byte write(byte b);
    byte read(byte ack);
    void setupTS(bool enable) {_tse = enable;};

  private:
      byte twi_start;
      s_pin _scl;
      s_pin _sda;
      byte _tse;
     
};
class TwoWire 
{
  private:
    
    uint8_t txAddress;
    uint8_t available_bytes;
    uint8_t state;
    uint8_t b_flags;
    uint8_t sucess_code;

    I2CDriver * p_driver;    
    


  public:
    TwoWire(I2CDriver * driver = NULL);
    void begin(I2CDriver * driver = NULL);
    void bindDriver(I2CDriver * driver);
    void beginTransmission(uint8_t, uint8_t forced_write = false);
    uint8_t endTransmission(uint8_t stop = true);
    uint8_t requestFrom(uint8_t, uint8_t, bool stop = true);
    uint8_t write(uint8_t);
    uint8_t available(void);
    uint8_t read(void);
    void enableTS(bool en) {p_driver->setupTS(en); };
    // Sometimes we have to know the status of connection before we 
    // call endTransmission. getStatus() routine do it right way.
    uint8_t getStatus(){return sucess_code; };

};


extern TwoWire Wire;


