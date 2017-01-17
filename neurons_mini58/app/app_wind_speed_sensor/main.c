/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 16/11/23 15:54a $
 * @brief    for neuron product
 *
 * @note
 * Copyright (C) 2016 myan@makeblock.cc. All rights reserved.
 *
 ******************************************************************************/
 
#include "sysinit.h"
#include <stdio.h>
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Micro defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

#define  FIRMWARE_VERSION           003

#define  CTL_READ_CURRENT_VALUE     0x01
#define  REPORT_CURRENT_VALUE       0x01
#define  CTL_SET_REPORT_MODE        0x7F

#define  SAMPLE_PERIOD              0x01
#define  FILTER_TIMES               64

#define  SENSOR_VALUE_MIN           0
#define  SENSOR_VALUE_MAX           45

#define  ANALOG_PIN  P3_1

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

static uint32_t s_online_start_mills;       // for comparison with currentMillis
static uint32_t s_offline_start_mills;
static uint32_t s_sample_start_mills = 0;   // sample start millis.
static uint8_t  s_report_mode = REPORT_MODE_DIFF ;
static uint8_t  s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

volatile uint8_t g_wind_speed_value=0;
volatile uint8_t g_pre_wind_speed_value=0;
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

int32_t main(void);
boolean is_equal(void);
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
                    g_pre_wind_speed_value = g_wind_speed_value;
                    send_sensor_report_online(); 
                }
            }			
        }
        // off line.
        else if(g_block_no == 0)
        {
            if(currentMillis - s_offline_start_mills > OFF_LINE_REPORT_PERIOD)
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
    g_block_sub_type = BLOCK_WIND_SPEED_SENSOR;
        
    uart0_recv_attach(sysex_process_online, NULL);
        
    set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
    static uint8_t s_filter_count = 0;
    static uint32_t s_sum_value = 0;
    volatile uint16_t g_analog_value=0;
    volatile float wind_speed_volt_value=0;
    
    uint32_t current_millis = millis();
    if((current_millis - s_sample_start_mills)> SAMPLE_PERIOD)
    {
        s_sample_start_mills = current_millis;
                              
        s_sum_value += analogRead(ANALOG_PIN);    
        s_filter_count++;
        
        if(s_filter_count >= FILTER_TIMES)
        {
            s_filter_count = 0;               
            g_analog_value = s_sum_value >> 6;
            s_sum_value = 0;
            
            wind_speed_volt_value=(float)g_analog_value*33/10240;
            
            if(wind_speed_volt_value<=0.662)
            {
                g_wind_speed_value=0;
            }
            else if(wind_speed_volt_value>=3.30)
            {
                g_wind_speed_value=45;
            }
            else if((wind_speed_volt_value>0.662)&&(wind_speed_volt_value<3.30))
            {             
                g_wind_speed_value=17*wind_speed_volt_value-11.25;
                if(g_wind_speed_value>=45)
                {
                    g_wind_speed_value=45;
                }
            } 
        }
    }
}

boolean is_equal(void)
{
	if(g_pre_wind_speed_value == g_wind_speed_value)
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
    add_sysex_data(BYTE_8, (void*)(&g_wind_speed_value), ON_LINE); 
  
    flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{       
     uint16_t uniform_value;
    
    init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
    add_sysex_data(BYTE_8, (void*)(&g_wind_speed_value), OFF_LINE);
    uniform_value = line_map(g_wind_speed_value, UNIFORM_MIN, UNIFORM_MAX, SENSOR_VALUE_MIN, SENSOR_VALUE_MAX);
    add_sysex_data(UNIFORM_16,(void*)&uniform_value, OFF_LINE);
    
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

