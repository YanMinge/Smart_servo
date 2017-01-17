#include "MeNumericDisplay.h"
#include "systime.h"
#include "string.h"
#include "math.h"

#define OFF_POSITION  sizeof(segcode) - 1

/* 0,1,2,3,4,5,6,7,8,9,a,b,c,d,e,f*/
/*  0-9 a-f dispaly code */

/*unsigned  char  segcode[16]={0xC0,0xF3,0x89,0xA1,0xB2,0xA4,0x84,0xF1,0x80,0xA0,0x90,0x86,0xCC,0x83,0x8C,0x9C};*/

// common positive.
static unsigned  char  segcode[13]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0xBF,0xFF}; /* 0,1,2,3,4,5,6,7,8,9,a,-,none*/ 

uint8_t display_buf[4] = {0, 0, 0, 0};
uint8_t update_display_buf[4] = {0, 0, 0, 0};

extern unsigned long currentMillis;
extern unsigned long previousMillis;

void update_display(void)
{
	for(int i = 0; i < 4; i++)
	{
		display_buf[i] = update_display_buf[i];
	}
}

// example: make 1.200 to 1.2
void code_cut(uint8_t DispData[])
{
	int i ,j;
	DispData[3] &= 0x0F;

	for(i = 0 ; i < 4; i++)
	{
		if((DispData[i]&0x10)) // has decimal
		{
			break;
		}
	}
	if(i == 4) // no decimal
	{
		return;
	}
	for(i = 0; i< 3; )
	{
		if(DispData[3-i] == 0)
		{
			i++;
		}
		else
		{
			break;
		}
	}
	if(i == 0)
	{
		return; // no need to cut.
	}
	for(j = 0; j<4-i; j++)
	{
		DispData[3-j] = DispData[3-j-i];
	}
	for(j = 0; j < i; j++)
	{
		DispData[j] = 0x0c;  // no display in this bit.
	}
	
	DispData[3] &= 0x0F;
}

static void seg_display(uint8_t value)
{
    uint8_t value_temp = value & 0x0f; //0~15;
    uint8_t bp_bit = value & 0x10; // has point?
    uint8_t i,dis_bit;
    uint8_t dis_code = segcode[value_temp];
    for(i=0; i<8; i++)
    {
        dis_bit = dis_code & 0x01;
        if(i == 0)
        {
            digitalWrite(A_PIN,dis_bit);
        }
        if(i == 1)
        {
            digitalWrite(B_PIN,dis_bit);
        }
        if(i == 2)
        {
            digitalWrite(C_PIN,dis_bit);
        }
        if(i == 3)
        {
            digitalWrite(D_PIN,dis_bit);
        }
        if(i == 4)
        {
            digitalWrite(E_PIN,dis_bit);
        }
        if(i == 5)
        {
            digitalWrite(F_PIN,dis_bit);
        }
        if(i == 6)
        {
            digitalWrite(G_PIN,dis_bit);
        }
        if(i == 7)
        {
            digitalWrite(DP_PIN,dis_bit);
        }
        dis_code = dis_code >> 1;
    }
    if(bp_bit == 0x10)
    {
        digitalWrite(DP_PIN,0);
    }
    else
    {
        digitalWrite(DP_PIN,1);
    }
}

int checkNum(float v,int b){
    if(b>=0){
        return floor((v-floor(v/pow(10,b+1))*(pow(10,b+1)))/pow(10,b));
    }else{
        b=-b;
        int i=0;
        for(i=0;i<b;i++){
            v = v*10;
        }
        return ((int)(v)%10);
    }
}


void NumericDisplayFloat(float value)
{
    int i=0;
    boolean isStart = false;
    int index = 0;
    uint8_t disp[]={0,0,0,0};
	
	if(value < -999)
	{
		value = -999;
	}
	else if(value > 9999)
	{
		value = 9999;
	}
	
    if(value<0){
        value = -value;
        disp[0] = 0x0b;
        index++;
    }
    for(i=0;i<7;i++){
        int n = checkNum(value,3-i);
        if(n>=1||i==3){
            isStart=true;
        }
        if(isStart){
            if(i==3){
                disp[index]=n+0x10;
            }else{
                disp[index]=n;
            }
            index++;
        }
        if(index>3){
            break;
        }
    }
	
    NumericDisplayBuf(disp);
}

void NumericDisplayStr(char * str)
{
    int i = 0;
    int j = 0;
    uint8_t str_to_num_buf[4] = {0};
    if(str == NULL)
    {
        return;
    }
    for(i = 0; j < 4; i++)
    {
        if(str[i] == '\0')
        {
            str_to_num_buf[j] = OFF_POSITION;
            j++;
        }
        else if(str[i] == '.')
        {
            str_to_num_buf[j-1] = (str_to_num_buf[j-1]| 0x10);
        }
        else if((str[i] >= '0')&&(str[i] <= '9'))
        {
            str_to_num_buf[j] = str[i] - '0';
            j++;
        }
        else
        {
            return;
        }
    }
    memcpy(update_display_buf, str_to_num_buf, sizeof(str_to_num_buf));
}

void NumericDisplayBuf(uint8_t DispData[])
{
	code_cut(DispData);
	
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		update_display_buf[i] = DispData[i];
	}	
}

void refresh_display(void)
{	
		static uint8_t s_index = 0;
	
		digitalWrite(A1_PIN,0);
		digitalWrite(A2_PIN,0);
		digitalWrite(A3_PIN,0);
		digitalWrite(A4_PIN,0);
	
		seg_display(display_buf[s_index]);
		if(s_index == 0)
		{
			digitalWrite(A1_PIN,1);
			digitalWrite(A2_PIN,0);
			digitalWrite(A3_PIN,0);
			digitalWrite(A4_PIN,0);
		}
	
		else if(s_index == 1)
		{
			digitalWrite(A1_PIN,0);
			digitalWrite(A2_PIN,1);
			digitalWrite(A3_PIN,0);
			digitalWrite(A4_PIN,0);
		
		}
		else if(s_index == 2)
		{
			digitalWrite(A1_PIN,0);
			digitalWrite(A2_PIN,0);
			digitalWrite(A3_PIN,1);
			digitalWrite(A4_PIN,0);
		}
		else if(s_index == 3)
		{
			digitalWrite(A1_PIN,0);
			digitalWrite(A2_PIN,0);
			digitalWrite(A3_PIN,0);
			digitalWrite(A4_PIN,1);
		
		}
	
		s_index++;
		if(4 == s_index)
		{
			s_index = 0;
		}	
}

void NumericDispalyInit(void)
		
{
	pinMode(A_PIN, GPIO_MODE_OUTPUT);
	pinMode(B_PIN, GPIO_MODE_OUTPUT);
	pinMode(C_PIN, GPIO_MODE_OUTPUT);
	pinMode(D_PIN, GPIO_MODE_OUTPUT);
	pinMode(E_PIN, GPIO_MODE_OUTPUT);
	pinMode(F_PIN, GPIO_MODE_OUTPUT);
	pinMode(G_PIN, GPIO_MODE_OUTPUT);
	pinMode(DP_PIN, GPIO_MODE_OUTPUT);
	pinMode(A1_PIN, GPIO_MODE_OUTPUT);
	pinMode(A2_PIN, GPIO_MODE_OUTPUT);
	pinMode(A3_PIN, GPIO_MODE_OUTPUT);
	pinMode(A4_PIN, GPIO_MODE_OUTPUT);
}


