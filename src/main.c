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

#define RESISTANCE_MAX_DEVIATION    30
#define RESISTANCE_VALUE_NOT_EXIST  0xff

/* timer variables */
unsigned long currentMillis;        // store the current value from millis()
unsigned long previousMillis;       // for comparison with currentMillis
volatile unsigned long system_time = 0;
static uint8_t Previous_service_val = RESISTANCE_VALUE_NOT_EXIST;

unsigned long key_debounced_time;
int key_debounced_count = 0;
int key_match_count = 0;
int key_debounced_value;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void service_detect(void);
void init_local_device(uint8_t service_val);
void get_sensor_data(void);

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
  volatile uint32_t u32InitCount = 0;

  pinMode(P3_2,GPIO_PMD_INPUT);
  pinMode(P3_4,GPIO_PMD_INPUT);
  pinMode(P3_5,GPIO_PMD_INPUT);   
  pinMode(P1_2,GPIO_PMD_INPUT);
  pinMode(P0_5,GPIO_PMD_INPUT);
  pinMode(P0_7,GPIO_PMD_INPUT);

  //    uint32_t MagicWord;
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

//    spi_flash_read(DATA_FLASH_ADDR_BASE,&MagicWord,4);

//    if(MagicWord != FLASH_MAGIC_WORD)  
//    { 
//      MagicWord = FLASH_MAGIC_WORD;
//      spi_flash_write(DATA_FLASH_ADDR_BASE,&MagicWord,4);
//    }
  init_local_device(0x11);
  samrt_servo_init();

  device_id = 0;
  device_type = SMART_SERVO;
  while(1)
  {
    flush_uart1_forward_buffer();	
    flush_uart0_local_buffer();
    get_sensor_data();
    currentMillis = millis();

    if (currentMillis - previousMillis > SAMPLING_INTERVAL_TIME)
    {
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
//      servo_move_test(smart_servo_target_speed*5);
//      uart_printf(UART0,"%d\r\n",adc_get_position_value());
//			uart_printf(UART0,"%d,,,,,%f,,,,,%f\r\n",adc_get_position_value(),calculate_temp(adc_get_temperature_value()),calculate_current(adc_get_current_value())); //print the temp and position.
//      uart_printf(UART0,"%d, %.2f\r\n",adc_get_position_value(), calculate_current(adc_get_current_value()));
//      spi_flash_read(DATA_FLASH_ADDR_BASE,&MagicWord,4);
//      service_detect();
      device_neep_loop_in_sampling(); 
      send_sensor_report();
      previousMillis = currentMillis;
    }
  }
}

uint8_t resistance_value_conversion(int value)
{
  uint8_t result;
  if(abs(value) < RESISTANCE_MAX_DEVIATION)
  {
    result = 0;
  }
  else if(abs(value - 131) < RESISTANCE_MAX_DEVIATION)
  {
    result = 1;
  }
  else if(abs(value - 372) < RESISTANCE_MAX_DEVIATION)
  {
    result = 2;
  }
  else if(abs(value - 625) < RESISTANCE_MAX_DEVIATION)
  {
    result = 3;
  }
  else if(abs(value - 871) < RESISTANCE_MAX_DEVIATION)
  {
    result = 4;
  }
  else if(abs(value - 1149) < RESISTANCE_MAX_DEVIATION)
  {
    result = 5;
  }
  else if(abs(value - 1383) < RESISTANCE_MAX_DEVIATION)
  {
    result = 6;
  }
  else if(abs(value - 1658) < RESISTANCE_MAX_DEVIATION)
  {
    result = 7;
  }
  else if(abs(value - 1845) < RESISTANCE_MAX_DEVIATION)
  {
    result = 8;
  }
  else if(abs(value - 2146) < RESISTANCE_MAX_DEVIATION)
  {
    result = 9;
  }
  else if(abs(value - 2458) < RESISTANCE_MAX_DEVIATION)
  {
    result = 10;
  }
  else if(abs(value - 2633) < RESISTANCE_MAX_DEVIATION)
  {
    result = 11;
  }
  else if(abs(value - 2633) < RESISTANCE_MAX_DEVIATION)
  {
    result = 12;
  }
  else if(abs(value - 2891) < RESISTANCE_MAX_DEVIATION)
  {
    result = 13;
  }
  else if(abs(value - 3143) < RESISTANCE_MAX_DEVIATION)
  {
    result = 14;
  }
  else if(abs(value - 3690) < RESISTANCE_MAX_DEVIATION)
  {
    result = 15;
  }
  else
  {
    result = RESISTANCE_VALUE_NOT_EXIST;
  }
  return result;
}
void service_detect(void)
{
  uint8_t service_val;
//	  uint8_t high_4,low_4;
//    int value_A1,value_A2;
//    value_A1 = analogRead(P1_2);
//    value_A2 = analogRead(P1_3);
//    high_4 = resistance_value_conversion(value_A1);
//    low_4 = resistance_value_conversion(value_A2);

//    if((high_4 == RESISTANCE_VALUE_NOT_EXIST) ||
//       (low_4 == RESISTANCE_VALUE_NOT_EXIST))
//    {
//        service_val = RESISTANCE_VALUE_NOT_EXIST;
//    }
//    else
//    {
//        service_val = ((high_4 & 0x0f) << 4) + (low_4 & 0x0f);
//    }
  service_val = 0x11;
  if(Previous_service_val != service_val)
  {
    Previous_service_val = service_val;
    init_local_device(service_val);
  }
}

void init_local_device(uint8_t service_val)
{
  switch(service_val)
  {
    //WS2812 defalut LED num value is 30
    case 0xf0:
      device_type = DIS_RGB_LED_WS2812;
      break;

    //servo motor
    case 0xa0:
      device_type = MOTOR_SERVO;
      break;
        
    //joystick
    case 0x20:
      device_type = JOYSTICK;
      break;

    //light sensor
    case 0x90:
      device_type = LIGHT_SENSOR;
      break;

    //Key button
    case 0x01:
      //pinMode(P2_3,GPIO_PMD_INPUT);
      device_type = BUTTON_KEY_1;
      break;

    //Servo Test
    case 0x11:
      samrt_servo_init();
      device_type = SMART_SERVO;
      break;


    //Limit switch
    case 0x70:
      pinMode(P2_3,GPIO_PMD_INPUT);
      device_type = LIMITSWITCH;
      break;

    //IR receiver
    case 0x10:
      /* Configure P3.2 external interrupt */
      GPIO_EnableEINT0(P3, 2, GPIO_INT_FALLING);
      NVIC_EnableIRQ(EINT0_IRQn);
      device_type = INFRARED_RECEIVER;
      break;

    //Temprature sensor
    case 0xd0:
      pinMode(P1_4,GPIO_PMD_QUASI);
      device_type = TEMPERATURE_18B20;
      break;

    //RGB LED
    case 0x60:
      pwm_init(P2_3,1000);
      pwm_init(P2_4,1000);
      pwm_init(P2_5,1000);
      device_type = DIS_RGB_LED_PWM;
      break;

    //LED Array
    case 0x50:
      pinMode(P2_3,GPIO_PMD_OUTPUT);
      pinMode(P2_4,GPIO_PMD_OUTPUT);
      pinMode(P2_5,GPIO_PMD_OUTPUT);
      pinMode(P3_2,GPIO_PMD_OUTPUT);
      pinMode(P1_5,GPIO_PMD_OUTPUT);
      pinMode(P1_4,GPIO_PMD_OUTPUT);
      device_type = DIS_MONOCHROME_LED;
      break;

    //7 segment display
    case 0x00:
      pinMode(P1_4,GPIO_PMD_OUTPUT);   //dp
      pinMode(P0_5,GPIO_PMD_OUTPUT);   //g
      pinMode(P0_6,GPIO_PMD_OUTPUT);   //f
      pinMode(P2_5,GPIO_PMD_OUTPUT);   //e
      pinMode(P2_6,GPIO_PMD_OUTPUT);   //d
      pinMode(P1_5,GPIO_PMD_OUTPUT);   //c
      pinMode(P2_3,GPIO_PMD_OUTPUT);   //b
      pinMode(P2_4,GPIO_PMD_OUTPUT);   //a
      device_type = DIS_SEGMENT_DIS;
      break;

    // MIC sensor
    case 0xe0:
      device_type = MIC_SENSOR;
      break;

    // Dc motor
    case 0xb0:
      pinMode(P0_6,GPIO_PMD_OUTPUT);
      digitalWrite(P0_6,1);
      pwm_init(P2_3,1000);
      pwm_init(P2_4,1000);
      pwm_init(P2_5,1000);
      pwm_init(P2_6,1000);
      device_type = MOTOR_DC;
      break;

    //Step motor
    case 0x41:
      device_type = MOTOR_STEP;
      break;

    //laser device
    case 0x30:
      pinMode(P2_3,GPIO_PMD_OUTPUT);
      device_type = LASER_DEVICE;
      break;

    //MPU6050
    case 0x80:
      device_type = MPU6050;
      break;

    case 0x0e:
    case 0x0f:
    default:
      Uart1Revhead  = 0;
      Uart1RevRtail  = 0;
      Uart0Sendhead  = 0;
      Uart0SendRtail  = 0;
      device_type = CUSTOM_TYPE;
      break;
  }
}

void get_sensor_data(void)
{ 
  if(device_type == SMART_SERVO)
  {
    smart_servo_pos_val = adc_get_position_value();
    smart_servo_temp_val = adc_get_temperature_value();
    smart_servo_voltage_val = adc_get_voltage_value();
    smart_servo_current_val = adc_get_current_value();
  }  
}
