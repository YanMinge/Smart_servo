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

#define FIRMWARE_VERSION                 003

#define BUZZER_PIN                       P0_4   
#define BUZZER_MIN                       0
#define BUZZER_MAX                       100
#define DEFAULT_BUZZER_FREQUENCE         2730
#define DEFAULT_BUZZER_DUTY              25

#define CMD_SET_FREQUENCE_DUTY           0x01

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
		
	init_block();

    while(1)
    {
		// protocol.
		parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();	
		
		// led.
		poll_led_request();
    }
}


void init_block(void)
{
	g_block_type = CLASS_AUDIO;
	g_block_sub_type = BLOCK_BUZZER;
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
    set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type, cmd_type;
	uint16_t frequence_value, duty;
	// read block type.
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if((block_type != g_block_type) || (sub_type != g_block_sub_type))
	{
        send_sysex_error_code(WRONG_TYPE);
		return;
	}
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE)== false)
	{
		return;
	}
    if(cmd_type == CMD_SET_FREQUENCE_DUTY)
    {
        // read set data.
        data_type = SHORT_24;
        read_next_sysex_data(&data_type, &frequence_value, ON_LINE);
        data_type = BYTE_8;
        read_next_sysex_data(&data_type, &duty, ON_LINE);
        // excute.
        pwm_write(BUZZER_PIN, frequence_value, duty);
	}  
}

void sysex_process_offline(void)
{
	int16_t uniform_value;
	uniform_value = read_uniform_value();
//	if(uniform_value>0 && uniform_value < 120)
//	{
//		uniform_value = 294;
//	}
//	else if(uniform_value > 150 && uniform_value <270)
//	{
//		uniform_value = 330;
//	}
//	else if(uniform_value > 300 && uniform_value <420)
//	{
//		uniform_value = 350;
//	}
//	else if(uniform_value > 450 && uniform_value <570)
//	{
//		uniform_value = 393;
//	}
//	else if(uniform_value > 600 && uniform_value <720)
//	{
//		uniform_value = 441;
//	}
//	else if(uniform_value > 750 && uniform_value < 870)
//	{
//		uniform_value = 495;
//	}
//	else if(uniform_value > 900 && uniform_value <= 1024)
//	{
//		uniform_value = 556;
//	}
//	else if(uniform_value == 0)
//	{
//		pwm_write(BUZZER_PIN, uniform_value, 0);
//		return;
//	}
//	else
//	{
//		return;
//	}
//	pwm_write(BUZZER_PIN, uniform_value, 25);
//  duty = line_map(uniform_value, 0, 90, UNIFORM_MIN, UNIFORM_MAX);
    if(uniform_value > 1000) // max is 1023, 1000 for unstable block. 
    {
        pwm_write(BUZZER_PIN, DEFAULT_BUZZER_FREQUENCE, DEFAULT_BUZZER_DUTY);
    }
    else
    {
         pwm_write(BUZZER_PIN, DEFAULT_BUZZER_FREQUENCE, 0);
    }
}
