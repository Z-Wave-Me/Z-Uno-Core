#pragma once

#include "Stream.h"


class HardwareSerial: public Stream
{
	private:
		BYTE   func_vec;

	public:

		HardwareSerial(BYTE bfn);  // bfn = like ZUNO_FUNC_SERIAL1_BEGIN

    	void begin();
        void begin_(word baud);
    	void begin(DWORD baud);
    	void end();
    	virtual uint8_t available(void);
    	virtual int peek(void);
    	virtual uint8_t read(void);
   
   		virtual void flush(void);
    	virtual void write(uint8_t);

    	uint8_t write(unsigned long n) {  write((uint8_t)n); return 1; }
    	uint8_t write(long n) { write((uint8_t)n); return 1; }
    	uint8_t write(unsigned int n) { write((uint8_t)n); return 1;}
    	uint8_t write(int n) { write((uint8_t)n); return 1; }


};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial0;
