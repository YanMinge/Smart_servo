#include "STC15F2K60S2.h"
#include "stdint.h"
#include <stdio.h>
#include <stdarg.h>

#define BAUD  0xFE80                  // 9600bps @ 11.0592MHz

sbit TXB = P3^1;


BYTE TBUF,RBUF;
BYTE TDAT,RDAT;
BYTE TCNT,RCNT;
BYTE TBIT,RBIT;
BOOL TING,RING;
BOOL TEND,REND;

void uart_init()
{
  TING = 0;
  RING = 0;
  TEND = 1;
  REND = 0;
  TCNT = 0;
  RCNT = 0;
	AUXR |= 0x80;
	TMOD &= 0xF0;
	TL0 = BAUD;
	TH0 = BAUD>>8;
	TF0 = 0;
	TR0 = 1;
	ET0 = 1; 
	PT0 = 1; 
	
}

void tm0() interrupt 1 using 1
{
  if (--TCNT == 0)
  {
    TCNT = 3;                       //reset send baudrate counter
    if (TING)                       //judge whether sending
    {
      if (TBIT == 0)
      {
        TXB = 0;                //send start bit
        TDAT = TBUF;            //load data from TBUF to TDAT
        TBIT = 9;               //initial send bit number (8 data bits + 1 stop bit)
      }
      else
      {
        TDAT >>= 1;             //shift data to CY
        if (--TBIT == 0)
        {
          TXB = 1;
          TING = 0;           //stop send
          TEND = 1;           //set send completed flag
        }
        else
        {
          TXB = CY;           //write CY to TX port
        }
      }
    }
  }
}

void uart_write(uint8_t dat)
{
  while (!TEND);
  TEND = 0;
  TBUF = dat;
  TING = 1;
}

void send_string(char *str)
{
  while(*str)
  {
    uart_write(*str++);
  }
}

