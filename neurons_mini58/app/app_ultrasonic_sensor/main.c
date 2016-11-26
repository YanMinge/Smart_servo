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
#include "MeUltrasonicSensor.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define  DISTANCE_MIN     		    2
#define  DISTANCE_MAX     		    400
#define  SAMPLE_INTERVAL	  	    50

#define  CTL_READ_CURRENT_VALUE     0x01
#define  REPORT_CURRENT_VALUE       0x01
#define  CTL_SET_REPORT_MODE        0x7F

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t s_online_start_mills = 0;
static uint32_t s_offline_start_mills = 0;
static uint32_t s_sample_start_millis = 0;
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static uint8_t  s_report_mode = REPORT_MODE_CYCLE;
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile float g_measure_distance;
volatile float g_pre_measure_distance;
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
				if (currentMillis - s_online_start_mills > s_report_period)
				{
					s_online_start_mills = currentMillis;
					send_sensor_report_online();
				}
				else if(s_report_mode == REPORT_MODE_DIFF)
				{
					if(abs(g_measure_distance - g_pre_measure_distance) > 0.1)
					{
						g_pre_measure_distance = g_measure_distance;
						send_sensor_report_online();
					}
				}
					
			}
		}
		
		// off line.
		else if(g_block_no == 0)
		{
			
			if (currentMillis - s_offline_start_mills > OFF_LINE_REPORT_PERIOD)
			{
				s_offline_start_mills = currentMillis;
				
				if(abs(g_measure_distance - g_pre_measure_distance) < 0.1)
				{
					g_measure_distance = g_pre_measure_distance;
				}
				g_pre_measure_distance = g_measure_distance;
				send_sensor_report_offline();
			}
		}
    }
}


void init_block(void)
{
	g_block_type = CLASS_SENSOR;
	g_block_sub_type = BLOCK_ULTRASONIC_SENSOR;
	ultrasonicSensorInit();
	uart0_recv_attach(sysex_process_online, NULL);
	
	set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
	uint32_t current_millis = millis();
	if((current_millis - s_sample_start_millis) > SAMPLE_INTERVAL)
	{
		s_sample_start_millis = current_millis;
		ultrasonic_measure_start();
	}
	
	if(poll_ultrasonic_state() == ULTRANSONIC_OVER)
	{
		g_measure_distance = ultrasonic_get_distance();
 	}
}

void send_sensor_report_online(void)
{
	uint8_t report_type = REPORT_CURRENT_VALUE;
	//response mesaage to UART0
	init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
	add_sysex_data(BYTE_8, &report_type, ON_LINE);
	add_sysex_data(FLOAT_40, (void*)(&g_measure_distance), ON_LINE);
	flush_sysex_to_send(ON_LINE); 
}

void send_sensor_report_offline(void)
{
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	add_sysex_data(FLOAT_40, (void*)(&g_measure_distance), OFF_LINE);
	int32_t temp_value = (int32_t)g_measure_distance;
	int16_t uniform_value =  real_convert_to_uinform(g_measure_distance, DISTANCE_MIN, DISTANCE_MAX);
	add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
	flush_sysex_to_send(OFF_LINE);
}

void sysex_process_online(void)
{
	uint8_t block_type = 0;
	uint8_t sub_type = 0;
	uint8_t data_type = 0;
	uint8_t cmd_type = 0;
	
	// read type.
	read_sysex_type(&block_type , &sub_type, ON_LINE);
	if((block_type != CLASS_SENSOR)||(sub_type != BLOCK_ULTRASONIC_SENSOR))
	{
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE)== false)
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
			if(read_next_sysex_data(&data_type, &s_report_period, ON_LINE) == false)
			{
				return;
			}
		}
	}
}
