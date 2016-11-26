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
#include "MeLedMatrix.h"
#include "string.h"



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
		// led.
		poll_led_request();
    }
}


void init_block(void)
{
	g_block_type = LED_MATRIX;
	led_matrix_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
}

void sysex_process_online(void)
{
	uint8_t block_type, sub_type, data_type;
	uint32_t led_matrix_right, led_matrix_left;
	uint8_t led_matrix[8];
	
	// read block type.
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if(block_type != g_block_type)
	{
		return;
	}
	
	// read set value.
	data_type = LONG_40;
	read_next_sysex_data(&data_type, (void*)(&led_matrix_right), ON_LINE);
	read_next_sysex_data(&data_type, (void*)(&led_matrix_left), ON_LINE);
	
	memcpy((void*)led_matrix, (void*)(&led_matrix_right), sizeof(uint32_t));
	memcpy((void*)(led_matrix + 4), (void*)(&led_matrix_left), sizeof(uint32_t));
	
	led_matrix_set(led_matrix);
}

void sysex_process_offline(void)
{
	
}
