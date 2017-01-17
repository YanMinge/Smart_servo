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
#include "acmp.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION         003
#define THRESHOLD_VALUE          650
#define FILTER_TIMES             50
#define LINE_DETECT_1_PIN        P1_0
#define LINE_DETECT_2_PIN        P1_5
#define LINE_DETECT_VALUE_MIN    0
#define LINE_DETECT_VALUE_MAX    3

#define CTL_READ_CURRENT_VALUE   0x01
#define REPORT_CURRENT_VALUE     0x01
#define CTL_SET_REPORT_MODE      0x7f

/*---------------------------------------------------------------------------------------------------------*/
/* local variable                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t s_online_start_mills = 0;
static uint32_t s_offline_start_mills = 0;
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static uint16_t s_detect_value;
static uint16_t s_pre_detect_value;
static uint8_t  s_before_filter_value = 0;
static uint8_t  s_filter_count = 0;
static uint8_t  s_report_mode = REPORT_MODE_CYCLE;

/*---------------------------------------------------------------------------------------------------------*/
/* global variable                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_message_process(void);
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
		
	init_block( );

    while(1)
    {
		parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();
		
		poll_led_request();
		
        get_sensor_data();
		
		uint32_t currentMillis = millis();
		// on line.
		if(g_block_no != 0) 
		{
			if(s_report_mode == REPORT_MODE_CYCLE)
			{	
				if ((currentMillis - s_online_start_mills) > s_report_period)
				{
					s_online_start_mills = currentMillis;
					send_sensor_report_online();
				}
			}
			else if(s_report_mode == REPORT_MODE_DIFF)
			{
				if(s_detect_value != s_pre_detect_value)
				{
					s_pre_detect_value = s_detect_value;
					send_sensor_report_online();
				}
			}			
		}
		
		// off line.
		else if(g_block_no == 0)
		{
			if (currentMillis - s_offline_start_mills > OFF_LINE_REPORT_PERIOD)
			{
				s_offline_start_mills = currentMillis;
				send_sensor_report_offline();
			}
		}
    }
}


void init_block(void)
{
	g_block_type = CLASS_SENSOR;
	g_block_sub_type = BLOCK_LINE_FOLLOWER;
	
	uart0_recv_attach(sysex_message_process, NULL);
	
	set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
	uint16_t value_line_detect_1 = 0;
	uint16_t value_line_detect_2 = 0;
	uint8_t detect_value  = 0;
	value_line_detect_1 = analogRead(LINE_DETECT_1_PIN);
	value_line_detect_2	= analogRead(LINE_DETECT_2_PIN);
	// compare
	if(value_line_detect_1 > THRESHOLD_VALUE)
	{
		detect_value |= 0x01;	
	}
	else
	{
		detect_value &= 0xfe;
	}
	if(value_line_detect_2 > THRESHOLD_VALUE)
	{
		detect_value |= 0x02;
	}
	else
	{
		detect_value &= 0xfd;	
	}
	
	//fiter.
	if(s_filter_count == 0)
	{
		s_before_filter_value = detect_value;
	}
	if(s_before_filter_value == detect_value)
	{
		s_filter_count++;
	}
	else
	{
		s_filter_count = 0;
	}
	if(s_filter_count>FILTER_TIMES)
	{
		s_detect_value = s_before_filter_value;
		s_filter_count = 0;
	}
}

void send_sensor_report_online(void)
{
	uint8_t report_type = REPORT_CURRENT_VALUE; 
	init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
	add_sysex_data(BYTE_8, (void*)(&report_type), ON_LINE);
	add_sysex_data(BYTE_8, (void*)(&s_detect_value), ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	add_sysex_data(BYTE_8, (void*)(&s_detect_value), OFF_LINE);
	int16_t uniform_value = line_map(s_detect_value, UNIFORM_MIN, UNIFORM_MAX, LINE_DETECT_VALUE_MIN, LINE_DETECT_VALUE_MAX);
	add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
	flush_sysex_to_send(OFF_LINE);
}

void sysex_message_process(void)
{
	uint8_t block_type = 0;
	uint8_t sub_type = 0;
	uint8_t data_type = 0;
	uint8_t cmd_type = 0;
	
	// read type.
	read_sysex_type(&block_type , &sub_type, ON_LINE);
	if((block_type != g_block_type)||(sub_type != g_block_sub_type))
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
	if(CTL_READ_CURRENT_VALUE == cmd_type)
	{		
		//response mesaage to UART0
		send_sensor_report_online();
	}
	else if(CTL_SET_REPORT_MODE == cmd_type)
	{
		data_type = BYTE_8;
		if(read_next_sysex_data(&data_type, &s_report_mode, ON_LINE) == false) // read report mode.
		{
			return;
		}
		if(s_report_mode == REPORT_MODE_CYCLE)
		{
			data_type = LONG_40;
			if(read_next_sysex_data(&data_type, &s_report_period, ON_LINE) == false) // read report period.
			{
				return;
			}
            if(s_report_period < 10)
            {
                s_report_period = 10;
            }
		}
	}
}
