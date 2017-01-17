/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 16/11/14 15:21a $
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

#define  FIRMWARE_VERSION        003   
#define  CTL_READ_CURRENT_VALUE  0x01
#define  REPORT_CURRENT_VALUE    0x01
#define  CTL_SET_REPORT_MODE     0x7f

#define  NET_TONUCH1_PIN P0_6
#define  NET_TONUCH2_PIN P0_7
#define  NET_TONUCH3_PIN P2_6
#define  NET_TONUCH4_PIN P2_5

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

static uint32_t s_online_start_mills = 0;
static uint32_t s_offline_start_mills = 0;
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;  
static uint8_t  s_report_mode = REPORT_MODE_CYCLE;
unsigned long key_debounced_time;
int key_debounced_count = 0;
int key_match_count = 0;
int key_debounced_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

volatile boolean g_logic_value;
volatile boolean g_pre_logic_value;
uint16_t g_firmware_version = FIRMWARE_VERSION;
volatile unsigned long system_time = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

int32_t main(void);
void init_block(void);
void get_sensor_data(void);
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
                if((currentMillis - s_online_start_mills) > s_report_period)
                {
                    s_online_start_mills = currentMillis;
                    send_digital_signal_to_host(g_block_type, g_block_sub_type, REPORT_CURRENT_VALUE, g_logic_value);
                }
            }
            else if(s_report_mode == REPORT_MODE_DIFF)
            {
                if(g_logic_value != g_pre_logic_value)
                {
                    g_pre_logic_value = g_logic_value;
                    send_digital_signal_to_host(g_block_type, g_block_sub_type, REPORT_CURRENT_VALUE, g_logic_value);
                }
            }  			
        }
				
        // off line.
        else if(g_block_no == 0)
        {
            if(currentMillis - s_offline_start_mills > OFF_LINE_REPORT_PERIOD)
            {
                s_offline_start_mills = currentMillis;
                send_digital_signal_to_block(g_block_type, g_block_sub_type, g_logic_value);
            }
        }
    }
}

void init_block()
{  	         
    g_block_type = CLASS_CONTROL;
    g_block_sub_type = BLOCK_MAKEY_MAKEY;

    pinMode(NET_TONUCH1_PIN,GPIO_MODE_INPUT);  
    pinMode(NET_TONUCH2_PIN,GPIO_MODE_INPUT);  
    pinMode(NET_TONUCH3_PIN,GPIO_MODE_INPUT);  
    pinMode(NET_TONUCH4_PIN,GPIO_MODE_INPUT);  
	
    uart0_recv_attach(sysex_process_online, NULL);
    
    set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{ 
    unsigned long current_time;
    int key_temp_value;
	
    int makey_temp_value_1; 
    int makey_temp_value_2; 
    int makey_temp_value_3; 
    int makey_temp_value_4; 
	
    current_time = millis();
	
    if(current_time - key_debounced_time > 3)
    {
        key_debounced_time = current_time;
			
        makey_temp_value_1 = digitalRead(NET_TONUCH1_PIN);     
        makey_temp_value_2 = digitalRead(NET_TONUCH2_PIN)<<1;  
        makey_temp_value_3 = digitalRead(NET_TONUCH3_PIN)<<2;  
        makey_temp_value_4 = digitalRead(NET_TONUCH4_PIN)<<3;  
			 
        key_temp_value =  ((makey_temp_value_1&0x0001)|(makey_temp_value_2&0x0002)|(makey_temp_value_3&0x0004)|(makey_temp_value_4&0x0008));
			
        if(key_debounced_count == 0)
        {
            key_debounced_value = key_temp_value;
        }
        if(key_temp_value == key_debounced_value)
        {
            key_match_count++;
        }
        key_debounced_count ++;
    }

    if(key_debounced_count == 10)
    {
        key_debounced_count = 0;
        if(key_match_count > 4)
        {  
            g_logic_value = key_debounced_value;					
        }
    }
}

void sysex_process_online(void)
{
    uint8_t block_type = 0;
    uint8_t sub_type = 0;
    uint8_t data_type = 0;
    uint8_t cmd_type = 0;
    
    // read type.
    read_sysex_type(&block_type , &sub_type, ON_LINE);
    if((block_type != g_block_type) || (sub_type != g_block_sub_type))
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
        send_digital_signal_to_host(g_block_type, g_block_sub_type, REPORT_CURRENT_VALUE, g_logic_value);
    }
    else if(CTL_SET_REPORT_MODE == cmd_type)
    {
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type,&s_report_mode, ON_LINE) == false)
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
            if(s_report_period < 10)
            {     
                s_report_period = 10;
            }
        }
    }
}
