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


/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis
volatile unsigned long system_time = 0;


unsigned long key_debounced_time;
int key_debounced_count = 0;
int key_match_count = 0;
int key_debounced_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile boolean button_key_val;
volatile boolean pre_button_key_val;
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void button_message_process(void);
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
		
	init_block();

    while(1)
    {
		parse_uart0_recv_buffer();
        flush_uart1_forward_buffer();
        flush_uart0_local_buffer();
		
        get_sensor_data();
        currentMillis = millis();
        if (currentMillis - previousMillis > samplingInterval)
        {
            previousMillis = currentMillis;
			if(device_id == 0)
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


void init_block()
{
	pinMode(P2_3,GPIO_PMD_INPUT);
	device_type = LIMIT_SWITCH;
	uart0_recv_attach(button_message_process, NULL);
}
	
void debounced_read_button_key(void)
{
    unsigned long current_time;
    int key_temp_value;
    current_time = millis();
    if (current_time - key_debounced_time > 3)
    {
        key_debounced_time = current_time;
        key_temp_value = digitalRead(P3_2);
        if(key_debounced_count == 0)
        {
            key_debounced_value = key_temp_value;
        }
        if(key_temp_value == key_debounced_value)
        {
            key_match_count++;
        }
        key_debounced_count ++;
    }

    if(key_debounced_count == 10)
    {
        key_debounced_count = 0;
        if(key_match_count > 4)
        {
            button_key_val = !key_debounced_value;
        }
    }
}

void get_sensor_data(void)
{
	debounced_read_button_key();
}

void send_sensor_report_online(void)
{
    if(((report_mode == REPORT_MODE_DIFF) &&(pre_button_key_val != button_key_val)) ||
		(report_mode == REPORT_MODE_CYCLE))
	{
		pre_button_key_val = button_key_val;
        send_digital_signal_to_host(device_type, 0, button_key_val);
	}
 
}

void send_sensor_report_offline(void)
{
	send_digital_signal_to_block(device_type, 0, button_key_val);      
}

void button_message_process(void)
{
	if(CTL_READ_CURRENT_VALUE == sysex.val.srv_id)
	{
		send_digital_signal_to_host(device_type, 0, button_key_val);
	}
}
