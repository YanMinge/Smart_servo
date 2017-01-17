/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/05/19 11:45a $
 * @brief    for neuron product
 *
 * @note
 * Copyright (C) makeblock.cc. All rights reserved.
 *
 ******************************************************************************/
#include "sysinit.h"
#include <stdint.h>
#include "uart_printf.h"
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"
#include "MeEncoderMotor.h"

#define FIRMWARE_VERSION    003 // for firmware version.

#define CTL_SET_ENCODER_ABSOLUTE_POSITION             0x01
#define CTL_SET_ENCODER_RELATIVE_POSITION             0x02
#define CTL_SET_ENCODER_SPEED_CLOSED_LOOP             0x03
#define CTL_SET_ENCODER_SPEED_OPEN_LOOP               0x04
#define CTL_READ_ENCODER_SPEED_POSITION               0x05

#define REPORT_ENCODER_SPEED_POSITION                 0x05
/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

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
void report_encoder_motor_speed_positon(void);
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
        encoder_motor_run_loop();
    }
}


void init_block(void)
{
    g_block_type = CLASS_MOTOR;
    g_block_sub_type = BLOCK_ENCODER_MOTOR;
    encoder_motor_init();
    uart0_recv_attach(sysex_process_online, sysex_process_offline);
	
    set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
    uint8_t block_type, sub_type, data_type, cmd_type;
    int16_t encoder_motor_speed = 0;
    int32_t encoder_motor_turns = 0;
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
    if(cmd_type == CTL_SET_ENCODER_ABSOLUTE_POSITION)
    {
        data_type = SHORT_24;
        if(read_next_sysex_data(&data_type, &encoder_motor_speed, ON_LINE) == false)
        {
            return;
        }
        data_type = LONG_40;
        if(read_next_sysex_data(&data_type, &encoder_motor_turns, ON_LINE) == false)
        {
            return;
        }
        encoder_motor_move_to(encoder_motor_turns, encoder_motor_speed);
    
    }
    else if(cmd_type == CTL_SET_ENCODER_RELATIVE_POSITION)
    {
        data_type = SHORT_24;
        if(read_next_sysex_data(&data_type, &encoder_motor_speed, ON_LINE) == false)
        {
            return;
        }
        data_type = LONG_40;
        if(read_next_sysex_data(&data_type, &encoder_motor_turns, ON_LINE) == false)
        {
            return;
        }
            encoder_motor_move(encoder_motor_turns, encoder_motor_speed);
    }
    else if(cmd_type == CTL_SET_ENCODER_SPEED_CLOSED_LOOP)
    {
        data_type = SHORT_24;
        if(read_next_sysex_data(&data_type, &encoder_motor_speed, ON_LINE) == false)
        {
            return;
        }
        encoder_motor_move_speed(encoder_motor_speed);
    }
    else if(cmd_type == CTL_SET_ENCODER_SPEED_OPEN_LOOP)
    {
        data_type = BYTE_16;
        if(read_next_sysex_data(&data_type, &encoder_motor_speed, ON_LINE) == false) 
        {
            return;
        }
        encoder_motor_speed = line_map(encoder_motor_speed, -255, 255, -100, 100);
        encoder_motor_move_with_pwm(encoder_motor_speed);
    }
    else if(cmd_type == CTL_READ_ENCODER_SPEED_POSITION)
    {
        report_encoder_motor_speed_positon();
    }
}

void sysex_process_offline(void)
{
    int16_t uniform_value = 0;
    uint8_t block_type = 0;
    uint8_t sub_type = 0;
    int16_t motor_speed = 0;
    read_sysex_type(&block_type, &sub_type, OFF_LINE);
    uniform_value = read_uniform_value();
    motor_speed = line_map(uniform_value, 0, 255, UNIFORM_MIN, UNIFORM_MAX);
    encoder_motor_move_with_pwm(motor_speed);
}

void report_encoder_motor_speed_positon(void)
{
    uint8_t report_type;
    int16_t encoder_motor_speed;
    int32_t encoder_motor_position;
    init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
    
    report_type = REPORT_ENCODER_SPEED_POSITION;
    add_sysex_data(BYTE_8, &report_type, ON_LINE);
   
    encoder_motor_speed = get_encoder_motor_speed();
    encoder_motor_position = get_encoder_motor_position();
    
    add_sysex_data(SHORT_24, (void*)(&encoder_motor_speed), ON_LINE);
    add_sysex_data(LONG_40, (void*)(&encoder_motor_position), ON_LINE);
     
    flush_sysex_to_send(ON_LINE);
}

