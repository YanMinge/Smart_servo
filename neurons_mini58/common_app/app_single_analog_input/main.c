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

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define FORCE_SENSOR_BASE           180 // no pression value.
#define CTL_READ_CURRENT_VALUE      0x01
#define REPORT_CURRENT_VALUE        0x01
#define CTL_SET_REPORT_MODE         0x7f

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t s_online_start_mills = 0; // on line report start millis.
static uint32_t s_offline_start_mills = 0; // off line report start millis.
static uint32_t s_report_period  = DEFAULT_REPORT_PERIOD_ON_LINE; 
static uint8_t s_report_mode = REPORT_MODE_CYCLE;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint16_t g_analog_value;
volatile uint16_t g_pre_analog_value;
volatile uint16_t g_pre_report_value;
volatile unsigned long system_time = 0;

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
		
        uint32_t current_millis = millis();
        // on line.
		if(g_block_no != 0) 
		{
			if(s_report_mode == REPORT_MODE_CYCLE)
			{
				if ((current_millis - s_online_start_mills) > s_report_period)
				{
					s_online_start_mills = current_millis;
					send_sensor_report_online();
				}
			}
			else if(s_report_mode == REPORT_MODE_DIFF)
			{
				if(g_analog_value != g_pre_analog_value)
				{
					g_pre_analog_value = g_analog_value;
					send_sensor_report_online();
				}	
			}
		}
		
		// off line.
		else if(g_block_no == 0)
		{
			if ((current_millis - s_offline_start_mills) > OFF_LINE_REPORT_PERIOD)
			{
				s_offline_start_mills = current_millis;
				send_sensor_report_offline();
			}
		}
    }
}


void init_block(void)
{
	
#if defined D_LIGHT_SNESOR
	g_block_type = CLASS_SENSOR;
	g_block_sub_type = BLOCK_LIGHT_SENSOR;
#elif defined D_VOLUME_SENSOR
	g_block_type = VOLUME_SENSOR;
#elif defined D_POTENTIOMETER
	g_block_type = CLASS_CONTROL;
	g_block_sub_type = BLOCK_POTENTIOMETER;
#else
	#error you should add a device type in preprocessor symbols.
#endif
	
	uart0_recv_attach(sysex_message_process, NULL);
	
	set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
	static uint8_t s_filter_count = 0;
	static uint32_t s_sum_value = 0;
	uint8_t analog_pin;
#if defined D_LIGHT_SNESOR
	analog_pin = P5_3;
	s_sum_value += analogRead(analog_pin);
#elif defined D_VOLUME_SENSOR
	analog_pin = P1_0;
	s_sum_value += analogRead(analog_pin);
#elif defined D_POTENTIOMETER
	analog_pin = P5_3;
	s_sum_value += analogRead(analog_pin);
	// anti shake.
#elif defined D_FORCE_SENSOR
	analog_pin = P5_3;
	s_sum_value += analogRead(analog_pin);
#else
	#error you should add a device type in preprocessor symbols.
#endif
	s_filter_count++;
	// average value in 8 times samples.
	if(s_filter_count >= 128)
	{
		s_filter_count = 0;
		g_analog_value = s_sum_value >> 7;
		s_sum_value = 0;
		
#if defined D_FORCE_SENSOR
		int16_t temp_value = UNIFORM_MAX - g_analog_value- FORCE_SENSOR_BASE; 
		if(temp_value < 20)
		{
			temp_value = 0;
		}
		g_analog_value = temp_value;
		pwm_write(P22, 1000,  g_analog_value >> 4); // driver the led.
#endif
	}
}

void send_sensor_report_online(void)
{
	send_analog_signal_to_host(g_block_type, g_block_sub_type, REPORT_CURRENT_VALUE, g_analog_value);
}

void send_sensor_report_offline(void)
{
	send_analog_signal_to_block(g_block_type, g_block_sub_type, g_analog_value);
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
		send_analog_signal_to_host(g_block_type, g_block_sub_type, REPORT_CURRENT_VALUE, g_analog_value);
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
			if(read_next_sysex_data(&data_type, &s_report_period, ON_LINE) == false)
			{
				return;
			}
		}
	}
}
