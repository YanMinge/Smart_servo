#include "systime.h"
#include "intrins.h"

void delay1ms(void)		//@11.0592MHz
{
	unsigned char i, j;
	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void delay_ms(unsigned int ms)
{
	while(--ms)
  delay1ms();
}