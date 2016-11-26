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
#include "MeST7735.h"


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
    }
}


void init_block(void)
{
	device_type = DISPLAY_SCREEN_LCD_144;
	Lcd_Init();
	Lcd_Clear(BLACK);
	Gui_DrawFont_GBK16(0, 0, WHITE, BLACK, "MAKEBLOCK IOT");
	Gui_DrawFont_GBK16(0, 18, WHITE, BLACK, "So interesting!");
	Gui_DrawFont_GBK16(0, 36, WHITE, BLACK, "------------------");
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
}

void sysex_process_online(void)
{
	uint8_t type, sub_type;
	uint8_t data_type = BYTE_8;
	uint8_t	string_display[128];
	int16_t string_length;
	uint8_t code_type;
	read_sysex_type(&data_type, (void*)(&code_type), ON_LINE);
	code_type = SHORT_24;
	read_next_sysex_data(&data_type, (void*)(&string_length), ON_LINE);
	if(string_length > 128)
	{
		string_length = 128;
	}
	int i = 0;
	data_type = BYTE_8;
	for(;i < string_length; i++)
	{
		read_next_sysex_data(&data_type, (void*)(string_display +i), ON_LINE);
	}
	string_display[string_length] = '\0';
	
	Gui_DrawFont_GBK16(0, 54, WHITE, BLACK, string_display);
}

void sysex_process_offline(void)
{

}
