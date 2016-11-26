#include "Me7SegmentDisplay.h"

//* 0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f
/*  0-9 a-f dispaly code */
static unsigned  char  segcode[16]={0xC0,0xF3,0x89,0xA1,0xB2,0xA4,0x84,0xF1,0x80,0xA0,0x90,0x86,0xCC,0x83,0x8C,0x9C};

void seg_display(uint8_t value)
{
    uint8_t value_temp = value & 0x0f;
    uint8_t bp_bit = value & 0x10;
    uint8_t i,dis_bit;
    uint8_t dis_code = segcode[value_temp];
    for(i=0; i<8; i++)
    {
        dis_bit = dis_code & 0x01;
        if(i == 0)
        {
            digitalWrite(P2_6,dis_bit);
        }
        if(i == 1)
        {
            digitalWrite(P1_5,dis_bit);
        }
        if(i == 2)
        {
            digitalWrite(P1_4,dis_bit);
        }
        if(i == 3)
        {
            digitalWrite(P0_6,dis_bit);
        }
        if(i == 4)
        {
            digitalWrite(P2_4,dis_bit);
        }
        if(i == 5)
        {
            digitalWrite(P2_3,dis_bit);
        }
        if(i == 6)
        {
            digitalWrite(P2_5,dis_bit);
        }
        if(i == 7)
        {
            digitalWrite(P0_5,dis_bit);
        }
        dis_code = dis_code >> 1;
    }
    if(bp_bit == 0x10)
    {
        digitalWrite(P0_5,0);
    }
    else
    {
        digitalWrite(P0_5,1);
    }
}
