#define __ZUNO_RAW_C
#include "LLCore_arduino.h"
#include "ZUNO_Channels.h"
#include "Custom.h"
#include "Custom_defs.h"

__xdata __at (CALLBACK_TRANSLATION_ADDR)                _xd_ZUNOChannelHandler_t    callback_data;
__xdata __at (SYSCALL_TRANSLATION_ADDR)                 _xd_ZUNOSysCallData_t       syscall_data;
__xdata __at (SKETCH_USERSTRUCT_ADDRESS)                _xd_zuno_sketch_t           g_user_sketch;
__xdata __at (SKETCH_ACCEPTEDSTRUCT_ADDRESS)            _xd_zuno_sketch_t           g_accepted_sketch;
__xdata __at (ZUNO_GLOBAL_CFG_ADDR)                     unsigned char               g_ptr_config[MAX_GLOBAL_CFG];
__xdata __at (ZUNO_DELAY_SAFE_STACK_ADDRESS)            unsigned char               stack_pointer_outside;
__xdata __at (ZUNO_DELAY_USER_STACK_DELTA_ADDRESS)      unsigned char               user_stack_pointer_delta;
__xdata __at (ZUNO_NZRAM_ADRR)                          unsigned char               g_nzram_data[MAX_USER_NZRAM];
__xdata __at (ZUNO_NID_ADRR)                            unsigned char               g_nzram_NID;
__xdata __at (ZUNO_BEAMCOUNT_ADRR)                      unsigned char               g_nzram_BeamCount;
__xdata __at (ZUNO_WUOPTIONS_ADRR)                      unsigned char               g_nzram_WUOpts;
__xdata __at (ZUNO_MSCOUNTER_ADDR)                      unsigned long               g_ms_counter;
__xdata __at (TXBUFF_ADDRESS)                           unsigned char *             g_txbuff;
__xdata __at (PCMD_ADDRESS)                             unsigned char *             g_cmd;

__sbit __at (0x20) ea_save;

#ifdef ZUNO_PRODREV
__code unsigned char  _cc_zuno_pinmap[] = {
  0x27,
  0x26,
  0x25,
  0x37,
  0x36,
  0x35,
  0x34,
  0x31,
  0x30,
  0x00,
  0x01,
  0x02,
  0x03,
  0x04,
  0x05,
  0x06,
  0x07,
  0x10,
  0x11,
  0x12,
  0x13,
  0x14,
  0x15,
  0x16,
  0x21,
  0x20,
  0x22,
  0x23,
  0x24
};           
#else             
#ifdef ZUNO_REV1
__code unsigned char  _cc_zuno_pinmap[] = {
  0x27,
  0x26,
  0x25,
  0x30,
  0x31,
  0x34,
  0x35,
  0x36,
  0x37,
  0x00,
  0x01,
  0x02,
  0x03,
  0x04,
  0x05,
  0x06,
  0x07,
  0x10,
  0x11,
  0x12,
  0x13,
  0x14,
  0x15,
  0x16,
  0x21,
  0x20
};                                  
#else
__code unsigned char  _cc_zuno_pinmap[] = {
  0x27,
  0x26,
  0x25,
  0x37,
  0x36,
  0x35,
  0x34,
  0x31,
  0x30,
  0x00,
  0x01,
  0x02,
  0x03,
  0x04,
  0x05,
  0x06,
  0x07,
  0x10,
  0x11,
  0x12,
  0x13,
  0x14,
  0x15,
  0x16,
  0x21,
  0x20
};                                  
#endif
#endif
void zunoCallback(void);
void __zuno_autosetup(void);

void zunoCall()
{
    __asm
    LCALL 0x002B00
    __endasm;   
}

/* ----------------------------------------------------------------------------
                            Z-Uno callback
-------------------------------------------------------------------------------*/

void begin_callback_code(void) __naked {
    __asm
          .area ABSCODE (ABS,CODE)
          .org 0x8040        // YOUR FUNCTION'S DESIRED ADDRESS HERE.
    __endasm;
}
byte g_usercode_inited = 0;

void zunoJumpTable(void) {
    switch(syscall_data.syscall_num) {
        /*case ZUNO_JUMP_TABLE_SETUP:
        InitArduinoEnvironment();
        break;*/
        case ZUNO_JUMP_TABLE_LOOP:
        ea_save = 1;
        if(!g_usercode_inited){
          InitArduinoEnvironment();
          g_usercode_inited = 1;
        }
        loop();
        #ifndef NO_LOOP_MINIMALDELAY
        delay(20);
        #endif
        break;
        case ZUNO_JUMP_TABLE_CALLBACK:
        zunoCallback();
        break;
        default:
        break;
    }
}
void end_callback_code(void) __naked {
    __asm
        .area CSEG (REL,CODE)
    __endasm;
}

// XRAM initialization
void xdata8051_init(void) {
  __asm
  ;
  ; check there is any user xdata
  MOV r0,#l_XINIT
  MOV a,r0
  ORL a,#(l_XINIT >> 8)
  JZ  EXIT
  ;
  ;
  ;
  ; load registers
  ; r0-1
  MOV r1,#((l_XINIT+255) >> 8)
  MOV r2,#s_XINIT
  MOV r3,#(s_XINIT >> 8)
  MOV r4,#s_XISEG
  MOV r5,#(s_XISEG >> 8)
CYCLE: clr a
  MOV dpl,r2
  MOV dph,r3
  MOVc  a,@a+dptr
  INC dptr
  MOV r2, dpl
  MOV r3, dph
  MOV dpl,r4
  MOV dph,r5
  MOVX  @dptr,a
  INC dptr
  MOV r4,dpl
  MOV r5,dph
  DJNZ  r0,CYCLE
  DJNZ  r1,CYCLE
EXIT:
  __endasm;
}
void zunoInitEvent(void) {
  callback_data.type = ZUNO_CALLBACK_LOG_EVENT;
  callback_data.param1.bParam = 0x00;
  callback_data.param2.bParam = g_ptr_config[ZUNO_CFG_BYTE_WUP_REASON];
  zunoCallback();
}
void InitArduinoEnvironment(void) {
  xdata8051_init();
  __zuno_autosetup();
  zunoInitEvent();
  setup();
}

