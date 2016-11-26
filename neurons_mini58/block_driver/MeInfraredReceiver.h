#ifndef __MEINFRAREDRECEIVER_H__
#define __MEINFRAREDRECEIVER_H__
#include "systime.h"
#include "mygpio.h"

#define NEC_BITS        32
#define NEC_HDR_MARK	9000
#define NEC_HDR_SPACE	4500
#define NEC_BIT_MARK	560
#define NEC_ONE_SPACE	1600
#define NEC_ZERO_SPACE	560
#define NEC_RPT_SPACE	2250
#define NEC_RPT_PERIOD	110000

#define LED_PIN P2_4

typedef enum _IR_STATE{
	IR_READY,
	IR_9000,
	IR_FRAME_START,
	IR_FRAME_END,
	IR_FRAME_REPEAT,
	IR_ERROR
}IR_STATE;

extern volatile IR_STATE g_ir_state;
extern volatile uint16_t  g_inframed_address;
extern volatile uint8_t  g_inframed_data;
extern volatile uint32_t  g_test_time;

/* Define a storage array encoding. 
1 bit boot code, 8 bit customer code, 8 bit customer Anti-code, 
8 bit data code, 8 bit data counter Anti-code, A total of 33bit
*/
extern volatile unsigned char ir_recv_bit[32];

//This flag used to mark the data location we have received.
extern volatile unsigned char bitnum;   
extern volatile unsigned int irtime;            //Infrared scanning time
extern volatile unsigned char ircode[4];        //Storage the 4 byte data.

boolean ir_recv_poll(void);
void InfraredReceiverInit(void);
#endif
