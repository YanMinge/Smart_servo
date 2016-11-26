#include<stdlib.h>
#include<string.h>
#include "MeLedChain.h"
#include "systime.h"
#include "uart_printf.h"

#ifdef PLL_CLOCK
#define F_CPU                     PLL_CLOCK
#else
#define F_CPU                     50000000
#endif

uint8_t rgb_led_quantity;
uint8_t pixels_array[MAX_RGB_LED_NUM*3];
volatile uint32_t* di_gpio_addr = NULL;

struct rgb_value light_panel[] = {{0x99, 0xff, 0x99}, {0x66, 0xff, 0x99}, {0x33, 0xff, 0x33},{0x33, 0xcc, 0x00}, {0x00, 0x99, 0x00},{0x00, 0x66, 0x00},{0x00, 0x33, 0x00},
	                              {0x99, 0xff, 0xff}, {0x33, 0xff, 0xff}, {0x66, 0xcc, 0xcc},{0x00, 0xcc, 0xcc}, {0x33, 0x99, 0x99},{0x33, 0x66, 0x66},{0x00, 0x33, 0x33},
								  {0xcc, 0xff, 0xff}, {0x66, 0xff, 0xff}, {0x33, 0xcc, 0xff},{0x33, 0x66, 0xff}, {0x33, 0x33, 0xff},{0x00, 0x00, 0x99},{0x00, 0x00, 0x66},
								  {0xcc, 0xcc, 0xff}, {0x99, 0x99, 0xff}, {0x66, 0x66, 0xcc},{0x66, 0x33, 0xff}, {0x66, 0x00, 0xcc},{0x33, 0x33, 0x99},{0x33, 0x00, 0x99},
								  {0xff, 0xcc, 0xff}, {0xff, 0x99, 0xff}, {0xcc, 0x66, 0xcc},{0xcc, 0x33, 0xcc}, {0x99, 0x33, 0x99},{0x66, 0x33, 0x66},{0x33, 0x00, 0x33}
                                 };
/*---------------------------------------------------------------------------------------------------------*/
/* GPIO config: should change the value in rgb_init()                                                      */
/*---------------------------------------------------------------------------------------------------------*/
// Timing in ns
#define t0h   400
#define t0l   650

#define t1h   850
#define t1l   400

// Fixed cycles used by the inner loop
#define w_fixedlow    20      //(MOVS+2LDR+STR+3BL)
#define w_fixedhigh   7       //(MOVS+2LDR+STR)

// Insert NOPs to match the timing, if possible
#define w_t0h           (((F_CPU/1000)*t0h          )/1000000)
#define w_t0l           (((F_CPU/1000)*t0l          )/1000000)

#define w_t1h           (((F_CPU/1000)*t1h          )/1000000)
#define w_t1l           (((F_CPU/1000)*t1l          )/1000000)

// w1 - nops between rising edge and falling edge - 0
#define w1 (w_t0h-w_fixedhigh)
// w2   nops between fe low and fe high
#define w2 (w_t0l-w_fixedlow)

// w3 - nops between rising edge and falling edge - 1
#define w3 (w_t1h-w_fixedhigh)
// w4   nops between fe low and fe high
#define w4 (w_t1l-w_fixedlow)


#if w1>0
  #define w0h_nops w1
#else
  #define w0h_nops  0
#endif

#if w2>0
#define w0l_nops w2
#else
#define w0l_nops  0
#endif

#if w3>0
  #define w1h_nops w3
#else
  #define w1h_nops  0
#endif

#if w4>0
#define w1l_nops w4
#else
#define w1l_nops  0
#endif

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
#define w_lowtime ((w0h_nops+w_fixedlow)*1000000)/(F_CPU/1000)

//#if w_lowtime >550
//   #error "Light_ws2812: Sorry, the clock speed is too low. Did you set F_CPU correctly?"
//#elif w_lowtime>450
//   #warning "Light_ws2812: The timing is critical and may only work on WS2812B, not on WS2812(S)."
//   #warning "Please consider a higher clockspeed, if possible"
//#endif

#define w_nop1  __NOP();
#define w_nop2  __NOP();__NOP();
#define w_nop4  w_nop2 w_nop2
#define w_nop8  w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8
#define w_nop32 w_nop16 w_nop16

void rgb_init(uint8_t led_quantity, uint8_t din_pin)
{
    pinMode(din_pin,GPIO_MODE_OUTPUT);
    set_led_quantity(led_quantity);
    di_gpio_addr = Pin2Addr(din_pin);
    digitalWrite(din_pin, 0);
	delayMicroseconds(40); // signal is not stable when mcu start, wait a frame intervl time, then clear all led.
	rgb_clear();
}

void set_led_quantity(int8_t led_quantity)
{
  
	rgb_led_quantity = led_quantity;
}

boolean setColor(uint8_t index, uint8_t red,uint8_t green,uint8_t blue)
{
    uint8_t i,tmp;
    if(index == 0)
    {
		for(i = 1; i <= rgb_led_quantity; i++)
        {
			tmp = (i-1) * 3;
            pixels_array[tmp] = green;
            pixels_array[tmp+1] = red;
            pixels_array[tmp+2] = blue;
        }
        return TRUE;
    }
	
    else
    {
         if(index <= rgb_led_quantity)
         {
             tmp = (index-1) * 3;
             pixels_array[tmp] = green;
             pixels_array[tmp+1] = red;
             pixels_array[tmp+2] = blue;
             return TRUE;
         }
    }
    return FALSE;
}

boolean setColorAll(uint8_t red,uint8_t green,uint8_t blue)
{
    setColor(0,red,green,blue);
    return TRUE;
}

void h_dat0(void)
{
  *di_gpio_addr = 1;
#if (w0h_nops&1)
  w_nop1
#endif
#if (w0h_nops&2)
  w_nop2
#endif
#if (w0h_nops&4)
  w_nop4
#endif
#if (w0h_nops&8)
  w_nop8
#endif
#if (w0h_nops&16)
  w_nop16
#endif
#if (w0h_nops&32)
	w_nop32
#endif
  *di_gpio_addr = 0;
#if (w0l_nops&1)
  w_nop1
#endif
#if (w0l_nops&2)
  w_nop2
#endif
#if (w0l_nops&4)
  w_nop4
#endif
#if (w0l_nops&8)
  w_nop8
#endif
#if (w0l_nops&16)
  w_nop16
#endif
#if (w0l_nops&32)
	w_nop32
#endif
}

void h_dat1(void)
{
  *di_gpio_addr = 1;
#if (w1h_nops&1)
	w_nop1
#endif
#if (w1h_nops&2)
	w_nop2
#endif
#if (w1h_nops&4)
	w_nop4
#endif
#if (w1h_nops&8)
	w_nop8
#endif
#if (w1h_nops&16)
	w_nop16
#endif
#if (w1h_nops&32)
	  w_nop32
#endif
  *di_gpio_addr = 0;
#if (w1l_nops&1)
  w_nop1
#endif
#if (w1l_nops&2)
  w_nop2
#endif
#if (w1l_nops&4)
  w_nop4
#endif
#if (w1l_nops&8)
  w_nop8
#endif
#if (w1l_nops&16)
  w_nop16
#endif
#if (w1l_nops&32)
	w_nop32
#endif
}

void led_reset(void)
{
  *di_gpio_addr = 0;
  delayMicroseconds(100);
}

void send_byte(uint8_t var)
{
	// TODO: The timing is critical, and the low level can't not satisfy the request ,the zero signal low level time is 820ns,and the one signal low level time is 580ns.
	uint32_t var_bit[8] = {0};
	
	for(int i = 0; i<8; i++)
	{
		var_bit[i] = var&0x80;
		var <<= 1;
		i++;
	}
	if(var_bit[0]&0x80)
    {
		h_dat1();
    }
    else
    {
		h_dat0();
    }
		
	if(var_bit[1]&0x80)
    {
		h_dat1();
    }
    else
    {
		h_dat0();
    }
		
	if(var_bit[2]&0x80)
    {
		h_dat1();
    }
    else
    {
		h_dat0();
    }
		
	if(var_bit[3]&0x80)
    {
		h_dat1();
	}
    else
    {
		h_dat0();
    }
		
	if(var_bit[4]&0x80)
    {
		h_dat1();
    }
    else
    {
		h_dat0();
    }
		
	if(var_bit[5]&0x80)
    {
		h_dat1();
	}
	else
    {
		h_dat0();
    }
		
	if(var_bit[6]&0x80)
    {
		h_dat1();
    }
    else
    {
		h_dat0();
    }
		
	if(var_bit[7]&0x80)
    {
		h_dat1();
	}
    else
    {
		h_dat0();
    }
}

void rgb_clear(void)
{
    uint8_t curbyte;
   
	for(curbyte = 0;curbyte < (3*rgb_led_quantity);curbyte++)
    {
		pixels_array[curbyte] = 0;
    }
	
    rgb_show(MAX_RGB_LED_NUM);
}

void rgb_show(uint8_t led_quantity)
{
    uint8_t curbyte = 0,b_index = 0;
    uint16_t datlen;
    datlen = 3*led_quantity;
    
	//__disable_irq();
    while (datlen--)
    {
		curbyte=pixels_array[b_index];
        send_byte(curbyte);
        b_index++;
    }
  	//__enable_irq();
    
	led_reset();
}

void led_chain_all_set(uint8_t led_quantity, uint8_t* led_array)
{
	uint8_t cur_byte, cur_led;
   
	rgb_clear();
	
	if(led_quantity > MAX_RGB_LED_NUM)
	{
		led_quantity = MAX_RGB_LED_NUM;
	}
	for(cur_byte = 0; cur_byte < (3*rgb_led_quantity); cur_byte++)
    {
		pixels_array[cur_byte] = 0;
    }
	
	for(cur_led = 0; cur_led < led_quantity; cur_led++)
	{
		cur_byte = 3*cur_led;
		pixels_array[cur_byte] = light_panel[led_array[cur_led]].g;
		pixels_array[cur_byte + 1] = light_panel[led_array[cur_led]].r;
		pixels_array[cur_byte + 2] = light_panel[led_array[cur_led]].b;
	}
	rgb_show(led_quantity);
}
