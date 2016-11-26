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

/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define SMART_SERVO              0x60
  /* Secondary command */
  #define SET_SERVO_PID                          0x10
  #define SET_SERVO_ABSOLUTE_POS                 0x11
  #define SET_SERVO_RELATIVE_POS                 0x12
  #define SET_SERVO_CONTINUOUS_ROTATION          0x13
  #define SET_SERVO_MOTION_COMPENSATION          0x14
  #define CLR_SERVO_MOTION_COMPENSATION          0x15
  #define SET_SERVO_BREAK                        0x16
  #define SET_SERVO_RGB_LED                      0x17
  #define SERVO_SHARKE_HAND                      0x18
  #define SET_SERVO_CMD_MODE                     0x19

  #define GET_SERVO_STATUS                       0x20
  #define GET_SERVO_PID                          0x21
  #define GET_SERVO_CUR_POS                      0x22
  #define GET_SERVO_SPEED                        0x23
  #define GET_SERVO_MOTION_COMPENSATION          0x24
  #define GET_SERVO_TEMPERATURE                  0x25
  #define GET_SERVO_ELECTRIC_CURRENT             0x26
  #define GET_SERVO_VOLTAGE                      0x27

  #define SET_SERVO_CURRENT_ANGLE_ZERO_DEGREES   0x30
  #define SET_SERVO_ABSOLUTE_ANGLE               0x31
  #define SET_SERVO_RELATIVE_ANGLE               0x32
  #define SET_SERVO_ABSOLUTE_ANGLE_LONG          0x33
  #define SET_SERVO_RELATIVE_ANGLE_LONG          0x34
  #define SET_SERVO_PWM_MOVE                     0x35
  #define GET_SERVO_CUR_ANGLE                    0x36
  #define SET_SERVO_INIT_ANGLE                   0x37
/*---------------------------------------------------------------------------------------------------------*/
/* local variable                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
// typedef
typedef struct{
    uint8_t dev_id;
    uint8_t srv_id;
    uint8_t value[DEFAULT_UART_BUF_SIZE - 2];
}sysex_message_type_s;

union sysex_message_s{
    uint8_t storedInputData[DEFAULT_UART_BUF_SIZE];
    sysex_message_type_s val;
};

union{
  uint8_t byteVal[8];
  double doubleVal;
}val8byte_s;

union{
    uint8_t byteVal[4];
    float floatVal;
    long longVal;
}val4byte_s;

union{
  uint8_t byteVal[2];
  short shortVal;
}val2byte_s;

union{
  uint8_t byteVal[1];
  uint8_t charVal;
}val1byte_s;

static union sysex_message_s s_sysex_0 = {0};
static union sysex_message_s s_sysex_1 = {0};
volatile boolean s_parsingSysexOnline_0 = false;
volatile boolean s_parsingSysexOnline_1 = false;
volatile boolean flash_sensor_data = false;
volatile boolean sensor_data_ok = false;
volatile boolean uart0_ready = false;
volatile boolean uart1_ready = false;
volatile uint16_t s_sysexBytesRead_0 = 0;
volatile uint16_t s_sysexBytesRead_1 = 0;
volatile uint16_t resFlag0 = 0x00;
volatile uint16_t resFlag1 = 0x00;
uint16_t key_debounced_count = 0;
uint16_t key_match_count = 0;
uint16_t key_debounced_value;
unsigned long cmdTimeOutValue0;
unsigned long cmdTimeOutValue1;
unsigned long reportSensor;
unsigned long key_debounced_time;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
volatile boolean button_key_val;
volatile boolean pre_button_key_val;
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void sysex_message_process(void);
void parse_uart0_recv_buffer_s(void);
void parse_uart1_recv_buffer_s(void);
void process_sensor_data(void *arg);
void sysex_message_process_uart0(void);
void sysex_message_process_uart1(void);
boolean assign_dev_id_request_0(void);
boolean assign_dev_id_request_1(void);
void enable_or_disable_sensor(void);
void assign_dev_id_response_0(void *arg);
void assign_dev_id_response_1(void *arg);
void error_code_check_response_0(void *arg);
void error_code_check_response_1(void *arg);
boolean moveTo(uint8_t dev_id,long angle_value,float speed);
boolean servo_init(void);
/*---------------------------------------------------------------------------------------------------------*/
/* Local Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static void write_byte_uart0(uint8_t inputData)
{
    UART_WRITE(UART0, inputData);
    UART_WAIT_TX_EMPTY(UART0);
}

static void write_byte_uart1(uint8_t inputData)
{
    UART_WRITE(UART1, inputData);
    UART_WAIT_TX_EMPTY(UART1);
}

static uint8_t readbyte(uint8_t *argv,int idx)
{
  uint8_t temp;
  val1byte_s.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val1byte_s.byteVal[0] |= temp;
  return val1byte_s.charVal;
}

static short readShort(uint8_t *argv,int idx,boolean ignore_high)
{
  uint8_t temp;
  val2byte_s.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val2byte_s.byteVal[0] |= temp;

  val2byte_s.byteVal[1] = (argv[idx+1] >> 1) & 0x7f;

  //Send analog can ignored high
  if(ignore_high == false)
  {
    temp = (argv[idx+2] << 6);
    val2byte_s.byteVal[1] |= temp;
  }
  return val2byte_s.shortVal;
}

static float readfloat(uint8_t *argv,int idx)
{
  uint8_t temp;
  val4byte_s.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val4byte_s.byteVal[0] |= temp;

  val4byte_s.byteVal[1] =  (argv[idx+1] >> 1) & 0x7f;
  temp = (argv[idx+2] << 6);
  val4byte_s.byteVal[1] += temp;

  val4byte_s.byteVal[2] =  (argv[idx+2] >> 2) & 0x7f;
  temp = (argv[idx+3] << 5);
  val4byte_s.byteVal[2] += temp;

  val4byte_s.byteVal[3] =  (argv[idx+3] >> 3) & 0x7f;
  temp = (argv[idx+4] << 4);
  val4byte_s.byteVal[3] += temp;

  return val4byte_s.floatVal;
}

static long readlong(uint8_t *argv,int idx)
{
  uint8_t temp;
  val4byte_s.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val4byte_s.byteVal[0] |= temp;

  val4byte_s.byteVal[1] =  (argv[idx+1] >> 1) & 0x7f;
  temp = (argv[idx+2] << 6);
  val4byte_s.byteVal[1] += temp;

  val4byte_s.byteVal[2] =  (argv[idx+2] >> 2) & 0x7f;
  temp = (argv[idx+3] << 5);
  val4byte_s.byteVal[2] += temp;

  val4byte_s.byteVal[3] =  (argv[idx+3] >> 3) & 0x7f;
  temp = (argv[idx+4] << 4);
  val4byte_s.byteVal[3] += temp;

  return val4byte_s.longVal;
}

static void sendbyte(int8_t val)
{
//    uint8_t val_7bit[2]={0};
//    val1byte.charVal = val;
//    val_7bit[0] = val1byte.byteVal[0] & 0x7f;
//	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[0];
//    val_7bit[1] = (val1byte.byteVal[0] >> 7) & 0x7f;
//	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[1];
}

static uint8_t sendShort(int val,boolean ignore_high)
{
	  uint8_t checksum;
    uint8_t val_7bit[3]={0};
    val2byte_s.shortVal = val;
    val_7bit[0] = val2byte_s.byteVal[0] & 0x7f;
    write_byte_uart1(val_7bit[0]);
    val_7bit[1] = ((val2byte_s.byteVal[1] << 1) | (val2byte_s.byteVal[0] >> 7)) & 0x7f;
    write_byte_uart1(val_7bit[1]);
    checksum = val_7bit[0] + val_7bit[1];
    //Send analog can ignored high
    if(ignore_high == false)
    {
        val_7bit[2] = (val2byte_s.byteVal[1] >> 6) & 0x7f;
        checksum += val_7bit[2];
        checksum = checksum & 0x7f;
        write_byte_uart1(val_7bit[2]);
    }
		return checksum;
}

static void sendFloat(float val)
{
//    uint8_t val_7bit[5]={0};
//    val4byte.floatVal = (float)val;
//    val_7bit[0] = val4byte.byteVal[0] & 0x7f;
//    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[0];
//    val_7bit[1] = ((val4byte.byteVal[1] << 1) | (val4byte.byteVal[0] >> 7)) & 0x7f;
//    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[1];
//    val_7bit[2] = ((val4byte.byteVal[2] << 2) | (val4byte.byteVal[1] >> 6)) & 0x7f;
//    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[2];
//    val_7bit[3] = ((val4byte.byteVal[3] << 3) | (val4byte.byteVal[2] >> 5)) & 0x7f;
//	sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[3];
//    val_7bit[4] = (val4byte.byteVal[3] >> 4) & 0x7f;
//    sysex_to_send.storedInputData[s_to_send_bytes_count++] = val_7bit[4];
}

static uint8_t sendLong(long val)
{
    uint8_t checksum;
    uint8_t val_7bit[5]={0};
    val4byte_s.longVal = val;
    val_7bit[0] = val4byte_s.byteVal[0] & 0x7f;
    write_byte_uart1(val_7bit[0]);
    val_7bit[1] = ((val4byte_s.byteVal[1] << 1) | (val4byte_s.byteVal[0] >> 7)) & 0x7f;
    write_byte_uart1(val_7bit[1]);
    val_7bit[2] = ((val4byte_s.byteVal[2] << 2) | (val4byte_s.byteVal[1] >> 6)) & 0x7f;
    write_byte_uart1(val_7bit[2]);
    val_7bit[3] = ((val4byte_s.byteVal[3] << 3) | (val4byte_s.byteVal[2] >> 5)) & 0x7f;
    write_byte_uart1(val_7bit[3]);
    val_7bit[4] = (val4byte_s.byteVal[3] >> 4) & 0x7f;
    write_byte_uart1(val_7bit[4]);
    checksum = (val_7bit[0] + val_7bit[1] + val_7bit[2] + val_7bit[3] + val_7bit[4]) & 0x7f;
    return checksum;
}

void debounced_read_button_key(void)
{
    unsigned long current_time;
    int key_temp_value;
    current_time = millis();
    if (current_time - key_debounced_time > 3)
    {
        key_debounced_time = current_time;
        key_temp_value = digitalRead(P3_6);
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
//	debounced_read_button_key();
	button_key_val = digitalRead(P3_6);
  if(button_key_val != pre_button_key_val)
  {
    pre_button_key_val = button_key_val;
//    uart_printf(UART0,"button_key_val:%d\r\n",button_key_val);
    if(button_key_val == true)
    {
        enable_or_disable_sensor();
        set_rgb_led(128, 128, 128);
        servo_init();
        delay(100);
    }
  }
}

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

	reportSensor = cmdTimeOutValue0 = cmdTimeOutValue1 = millis();
	
	set_rgb_led(0, 0, 0);
	
	pinMode(P3_6,0x0UL);

  while(1)
  {
		parse_uart0_recv_buffer_s();
		parse_uart1_recv_buffer_s();
    get_sensor_data();
		uint32_t currentMillis = millis();
		if(currentMillis - reportSensor > 500)
    {
			reportSensor = currentMillis;
      if(assign_dev_id_request_0() == true)
      {
//        uart_printf(UART0,"uart0_ready\r\n");
				uart0_ready = true;
			}
			else
      { 
//        uart_printf(UART0,"uart0 not ready\r\n");
				uart0_ready = false;
      }
      if(assign_dev_id_request_1() == true)
      {
//        uart_printf(UART0,"uart1_ready\r\n");
				uart1_ready = true;
			}
			else
      {
//        uart_printf(UART0,"uart1 not ready\r\n");
				uart1_ready = false;
			}
		}

		if((uart0_ready == true) && (uart1_ready == true))
    {
			sensor_data_ok = true;
		}
    else
		{
      sensor_data_ok = false;
		}

	  if((sensor_data_ok == true) && (flash_sensor_data == true))
    {
      set_rgb_led(0, 0, 128);
		}
    else if((uart0_ready == true) && (uart1_ready == true))
    {
      set_rgb_led(128, 0, 128);
		}
		else if((uart0_ready == false) && (uart1_ready == true))
    {
      set_rgb_led(0, 128, 0);
		}
		else if((uart1_ready == false) && (uart0_ready == true))
    {
      set_rgb_led(128, 0, 0);
		}
		else
    {
      set_rgb_led(0,0,0);
		}
  }
}

void parse_uart0_recv_buffer_s(void)
{
	uint8_t inputData = 0xFF;
	uint16_t tmp;
  tmp = Uart0RevRtail;
	while(Uart0Revhead != tmp)
  {
    tmp = Uart0RevRtail;
    inputData = Uart0RecData[Uart0Revhead];
    if(s_parsingSysexOnline_0)
		{
      if (inputData == END_SYSEX)
      {
        //end of sysex
        s_parsingSysexOnline_0 = false;
        sysex_message_process_uart0();
      }
			else
			{
				//normal data byte - add to buffer
				s_sysex_0.storedInputData[s_sysexBytesRead_0] = inputData;
				s_sysexBytesRead_0++;
				if(s_sysexBytesRead_0 > DEFAULT_UART_BUF_SIZE-1)
				{
					s_parsingSysexOnline_0 = false;
					s_sysexBytesRead_0 = 0;
				}
			}
		}
		else if(inputData == START_SYSEX)
		{
			s_parsingSysexOnline_0 = true;
			s_sysexBytesRead_0 = 0;
		}
		Uart0Revhead = (Uart0Revhead == (UART0_REV_BUF_SIZE - 1)) ? 0 : (Uart0Revhead + 1);
    Uart0RecvBufferPopBytes++;
	}
}

void parse_uart1_recv_buffer_s(void)
{
	uint8_t inputData = 0xFF;
	uint16_t tmp;
  tmp = Uart1RevRtail;
	while(Uart1Revhead != tmp)
  {
    tmp = Uart1RevRtail;
    inputData = Uart1RecData[Uart1Revhead];
    if(s_parsingSysexOnline_1)
		{
      if (inputData == END_SYSEX)
      {
        //end of sysex
        s_parsingSysexOnline_1 = false;
        sysex_message_process_uart1();
      }
			else
			{
				//normal data byte - add to buffer
				s_sysex_1.storedInputData[s_sysexBytesRead_1] = inputData;
				s_sysexBytesRead_1++;
				if(s_sysexBytesRead_1 > DEFAULT_UART_BUF_SIZE-1)
				{
					s_parsingSysexOnline_1 = false;
					s_sysexBytesRead_1 = 0;
				}
			}
		}
		else if(inputData == START_SYSEX)
		{
			s_parsingSysexOnline_1 = true;
			s_sysexBytesRead_1 = 0;
		}
		Uart1Revhead = (Uart1Revhead == (UART1_REV_BUF_SIZE - 1)) ? 0 : (Uart1Revhead + 1);
    Uart1RecvBufferPopBytes++;
	}
}

void sysex_message_process_uart0(void)
{
  if(s_sysex_0.val.dev_id != ALL_DEVICE)
  {
    switch(s_sysex_0.val.srv_id)
    {
      case CTL_ASSIGN_DEV_ID:
        assign_dev_id_response_0((void*)NULL);
        break;
      case GENERAL_RESPONSE:
        error_code_check_response_0((void*)NULL);
        break;
      case CLASS_CONTROL:
        process_sensor_data((void*)NULL);
        break;
      default:
        break;
    }
  } 
}

void sysex_message_process_uart1(void)
{
  if(s_sysex_1.val.dev_id != ALL_DEVICE)
  {
    switch(s_sysex_1.val.srv_id)
    {
      case CTL_ASSIGN_DEV_ID:
        assign_dev_id_response_1((void*)NULL);
        break;
      case GENERAL_RESPONSE:
        error_code_check_response_1((void*)NULL);
        break;
      default:
        break;
    }
  } 
}

boolean assign_dev_id_request_0(void)
{
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(ALL_DEVICE);
  write_byte_uart0(CTL_ASSIGN_DEV_ID);
  write_byte_uart0(0x00);
  write_byte_uart0(0x0f); 
  write_byte_uart0(END_SYSEX);
  resFlag0 &= 0xfe;
  cmdTimeOutValue0 = millis();
  while((resFlag0 & 0x01) != 0x01)
  {
    parse_uart0_recv_buffer_s();
		get_sensor_data();
    if(millis() - cmdTimeOutValue0 > 2000)
    {
      resFlag0 &= 0xfe;
      return false;
    }
  }
  resFlag0 &= 0xfe;
  return true;
}

boolean assign_dev_id_request_1(void)
{
  write_byte_uart1(START_SYSEX);
  write_byte_uart1(ALL_DEVICE);
  write_byte_uart1(CTL_ASSIGN_DEV_ID);
  write_byte_uart1(0x00);
  write_byte_uart1(0x0f); 
  write_byte_uart1(END_SYSEX);
  resFlag1 &= 0xfe;
  cmdTimeOutValue1 = millis();
  while((resFlag1 & 0x01) != 0x01)
  {
    parse_uart1_recv_buffer_s();
		get_sensor_data();
    if(millis() - cmdTimeOutValue1 > 5000)
    {
      resFlag1 &= 0xfe;
      return false;
    }
  }
  resFlag1 &= 0xfe;
  return true;
}

void enable_or_disable_sensor(void)
{
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(ALL_DEVICE);
  write_byte_uart0(CLASS_CONTROL);
  write_byte_uart0(ANGLE_SENSOR);
  write_byte_uart0(0x02); 
  write_byte_uart0(0x68);
  write_byte_uart0(END_SYSEX);
}

void process_sensor_data(void *arg)
{
  //The arg from value[0]
  uint8_t DeviceId = 0;
  uint8_t ServiceId = 0;
  uint8_t subSevice = 0;
  uint8_t reportType = 0;
	long angle;
  DeviceId = s_sysex_0.val.dev_id;
  ServiceId = s_sysex_0.val.srv_id;
  subSevice = s_sysex_0.val.value[0];
	reportType = s_sysex_0.val.value[1];
	angle = readlong(s_sysex_0.val.value,2);
//  uart_printf(UART0,"reportType:%d,angle:%ld\r\n",reportType,angle);
	if(DeviceId == 2)
  {
   angle = -angle;
	}
	moveTo(DeviceId,angle,40);
	flash_sensor_data = true;
}

void assign_dev_id_response_0(void *arg)
{
  //The arg from value[0]
  uint8_t DeviceId = 0;
  uint8_t ServiceId = 0;
  DeviceId = s_sysex_0.val.dev_id;
  ServiceId = s_sysex_0.val.srv_id;
  resFlag0 |= 0x01;
}

void error_code_check_response_0(void *arg)
{
  uint8_t DeviceId = 0;
  uint8_t ServiceId = 0;
  uint8_t errorcode = CHECK_ERROR;
  DeviceId = s_sysex_0.val.dev_id;
  ServiceId = s_sysex_0.val.srv_id;
  if(ServiceId == GENERAL_RESPONSE)
  {
    errorcode = s_sysex_0.val.value[0];
    resFlag0 |= 0x40;
  }
}

void assign_dev_id_response_1(void *arg)
{
  //The arg from value[0]
  uint8_t DeviceId = 0;
  uint8_t ServiceId = 0;
  DeviceId = s_sysex_1.val.dev_id;
  ServiceId = s_sysex_1.val.srv_id;
  resFlag1 |= 0x01;
}

void error_code_check_response_1(void *arg)
{
  uint8_t DeviceId = 0;
  uint8_t ServiceId = 0;
  uint8_t errorcode = CHECK_ERROR;
  DeviceId = s_sysex_1.val.dev_id;
  ServiceId = s_sysex_1.val.srv_id;
  if(ServiceId == GENERAL_RESPONSE)
  {
    errorcode = s_sysex_1.val.value[0];
    resFlag1 |= 0x40;
  }
}

boolean moveTo(uint8_t dev_id,long angle_value,float speed)
{
  uint8_t checksum;
  write_byte_uart1(START_SYSEX);
  write_byte_uart1(dev_id);
  write_byte_uart1(SMART_SERVO);
  write_byte_uart1(SET_SERVO_ABSOLUTE_ANGLE_LONG);
  checksum = (dev_id + SMART_SERVO + SET_SERVO_ABSOLUTE_ANGLE_LONG);
  checksum += sendLong(angle_value);
  checksum = checksum & 0x7f;
  checksum += sendShort((int)speed,true);
  checksum = checksum & 0x7f;
  write_byte_uart1(checksum);
  write_byte_uart1(END_SYSEX);
  return true;
}

boolean servo_init(void)
{
  uint8_t checksum;
  write_byte_uart1(START_SYSEX);
  write_byte_uart1(ALL_DEVICE);
  write_byte_uart1(SMART_SERVO);
  write_byte_uart1(SET_SERVO_INIT_ANGLE);
  write_byte_uart1(0x00);
	checksum = (ALL_DEVICE + SMART_SERVO + SET_SERVO_INIT_ANGLE) & 0x7f;
	checksum += sendShort(40,true);
  checksum = checksum & 0x7f;
  write_byte_uart1(checksum);
  write_byte_uart1(END_SYSEX);
  return true;
}