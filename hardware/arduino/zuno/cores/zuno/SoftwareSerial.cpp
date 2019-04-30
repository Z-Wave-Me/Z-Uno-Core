#include "SoftwareSerial.h"
#include "Arduino.h"

extern s_pin  g_rx_pin;
extern byte   g_rcv_state; 
extern byte   g_cb;
extern byte   g_rcv_buff[MAX_RCV_BUFFER];
extern byte   g_write_pos;
extern byte   g_read_pos;
extern byte   g_rcv_mask;

ZUNO_SETUP_ISR_GPTIMER(softserial_gpt_handler);

#define CONST_USECONDS_OFFSET 10 // uS
#define START_BIT_1HALF       0
#define START_BIT_2HALF       1
#define STOP_BIT_1HALF        18
#define STOP_BIT_2HALF        19


void softserial_gpt_handler() {
   
   if(g_rcv_state == START_BIT_1HALF){
        if(!digitalRead(g_rx_pin)) {
            g_cb = 0;
            g_rcv_state++;
        }
        return;
   }
   if(g_rcv_state == START_BIT_2HALF){
      g_rcv_state++;
      return;
   }
   if(g_rcv_state == STOP_BIT_1HALF){
        g_rcv_buff[g_write_pos] = g_cb;
        g_rcv_state++;
        return;
   }
   if(g_rcv_state == STOP_BIT_2HALF){
       g_write_pos++;
       g_write_pos &= (MAX_RCV_BUFFER-1);
       g_rcv_state = 0;
       return;

   }
   if(!(g_rcv_state & 0x01)){
    g_cb >>= 1;
    if(digitalRead(g_rx_pin)){
        g_cb |= 0x80;
    }
   }
   g_rcv_state++;
}
SoftwareSerial::SoftwareSerial(s_pin tx_pin, s_pin rx_pin) {
     m_tx_pin = tx_pin;
     g_rx_pin = rx_pin;
     duplex = true;

}
SoftwareSerial::SoftwareSerial(s_pin tx_pin){
    m_tx_pin = tx_pin;
    duplex = false;
}
void SoftwareSerial::begin(word baud) {
    dword ticks = 2000000L;
    m_baud = baud;
    if(duplex) {
      ticks /= baud;
      zunoGPTEnable(0); 
      pinMode(g_rx_pin, INPUT_PULLUP);
      zunoGPTInit(ZUNO_GPT_CYCLIC);   
      zunoGPTSet(word(ticks)); 
      zunoGPTEnable(1);
    }
    ticks = 1000000L; // uS 
    ticks /= baud;
    bit_time = ticks;
    bit_time -= CONST_USECONDS_OFFSET;
    pinMode(m_tx_pin, OUTPUT);
    digitalWrite(m_tx_pin, HIGH);

}
void SoftwareSerial::end() {
    zunoGPTEnable(0); 
}
uint8_t SoftwareSerial::available(void) {
    if(!duplex)
      return 0;
    if(g_write_pos < g_read_pos)
        return (MAX_RCV_BUFFER - g_read_pos) + g_write_pos;
    return g_write_pos - g_read_pos;
}
int SoftwareSerial::peek(void) {
    if(!duplex)
      return 0;
    return g_rcv_buff[g_read_pos];
}
uint8_t SoftwareSerial::read(void) {
    if(!duplex)
      return 0;
    byte val = g_rcv_buff[g_read_pos];
    // We use cyclic buffer here
    g_read_pos ++;
    g_read_pos &= (MAX_RCV_BUFFER-1);
    return val;

}
void SoftwareSerial::flush(void) {

}
void SoftwareSerial::write(uint8_t d){
  byte i = 8;
  
  noInterrupts();
  // to be shure that timing is right
  sysClockSet(SYSCLOCK_NORMAL);
  // Start Bit
  digitalWrite(m_tx_pin, 0);
  delayMicroseconds(bit_time);
  delayMicroseconds(4);
  // Bit sequnce from the LSB
  while(i--) {
    if(d & 0x01){
        digitalWrite(m_tx_pin, 1);
    } else {
        digitalWrite(m_tx_pin, 0);
    }
    d >>= 1;
    delayMicroseconds(bit_time);
    delayMicroseconds(3);
  }
  // Stop Bit
  digitalWrite(m_tx_pin, 1);
  delayMicroseconds(bit_time);
  delayMicroseconds(4);
  // revert back system frequency
  sysClockNormallize();
  interrupts();
}
s_pin  g_rx_pin = 12;
byte   g_rcv_state = 0; 
byte   g_cb;
byte   g_rcv_buff[MAX_RCV_BUFFER];
byte   g_write_pos = 0;
byte   g_read_pos = 0;

