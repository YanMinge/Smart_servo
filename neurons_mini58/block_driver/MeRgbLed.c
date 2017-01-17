#include "MeRgbLed.h"
#include "systime.h"

#define HIGH_LAST_1        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();__NOP(); __NOP(); __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP()
#define LOW_LAST_1         __NOP(); __NOP(); __NOP(); __NOP(); __NOP()
#define HIGH_LAST_0        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();__NOP();__NOP()
#define LOW_LAST_0         __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();__NOP(); __NOP(); __NOP();__NOP();__NOP();__NOP();__NOP();__NOP()

#define H_DAT1             *di_gpio_addr = 1; HIGH_LAST_1; HIGH_LAST_1; *di_gpio_addr = 0; LOW_LAST_1; LOW_LAST_1;
#define H_DAT0             *di_gpio_addr = 1; HIGH_LAST_0; HIGH_LAST_0; *di_gpio_addr = 0; LOW_LAST_0; LOW_LAST_0;

void rgb_led_set(uint8_t r_value, uint8_t g_value, uint8_t b_value)
{
	uint32_t var_bit[24] = {0};
    
    uint32_t* di_gpio_addr = Pin2Addr(LED_PIN);
    
	for(int i = 0; i<24; i++)
	{
        if(i < 8)
        {
            var_bit[i] = g_value&0x80;
            g_value <<= 1;
            i++;
        }
        else if(i < 16)
        {
            var_bit[i] = r_value&0x80;
            r_value <<= 1;
            i++;
        }
        else if(i < 24)
        {
            var_bit[i] = b_value&0x80;
            b_value <<= 1;
            i++;
        } 
	}
    __disable_irq();
	if(var_bit[0])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[1])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[2])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[3])
    {
        H_DAT1;
    }
    else
    {
        H_DAT0;
    }
	if(var_bit[4])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[5])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }
	if(var_bit[6])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[7])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }
    if(var_bit[8])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[9])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[10])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
	if(var_bit[11])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[12])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[13])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[14])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[15])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
    if(var_bit[16])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[17])
   {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[18])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[19])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[20])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[21])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[22])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }		
	if(var_bit[23])
    {
		H_DAT1;
    }
    else
    {
        H_DAT0;
    }	
    __enable_irq();
}

void rgb_led_init(void)
{
    pinMode(LED_PIN, GPIO_MODE_OUTPUT);
    digitalWrite(LED_PIN, 0);
	delayMicroseconds(100); // signal is not stable when mcu start, wait a frame intervl time, then clear all led.
	rgb_led_set(0, 0 , 0); // clear
}
