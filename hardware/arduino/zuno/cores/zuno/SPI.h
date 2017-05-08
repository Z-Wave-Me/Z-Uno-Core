/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@arduino.cc>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * Copyright (c) 2014 by Andrew J. Kroll <xxxajk@gmail.com> (atomicity fixes)
 * Copyright (c) 2016 by Z-Wave>ME info@z-wave.me 
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 * Adoptated for Z-Uno project.
 */
#pragma once

#include "ArduinoTypes.h"
#include "ZUNO_Definitions.h"

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 1

// SPI_ATOMIC_VERSION means that SPI has atomicity fixes and what version.
// This way when there is a bug fix you can check this define to alert users
// of your code if it uses better version of this library.
// This also implies everything that SPI_HAS_TRANSACTION as documented above is
// available too.
#define SPI_ATOMIC_VERSION 1

// Uncomment this line to add detection of mismatched begin/end transactions.
// A mismatch occurs if other libraries fail to use SPI.endTransaction() for
// each SPI.beginTransaction().  Connect an LED to this pin.  The LED will turn
// on if any mismatch is ever detected.
//#define SPI_TRANSACTION_MISMATCH_LED 5


class SPISettings {
public:
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) {
     init_it(clock, bitOrder, dataMode);   
  }
  SPISettings() {
    init_it(4000000, MSBFIRST, SPI_MODE0);
  }

  uint8_t getClock(){return m_clock;};
  uint8_t getBitOrder(){return m_bitorder;};
  uint8_t getMode(){return m_mode;};



private:
  void init_it(uint32_t clock, uint8_t bitOrder, uint8_t dataMode);
  
  uint8_t m_clock;
  uint8_t m_bitorder;
  uint8_t m_mode;
  
};


class SPIClass {
public:
  SPIClass(uint8_t begin_func_vec);
  // Initialize the SPI library
  void begin();
  void beginTransaction(SPISettings * settings);
  // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, size_t count);
  // After performing a group of transfers and releasing the chip select
  // signal, this function allows others to access the SPI bus
  void endTransaction(void);
  // Disable the SPI bus
  void end();

private:
  uint8_t begin_func_vec;
  uint8_t data_tmp;
  
};

#define SPI SPI0
extern SPIClass SPI0;


