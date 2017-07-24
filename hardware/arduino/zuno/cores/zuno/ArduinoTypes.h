#ifndef ARDUINO_LIKE_TYPES__
#define ARDUINO_LIKE_TYPES__
#include "ZUNO_Definitions.h"

typedef unsigned char 	boolean;
typedef unsigned char 	uint8_t;
typedef unsigned char 	s_pin;
typedef unsigned short  uint16_t;
typedef unsigned long   uint32_t;

typedef char 	int8_t;
typedef short  int16_t;
typedef long   int32_t;


typedef unsigned short size_t;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#define byte 	BYTE
#define word    WORD
#define bool 	BYTE

#define TRUE    1
#define FALSE   0

typedef unsigned long   time_t;

struct tmElements_s {
uint8_t Second;
uint8_t Minute;
uint8_t Hour;
uint8_t Wday; // day of week, sunday is day 1
uint8_t Day;
uint8_t Month;
uint8_t Year; // offset from 1970;
};


typedef struct tmElements_s tmElements_t;
typedef enum tmElementsFields{
    tmSecond, tmMinute, tmHour, tmWday, tmDay,tmMonth, tmYear, tmNbrFields
}tmElementsFields_t;	 


//#define true 	TRUE
//#define false	FALSE

#include "string.h"


#endif // ARDUINO_LIKE_TYPES__