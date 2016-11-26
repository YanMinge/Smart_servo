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
#include "MeDht11.h"
#include "block_param.h"

#define  TEMPERATURE_MIN     0
#define  TEMPERATURE_MAX     50
#define  HUMIDITY_MIN        20
#define  HUMIDITY_MAX        90

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static unsigned long s_online_start_mills;       // for comparison with currentMillis
static unsigned long s_offline_start_mills;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_temperature;
volatile uint8_t g_humidity;
volatile uint8_t g_pre_temperature;
volatile uint8_t g_pre_humidity;
volatile unsigned long system_time = 0;
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
		if(g_report_mode == REPORT_MODE_CYCLE)
		{	
			if (currentMillis - s_online_start_mills > samplingInterval)
			{
				s_online_start_mills = currentMillis;
				send_sensor_report_online();
			}
		}
		else if(g_report_mode == REPORT_MODE_DIFF)
		{
			if(is_equal()== false)
			{
				g_pre_temperature = g_temperature;
				g_pre_humidity = g_humidity;
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
	g_block_type = TEMPERATURE_HUMIDITY;
	DHT11_Init();
	samplingInterval = TEMPERATURE_HUMIDITY_PERIOD;
	uart0_recv_attach(sysex_process_online, NULL);
}
	
void get_sensor_data(void)
{
	g_temperature = DHT11_ReadTemp();
	g_humidity = DHT11_ReadHumidity();
}

boolean is_equal(void)
{
	if((g_pre_temperature == g_temperature)&&(g_pre_humidity == g_humidity))
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
	
    if(((g_report_mode == REPORT_MODE_DIFF) &&(is_equal() == false)) ||
		(g_report_mode == REPORT_MODE_CYCLE))
	{
		g_pre_temperature = g_temperature;
		g_pre_humidity = g_humidity;
		//response mesaage to UART0
		init_sysex_to_send(g_block_type, 0, ON_LINE);
		add_sysex_data(BYTE_8, (void*)(&g_temperature), ON_LINE);
		add_sysex_data(BYTE_8, (void*)(&g_humidity), ON_LINE);
		flush_sysex_to_send(ON_LINE);
	}
 
}

void send_sensor_report_offline(void)
{
	init_sysex_to_send(g_block_type, 0, OFF_LINE);
	add_sysex_data(BYTE_8, (void*)(&g_temperature), OFF_LINE);
	add_sysex_data(BYTE_8, (void*)(&g_humidity), OFF_LINE);
	int16_t uniform_t =  real_convert_to_uinform(g_temperature, TEMPERATURE_MIN, TEMPERATURE_MAX);
	int16_t uniform_h =  real_convert_to_uinform(g_humidity, HUMIDITY_MIN, HUMIDITY_MAX);
	int16_t uniform_value = (uniform_t + uniform_h) >> 1;
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
	if(block_type != g_block_type)
	{
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	read_next_sysex_data(&data_type, &cmd_type, ON_LINE);
	
	if(CTL_READ_CURRENT_VALUE == cmd_type)
	{
		
		//response mesaage to UART0
		init_sysex_to_send(g_block_type, 0, ON_LINE);
		add_sysex_data(BYTE_8, (void*)(&g_temperature), ON_LINE);
		add_sysex_data(BYTE_8, (void*)(&g_humidity), ON_LINE);
		flush_sysex_to_send(ON_LINE);
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
