#include "MeSoftSerial.h"
#include "mygpio.h"

static uint32_t* pin_address; 
static uint16_t s_tx_delay;

static void BitDelay(uint16_t delay)
{
	SysTick->LOAD = delay;
    SysTick->VAL  =  (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}
	
// initial the software serial.
void SoftwareSerialBegin(uint8_t pin, uint32_t baud_rate)
{
	pinMode(pin, GPIO_MODE_OUTPUT);
	pin_address =Pin2Addr(pin) ;
	digitalWrite(pin, 1);
	s_tx_delay = PLL_CLOCK/baud_rate;
}

// write data.
void SoftwareSerialWrite(uint8_t data)
{
	uint16_t delay = s_tx_delay;

	// start bit.
	*pin_address = 0;
	BitDelay(delay);

	// Write each of the 8 bits
	for (uint8_t i = 8; i > 0; --i)
	{
		if (data & 1) // choose bit
		{
			*pin_address = 1; // send 1.
		}
		else
		{
			*pin_address = 0; // send 0.
		}

		BitDelay(delay);
		data >>= 1;
	}
	
	// stop bit
	*pin_address = 1;
	BitDelay(s_tx_delay);
	
	*pin_address = 1;
}
