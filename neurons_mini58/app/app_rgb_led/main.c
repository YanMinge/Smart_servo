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
#include "MeRgbLed.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Micro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION       003
#define CTL_SET_RGB_VALUE      0x01

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile static uint8_t s_r_value;
volatile static uint8_t s_g_value;
volatile static uint8_t s_b_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);

void init_block(void);
void get_sensor_data(void);
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
	
	init_block();

    while(1)
    {
		// protocol.
		parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();
		
		// led.
		poll_led_request();
    }
}


void init_block(void)
{
	g_block_type = CLASS_DISPLAY;
	g_block_sub_type = BLOCK_SINGLE_COLOUR_LED;
	rgb_led_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
	
	set_rgb_led(0, 0, 128); // for the indicate led.
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, cmd_type, data_type;
	int16_t r_value, g_value, b_value;
	
	// read block type.
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if(block_type != g_block_type)
	{
        send_sysex_error_code(WRONG_TYPE);
		return;
	}
	
	// read cmd_type;
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE) == false)
	{
		return;
	}
	
	if(cmd_type == CTL_SET_RGB_VALUE)
	{
		// read led set value.
		data_type = SHORT_16;
		if(read_next_sysex_data(&data_type, &r_value, ON_LINE) == false)
		{
			return;
		}
		data_type = SHORT_16;
		if(read_next_sysex_data(&data_type, &g_value, ON_LINE) == false)
		{
			return;
		}
		data_type = SHORT_16;
		if(read_next_sysex_data(&data_type, &b_value, ON_LINE) == false)
		{
			return;
		}
	
		// get ride of repeated setting value.
		if((s_r_value == r_value)&&(s_g_value == g_value)&&(s_b_value == b_value))
		{
			return;
		}
		else
		{
			s_r_value = r_value;
			s_g_value = g_value;
			s_b_value = b_value;
		}
	}
    rgb_led_set(s_r_value, s_g_value, s_b_value);
}

void sysex_process_offline(void)
{
	uint8_t r_value, b_value, g_value, block_type, sub_block_type, data_type;
	int16_t colour_sensor_r, colour_sensor_g, colour_sensor_b;
	uint16_t uniform_value;
	
	read_sysex_type(&block_type, &sub_block_type, OFF_LINE);
	
	if((block_type == CLASS_SENSOR)&&(sub_block_type == BLOCK_COLOUR_SENSOR))
	{
		if(read_next_sysex_data(&data_type, &colour_sensor_r, OFF_LINE) == false)
		{
			return;
		}
		if(read_next_sysex_data(&data_type, &colour_sensor_g, OFF_LINE) == false)
		{
			return;
		}
		if(read_next_sysex_data(&data_type, &colour_sensor_b, OFF_LINE) == false)
		{
			return;
		}
		r_value = colour_sensor_r;
		g_value = colour_sensor_g;
		b_value = colour_sensor_b;		
	}
	else
	{
		uniform_value = read_uniform_value();
		uint8_t temp_value = line_map(uniform_value, 0, 255, UNIFORM_MIN, UNIFORM_MAX);
	
		r_value = temp_value;
		g_value = temp_value;
		b_value = temp_value;
	}

	// get ride of repeated setting value.
	if((s_r_value == r_value)&&(s_g_value == g_value)&&(s_b_value == b_value))
	{
		return;
	}
	else
	{
		s_r_value = r_value;
		s_g_value = g_value;
		s_b_value = b_value;
	}
	rgb_led_set(s_r_value, s_g_value, s_b_value);
}
