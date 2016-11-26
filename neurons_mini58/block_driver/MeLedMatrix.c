#include "MeLedMatrix.h"
#include "systime.h"

#define TM1639_CLK    P2_3
#define TM1639_DATA   P0_6
#define TM1639_STB    P2_2

#define AUTO_ADD_MODE  0x40
#define FIX_ADD_MODE   0x44
#define BRIGHTNESS     0x87
#define START_ADDRESS  0xc0
#define DISPLAY_ON     0x88


unsigned char display_matrix[8]={
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
};

static void send_data(uint8_t x)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		digitalWrite(TM1639_CLK, 0);
		delayMicroseconds(1);
		digitalWrite(TM1639_DATA, x&(0x01));
		delayMicroseconds(1);
		digitalWrite(TM1639_CLK, 1);
		x>>=1;
	}

}

void led_matrix_set(uint8_t matrix[])
{
	uint8_t i;
	for(i = 0; i < 8; i++)
	{
		display_matrix[i] = matrix[i]; 
	}
	
	digitalWrite(TM1639_STB, 0);
	delayMicroseconds(1);
	send_data(BRIGHTNESS);       // set brightness.
	digitalWrite(TM1639_STB, 1);
	delayMicroseconds(1);
	
	digitalWrite(TM1639_STB, 0);
	delayMicroseconds(1);
	send_data(AUTO_ADD_MODE);    // address auto adding mode
	digitalWrite(TM1639_STB, 1);
	delayMicroseconds(1);
	
	digitalWrite(TM1639_STB, 0);
	delayMicroseconds(1);
	send_data(START_ADDRESS);  // send start address.
	digitalWrite(TM1639_STB, 0);
	delayMicroseconds(1);
	
	for(i = 0; i < 8; i++)
	{
		send_data(display_matrix[i]&0x0f);
		send_data((display_matrix[i]>>4)&0x0f);
	}
	digitalWrite(TM1639_STB, 1);
	delayMicroseconds(1);
	
	digitalWrite(TM1639_STB, 0);
	send_data(DISPLAY_ON);
	digitalWrite(TM1639_STB, 1);	
}

void led_matrix_init(void)
{
	pinMode(TM1639_CLK, GPIO_MODE_OUTPUT);
	pinMode(TM1639_DATA, GPIO_MODE_OUTPUT);
	pinMode(TM1639_STB, GPIO_MODE_OUTPUT);
	digitalWrite(TM1639_STB, 1);
    digitalWrite(TM1639_DATA, 1);
	digitalWrite(TM1639_CLK, 1);   
}
