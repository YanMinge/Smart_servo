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
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"
#include "MeSoftSerial.h"

/*---------------------------------------------------------------------------------------------------------*/
/* micro defines                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION           003
#define PM_VALUE_MIN               0
#define PM_VALUE_MAX               6000

#define CTL_READ_PM2_5             0x01
#define CTL_READ_PM1_0             0x02
#define CTL_READ_PM10              0x03
#define REPORT_PM2_5               0x01
#define REPORT_PM1_0               0x02
#define REPORT_PM10                0x03
#define CTL_SET_REPORT_MODE        0x7F

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static uint16_t s_pm_value;
static uint16_t s_pre_pm_value;
static uint16_t s_pm2_5_value;
static uint16_t s_pm1_0_value;
static uint16_t s_pm10_value;
static uint32_t s_online_start_mills = 0;
static uint32_t s_offline_start_mills = 0;
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static uint8_t  s_report_mode = REPORT_MODE_DIFF;
static uint8_t  s_report_type = CTL_READ_PM2_5;
static uint8_t  s_pm_package[32] = {0};
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;

/*---------------------------------------------------------------------------------------------------------*/
/* local Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static void parse_pm_package(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(uint8_t type);
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
		// led
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
					send_sensor_report_online(s_report_type);
				}
			}
			else if(s_report_mode == REPORT_MODE_DIFF)
			{
				if(s_pre_pm_value != s_pm_value)
				{
					s_pre_pm_value = s_pm_value;
					send_sensor_report_online(s_report_type);
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
	g_block_sub_type = BLOCK_PM_SENSOR;
    SoftwareSerialBeginInterrupt(9600);
	uart0_recv_attach(sysex_process_online, NULL);
	
	set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
    static uint8_t s_pm_package_start_flag = 0;
    static uint8_t s_pm_package_index = 0;
    static uint16_t s_pm_package_len = 0;
    while(SoftwareSerialReadAvaiable())
    {
        uint8_t temp = SoftwareSerialReadByte();
        
        
        if(s_pm_package_start_flag)
        {
            s_pm_package[s_pm_package_index++] = temp;
            if(s_pm_package_index == 3)
            {
                s_pm_package_len  = temp;
            }
            else if(s_pm_package_index == 4)
            {
                s_pm_package_len = (s_pm_package_len<<8)| temp;
            }
            else if(s_pm_package_index > 4)
            {
                if(s_pm_package_index >= (s_pm_package_len + 4))
                {
                    parse_pm_package();
                    s_pm_package_start_flag = 0;
                    s_pm_package_index  = 0;
                }
            }
            
        }
        else
        {
            if((s_pm_package_index == 0) && (temp == 0x32))
            {
                s_pm_package[s_pm_package_index] = temp;
                s_pm_package_index++;
            }
            else if(s_pm_package_index == 1)
            {
                if(temp == 0x3d)
                {
                    s_pm_package[s_pm_package_index] = temp;
                    s_pm_package_start_flag = 1;
                    s_pm_package_index++;
                }
                else
                {
                    s_pm_package_index = 0;
                }
            }
            
        }
    }
}

void send_sensor_report_online(uint8_t type)
{	
	init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
	add_sysex_data(BYTE_8, &type, ON_LINE);
    switch(type)
    {
        case REPORT_PM2_5:
            add_sysex_data(SHORT_16, (void*)(&s_pm2_5_value), ON_LINE);
        break;
        case REPORT_PM1_0:
            add_sysex_data(SHORT_16, (void*)(&s_pm1_0_value), ON_LINE);
        break;
        case REPORT_PM10:
            add_sysex_data(SHORT_16, (void*)(&s_pm10_value), ON_LINE);
        break;
        default:
        return;
    }
	flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	add_sysex_data(SHORT_16, (void*)(&s_pm2_5_value), OFF_LINE);
	int16_t uniform_value =  line_map(s_pm2_5_value, UNIFORM_MIN, UNIFORM_MAX, PM_VALUE_MIN, PM_VALUE_MAX);
	add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
	flush_sysex_to_send(OFF_LINE);
}

void sysex_process_online(void)
{
	uint8_t block_type = 0;
	uint8_t sub_type = 0;
	uint8_t data_type = 0;
	uint8_t cmd_type = 0;
	uint8_t report_mode = 0;
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
	
	if(CTL_READ_PM2_5== cmd_type)
	{
		send_sensor_report_online(REPORT_PM2_5);
	}
    else if(CTL_READ_PM1_0== cmd_type)
    {
        send_sensor_report_online(REPORT_PM1_0);
    }
    else if(CTL_READ_PM10== cmd_type)
    {
        send_sensor_report_online(REPORT_PM10);
    }
	else if(CTL_SET_REPORT_MODE == cmd_type)
	{
		data_type = BYTE_8;
		if(read_next_sysex_data(&data_type, &report_mode, ON_LINE) == false)
		{
			return;
		}
		if(report_mode == REPORT_MODE_CYCLE)
		{
            data_type = LONG_40;
			if(read_next_sysex_data(&data_type, &s_report_period, ON_LINE) == false)
			{
				return;
			}
            if(s_report_period < 10)
            {
                s_report_period = 10;
            }
            s_report_mode = report_mode;
		}
        else if((report_mode == REPORT_MODE_DIFF) || (report_mode == REPORT_MODE_REQ))
        {
            s_report_mode = report_mode;
        }
	}
	
}

static void parse_pm_package(void)
{
    uint16_t cal_check_sum = 0;
    uint16_t read_check_sum = 0;
    uint8_t i = 0;
    // calculate check_sum, from 0 ~ 30 byte.
    for(i = 0; i < 30; i++)
    {
        cal_check_sum += s_pm_package[i]; 
    }
    read_check_sum = s_pm_package[30];
    read_check_sum = (read_check_sum << 8) | s_pm_package[31];
    if(cal_check_sum != read_check_sum)
    {
        return;
    }
    s_pm2_5_value = s_pm_package[6];
    s_pm2_5_value = (s_pm2_5_value<<8) | s_pm_package[7];
    s_pm1_0_value = s_pm_package[4];
    s_pm1_0_value = (s_pm1_0_value<<8) | s_pm_package[5];
    s_pm10_value = s_pm_package[8];
    s_pm10_value = (s_pm10_value<<8) | s_pm_package[9];
    switch(s_report_type)
    {
        case REPORT_PM2_5:
            s_pm_value = s_pm2_5_value;
        break;
        case REPORT_PM1_0:
            s_pm_value = s_pm1_0_value;
        break;
        case REPORT_PM10:
            s_pm_value = s_pm10_value;
        break;
        default:
        break;
    }
}
