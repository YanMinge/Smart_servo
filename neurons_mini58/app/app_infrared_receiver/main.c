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
#include "MeInfraredReceiver.h"

/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis          //
volatile unsigned long system_time = 0;


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile boolean  g_report_flag = 0;

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
		
	init_block();

    while(1)
    {
		// protocol.
		parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();
		// led.
		poll_led_request();
		
        get_sensor_data();
        if (g_report_flag)
        {
			if(g_block_no == 0)
			{
				send_sensor_report_offline();
			}
			else
			{
				send_sensor_report_online();
			}
        }
    }
}


void init_block(void)
{
	g_block_type = INFRARED_RECEIVER;
	InfraredReceiverInit();
}
	
void get_sensor_data(void)
{
	if(ir_recv_poll())
	{
		g_report_flag = true;
	}
	else
	{
		g_report_flag = false;
	}
}

void send_sensor_report_online(void)
{
    init_sysex_to_send(g_block_type, 0, ON_LINE);
	add_sysex_data(SHORT_24, (void*)(&g_inframed_address), ON_LINE);
	add_sysex_data(SHORT_16, (void*)(&g_inframed_data), ON_LINE);
	flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
	uint8_t data_type;
	uint16_t uinform_value = 0;
	
	init_sysex_to_send(g_block_type, 0, OFF_LINE);
	data_type = SHORT_16;
	add_sysex_data(data_type, (void*)(&g_inframed_data), OFF_LINE);
	data_type = UNIFORM_16;
	add_sysex_data(data_type, &uinform_value, OFF_LINE);
	flush_sysex_to_send(OFF_LINE);
}
