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
#include "MeMp3.h"
#include "MeSoftSerial.h"


/* local variables */
volatile static enum CMD_TYPE s_cmd_type;
volatile static uint8_t s_music_no = 0;
volatile static int8_t 	s_mp3_volume = 0;
volatile static uint8_t s_key3_long_or_short_count = 0;
volatile static uint8_t s_key4_long_or_short_count = 0;
volatile static uint8_t s_cmd_flag = 0;
static enum MP3_KEY_CONTROL_STATE s_mp3_state = STATE_NO_PRESS;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
unsigned long key_debounced_time = 0;
int key_debounced_count = 0;
int key_match_count = 0;
int key_debounced_value = 0;
 volatile unsigned long system_time = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);

void init_block(void);
void get_sensor_data(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void sysex_process_offline(void);
void poll_command_from_host(void);
void poll_key_status(void);
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
		poll_command_from_host();	
		poll_key_status();
		poll_led_request();
	}	
}

void init_block(void)
{
	g_block_type = CLASS_AUDIO;
	g_block_sub_type = BLOCK_MP3;
	mp3_init();
	uart0_recv_attach(sysex_process_online, sysex_process_offline);
	
	set_rgb_led(0, 0, 128);
}
void task_receive_data_process(void)
{
	parse_uart0_recv_buffer();
    flush_uart1_to_uart0();
    flush_uart0_local_buffer();
}

void poll_key_status(void)
{
	unsigned long current_time;
    int key_temp_value = 0;
    current_time = millis();
    if (current_time - key_debounced_time > 3)
    {
        key_debounced_time = current_time;
        key_temp_value = !digitalRead(FUNCTION_KEY_1);
		key_temp_value |= (!digitalRead(FUNCTION_KEY_2))<<1;
		key_temp_value |= (!digitalRead(FUNCTION_KEY_3))<<2;
		key_temp_value |= (!digitalRead(FUNCTION_KEY_4))<<3;
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

    if(key_debounced_count == 30)
    {
        key_debounced_count = 0;
        if(key_match_count > 20)
        {
			key_match_count = 0;
			switch(s_mp3_state)
			{
				case STATE_NO_PRESS:
					switch(key_debounced_value)
					{
						case 0x01:
							s_mp3_state = STATE_RECORD_PRESS;
							mp3_send_cmd(MP3_RECORD);
						break;
						case 0x02:
							s_mp3_state = STATE_PAUSE_OR_CONTINUE_PRESS;
						break;
						case 0x04:
							s_mp3_state = STATE_KEY3_LONG_OR_SHORT_PRESS;
							s_key3_long_or_short_count++;
						break;
						case 0x08:
							s_mp3_state = STATE_KEY4_LONG_OR_SHORT_PRESS;
							s_key4_long_or_short_count++;
						break;
						default:
						break;
					}
				break;
				case STATE_PAUSE_OR_CONTINUE_PRESS:
					switch(key_debounced_value&0x02)
					{
						case 0:
							s_mp3_state = STATE_NO_PRESS;
							mp3_send_cmd(MP3_PAUSE_OR_CONTINUE_PLAY);
						break;
						case 0x02:
						break;
						default:
						break;
					}
				break;
				case STATE_RECORD_PRESS:
					switch(key_debounced_value&0x01)
					{
						case 0:
							s_mp3_state = STATE_NO_PRESS;
							mp3_send_cmd(MP3_STOP_RECORD);
						break;
						case 1:
						break;
						default:
						break;
					}
				break;
				case STATE_KEY3_LONG_OR_SHORT_PRESS:
					switch(key_debounced_value&0x04)
					{
						case 0:
							if(s_key3_long_or_short_count > 0 && s_key3_long_or_short_count < 4) // short press release.
							{
								s_key3_long_or_short_count = 0;
								s_mp3_state = STATE_NO_PRESS;
								mp3_send_cmd(MP3_NEXT);
							}
							else // long press release.
							{
								s_key3_long_or_short_count = 0;
								s_mp3_state = STATE_NO_PRESS;
							}
						break;
						case 0x04:
							if(s_key3_long_or_short_count > 8)
							{
								s_mp3_volume += 4;
								if(s_mp3_volume > 31) // volume range 0-31.
								{
									s_mp3_volume = 31 ;
								}
								
								mp3_set_volume(s_mp3_volume);
								s_key3_long_or_short_count = 5; // no set to 0, for distinguish with short release when realease.
							}
							else
							{
								s_key3_long_or_short_count++;
							}
						break;
						default:
						break;
					}
				break;
				case STATE_KEY4_LONG_OR_SHORT_PRESS:
					switch(key_debounced_value&0x08)
					{
						case 0:
							if(s_key4_long_or_short_count > 0 && s_key4_long_or_short_count < 4)
							{
								s_key4_long_or_short_count = 0;
								s_mp3_state = STATE_NO_PRESS;
								mp3_send_cmd(MP3_LAST);
							}
							else
							{
								s_key4_long_or_short_count = 0;
								s_mp3_state = STATE_NO_PRESS;
							}
						break;
						case 0x08:
							if(s_key4_long_or_short_count > 8)
							{
								s_mp3_volume -=4;
								if(s_mp3_volume < 0)
								{
									s_mp3_volume = 0;
								}
								mp3_set_volume(s_mp3_volume);
								s_key4_long_or_short_count = 5; // no set to 0, for distinguish with short release when realease.
							}
							else
							{
								s_key4_long_or_short_count++;
							}
						break;
						default:
						break;
					}
				break;
				default:
				break;
			}
							
			
        }
		else
		{
			key_match_count = 0;
		}
    }
	
}

void poll_command_from_host(void)
{
	if(s_cmd_flag == 0)
	{
		return;
	}

	switch(s_cmd_type)
	{
		case MP3_PLAY:
			mp3_play_music(s_music_no);
			break;
		case MP3_DELETE :
			mp3_delete_music(s_music_no);
			break;
		default:
			mp3_send_cmd(s_cmd_type);
			break;
	}
	
	s_cmd_flag = 0;
}

void sysex_process_online(void)
{
	s_cmd_flag = 1;
	
	// read block type.
	uint8_t block_type, sub_type, data_type;
	read_sysex_type(&block_type, &sub_type, ON_LINE);
	if((block_type != g_block_type)||(sub_type != g_block_sub_type)) // type error.
	{
		return;
	}
	
	// read command type.
	data_type = BYTE_8;
	if(read_next_sysex_data(&data_type, (void*)(&s_cmd_type), ON_LINE) == false)
	{
		return;
	}
	switch(s_cmd_type)
	{
		case MP3_PLAY:
			if(read_next_sysex_data(&data_type, (void*)(&s_music_no), ON_LINE) == false)
			{
				return;
			}
			break;
		case MP3_DELETE :
			if(read_next_sysex_data(&data_type, (void*)(&s_music_no), ON_LINE) == false)
			{
				return;
			}
			break;
		default:
			break;
	}
}

void sysex_process_offline(void)
{
	
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