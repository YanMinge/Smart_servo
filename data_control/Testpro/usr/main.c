/*************************************************************************
* File Name          :main.c
* Author             : Mark Yan
* Updated            : Mark Yan
* Version            : V21.01.001
* Date               : 11/25/2016
* Description        : Firmware for Makeblock smart servo data board.  
* License            : CC-BY-SA 3.0
* Copyright (C) 2013 - 2016 Maker Works Technology Co., Ltd. All right reserved.
* http://www.makeblock.cc/
**************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "systime.h"
#include "sysinit.h"
#include "mygpio.h"
#include "uart_printf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define POWER_OFF           0
#define POWER_ON            1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool blink_flag = false;
static bool devid_assigned = false;
static bool start_check = false;

static uint8_t servo_num[USART_MAX_INDEX] = {0};
static uint8_t cur_servo_num[USART_MAX_INDEX] = {0};
static uint8_t pre_servo_num[USART_MAX_INDEX] = {0};
static uint8_t power_state = POWER_OFF;
static uint16_t detect_debounce_counter[USART_MAX_INDEX]  = {0};
static uint16_t detect_debounce_match_counter[USART_MAX_INDEX]  = {0};
static uint16_t detect_key_debounce_match_counter = 0;
unsigned long detect_interval_time = 0;
unsigned long detect_key_start_time = 0;
unsigned long detect_key_interval_time = 0;

union sysex_message uart1_sysex;
union sysex_message uart2_sysex;
union sysex_message uart3_sysex;
union sysex_message uart4_sysex;
union sysex_message uart5_sysex;



/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
//static void flush_uart1_local_buffer(void);
//static void uart1_port_check_event_handle(void);
static void uart2_port_check_event_handle(void);
static void uart3_port_check_event_handle(void);
static void uart4_port_check_event_handle(void);
static void uart5_port_check_event_handle(void);
static void uart_port_check(uint8_t uartId);
static void parse_uart1_recv_buffer(void);
static void flush_uart2_forward_buffer(void);
static void flush_uart3_forward_buffer(void);
static void flush_uart4_forward_buffer(void);
static void flush_uart5_forward_buffer(void);
static void flush_uart5_forward_buffer(void);
static void assign_dev_id_response(uint8_t uartId);
static void assign_cmd_to_uart(uint8_t devId);
static void detect_the_num_of_servo(void);
static void power_management(void);
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t delaytime;

  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f0xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f0xx.c file
  */ 
  if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* error */ 
    while (1);
  }
  /* Configure clock GPIO, USARTs */
  RCC_Configuration();
  
  /* Configure GPIO Pin Tx/Rx for Usart communication */
  GPIO_Configuration();
  
  /* Configure NVIC */
  NVIC_Configuration();

  gpio_init();
	
  power_state = POWER_OFF;

  uart_init(USART1,115200);
  uart_init(USART2,115200);
  uart_init(USART3,115200);
  uart_init(USART4,115200);
  uart_init(USART5,115200);

  memset(&uart2_sysex, 0x0, BUFFER_SIZE);
  memset(&uart3_sysex, 0x0, BUFFER_SIZE);
  memset(&uart4_sysex, 0x0, BUFFER_SIZE);
  memset(&uart5_sysex, 0x0, BUFFER_SIZE);
 
  delaytime = millis();
  detect_interval_time = millis();
  /* Infinite Loop */
  while(1)
  {
    if(devid_assigned == false)
    {
      detect_the_num_of_servo();
    }
    else
    {
      flush_uart2_forward_buffer();
      flush_uart3_forward_buffer();
      flush_uart4_forward_buffer();
      flush_uart5_forward_buffer();
    }
    parse_uart1_recv_buffer();
    if(millis() - delaytime > 500)
    {
      delaytime = millis();
      blink_flag = !blink_flag;
    }
    if(blink_flag == false)
    {
      digitalWrite(RUN_LED, HIGH);
    }
    else
    {
      digitalWrite(RUN_LED, LOW);
    }
		power_management();
  }
}

//void flush_uart1_local_buffer(void)
//{
//  uint8_t inputData = 0xFF;	
//  while(Uart1SendHead != Uart1SendRtail)
//  {
//		inputData = Uart1SendData[Uart1SendHead];
//    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
//    USART_SendData(USART1,(uint16_t)inputData);
//    Uart1SendHead = (Uart1SendHead == (BUFFER_SIZE - 1)) ? 0 : (Uart1SendHead + 1);
//    Uart1SendBytes--;
//	}
//}

//void uart1_port_check_event_handle(void)
//{
//  uint8_t inputData = 0xFF;
//  while(Uart1RevHead != Uart1RevRtail)
//  {
//    inputData = Uart1RecData[Uart1RevHead];
//    if(true == uart1ParsingSysex)
//    {
//      if(inputData == END_SYSEX)
//      {
//        uart1ParsingSysex = false;
//        //process fucntion
//      }
//      else
//      {
//        //normal data byte - add to buffer
//        uart1_sysex.storedInputData[Uart1SysexBytesRead] = inputData;
//        Uart1SysexBytesRead++;
//        if(Uart1SysexBytesRead > BUFFER_SIZE-1)
//        {
//          uart1ParsingSysex = false;
//          Uart1SysexBytesRead = 0;
//        }
//      }
//    }
//    else if(inputData == START_SYSEX)
//    {
//      uart1ParsingSysex = true;
//      Uart1SysexBytesRead = 0;
//    }
//    Uart1RevHead = (Uart1RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart1RevHead + 1);
//    Uart1RevBytes--;
//  }
//}

void uart2_port_check_event_handle(void)
{
  uint8_t inputData = 0xFF;
  while(Uart2RevHead != Uart2RevRtail)
  {
    inputData = Uart2RecData[Uart2RevHead];
    if(true == uart2ParsingSysex)
    {
      if(inputData == END_SYSEX)
      {
        uart2ParsingSysex = false;
        //process fucntion
        if(uart2_sysex.val.srv_id == CTL_ASSIGN_DEV_ID_2)
        {
          if(cur_servo_num[UART2] < uart2_sysex.val.dev_id)
          {
            cur_servo_num[UART2] = uart2_sysex.val.dev_id;
          }
        }
      }
      else
      {
        //normal data byte - add to buffer
        uart2_sysex.storedInputData[Uart2SysexBytesRead] = inputData;
        Uart2SysexBytesRead++;
        if(Uart2SysexBytesRead > BUFFER_SIZE-1)
        {
          uart2ParsingSysex = false;
          Uart2SysexBytesRead = 0;
        }
      }
    }
    else if(inputData == START_SYSEX)
    {
      uart2ParsingSysex = true;
      Uart2SysexBytesRead = 0;
    }
    Uart2RevHead = (Uart2RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart2RevHead + 1);
    Uart2RevBytes--;
  }
}

void uart3_port_check_event_handle(void)
{
  uint8_t inputData = 0xFF;
  while(Uart3RevHead != Uart3RevRtail)
  {
    inputData = Uart3RecData[Uart3RevHead];
    if(true == uart3ParsingSysex)
    {
      if(inputData == END_SYSEX)
      {
        uart3ParsingSysex = false;
        //process fucntion
        if(uart3_sysex.val.srv_id == CTL_ASSIGN_DEV_ID_2)
        {
          if(cur_servo_num[UART3] < uart3_sysex.val.dev_id)
          {
            cur_servo_num[UART3] = uart3_sysex.val.dev_id;
          }
        }
      }
      else
      {
        //normal data byte - add to buffer
        uart3_sysex.storedInputData[Uart3SysexBytesRead] = inputData;
        Uart3SysexBytesRead++;
        if(Uart3SysexBytesRead > BUFFER_SIZE-1)
        {
          uart3ParsingSysex = false;
          Uart3SysexBytesRead = 0;
        }
      }
    }
    else if(inputData == START_SYSEX)
    {
      uart3ParsingSysex = true;
      Uart3SysexBytesRead = 0;
    }
    Uart3RevHead = (Uart3RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart3RevHead + 1);
    Uart3RevBytes--;
  }
}

void uart4_port_check_event_handle(void)
{
  uint8_t inputData = 0xFF;
  while(Uart4RevHead != Uart4RevRtail)
  {
    inputData = Uart4RecData[Uart4RevHead];
    if(true == uart4ParsingSysex)
    {
      if(inputData == END_SYSEX)
      {
        uart4ParsingSysex = false;
        //process fucntion
        if(uart4_sysex.val.srv_id == CTL_ASSIGN_DEV_ID_2)
        {
          if(cur_servo_num[UART4] < uart4_sysex.val.dev_id)
          {
            cur_servo_num[UART4] = uart4_sysex.val.dev_id;
          }
        }
      }
      else
      {
        //normal data byte - add to buffer
        uart4_sysex.storedInputData[Uart4SysexBytesRead] = inputData;
        Uart4SysexBytesRead++;
        if(Uart4SysexBytesRead > BUFFER_SIZE-1)
        {
          uart4ParsingSysex = false;
          Uart4SysexBytesRead = 0;
        }
      }
    }
    else if(inputData == START_SYSEX)
    {
      uart4ParsingSysex = true;
      Uart4SysexBytesRead = 0;
    }
    Uart4RevHead = (Uart4RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart4RevHead + 1);
    Uart4RevBytes--;
  }
}

void uart5_port_check_event_handle(void)
{
  uint8_t inputData = 0xFF;
  while(Uart5RevHead != Uart5RevRtail)
  {
    inputData = Uart5RecData[Uart5RevHead];
    if(true == uart5ParsingSysex)
    {
      if(inputData == END_SYSEX)
      {
        uart5ParsingSysex = false;
        //process fucntion
        if(uart5_sysex.val.srv_id == CTL_ASSIGN_DEV_ID_2)
        {
          if(cur_servo_num[UART5] < uart5_sysex.val.dev_id)
          {
            cur_servo_num[UART5] = uart5_sysex.val.dev_id;
          }
        }
      }
      else
      {
        //normal data byte - add to buffer
        uart5_sysex.storedInputData[Uart5SysexBytesRead] = inputData;
        Uart5SysexBytesRead++;
        if(Uart5SysexBytesRead > BUFFER_SIZE-1)
        {
          Uart5SysexBytesRead = false;
          Uart5RevBytes = 0;
        }
      }
    }
    else if(inputData == START_SYSEX)
    {
      uart5ParsingSysex = true;
      Uart5SysexBytesRead = 0;
    }
    Uart5RevHead = (Uart5RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart5RevHead + 1);
    Uart5RevBytes--;
  }
}

void uart_port_check(uint8_t uartId)
{
  uint8_t checksum;
  write_byte_uart(UsartInstance[uartId],START_SYSEX);
  write_byte_uart(UsartInstance[uartId],ALL_DEVICE);
  write_byte_uart(UsartInstance[uartId],CTL_ASSIGN_DEV_ID_2);
  write_byte_uart(UsartInstance[uartId],0x00);
  checksum = (ALL_DEVICE + CTL_ASSIGN_DEV_ID_2 + 0x00) & 0x7f;
  write_byte_uart(UsartInstance[uartId],checksum);
  write_byte_uart(UsartInstance[uartId],END_SYSEX);
  detect_debounce_counter[uartId]++;
  if(cur_servo_num[uartId] == pre_servo_num[uartId])
  {
    detect_debounce_match_counter[uartId]++;
  }
  if(detect_debounce_counter[uartId] == 3)
  {
    if(detect_debounce_match_counter[uartId] >=2 )
    {
      servo_num[uartId] = cur_servo_num[uartId];
    }
    detect_debounce_match_counter[uartId] = 0;
    detect_debounce_counter[uartId] = 0;
  }
  pre_servo_num[uartId] = cur_servo_num[uartId];
	cur_servo_num[uartId] = 0;
}

void parse_uart1_recv_buffer(void)
{
  uint8_t inputData = 0xFF;
  while(Uart1RevHead != Uart1RevRtail)
  {
    inputData = Uart1RecData[Uart1RevHead];
    if(true == uart1ParsingSysex)
    {
      if(inputData == END_SYSEX)
      {
        uart1ParsingSysex = false;
        //process fucntion
				if(uart1_sysex.val.srv_id == CTL_ASSIGN_DEV_ID)
        {
					devid_assigned = true;
          assign_dev_id_response(UART2);
          assign_dev_id_response(UART3);
          assign_dev_id_response(UART4);
          assign_dev_id_response(UART5);
				}
				else
        {
          assign_cmd_to_uart(uart1_sysex.val.dev_id);
				}
      }
      else
      {
        //normal data byte - add to buffer
        uart1_sysex.storedInputData[Uart1SysexBytesRead] = inputData;
        Uart1SysexBytesRead++;
        if(Uart1SysexBytesRead > BUFFER_SIZE-1)
        {
          uart1ParsingSysex = false;
          Uart1SysexBytesRead = 0;
        }
      }
    }
    else if(true == uart1VersionSysex)
    {
      //normal data byte - add to buffer
      uart1_sysex.storedInputData[Uart1SysexBytesRead] = inputData;
      Uart1SysexBytesRead++;
      if(Uart1SysexBytesRead == 5)
      {
        uart1VersionSysex = false;
        Uart1SysexBytesRead = 0;
        if((uart1_sysex.storedInputData[0] == 0x55) &&
           (uart1_sysex.storedInputData[1] == 0x03) &&
           (uart1_sysex.storedInputData[3] == 0x01) &&
           (uart1_sysex.storedInputData[4] == 0x00))
        {
          devid_assigned = true;
          if(servo_num[UART2] != 0)
          {
            write_byte_uart(UsartInstance[UART2],VERSION_READ_SYSEX);
            write_byte_uart(UsartInstance[UART2],0x55);
            write_byte_uart(UsartInstance[UART2],0x03);
            write_byte_uart(UsartInstance[UART2],0x00);
            write_byte_uart(UsartInstance[UART2],0x01);
            write_byte_uart(UsartInstance[UART2],0x00); 
          }
          else if(servo_num[UART3] != 0)
          {
            write_byte_uart(UsartInstance[UART3],VERSION_READ_SYSEX);
            write_byte_uart(UsartInstance[UART3],0x55);
            write_byte_uart(UsartInstance[UART3],0x03);
            write_byte_uart(UsartInstance[UART3],0x00);
            write_byte_uart(UsartInstance[UART3],0x01);
            write_byte_uart(UsartInstance[UART3],0x00); 
          }
          else if(servo_num[UART4] != 0)
          {
            write_byte_uart(UsartInstance[UART4],VERSION_READ_SYSEX);
            write_byte_uart(UsartInstance[UART4],0x55);
            write_byte_uart(UsartInstance[UART4],0x03);
            write_byte_uart(UsartInstance[UART4],0x00);
            write_byte_uart(UsartInstance[UART4],0x01);
            write_byte_uart(UsartInstance[UART4],0x00); 
          }
          else if(servo_num[UART5] != 0)
          {
            write_byte_uart(UsartInstance[UART5],VERSION_READ_SYSEX);
            write_byte_uart(UsartInstance[UART5],0x55);
            write_byte_uart(UsartInstance[UART5],0x03);
            write_byte_uart(UsartInstance[UART5],0x00);
            write_byte_uart(UsartInstance[UART5],0x01);
            write_byte_uart(UsartInstance[UART5],0x00); 
          }
        }
      }
    }
    else if(inputData == START_SYSEX)
    {
      uart1ParsingSysex = true;
      Uart1SysexBytesRead = 0;
    }
    else if(inputData == VERSION_READ_SYSEX)
    {
      uart1VersionSysex = true;
      Uart1SysexBytesRead = 0;
    }
    Uart1RevHead = (Uart1RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart1RevHead + 1);
    Uart1RevBytes--;
  }
}

void flush_uart2_forward_buffer(void)
{
  uint8_t inputData = 0xFF;
  while(Uart2RevHead != Uart2RevRtail)
  {
    inputData = Uart2RecData[Uart2RevHead];
    write_byte_uart(USART1,inputData);
    Uart2RevHead = (Uart2RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart2RevHead + 1);
    Uart2RevBytes--;
  }
}

void flush_uart3_forward_buffer(void)
{
  uint8_t inputData = 0xFF;
  while(Uart3RevHead != Uart3RevRtail)
  {
    inputData = Uart3RecData[Uart3RevHead];
    write_byte_uart(USART1,inputData);
    Uart3RevHead = (Uart3RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart3RevHead + 1);
    Uart3RevBytes--;
  }
}

void flush_uart4_forward_buffer(void)
{
  uint8_t inputData = 0xFF;
  while(Uart4RevHead != Uart4RevRtail)
  {
    inputData = Uart4RecData[Uart4RevHead];
    write_byte_uart(USART1,inputData);
    Uart4RevHead = (Uart4RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart4RevHead + 1);
    Uart4RevBytes--;
  }
}

void flush_uart5_forward_buffer(void)
{
  uint8_t inputData = 0xFF;
  while(Uart5RevHead != Uart5RevRtail)
  {
    inputData = Uart5RecData[Uart5RevHead];
    write_byte_uart(USART1,inputData);
    Uart5RevHead = (Uart5RevHead == (int16_t)(BUFFER_SIZE - 1)) ? 0 : (Uart5RevHead + 1);
    Uart5RevBytes--;
  }
}

void assign_dev_id_response(uint8_t uartId)
{
  uint8_t deviceId;
  uint8_t checksum;
  switch(uartId)
  {
    case UART2:
      deviceId = 0;
      break;
    case UART3:
      deviceId = servo_num[UART2];
      break;
    case UART4:
      deviceId = servo_num[UART2] + servo_num[UART3];
      break;
    case UART5:
      deviceId = servo_num[UART2] + servo_num[UART3] + servo_num[UART4];
      break;
    default:
      break;
	}
  write_byte_uart(UsartInstance[uartId],START_SYSEX);
  write_byte_uart(UsartInstance[uartId],ALL_DEVICE);
  write_byte_uart(UsartInstance[uartId],CTL_ASSIGN_DEV_ID);
  write_byte_uart(UsartInstance[uartId],deviceId);
  checksum = (ALL_DEVICE + CTL_ASSIGN_DEV_ID + deviceId) & 0x7f;
  write_byte_uart(UsartInstance[uartId],checksum);
  write_byte_uart(UsartInstance[uartId],END_SYSEX);
}

void detect_the_num_of_servo(void)
{
  if(millis() - detect_interval_time > 100)
  {
    detect_interval_time = millis();
    uart_port_check(UART2);
    uart_port_check(UART3);
    uart_port_check(UART4);
    uart_port_check(UART5);
    //uart_printf(USART1,"uart2:%d,uart3:%d,uart4:%d,uart5:%d\r\n",servo_num[UART2],servo_num[UART3],servo_num[UART4],servo_num[UART5]);
  }
  uart2_port_check_event_handle();
  uart3_port_check_event_handle();
  uart4_port_check_event_handle();
  uart5_port_check_event_handle();
}


void assign_cmd_to_uart(uint8_t devId)
{
  uint8_t transfer_byte = 0;
  uint8_t uartId = 0;
  if(devId == ALL_DEVICE)
  {
    for(uartId = UART2;uartId <= UART5;uartId++)
    {
      write_byte_uart(UsartInstance[uartId],START_SYSEX);
      for(transfer_byte = 0; transfer_byte <= Uart1SysexBytesRead; transfer_byte++)
      {
        write_byte_uart(UsartInstance[uartId],uart1_sysex.storedInputData[transfer_byte]);
      }
      write_byte_uart(UsartInstance[uartId],END_SYSEX);
    }
  }
  else if(devId <= servo_num[UART2])
  {
    write_byte_uart(USART2,START_SYSEX);
    for(transfer_byte = 0; transfer_byte <= Uart1SysexBytesRead; transfer_byte++)
    {
      write_byte_uart(USART2,uart1_sysex.storedInputData[transfer_byte]);
    }
    write_byte_uart(USART2,END_SYSEX);
  }
  else if((devId > servo_num[UART2]) && (devId <= (servo_num[UART2] + servo_num[UART3])))
  {
    write_byte_uart(USART3,START_SYSEX);
    for(transfer_byte = 0; transfer_byte <= Uart1SysexBytesRead; transfer_byte++)
    {
      write_byte_uart(USART3,uart1_sysex.storedInputData[transfer_byte]);
    }
    write_byte_uart(USART3,END_SYSEX);
  }
  else if((devId > (servo_num[UART2] + servo_num[UART3])) && (devId <= (servo_num[UART2] + servo_num[UART3] + servo_num[UART4])))
  {
    write_byte_uart(USART4,START_SYSEX);
    for(transfer_byte = 0; transfer_byte <= Uart1SysexBytesRead; transfer_byte++)
    {
      write_byte_uart(USART4,uart1_sysex.storedInputData[transfer_byte]);
    }
    write_byte_uart(USART4,END_SYSEX);
  }
  else if(devId > (servo_num[UART2] + servo_num[UART3] + servo_num[UART4]))
  {
    write_byte_uart(USART5,START_SYSEX);
    for(transfer_byte = 0; transfer_byte <= Uart1SysexBytesRead; transfer_byte++)
    {
      write_byte_uart(USART5,uart1_sysex.storedInputData[transfer_byte]);
    }
    write_byte_uart(USART5,END_SYSEX);
  }
}

void power_management(void)
{
  if((digitalRead(GET_ON) == LOW) && (start_check == false))
  {
    start_check = true;
		detect_key_start_time = millis();
		detect_key_interval_time = millis();
		detect_key_debounce_match_counter = 0;
  }
  if((power_state == POWER_OFF) && (start_check == true))
  {
    if(millis() - detect_key_interval_time > 4)
    {
			detect_key_interval_time = millis();
      if(digitalRead(GET_ON) == LOW)
      {
        detect_key_debounce_match_counter++;
			}
			if(millis() - detect_key_start_time > 100)
      {
        detect_key_start_time = millis();
			  if(detect_key_debounce_match_counter > 15)
        {
          digitalWrite(SET_ON,HIGH);
          delay(100);
          digitalWrite(SW_LED,HIGH);
					while(digitalRead(GET_ON) == LOW);
          power_state = POWER_ON;
				}
        detect_key_debounce_match_counter = 0;
			}
		}
	}
	else if((power_state == POWER_ON) && (start_check == true))
  {
    if(millis() - detect_key_interval_time > 4)
    {
			detect_key_interval_time = millis();
      if(digitalRead(GET_ON) == LOW)
      {
        detect_key_debounce_match_counter++;
			}
			if(millis() - detect_key_start_time > 60)
      {
        detect_key_start_time = millis();
			  if(detect_key_debounce_match_counter > 10)
        {
					while(digitalRead(GET_ON) == LOW)
          {
            digitalWrite(SET_ON,LOW);
            digitalWrite(SW_LED,LOW);
					}
          power_state = POWER_OFF;
					delay(100);
				}
				detect_key_debounce_match_counter = 0;
			}
		}
	}
}

