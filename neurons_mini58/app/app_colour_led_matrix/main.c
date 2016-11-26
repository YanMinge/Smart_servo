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
#include "MeColourLedMatrix.h"
#include "string.h"
#include "block_param.h"

#define CMD_DRAW_BIT_MAP 0x01
#define CMD_DRAW_BIT     0x02

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
volatile uint8_t g_start_flag =1;

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void sysex_process_online(void);
void sysex_process_offline(void);
/*---------------------------------------------------------------------------------------------------------*/
/* local Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void led_matrix_poll_led_request(void);

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
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
		// protocol
		parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();	
		// led.
		led_matrix_poll_led_request();
    }
}


void init_block(void)
{
	g_block_type = CLASS_DISPLAY;
	g_block_sub_type = BLOCK_LED_COLOUR_MATRIX_8_8;
	led_colour_matrix_init();
	single_led_set(0, 0 , 0 , 12);
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type, cmd_type;
	uint8_t led_matrix[8];
	int16_t r_value, g_value, b_value;
	int i = 0;
	
	// read block type.
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if((block_type != g_block_type)||(sub_type != g_block_sub_type))
	{
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, (void*)(&cmd_type), ON_LINE) == false)
	{
		return; 
	}
	switch(cmd_type)
	{
		case CMD_DRAW_BIT_MAP:
			// read matrix value.
			data_type = LONG_40;
			for(i = 0; i<2 ; i++)
			{
				if(read_next_sysex_data(&data_type, (void*)(&(led_matrix[4*i])), ON_LINE) == false)
				{
					return;
				}
			}
			// read set red colour value.
			data_type = SHORT_16;
			if(read_next_sysex_data(&data_type, (void*)(&r_value), ON_LINE) == false)
			{
				return;
			}
			if(read_next_sysex_data(&data_type, (void*)(&g_value), ON_LINE) == false)
			{
				return;
			}
			if(read_next_sysex_data(&data_type, (void*)(&b_value), ON_LINE) == false)
			{
				return;
			}
			MeColourLEDMatrix_DrawBitMap(led_matrix, r_value, g_value, b_value);
		break;
		case CMD_DRAW_BIT:
			data_type = BYTE_8;
			uint8_t led_index;
			if(read_next_sysex_data(&data_type, (void*)(&led_index), ON_LINE) == false)
			{
				return;
			}
			data_type = SHORT_16;
			if(read_next_sysex_data(&data_type, (void*)(&r_value), ON_LINE) == false)
			{
				return;
			}
			if(read_next_sysex_data(&data_type, (void*)(&g_value), ON_LINE) == false)
			{
				return;
			}
			if(read_next_sysex_data(&data_type, (void*)(&b_value), ON_LINE) == false)
			{
				return;
			}
			single_led_set(led_index, r_value, g_value, b_value);
		default:
		break;
	}
}

void sysex_process_offline(void)
{ 
	if(g_start_flag == 1) // when start, the led show the start singal, not response to offline package.
	{
		return;
	}
	
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
	
	single_led_set(ALL_LED, r_value, g_value, b_value);
}

void led_matrix_poll_led_request(void)
{
	// blink the led.
	
	if(g_start_flag)
	{
		static uint32_t previous_time = 0;
		static uint8_t  s_count = 0; // blink times.
		static boolean s_blink_flag = 0;
		
		if(s_count < 3)
		{
			uint32_t now_time = millis();
			if(now_time - previous_time > 500) //blink
			{
				if(s_blink_flag == 0) // led off, led common anode.
				{
					single_led_set(0, 0 , 0 ,0);
					s_blink_flag = 1;
					s_count++; // blink one time.
				}
				else // led on.
				{
					single_led_set(0, 0 , 0 , 12);
					s_blink_flag = 0;
				}
				previous_time = now_time;
			}
		}
		else
		{
			g_start_flag = 0;
			s_count = 0;
		}
	}
}
