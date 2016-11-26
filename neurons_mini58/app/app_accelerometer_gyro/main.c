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
#include "MeMPU6050.h"
#include "block_param.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define CTL_READ_ACC_VALUE              0x01
#define CTL_READ_GYRO_VALUE             0x02
#define CTL_READ_ANGLE_VALUE            0x03
#define CTL_SET_PERIOD_REPORT_TYPE      0x04 
#define REPORT_TYPE_ACC                 0x01
#define REPORT_TYPE_GYRO                0x02
#define REPORT_TYPE_ANGLE               0x03
#define CTL_SET_REPORT_MODE             0x7F

#define SAMPLE_PERIOD					20

/*---------------------------------------------------------------------------------------------------------*/
/* Local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static uint32_t s_online_start_mills = 0; // used for period reporting in online mode
static uint32_t s_offline_start_mills = 0; // used for period reporting in offline mode
static uint32_t s_sample_start_millis = 0; // used for period sampling.
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static float s_pre_value_x = 0;
static float s_pre_value_y = 0;
static float s_pre_value_z = 0;
static uint8_t s_report_mode = REPORT_MODE_CYCLE;
static uint8_t s_report_type = REPORT_TYPE_ACC;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
struct mpu6050_data mpu6050_data_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
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
	
	init_block( );

    while(1)
    {
		// protocol.
	    parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();
		
		// led.
		poll_led_request();
		
		get_sensor_data();
		
		uint32_t current_millis = millis();
		
		// on line.
		if(g_block_no != 0) 
		{
			if((current_millis - s_online_start_mills) > s_report_period)
			{
				s_online_start_mills = current_millis;
				send_sensor_report_online();
			}
		}
		
		// off line.
		else if(g_block_no == 0)
		{
			
			if((current_millis - s_offline_start_mills) > OFF_LINE_REPORT_PERIOD)
			{
				s_offline_start_mills = current_millis;
				send_sensor_report_offline();
			}
		}
    }
}


void init_block(void)
{
	g_block_type = CLASS_SENSOR;
	g_block_sub_type = BLOCK_ACCELEROMETER_GYRO;
	
	mpu6050_init();
	
	uart0_recv_attach(sysex_process_online, NULL);
	
	set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
	uint32_t current_millis = millis();
	if((current_millis - s_sample_start_millis) > SAMPLE_PERIOD)
	{
		s_sample_start_millis = current_millis;
		ReadMpu6050(&mpu6050_data_value);
	}
}

static void report_sensor_value_on_line(uint8_t type)
{
	init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
	add_sysex_data(BYTE_16, &type, ON_LINE);
	if(type == REPORT_TYPE_ACC)
	{
		add_sysex_data(FLOAT_40, &(mpu6050_data_value.x_accel_value), ON_LINE);
		add_sysex_data(FLOAT_40, &(mpu6050_data_value.y_accel_value), ON_LINE);
		add_sysex_data(FLOAT_40, &(mpu6050_data_value.z_accel_value), ON_LINE);
			
	}
	else if(type == REPORT_TYPE_GYRO)
	{
		add_sysex_data(FLOAT_40, &(mpu6050_data_value.x_gyro_value), ON_LINE);
		add_sysex_data(FLOAT_40, &(mpu6050_data_value.y_gyro_value), ON_LINE);
		add_sysex_data(FLOAT_40, &(mpu6050_data_value.z_gyro_value), ON_LINE);
	}
	else if(type == REPORT_TYPE_ANGLE)
	{
		add_sysex_data(SHORT_24, &(mpu6050_data_value.x_angle), ON_LINE);
		add_sysex_data(SHORT_24, &(mpu6050_data_value.y_angle), ON_LINE);
		add_sysex_data(SHORT_24, &(mpu6050_data_value.z_angle), ON_LINE);
	}
		
	flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_online(void)
{
	//response mesaage to UART0	
	report_sensor_value_on_line(s_report_type);	
}

void send_sensor_report_offline(void)
{
	uint16_t uniform_value = 0;
	
	if(abs(mpu6050_data_value.x_accel_value - s_pre_value_x) < 0.01)
	{
		mpu6050_data_value.x_accel_value = s_pre_value_x;
	}
	if(abs(mpu6050_data_value.y_accel_value - s_pre_value_y) < 0.01)
	{
		mpu6050_data_value.y_accel_value = s_pre_value_y;
	}
	if(abs(mpu6050_data_value.z_accel_value - s_pre_value_z) < 0.01)
	{
		mpu6050_data_value.z_accel_value = s_pre_value_z;
	}
	s_pre_value_x = mpu6050_data_value.x_accel_value;
	s_pre_value_y = mpu6050_data_value.y_accel_value;
	s_pre_value_z = mpu6050_data_value.z_accel_value;
	
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	
	add_sysex_data(FLOAT_40, &(mpu6050_data_value.x_accel_value), OFF_LINE);
	add_sysex_data(FLOAT_40, &(mpu6050_data_value.y_accel_value), OFF_LINE);
	add_sysex_data(FLOAT_40, &(mpu6050_data_value.z_accel_value), OFF_LINE);
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
	if((block_type != g_block_type) ||(sub_type != g_block_sub_type))
	{
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE) == false)
    {
        return;
    }
	switch(cmd_type)
	{
		case CTL_READ_ACC_VALUE:
		case CTL_READ_GYRO_VALUE:
		case CTL_READ_ANGLE_VALUE:
			report_sensor_value_on_line(cmd_type);
			break;
		case CTL_SET_PERIOD_REPORT_TYPE:
			data_type = BYTE_8;
			if(read_next_sysex_data(&data_type, &s_report_type, ON_LINE) == false)
			{
				return;
			}
			break;
		case CTL_SET_REPORT_MODE:
			data_type = BYTE_8;
			if(read_next_sysex_data(&data_type, &s_report_mode, ON_LINE) == false)
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
			}
			break;
		default:
			break;
	}

}
