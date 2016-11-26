/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/05/19 11:45a $
 * @brief    for neuron product
 *
 * @note
 * Copyright (C) 2015 myan@makeblock.cc. All rights reserved.
 ******************************************************************************/
#include "sysinit.h"
#include <stdio.h>
#include <string.h>
#include "uart_printf.h"
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "MeColourSensor.h"
#include "block_param.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define CTL_READ_CURRENT_VALUE      0x01
#define REPORT_CURRENT_VALUE        0x01
#define CTL_SET_REPORT_MODE         0x7F
#define SAMPLE_PERIOD               50

/*---------------------------------------------------------------------------------------------------------*/
/* Local variables                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t s_online_start_mills = 0; // used for period reporting sensor value in online mode. 
static uint32_t s_offline_start_mills = 0; // used for period reporting sensonr value in offline mode.
static uint32_t s_sample_start_mills = 0; // used for period sampling.
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static struct colour_sensor_data s_colour_sensor_value, s_pre_colour_sensor_value;
static uint32_t s_report_mode = REPORT_MODE_CYCLE ;

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
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
/*---------------------------------------------------------------------------------------------------------*/
/* Local interface.                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static boolean is_equal(void);

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
	
	init_block( );

    while(1)
    {
		// protocol.
		parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();
		
		// led.
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
				if(is_equal()== false)
				{
					memcpy(&s_pre_colour_sensor_value, &s_colour_sensor_value, sizeof(s_pre_colour_sensor_value));
					
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

static boolean is_equal(void)
{
	if(memcmp(&s_pre_colour_sensor_value, &s_colour_sensor_value, sizeof(s_pre_colour_sensor_value)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void init_block(void)
{
	g_block_type = CLASS_SENSOR;
	g_block_sub_type = BLOCK_COLOUR_SENSOR;
	
	colour_sensor_init();
	uart0_recv_attach(sysex_process_online, NULL);
	
	set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
	uint32_t current_millis = millis();
	if((current_millis - s_sample_start_mills)> SAMPLE_PERIOD)
	{
		s_sample_start_mills = current_millis;
		s_colour_sensor_value = colour_sensor_read();
	}
}

void send_sensor_report_online(void)
{
	//response mesaage to UART0
	uint8_t report_type;
	uint16_t r_value, g_value, b_value;
	report_type = REPORT_CURRENT_VALUE;
	
	r_value = s_colour_sensor_value.r_value;
	g_value = s_colour_sensor_value.g_value;
	b_value = s_colour_sensor_value.b_value;

	init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
	add_sysex_data(BYTE_8, &(report_type), ON_LINE);
	add_sysex_data(SHORT_16, &(r_value), ON_LINE);
	add_sysex_data(SHORT_16, &(g_value), ON_LINE);
	add_sysex_data(SHORT_16, &(b_value), ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	uint16_t r_value, g_value, b_value;
	
	r_value = s_colour_sensor_value.r_value;
	g_value = s_colour_sensor_value.g_value;
	b_value = s_colour_sensor_value.b_value;
	
	add_sysex_data(SHORT_16, &(r_value), OFF_LINE);
	add_sysex_data(SHORT_16, &(g_value), OFF_LINE);
	add_sysex_data(SHORT_16, &(b_value), OFF_LINE);

	int16_t uniform_value = 0;
	add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
	
	flush_sysex_to_send(OFF_LINE);
}

void sysex_process_online(void)
{
	uint8_t block_type = 0;
	uint8_t sub_type = 0;
	uint8_t data_type = 0;
	uint8_t cmd_type = 0;
	
	// read block type.
	read_sysex_type(&block_type , &sub_type, ON_LINE);
	if((block_type != g_block_type)|| (sub_type != g_block_sub_type))
	{
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
		send_sensor_report_online();
	}
	else if(CTL_SET_REPORT_MODE == cmd_type)
	{
		data_type = BYTE_8;
		if(read_next_sysex_data(&data_type, &s_report_mode, ON_LINE) == false)
		{
			return;
		}
		if(s_report_mode == REPORT_MODE_CYCLE)
		{
            data_type = LONG_40;
			if(read_next_sysex_data(&data_type, &s_report_period, ON_LINE))
			{
				return;
			}
		}
	}
}
