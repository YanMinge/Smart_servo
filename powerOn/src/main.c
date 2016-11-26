#include "uart_printf.h"
#include "systime.h"
#include "config.h"
#include "wdt.h"

#define POWER_OFF           0
#define POWER_ON            1

sbit INPUT = P3^4;
sbit OUTPUT = P3^5;
sbit LED = P3^3;

uint16_t time_counter = 0;
uint16_t zero_time_counter = 0;
uint8_t key_temp_value;
uint16_t key_debounced_count;
uint16_t key_match_count;
uint8_t power_state = POWER_OFF;
BOOL start_check = false;

void main()
{
	LED = 0;
	delay_ms(5);
  uart_init();
	interrupts();
  OUTPUT = 0;
	wdt_enable(D_WDT_SCALE_4);//开门狗定时142.2ms 11.0592MHz
	while(1)
  {
		if((start_check == false) && (INPUT == 0))
		{
      send_string("start check\r\n");
      start_check = true;
			time_counter = 0;
			key_match_count = 0;
    }
		
		if((power_state == POWER_OFF) && (start_check == true))
		{
		  time_counter ++;
			if(INPUT == 0)
			{
			  key_match_count ++;
			}
			if(time_counter > 100)
			{
				start_check = false;
			  time_counter = 0;
				if(key_match_count > 75)
				{
					send_string("power on\r\n");
					OUTPUT = 1;
					delay_ms(100);
					LED = 1;
				}
				key_match_count = 0;
				while(!INPUT)
				{
          wdt_reset();
					power_state = POWER_ON;
				  send_string("wait 1!\r\n"); 
				}
			}
		}
    if((power_state == POWER_ON) && (start_check == true))
		{
		  time_counter ++;
			if(INPUT == 0)
			{
			  key_match_count ++;
			}
			if(time_counter > 100)
			{
				start_check = false;
			  time_counter = 0;
				if(key_match_count > 60)
				{
          send_string("power off\r\n");
          power_state = POWER_OFF;
          LED = 0;
					OUTPUT = 0;
				}
				key_match_count = 0;
				while(!INPUT)
				{
          wdt_reset();
				  send_string("wait 2!\r\n"); 
				}
			}
		}
		delay1ms();
		wdt_reset();
  }
}