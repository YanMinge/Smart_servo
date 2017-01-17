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

#define FIRMWARE_VERSION    003 // for firmware version.

#define SERVO_POSITION_MIN  0
#define SERVO_POSITION_MAX  180
#define PWM_PIN             P0_4
#define PWM_FREQUENCE       50
#define MIN_POSITION_DUTY   3    // 0.6ms
#define MAX_POSITION_DUTY   11   // 3ms

#define CMD_SET_ANGLE  		0x01     
  
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
static uint8_t angle_to_duty(uint16_t angle);

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
	g_block_type = CLASS_MOTOR;
	g_block_sub_type = BLOCK_9G_SERVO;
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
	
	set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type, cmd_type;
	uint16_t servo_position;
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
	if(cmd_type == CMD_SET_ANGLE)
	{
		data_type = SHORT_16;
		if(read_next_sysex_data(&data_type, &servo_position, ON_LINE) == false)
		{
			return;
		}
		if(servo_position > 180)
		{
			servo_position = 180;
		}
	
		pwm_write(PWM_PIN, PWM_FREQUENCE, angle_to_duty(servo_position));
	}
}

void sysex_process_offline(void)
{
	int16_t uniform_value;
	uniform_value = read_uniform_value();
	uniform_value = line_map(uniform_value, SERVO_POSITION_MIN, SERVO_POSITION_MAX, UNIFORM_MIN, UNIFORM_MAX);
	pwm_write(PWM_PIN, PWM_FREQUENCE, angle_to_duty(uniform_value));
}

static uint8_t angle_to_duty(uint16_t angle)
{
	return (100 - line_map(angle, MIN_POSITION_DUTY, MAX_POSITION_DUTY, SERVO_POSITION_MIN, SERVO_POSITION_MAX));
}	
