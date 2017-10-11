#define __ZUNO_RAW_C
#include "LLCore_arduino.h"
#include "ZUNO_Channels.h"
#include "Custom.h"

__xdata __at (CALLBACK_TRANSLATION_ADDR)                _xd_ZUNOChannelHandler_t    callback_data;
__xdata __at (SYSCALL_TRANSLATION_ADDR)                 _xd_ZUNOSysCallData_t       syscall_data;
__xdata __at (SKETCH_USERSTRUCT_ADDRESS)                _xd_zuno_sketch_t           g_user_sketch;
__xdata __at (ZUNO_GLOBAL_CFG_ADDR)                     unsigned char               g_ptr_config[MAX_GLOBAL_CFG];
__xdata __at (ZUNO_DELAY_SAFE_STACK_ADDRESS)            unsigned char               stack_pointer_outside;
__xdata __at (ZUNO_DELAY_USER_STACK_DELTA_ADDRESS)      unsigned char               user_stack_pointer_delta;

__sbit __at (0x20) ea_save;

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
void zunoJumpTable(void) {
    switch(syscall_data.syscall_num) {
        case ZUNO_JUMP_TABLE_SETUP:
        InitArduinoEnvironment();
        break;
        case ZUNO_JUMP_TABLE_LOOP:
        ea_save = 1;
        loop();
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

void InitArduinoEnvironment(void) {
  xdata8051_init();
  __zuno_autosetup();
  setup();
}