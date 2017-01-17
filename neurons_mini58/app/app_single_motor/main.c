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
#include "MeDCMotor.h"
#include "block_param.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION          003
#define CTL_SET_MOTOR_SPEED       0x01

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;
/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/


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
		
		// led
		poll_led_request();
    }
}


void init_block(void)
{
	g_block_type = CLASS_MOTOR;
	g_block_sub_type = BLOCK_SINGLE_DC_MOTOR;
	dc_motor_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
	
	set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type, cmd_type;
	int8_t motor_speed;
	
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
	
	if(cmd_type == CTL_SET_MOTOR_SPEED)
	{
		// read data.
		data_type = BYTE_16;
		if(read_next_sysex_data(&data_type, &motor_speed, ON_LINE) == false)
		{
			return;
		}
	
		// excute.
		dc_run(MOTOR_1, motor_speed);
	}
}

void sysex_process_offline(void)
{
	uint16_t uniform_value = 0;
	uint8_t motor_speed = 0;
	uniform_value = read_uniform_value();
	motor_speed = line_map(uniform_value, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX, UNIFORM_MIN, UNIFORM_MAX);
	dc_run(MOTOR_1, motor_speed);
}
