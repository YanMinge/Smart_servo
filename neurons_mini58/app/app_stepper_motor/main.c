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
#include "MeStepMotor.h"


/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis

volatile unsigned long system_time = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);

void init_block(void);
void get_sensor_data(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void sysex_process_offline(void);

#define MOTOR_SPEED_MIN  0
#define MOTOR_SPEED_MAX  255

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
	
	TMR1_Init(2000);	
		
	init_block();

    while(1)
    {
		// led.
		poll_led_request();
		
		run();
    }
}

void task_receive_data_process(void)
{
	parse_uart0_recv_buffer();
    flush_uart1_to_uart0();
    flush_uart0_local_buffer();
}

void init_block(void)
{
	g_block_type = STEP_MOTOR;
	stepper_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_device_type, command_type;
	long position;
	int16_t speed;
	
	//read block type.
	read_sysex_type(&block_type, &sub_device_type, ON_LINE);
	if(block_type != g_block_type)
	{
		return;
	}
	
	// read command type.
	uint8_t type = BYTE_8;
	read_next_sysex_data(&type, (void*)(&command_type), ON_LINE);
	
	// 	read speed.
	type = SHORT_16;
	read_next_sysex_data(&type, (void*)(&speed), ON_LINE);
	
	// read target position.
	type = LONG_40;
	read_next_sysex_data(&type, (void*)(&position), ON_LINE);
	
	setSpeed((float)speed);
	move(position);
}

void sysex_process_offline(void)
{
	//int16_t uniform_value;
	//uniform_value = read_uniform_value();
	//uniform_value = uniform_convert_to_real(uniform_value, MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
	//dc_run(0, uniform_value);
	//dc_run(1, uniform_value);k
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
