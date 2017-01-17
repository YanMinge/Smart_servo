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

#define FIRMWARE_VERSION            003

#define CMD_SET_MOTOR_SPEED  		0x01  

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
		// led
		poll_led_request();
    }
}


void init_block(void)
{
	g_block_type = CLASS_MOTOR;
	g_block_sub_type = BLOCK_DOUBLE_DC_MOTOR;
	dc_motor_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
	
	set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type, cmd_type;
	int8_t speed_motor_1, speed_motor_2;
	
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
	if(cmd_type == CMD_SET_MOTOR_SPEED)
	{
		// read data.
		data_type = BYTE_16;
		read_next_sysex_data(&data_type, &speed_motor_1, ON_LINE);
		data_type = BYTE_16;
		read_next_sysex_data(&data_type, &speed_motor_2, ON_LINE);
	
		// excute.
		dc_run(MOTOR_1, speed_motor_1);
		dc_run(MOTOR_2, speed_motor_2);
	}
}

void sysex_process_offline(void)
{
	int16_t uniform_value = 0;
	uint8_t block_type = 0;
	uint8_t sub_type = 0;
	uint8_t data_type = 0;
	uint8_t motor_1_speed = 0;
	uint8_t motor_2_speed = 0;
	read_sysex_type(&block_type, &sub_type, OFF_LINE);
	uniform_value = read_uniform_value();
//	if(block_type == ACCELEROMETER_GYRO)
//	{
//		data_type = FLOAT_40;
//		float x_value = 0;
//		float y_value = 0;
//		int32_t motor1_speed = 0;
//		int32_t motor2_speed = 0;
//		read_next_sysex_data(&data_type, &x_value, OFF_LINE);
//		read_next_sysex_data(&data_type, &y_value, OFF_LINE);
//		int16_t x_temp = real_convert_to_uinform(x_value, MPU6050_MIN, MPU6050_MAX);
//		int16_t y_temp = real_convert_to_uinform(y_value, MPU6050_MIN, MPU6050_MAX);
//		
//		x_temp = uniform_convert_to_real(x_temp, 0 - MOTOR_SPEED_MAX, MOTOR_SPEED_MAX);
//		y_temp = uniform_convert_to_real(y_temp, 0 - MOTOR_SPEED_MAX, MOTOR_SPEED_MAX);
//		
//		motor1_speed = x_temp + (y_temp>>2);
//		motor2_speed = x_temp - (y_temp>>2);
//	
//		motor1_speed = constrain(motor1_speed, (0- MOTOR_SPEED_MAX), MOTOR_SPEED_MAX);
//		motor2_speed = constrain(motor2_speed, (0- MOTOR_SPEED_MAX), MOTOR_SPEED_MAX);
//		 
//		dc_run(0, motor1_speed);
//		dc_run(1, motor2_speed);
//	}
	if((block_type == CLASS_SENSOR)&&(sub_type == BLOCK_LINE_FOLLOWER))
	{
		uint8_t line_detect_value;
		data_type = BYTE_8;
		read_next_sysex_data(&data_type, &line_detect_value, OFF_LINE);
//		switch(line_detect_value)
//		{
//			case 
//		}
	}
	else
	{
		motor_1_speed = line_map(uniform_value, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX, UNIFORM_MIN, UNIFORM_MAX);
		dc_run(MOTOR_1, motor_1_speed);
		motor_2_speed = motor_1_speed;
		dc_run(MOTOR_2, motor_2_speed);
	}
}
