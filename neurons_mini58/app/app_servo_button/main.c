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
#include "math.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CTL_READ_CURRENT_VALUE   0x01
#define REPORT_CURRENT_VALUE     0x01
#define CTL_SET_LED              0x02
#define CTL_SET_REPORT_MODE      0x7f

#define FIRMWARE_VERSION        003

#define PIN_LED1                P0_0
#define PIN_LED2                P0_1
#define PIN_LED3                P0_4
#define PIN_LED4                P0_5
#define PIN_LED5                P0_6
#define PIN_LED6                P0_7
#define PIN_LED7                P3_0
#define PIN_LED8                P3_1
#define PIN_LED9                P3_2
#define PIN_LED10               P3_4
#define PIN_LED11               P3_5
#define PIN_LED12               P3_6

const uint8_t led_num[12] = {PIN_LED1,PIN_LED2,PIN_LED3,PIN_LED4,PIN_LED5,PIN_LED6,PIN_LED7,PIN_LED8,PIN_LED9,PIN_LED10,PIN_LED11,PIN_LED12};

static unsigned long s_online_start_mills;       // for comparison with currentMillis
static unsigned long s_offline_start_mills;
static unsigned long s_previous_millis;
static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static uint8_t  s_report_mode = REPORT_MODE_CYCLE;

unsigned long key_debounced_time;
int key_debounced_count = 0;
int key_match_count = 0;
int key_debounced_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_logic_pin;
volatile boolean g_logic_value;
volatile boolean g_pre_logic_value;
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void sysex_message_process(void);
void set_led(uint16_t led_mask);
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
    parse_uart0_recv_buffer();
    flush_uart1_to_uart0();
    flush_uart0_local_buffer();
    get_sensor_data();
    uint32_t currentMillis = millis();
    // on line.
    if(g_block_no != 0)
    {
      if(s_report_mode == REPORT_MODE_CYCLE)
      {	
        if((currentMillis - s_online_start_mills > s_report_period))
        {
          s_online_start_mills = currentMillis;
          send_sensor_report_online();
        }
      }
      else if(s_report_mode == REPORT_MODE_DIFF)
      {
        if(g_logic_value != g_pre_logic_value)
        {
          g_pre_logic_value = g_logic_value;
          send_sensor_report_online();
        }
      }			  
    }
    // off line.
    else if(g_block_no == 0)
    {
      if (currentMillis - s_offline_start_mills > OFF_LINE_REPORT_PERIOD)
      {
        s_offline_start_mills = currentMillis;
        send_sensor_report_offline();
      }
    }
		
    if(currentMillis - s_previous_millis > 10)
    {
      s_previous_millis = currentMillis;
    }
  }
}


void init_block(void)
{
  g_logic_pin = P2_5;
  g_block_type = CLASS_CONTROL;
  g_block_sub_type = BLOCK_SLIGHT_TOUCH_BUTTON;

  pinMode(g_logic_pin,GPIO_MODE_INPUT);
  uart0_recv_attach(sysex_message_process, NULL);

  set_rgb_led(0, 0, 128);

  pinMode(PIN_LED1,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED2,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED3,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED4,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED5,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED6,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED7,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED8,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED9,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED10,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED11,GPIO_MODE_OUTPUT);
  pinMode(PIN_LED12,GPIO_MODE_OUTPUT);
  set_led(0x0fff);
}

void get_sensor_data(void)
{
  unsigned long current_time;
  int key_temp_value;
  current_time = millis();
  if (current_time - key_debounced_time > 3)
  {
    key_debounced_time = current_time;
    key_temp_value = digitalRead(g_logic_pin);
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
      g_logic_value = !key_debounced_value;
    }
  }
}

void send_sensor_report_online(void)
{
  uint8_t report_type = REPORT_CURRENT_VALUE; 
  init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
  add_sysex_data(BYTE_8, (void*)(&report_type), ON_LINE);
  add_sysex_data(BYTE_8, (void*)(&g_logic_value), ON_LINE);
  flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
  init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
  add_sysex_data(BYTE_8, (void*)(&g_logic_value), OFF_LINE);
  int16_t uniform_value = line_map(g_logic_value, UNIFORM_MIN, UNIFORM_MAX, 0, 1);
  add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
  flush_sysex_to_send(OFF_LINE);
}

void sysex_message_process(void)
{
  uint8_t block_type = 0;
  uint8_t sub_type = 0;
  uint8_t data_type = 0;
  uint8_t cmd_type = 0;
  uint16_t led_mask = 0;
	
  // read type.
  read_sysex_type(&block_type , &sub_type, ON_LINE);
  if((block_type != g_block_type)||(sub_type != g_block_sub_type))
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
  if(CTL_READ_CURRENT_VALUE == cmd_type)
  {		
    //response mesaage to UART0
    send_sensor_report_online();
  }
  else if(CTL_SET_REPORT_MODE == cmd_type)
  {
    data_type = BYTE_8;
    if(read_next_sysex_data(&data_type, &s_report_mode, ON_LINE) == false)     // read report mode.
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
      if(s_report_period < 10)
      {
        s_report_period = 10;
      }
    }
  }
  else if(CTL_SET_LED == cmd_type)
  {
    data_type = SHORT_16;
    read_next_sysex_data(&data_type, &led_mask, ON_LINE);
    set_led(led_mask);
  }
}

void set_led(uint16_t led_mask)
{
  uint8_t i;
  uint8_t led_value;
  uint16_t led_mask_temp = led_mask;
  for(i=0; i< 12;i++)
  {
    led_value = (led_mask_temp >> i) & 0x0001;
    digitalWrite(led_num[i],!led_value);
  }
}
