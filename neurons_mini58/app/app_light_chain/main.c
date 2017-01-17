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
#include "MeLedChain.h"
#include "uart_printf.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Marco defines                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION            003

#define BRIGHTNESS_MIN  0
#define BRIGHTNESS_MAX  255

//CMD for led chain.
#define CMD_SET_SIGNAL_LED        0x01
#define CMD_SET_ALL_LED           0x02

/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile static uint8_t s_r_value;
volatile static uint8_t s_g_value;
volatile static uint8_t s_b_value;

static uint8_t s_display_mode;
static uint32_t s_dinamic_display_interval;
static uint32_t s_pre_display_time;

static uint8_t s_led_colour_array[30];
static uint8_t s_led_quantity;
static uint8_t s_dinamic_count;


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
void get_sensor_data(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void sysex_process_offline(void);
void light_bar_dinamic_display(void);
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
        // led.
        poll_led_request();
        light_bar_dinamic_display();
    }
}

void init_block(void)
{
    g_block_type = CLASS_DISPLAY;
    g_block_sub_type = BLOCK_LIGHT_BAR;
	
    rgb_init();
    uart0_recv_attach(sysex_process_online, sysex_process_offline);
    NVIC_SetPriority(UART0_IRQn, 1);
    NVIC_SetPriority(UART1_IRQn, 1);
    set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
    uint8_t block_type, sub_type, command_type, data_type;
    uint8_t led_index;
    uint8_t dinamic_display_speed;
    uint16_t r_value, g_value, b_value;
    uint8_t  led_quantity = 0;
    uint8_t display_mode = 0;
    uint8_t led_colour_array[MAX_RGB_LED_NUM] = {0};
    int i;
    // read block type.
    read_sysex_type(&block_type, &sub_type, ON_LINE);
    if((block_type != g_block_type) || (sub_type != g_block_sub_type))
    {
        return;
    }
    // read command.
    data_type = BYTE_8;
    if(read_next_sysex_data(&data_type, &command_type, ON_LINE) == false)
    {
        return;
    }
    if(command_type == CMD_SET_SIGNAL_LED)
    {
        s_display_mode = DISPLAY_MODE_STATIC;
        
        // read set value.
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type, &led_index, ON_LINE) == false)
        {
            return;
        }
        data_type = SHORT_16;
        if(read_next_sysex_data(&data_type, &r_value, ON_LINE)== false)
        {
            return;
        }
        data_type = SHORT_16;
        if(read_next_sysex_data(&data_type, &g_value, ON_LINE) == false)
        {
            return;
        }
        data_type = SHORT_16;
        if(read_next_sysex_data(&data_type, &b_value, ON_LINE)== false)
        {
            return;
        }
	
        setColor(led_index, r_value, g_value, b_value);
        rgb_show(MAX_RGB_LED_NUM);
    }
    else if(command_type == CMD_SET_ALL_LED)
    {
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type, &display_mode, ON_LINE) == false)
        {
            return;
        }
        if(read_next_sysex_data(&data_type, &dinamic_display_speed, ON_LINE) == false)
        {
            return;
        }
        
        if(read_next_sysex_data(&data_type, &led_quantity, ON_LINE) == false)
        {
            return;
        }
        if(led_quantity > 30)
        {
            led_quantity = 30;
        }
        for(i = 0; i < led_quantity; i++)
        {
            if(read_next_sysex_data(&data_type, (void*)(led_colour_array + i), ON_LINE)== false)
            {
                return;
            }
        }
        if(display_mode == DISPLAY_MODE_STATIC)
        {    
            s_display_mode = display_mode;
            led_chain_all_set(MAX_RGB_LED_NUM, led_colour_array);
        }
        else if(display_mode == DISPLAY_MODE_REPEAT)
        {
            s_display_mode = display_mode;
            uint8_t colour_array[MAX_RGB_LED_NUM];
            for(i = 0; i < MAX_RGB_LED_NUM; i++)
            {
                colour_array[i]= 0;
            }
            int j = 0;
            for(i = 0; i < MAX_RGB_LED_NUM; i++)
            {
                colour_array[i] = led_colour_array[j];
                j++;
                if(j == led_quantity) // the last 
                {
                    j = 0;
                }
            }
            
            led_chain_all_set(MAX_RGB_LED_NUM, colour_array);
        }
        else if(display_mode == DISPLAY_MODE_MARQUEE || display_mode == DISPLAY_MODE_ROLL)
        {
            s_display_mode = display_mode;
            s_led_quantity = led_quantity;
            switch(dinamic_display_speed)
            {
                case 0:
                    s_dinamic_display_interval = 1000; // 2s, change.
                break;
                case 1:
                    s_dinamic_display_interval = 500; // 1s, change.
                break;
                case 2:
                    s_dinamic_display_interval = 200; // 0.5s, change.
                break;
                default:
                    s_dinamic_display_interval = 500;
                break;
            }
            // initialize the led_chain_array, all black.
            for(i = 0; i < led_quantity; i++)
            {
                s_led_colour_array[i]= led_colour_array[i];
            }
            for(i = led_quantity; i < MAX_RGB_LED_NUM; i++)
            {
                s_led_colour_array[i] = 0;
            }
            s_dinamic_count = 0;
            rgb_clear();
        }
    }
}

void sysex_process_offline(void)
{
    uint8_t r_value, b_value, g_value, block_type, sub_block_type, data_type;
    int16_t colour_sensor_r, colour_sensor_g, colour_sensor_b;
    uint16_t uniform_value;
	
    read_sysex_type(&block_type, &sub_block_type, OFF_LINE);
	
    if((block_type == CLASS_SENSOR)&&(sub_block_type == BLOCK_COLOUR_SENSOR))
    {
        if(read_next_sysex_data(&data_type, &colour_sensor_r, OFF_LINE) == false)
        {
            return;
        }
        if(read_next_sysex_data(&data_type, &colour_sensor_g, OFF_LINE) == false)
        {
            return;
        }
        if(read_next_sysex_data(&data_type, &colour_sensor_b, OFF_LINE) == false)
        {
            return;
        }
        r_value = colour_sensor_r;
        g_value = colour_sensor_g;
        b_value = colour_sensor_b;		
    }
    else
    {
        uniform_value = read_uniform_value();
        uint8_t temp_value = line_map(uniform_value, 0, 255, UNIFORM_MIN, UNIFORM_MAX);
	
        r_value = temp_value;
        g_value = temp_value;
        b_value = temp_value;
    }

    // get ride of repeated setting value.
    if((s_r_value == r_value)&&(s_g_value == g_value)&&(s_b_value == b_value))
    {
        return;
    }
    else
    {
        s_r_value = r_value;
        s_g_value = g_value;
        s_b_value = b_value;
    }
    s_b_value=line_map(uniform_value, 0, 100, UNIFORM_MIN, UNIFORM_MAX);
    Set_SevenColor_Regular(s_b_value);
}

void light_bar_dinamic_display(void)
{
    int i = 0;
    uint8_t led_colour_array[MAX_RGB_LED_NUM];
    for(i = 0; i < MAX_RGB_LED_NUM; i++)
    {
        led_colour_array[i]= 0;
    }
    
    if(s_display_mode != DISPLAY_MODE_ROLL && s_display_mode != DISPLAY_MODE_MARQUEE)
    {
        return;
    }
    uint32_t current_time = millis();
    if((current_time - s_pre_display_time) > s_dinamic_display_interval)
    {
        s_pre_display_time = current_time;
        
        if(s_display_mode == DISPLAY_MODE_ROLL)
        {
            if(s_dinamic_count<=MAX_RGB_LED_NUM)
            {
                for( i = 0; i < s_dinamic_count; i++)
                {
                    led_colour_array[i] = 0;
                }
                for(i =0 ; i < s_led_quantity; i++)
                {
                    led_colour_array[i + s_dinamic_count] = s_led_colour_array[i];
                }
                for(i = s_dinamic_count + s_led_quantity; i < MAX_RGB_LED_NUM; i++)
                {
                    led_colour_array[i] = 0;
                }
                led_chain_all_set(MAX_RGB_LED_NUM, led_colour_array);
                s_dinamic_count++;
            }
            else
            {
                s_display_mode = DISPLAY_MODE_STATIC;
                s_dinamic_count = 0;
            }
        }
        else if(s_display_mode == DISPLAY_MODE_MARQUEE)
        {
            int j = 0;
            for(i = 0; i < MAX_RGB_LED_NUM; i++)
            {
                led_colour_array[i] = s_led_colour_array[j];
                j++;
                if(j == s_led_quantity) // the last 
                {
                    j = 0;
                }
            }
            led_chain_all_set(MAX_RGB_LED_NUM, led_colour_array);
            
            uint8_t first_data;
            first_data = s_led_colour_array[0];
            for(i = 0; i < s_led_quantity - 1; i++)
            {
                s_led_colour_array[i] = s_led_colour_array[i+1];
            }
            s_led_colour_array[i] = first_data;
        }
    }
}
