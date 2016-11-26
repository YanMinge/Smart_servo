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
#include "MePwm.h"
#include "Interrupt.h"
#include "dataflash.h"
#include "smartservo.h"
#include "mp9960.h"

#define RESISTANCE_MAX_DEVIATION    30
#define RESISTANCE_VALUE_NOT_EXIST  0xff

/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis
unsigned long report_time;
volatile unsigned long system_time = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void get_sensor_data(void);

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int32_t main(void)
{
  volatile uint32_t u32InitCount = 0;

  pinMode(P1_0,GPIO_PMD_INPUT);
  pinMode(P1_2,GPIO_PMD_INPUT);
  pinMode(P3_4,GPIO_PMD_OUTPUT);
  pinMode(P3_5,GPIO_PMD_OUTPUT); 

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

  /* Enable I2C0 module clock */
  CLK_EnableModuleClock(I2C0_MODULE);

  /* Open SPI */
  mp9960_init();

  set_data_flash_fix_4k();
  command_mode = flash_read_data_mode();
  command_mode = command_mode & 0x01;
  mVersion[4] = command_mode+1+'0';

  angle_pos_offset = (int16_t)flash_read_angle_offset();
  samrt_servo_init();
  device_id = 0;
  device_type = SMART_SERVO;
  delay(50);
  uart_printf(UART0,"V%s\r\n",mVersion);
  while(1)
  {
    parse_uart0_recv_buffer();
    flush_uart1_forward_buffer();	
    flush_uart0_local_buffer();
    get_sensor_data();
    currentMillis = millis();
    if (currentMillis - previousMillis > (SAMPLING_INTERVAL_TIME - 1))
    {
      period_time = currentMillis - previousMillis;
      previousMillis = currentMillis;
      if(shake_hand_flag == true)
      {
        if(blink_count < 10)
        {
          smart_led_blink(500,255,255,255);
        }
        else
        {
          shake_hand_flag = false;
        }
      }
      else
      {
        motor_protection();
      }
      device_neep_loop_in_sampling();
    }
    if (millis() - report_time > 4 * SAMPLING_INTERVAL_TIME)
    {
      report_time = millis();
      send_sensor_report();
    }
  }
}

void get_sensor_data(void)
{ 
  if(device_type == SMART_SERVO)
  {
    smart_servo_angle_val = (long)round((smart_servo_cur_pos * 360.0)/4096.0);
    smart_servo_pos_val = (abs(smart_servo_cur_pos) % 4096);
    smart_servo_temp_val = adc_get_temperature_value();
    smart_servo_voltage_val = adc_get_voltage_value();
    smart_servo_current_val = adc_get_current_value();
  }  
}
