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
#include "MeTftDisplay.h"


/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis          //
volatile unsigned long system_time = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint16_t g_volume_value;
volatile uint16_t g_pre_volume_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_message_process(void);
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

    /* Init UART0 for printf */
    UART0_Init(115200);

    /* Init UART1*/
    UART1_Init(115200);

    /* Set UART Configuration */
    UART_SetLine_Config(UART0, 115200, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);

    /* Set UART Configuration */
    UART_SetLine_Config(UART1, 115200, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);

    /* UART sample function */
    UART_Function_Init();

    TMR0_Init(100000);
		
	init_block( );

    while(1)
    {
//		parse_uart0_recv_buffer();
//        flush_uart1_forward_buffer();
//        flush_uart0_local_buffer();
//        get_sensor_data();
//        currentMillis = millis();
//        if (currentMillis - previousMillis > samplingInterval)
//        {
//            previousMillis = currentMillis;
//			if(g_block_no == 0)
//			{
//				send_sensor_report_offline();
//			}
//			else
//			{
//				send_sensor_report_online();
//			}
//        }
    }
}


void init_block(void)
{
	//LCD_Init();
	DispStr("Makeblock", 0, 0, WHITE, BLACK);
	device_type = VOLUME_SENSOR;
	uart0_recv_attach(sysex_message_process, NULL);
}
	
void get_sensor_data(void)
{
	g_volume_value = analogRead(P1_5);	
}

void send_sensor_report_online(void)
{
	
    if(((report_mode == REPORT_MODE_DIFF) &&(g_volume_value != g_pre_volume_value)) ||
		(report_mode == REPORT_MODE_CYCLE))
	{
		g_pre_volume_value = g_volume_value;
		
		send_analog_signal_to_host(device_type, 0, g_volume_value);
	}
 
}

void send_sensor_report_offline(void)
{
	send_analog_signal_to_block(device_type, 0, g_volume_value);
}

void sysex_message_process(void)
{
	
	if(CTL_READ_CURRENT_VALUE == sysex.val.srv_id)
	{		
		//response mesaage to UART0
		send_analog_signal_to_host(device_type, 0 ,g_volume_value);
	}
}
