/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/05/19 11:45a $
 * @brief    for neuron product
 *
 * @note
 * Copyright (C) 2015 myan@makeblock.cc. All rights reserved.
 *
 ******************************************************************************/
#include "sysinit.h"
#include <stdio.h>
#include "uart_printf.h"
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"
#include "MeNumericDisplay.h"

/*---------------------------------------------------------------------------------------------------------*/
/* MACRO define                                                                                  		  */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION            003
#define CTL_SET_DISPLAY_VALUE       0x01

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint16_t g_firmware_version = FIRMWARE_VERSION;
// time variables.
uint32_t currentMillis;        // store the current value from millis()
uint32_t previousMillis;       // for comparison with currentMillis
volatile unsigned long system_time = 0;

union offline_data{
    uint8_t data_buf[8];
    uint8_t byte8_val;
	int8_t  byte16_val;
	int16_t short16_val;
	int32_t long32_val;
	float   float32_val;
};


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);

void init_block(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void sysex_process_offline(void);
/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    volatile uint32_t u32InitCount;
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
	
	init_neuron_system();
	
	TMR1_Init(300);
	
	init_block();

    while(1)
    {
		// protocol.
		parse_uart0_recv_buffer(); // parse uart0 receviced data.	
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();	
		
		// led
		poll_led_request();
		
		currentMillis = millis();
		if(currentMillis - previousMillis > 100)
		{
			previousMillis = currentMillis;
			update_display();
		}
    }
}


void init_block(void)
{
	g_block_type = CLASS_DISPLAY;
	g_block_sub_type = BLOCK_NUMERIC_DISPLAY;
	NumericDispalyInit();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
	set_rgb_led(0, 0, 128);
}
	


void sysex_process_online(void)
{
	float display_value;
	uint8_t block_type, sub_type, data_type, cmd_type;
	
	// read block type.
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if((block_type != g_block_type)||(sub_type != g_block_sub_type)) // block type wrong!
	{
        send_sysex_error_code(WRONG_TYPE);
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE) == false)
	{
		return;
	}
	if(cmd_type == CTL_SET_DISPLAY_VALUE)
	{
		// read block data.
		data_type = FLOAT_40;
		if(read_next_sysex_data(&data_type, &display_value, ON_LINE) == false)
		{
			return;
		}
		// display.
		NumericDisplayFloat(display_value);
	}
}

void sysex_process_offline(void)
{
	union offline_data first_data ={0};
	float display_value;
	uint8_t block_type, sub_type, data_type;
    char makey_makey_display_buf[5] = {0};
	
	// read block type.
	read_sysex_type(&block_type, &sub_type, OFF_LINE);
	if((block_type == CLASS_CONTROL)&&(sub_type == BLOCK_MAKEY_MAKEY))
    {
         read_next_sysex_data(&data_type, &first_data, OFF_LINE);
         for(int i = 0; i < 4; i++)
        {
            if(((first_data.byte8_val)&(0x01<<i)) == (0x01<<i))
            {
                makey_makey_display_buf[i] = '1';
            }
            else
            {
                makey_makey_display_buf[i] = '0';
            }
        }
        makey_makey_display_buf[4] = '\0';
        NumericDisplayStr(makey_makey_display_buf);
    }
    else
    {
        // read data.
        read_next_sysex_data(&data_type, &first_data, OFF_LINE);
	
        switch(data_type)
        {
            case BYTE_8:
            {
                display_value = (float)first_data.byte8_val;
            }
            break;
            case BYTE_16:
            {
                display_value = (float)(first_data.byte16_val);
            }
            break;
            case SHORT_16:
            {
                display_value = (float)(first_data.short16_val);
            }
            break;
            case SHORT_24:  
            {
                display_value = (float)(first_data.short16_val);
            }
            break;
            case LONG_40:
            {
                display_value = (float)(first_data.long32_val);
            }
            break;
            case FLOAT_40:
            {
                display_value = (float)(first_data.float32_val);
            }
            break;
            default:
            break;
        }
        NumericDisplayFloat(display_value);
    }
}

void TMR1_IRQHandler(void)
{
	  if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
		refresh_display();
    }
}
