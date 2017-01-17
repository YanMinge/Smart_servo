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
#include "MeSht2x.h"
#include "block_param.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Micro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define  FIRMWARE_VERSION           003
#define  TEMPERATURE_MIN            -40
#define  TEMPERATURE_MAX            125
#define  HUMIDITY_MIN               0
#define  HUMIDITY_MAX               100
#define  CTL_READ_CURRENT_VALUE     0x01
#define  CTL_SET_REPORT_MODE        0x7f
#define  REPORT_CURRENT_VALUE       0x01
#define  SAMPLE_PERIOD              1000

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t s_online_start_mills;       // for comparison with currentMillis
static uint32_t s_offline_start_mills;
static uint32_t s_sample_start_mills;
static int8_t   s_temperature;
static uint8_t  s_humidity;
static int8_t   s_pre_temperature;
static uint8_t  s_pre_humidity;
static uint8_t  s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static uint8_t  s_report_mode = REPORT_MODE_DIFF ;
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
boolean is_equal(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void task_receive_data_process(void);
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
	
	TMR1_Init(2000);
	
	init_block();

    while(1)
    {	
		// led.
		poll_led_request();
		
        get_sensor_data();     
    }
}

void task_receive_data_process(void)
{
	parse_uart0_recv_buffer();
	flush_uart1_to_uart0();
	flush_uart0_local_buffer();
	
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
		}
		else if(s_report_mode == REPORT_MODE_DIFF)
		{
			if(is_equal()== false)
			{
				s_pre_temperature = s_temperature;
				s_pre_humidity = s_humidity;
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

void init_block(void)
{
	g_block_type = CLASS_SENSOR;
	g_block_sub_type = BLOCK_TEMPERATURE_HUMIDITY;
	SHT2X_Init();
	uart0_recv_attach(sysex_process_online, NULL);
    
    set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
    uint32_t current_millis = millis();
	if((current_millis - s_sample_start_mills)> SAMPLE_PERIOD)
	{
        s_sample_start_mills = current_millis;
        s_temperature = (int16_t)readT();
        s_humidity = (int16_t)readRH();
    }
}

boolean is_equal(void)
{
	if((s_pre_temperature == s_temperature)&&(s_pre_humidity == s_humidity))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void send_sensor_report_online(void)
{
    uint8_t report_type;
    report_type = REPORT_CURRENT_VALUE;
    
    init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
    add_sysex_data(BYTE_8, (void*)(&report_type), ON_LINE);
    add_sysex_data(BYTE_16, (void*)(&s_temperature), ON_LINE);
    add_sysex_data(BYTE_8, (void*)(&s_humidity), ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	add_sysex_data(BYTE_16, (void*)(&s_temperature), OFF_LINE);
	add_sysex_data(BYTE_8, (void*)(&s_humidity), OFF_LINE);
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
            if(s_report_period < 10)
            {
                s_report_period = 10;
            }
		}
	}
}

void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
		task_receive_data_process();
    }
}
