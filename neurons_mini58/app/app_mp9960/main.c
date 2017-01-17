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
#include "mp9960.h"
#include "math.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CTL_READ_CURRENT_VALUE   0x01
#define REPORT_CURRENT_VALUE     0x01
#define SET_CURRENT_ANGLE_ZERO   0x02
#define CTL_SET_REPORT_MODE      0x7f

#define ANGLE_MIN_POSITION  (0)
#define ANGLE_MAX_POSITION  (4095)

static unsigned long s_online_start_mills;       // for comparison with currentMillis
static unsigned long s_offline_start_mills;
static unsigned long s_previous_millis;

static long s_angle_cur_pos = 0;
static long s_angle_value = 0;
static long s_pre_angle_value = 0;
static long s_angle_circular_turn_count = 0;


static uint32_t s_report_period = DEFAULT_REPORT_PERIOD_ON_LINE;
static int16_t s_angle_pos_offset = 0;
static int16_t s_pre_pos = 0;



static uint8_t  s_report_mode = REPORT_MODE_CYCLE;
static boolean  s_report_flag = false;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void angle_update(void);
void sysex_message_process(void);
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
    if((s_report_flag == true) && (g_block_no != 0))
    {
      if(s_report_mode == REPORT_MODE_CYCLE)
      {	
        if((currentMillis - s_online_start_mills > s_report_period))
        {
          s_online_start_mills = currentMillis;
//          uart_printf(UART0,"angle:%ld\r\n",s_angle_value);
          send_sensor_report_online();
        }
      }
      else if(s_report_mode == REPORT_MODE_DIFF)
      {
        if(s_angle_value != s_pre_angle_value)
        {
          s_pre_angle_value = s_angle_value;
          send_sensor_report_online();
        }
      }			  
    }
    // off line.
    else if((s_report_flag == true) && (g_block_no == 0))
    {
      if (currentMillis - s_offline_start_mills > OFF_LINE_REPORT_PERIOD)
      {
        s_offline_start_mills = currentMillis;
        send_sensor_report_offline();
      }
    }
		
    if(currentMillis - s_previous_millis > 10)
    {
      angle_update();
      s_previous_millis = currentMillis;
    }
  }
}


void init_block(void)
{
  g_block_type = CLASS_CONTROL;
  g_block_sub_type = BLOCK_ANGLE_SENSOR;

  CLK_EnableModuleClock(SPI0_MODULE);
  CLK_SetModuleClock(SPI0_MODULE,CLK_CLKSEL1_SPISEL_XTAL,0);

  SYS->P0_MFP |= (SYS_MFP_P04_SPI0_SS | SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO|SYS_MFP_P07_SPI0_CLK); 

  mp9960_init();

  uart0_recv_attach(sysex_message_process, NULL);

  set_rgb_led(128, 0, 0);
}

int16_t normalize_position_difference(int16_t posdiff)
{
  if (posdiff > ((ANGLE_MAX_POSITION - ANGLE_MIN_POSITION) / 2))
  {
    posdiff -= (ANGLE_MAX_POSITION - ANGLE_MIN_POSITION);
  }

  if (posdiff < -((ANGLE_MAX_POSITION - ANGLE_MIN_POSITION) / 2))
  {
    posdiff += (ANGLE_MAX_POSITION - ANGLE_MIN_POSITION);
  }
  return posdiff;
}

void smart_servo_circular_turn_calc(int16_t cur_pos, int16_t pre_pos)
{
  if(abs(cur_pos - pre_pos) > 2048)
  {
    if((pre_pos > 2048) && (cur_pos < 2048))
    {
      s_angle_circular_turn_count = s_angle_circular_turn_count + 1;
    }
    else if((pre_pos < 2048) && (cur_pos > 2048))
    {
      s_angle_circular_turn_count = s_angle_circular_turn_count - 1;
    }
  }
}

int16_t adc_get_position_value(void)
{
  int value = -1;
  value = 4095 - mp9960_read_raw_angle();
  value = value - s_angle_pos_offset;
  if(value < 0)
  {
    value = 4096 + value;
  }
  return value;
}

void angle_update(void)
{
  int16_t cur_pos;
  cur_pos = adc_get_position_value();
  smart_servo_circular_turn_calc(cur_pos,s_pre_pos);
  s_pre_pos = cur_pos;
  s_angle_cur_pos = s_angle_circular_turn_count * 4096 + cur_pos;
}

void get_sensor_data(void)
{
  s_angle_value = round((s_angle_cur_pos * 360.0)/4096.0);
}


void send_sensor_report_online(void)
{
  uint8_t report_type = REPORT_CURRENT_VALUE; 
  init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
  add_sysex_data(BYTE_8, (void*)(&report_type), ON_LINE);
  add_sysex_data(LONG_40, (void*)(&s_angle_value), ON_LINE);
  flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_offline(void)
{
  init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
  add_sysex_data(LONG_40, (void*)(&s_angle_value), OFF_LINE);
  int16_t uniform_value = line_map(s_angle_value, UNIFORM_MIN, UNIFORM_MAX, 0, 4095);
  add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
  flush_sysex_to_send(OFF_LINE);
}

void sysex_message_process(void)
{
  uint8_t block_type = 0;
  uint8_t sub_type = 0;
  uint8_t data_type = 0;
  uint8_t cmd_type = 0;
	
  // read type.
  read_sysex_type(&block_type , &sub_type, ON_LINE);
  if((block_type != g_block_type)||(sub_type != g_block_sub_type))
  {
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
    if(read_next_sysex_data(&data_type, &s_report_mode, ON_LINE) == false) // read report mode.
    {
      return;
    }
    if(s_report_mode == REPORT_MODE_CYCLE)
    {
      data_type = LONG_40;
      if(read_next_sysex_data(&data_type, &s_report_period, ON_LINE) == false) // read report period.
      {
        return;
      }
    }
  }
  else if(SET_CURRENT_ANGLE_ZERO == cmd_type)
  {
    if(s_report_flag == false)
    {
      s_angle_pos_offset = 4095 - mp9960_read_raw_angle();
      set_rgb_led(0, 0, 128);
      s_report_flag = true;
    }
    else
    {
      s_report_flag = false;
      set_rgb_led(128, 0, 0);
    }
  }
}

