// Здесь только то, что действительно нужно из CoreArduino.
BYTE zunoPopByte(void);
WORD zunoPopWord(void);
DWORD zunoPopDWORD(void);
void zunoPushByte(BYTE value);
void zunoPushWord(WORD value);
void zunoPushDword(DWORD value);
void zunoCall(void);
WORD reinterpPOINTER(byte * ptr);
