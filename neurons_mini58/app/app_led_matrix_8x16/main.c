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
#include "MeLedMatrix8_16.h"
#include "string.h"

//CMD for led matrix 8x16.
#define CMD_DRAW_BIT_MAP 0x01
#define CMD_DRAW_STRING  0x02
#define CMD_DRAW_TIME	 0x03

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile static uint8_t s_cmd_type = CMD_DRAW_STRING;
volatile static uint32_t s_pre_time = 0;
volatile static int16_t s_x_position_show = 0;
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
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
		poll_led_request();
		
		if(s_cmd_type == CMD_DRAW_STRING)
		{
			uint32_t current_time = millis();
			if((current_time - s_pre_time) > 200)
			{
				s_pre_time = current_time;
				MeLEDMatrix_DrawStr(s_x_position_show--, 7, i8_Str_Display_Buffer);
				if(s_x_position_show <= -strlen(i8_Str_Display_Buffer)*6)
				{
					s_x_position_show = 0;
				}
			}
		}
    }
}


void init_block(void)
{
	g_block_type = LED_MATRIX_8_16;
	led_matrix8_16_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type, array_type;
	uint8_t led_matrix[16];
	char string[STRING_DISPLAY_BUFFER_SIZE];
	uint16_t string_len = 0;
	uint8_t hour, minute;
	int i = 0;
	
	// read block type.
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if(block_type != g_block_type)
	{
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, (void*)(&s_cmd_type), ON_LINE) == false)
	{
		return; 
	}
	switch(s_cmd_type)
	{
		case CMD_DRAW_BIT_MAP:
			// read set value.
			data_type = LONG_40;
			for(i = 0; i<4 ; i++)
			{
				if(read_next_sysex_data(&data_type, (void*)(&(led_matrix[4*i])), ON_LINE) == false)
				{
					return;
				}
			}
			
			MeLEDMatrix_DrawBitMap(led_matrix);
		break;
		case CMD_DRAW_STRING:
			s_x_position_show = 0;
		
			data_type = BYTE_8;
			if(read_next_sysex_data(&data_type, (void*)(&array_type), ON_LINE) == false)
			{
				return;
			}
			data_type = SHORT_24;
			if(read_next_sysex_data(&data_type, (void*)(&string_len), ON_LINE) == false)
			{
				return;
			}
			if(string_len > STRING_DISPLAY_BUFFER_SIZE -2)
			{
				string_len = STRING_DISPLAY_BUFFER_SIZE - 2;
			}
			
			data_type = BYTE_8;
			for(i = 0; i < string_len; i++)
			{
				if(read_next_sysex_data(&data_type, (void*)(&(string[i])), ON_LINE) == false)
				{
					return;
				}
			}
			string[string_len + 1] = '\0';
			MeLEDMatrix_DrawStr(s_x_position_show--, 7, string);
		break;
		case CMD_DRAW_TIME:
			data_type = BYTE_8;
			if(read_next_sysex_data(&data_type, (void*)(&hour), ON_LINE) == false)
			{
				return;
			}
			if(read_next_sysex_data(&data_type, (void*)(&minute), ON_LINE) == false)
			{
				return;
			}
			MeLEDMatrix_DrawTime(hour, minute, 1);
		break;
		default:
		break;
	}
}

void sysex_process_offline(void)
{
	
}
