/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/05/19 11:45a $
 * @brief    for neuron product
 *
 * @note
 * Copyright (C) makeblock.cc. All rights reserved.
 *
 ******************************************************************************/
#include "sysinit.h"
#include <stdio.h>
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"
#include "MeCoolLight.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Micro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION            003
#define CTL_SET_COOL_LIGHT          0x01
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint8_t s_start_flag = 1;

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
void sysex_process_online(void);
void sysex_process_offline(void);
void cool_light_start_run(void);
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
         // protocol.
        parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();	
        // led
        poll_led_request();
        cool_light_start_run();
    }
}


void init_block(void)
{
    g_block_type = CLASS_DISPLAY;
    g_block_sub_type = BLOCK_COOL_LIGHT;
    cool_light_init();
    uart0_recv_attach(sysex_process_online, sysex_process_offline);
    set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
    uint8_t block_type, sub_type, data_type, cmd_type;
    uint8_t cool_light_value = 0;
	
    // read block type.
    read_sysex_type(&block_type, &sub_type, ON_LINE);
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
    // excute command.
    if(cmd_type == CTL_SET_COOL_LIGHT)
    {
        // read data.
        data_type = BYTE_8;
        read_next_sysex_data(&data_type, &cool_light_value, ON_LINE);
        // excute.
        if((cool_light_value& 0x01) == 0x01)
        {
            cool_light_set(1, 1);
        }
        else if((cool_light_value& 0x01) == 0)
        {
            cool_light_set(1, 0);
        }
        
        if((cool_light_value& 0x02) == 0x02)
        {
            cool_light_set(2, 1);
        }
        else if((cool_light_value& 0x02) == 0)
        {
            cool_light_set(2, 0);
        }
        
         if((cool_light_value& 0x04) == 0x04)
        {
            cool_light_set(3, 1);
        }
        else if((cool_light_value& 0x04) == 0)
        {
            cool_light_set(3, 0);
        }
        
        if((cool_light_value& 0x08) == 0x08)
        {
            cool_light_set(4, 1);
        }
        else if((cool_light_value& 0x08) == 0)
        {
            cool_light_set(4, 0);
        }
    }
}

void sysex_process_offline(void)
{
    int16_t uniform_value = 0;
    uint8_t block_type = 0;
    uint8_t sub_type = 0;
    
    if(s_start_flag) // // when start, the led show the start singal, not response to offline package.
    {
        return;
    }
    
    read_sysex_type(&block_type, &sub_type, OFF_LINE);
    uniform_value = read_uniform_value();

    if(uniform_value >= 1000)
    {
        cool_light_set(1, 1);
        cool_light_set(2, 1);
        cool_light_set(3, 1);
        cool_light_set(4, 1);
    }
    else
    {
        cool_light_set(1, 0);
        cool_light_set(2, 0);
        cool_light_set(3, 0);
        cool_light_set(4, 0);
    }
}



void cool_light_start_run(void)
{
    static uint32_t previous_time = 0;
    static uint8_t  s_count = 0; // blink times.
    static boolean s_blink_flag = 0;
	
    // blink the led. 
    if(s_start_flag)
    {
        if(s_count < 3)
        {
            uint32_t now_time = millis();
            if(now_time - previous_time > 500) //blink
            {
                
                previous_time = now_time;
                if(s_blink_flag == 0) // led off, led common anode.
                {
                    cool_light_set(1, 1);
                    cool_light_set(2, 1);
                    cool_light_set(3, 1);
                    cool_light_set(4, 1);
                    s_blink_flag = 1;
                }
                else // led on.
                {
                    cool_light_set(1, 0);
                    cool_light_set(2, 0);
                    cool_light_set(3, 0);
                    cool_light_set(4, 0);
                    s_count++; // blink one time.
                    s_blink_flag = 0;
                }
            }
        }
        else
        {
            s_start_flag = 0;
            s_count = 0;
        }
    }
}
