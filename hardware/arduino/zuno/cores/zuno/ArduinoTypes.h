#ifndef ARDUINO_LIKE_TYPES__
#define ARDUINO_LIKE_TYPES__
#include "ZUNO_Definitions.h"

typedef unsigned char uint8_t;
typedef unsigned short size_t;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

//#define size_t  WORD
#define byte 	BYTE
#define word    WORD
#define bool 	BYTE

/*enum 
{
	FALSE  = 0,
	TRUE   = 1
};*/

/*
enum 
{
	false = 0,
	true = 1
}*/
#define TRUE    1
#define FALSE   0

//#define true 	TRUE
//#define false	FALSE


#endif // ARDUINO_LIKE_TYPES__