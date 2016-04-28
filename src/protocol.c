#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "protocol.h"
#include "uart_printf.h"
#include "smartservo.h"
#include "MePwm.h"
#include "math.h"
#include "systime.h"

uint8_t Uart0SendData[UART0_SEND_BUF_SIZE]={0};
uint8_t Uart1RecData[UART1_REV_BUF_SIZE]={0};
uint8_t Uart1SendData[DEFAULT_UART_BUF_SIZE]={0};

volatile boolean parsingSysex = false;
volatile boolean parsingSysexComplete = true;
volatile boolean parsingGcode = false;
volatile boolean parsingGcodeComplete = true;
volatile boolean isLocalOutput = false;
volatile boolean isForwardOutput = false;

volatile boolean shake_hand_flag = false;

volatile uint16_t sysexBytesRead = 0 ;
volatile uint16_t ForwardBytesRead = 0;
volatile uint16_t Uart0SendBytes = 0;
volatile uint16_t Uart1Revhead  = 0;
volatile uint16_t Uart1RevRtail  = 0;
volatile uint16_t Uart0Sendhead  = 0;
volatile uint16_t Uart0SendRtail  = 0;

uint8_t device_id = 0;              //hardware ID, Uniqueness in a link.
uint8_t device_type = CUSTOM_TYPE;  //device type, Indicates the type of module, 0x00 indicates no external module
uint8_t command_mode = G_CODE_MODE;    //G_CODE_MODE,FIRMATA_DATA_MODE

union sysex_message sysex = {0};

//Sensor value
volatile int16_t report_mode = 0;
volatile int16_t report_smart_pos_mode = 0;
volatile int16_t report_smart_temp_mode = 0;
volatile int16_t report_smart_speed_mode = 0;
volatile int16_t report_smart_voltage_mode = 0;
volatile int16_t report_smart_current_mode = 0;

volatile int16_t smart_servo_pos_val = 0;
volatile int16_t pre_smart_servo_pos_val = 0;

volatile int16_t smart_servo_temp_val = 0;
volatile int16_t pre_smart_servo_temp_val = 0;

volatile int16_t smart_servo_speed_val = 0;
volatile int16_t pre_smart_servo_speed_val = 0;

volatile int16_t smart_servo_voltage_val = 0;
volatile int16_t pre_smart_servo_voltage_val = 0;

volatile int16_t smart_servo_current_val = 0;
volatile int16_t pre_smart_servo_current_val = 0;

volatile unsigned long flush_uart1_forward_time = 0;
volatile unsigned long flush_uart0_local_time = 0;

union{
  uint8_t byteVal[8];
  double doubleVal;
}val8byte;

union{
  uint8_t byteVal[4];
  float floatVal;
  long longVal;
}val4byte;

union{
  uint8_t byteVal[2];
  short shortVal;
}val2byte;

union{
  uint8_t byteVal[1];
  uint8_t charVal;
}val1byte;

uint8_t readbyte(uint8_t *argv,int idx)
{
  uint8_t temp;
  val1byte.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val1byte.byteVal[0] |= temp;
  return val1byte.charVal;
}

short readShort(uint8_t *argv,int idx,boolean ignore_high)
{
  uint8_t temp;
  val2byte.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val2byte.byteVal[0] |= temp;

  val2byte.byteVal[1] = (argv[idx+1] >> 1) & 0x7f;

  //Send analog can ignored high
  if(ignore_high == false)
  {
    temp = (argv[idx+2] << 6);
    val2byte.byteVal[1] |= temp;
  }

  return val2byte.shortVal;
}

float readFloat(uint8_t *argv,int idx){
  uint8_t temp;
  val4byte.byteVal[0] = argv[idx] & 0x7f;
  temp = argv[idx+1] << 7;
  val4byte.byteVal[0] |= temp;

  val4byte.byteVal[1] =  (argv[idx+1] >> 1) & 0x7f;
  temp = (argv[idx+2] << 6);
  val4byte.byteVal[1] += temp;

  val4byte.byteVal[2] =  (argv[idx+2] >> 2) & 0x7f;
  temp = (argv[idx+3] << 5);
  val4byte.byteVal[2] += temp;

  val4byte.byteVal[3] =  (argv[idx+3] >> 3) & 0x7f;
  temp = (argv[idx+4] << 4);
  val4byte.byteVal[3] += temp;

  return val4byte.floatVal;
}

uint8_t sendbyte(uint8_t val)
{
  uint8_t checksum;
  uint8_t val_7bit[2]={0};
  val1byte.charVal = val;
  val_7bit[0] = val1byte.byteVal[0] & 0x7f;
  write_byte_uart0(val_7bit[0]);
  val_7bit[1] = (val1byte.byteVal[0] >> 7) & 0x7f;
  write_byte_uart0(val_7bit[1]);
  checksum = val_7bit[0] + val_7bit[1];
  checksum = checksum & 0x7f;
  return checksum;
}

uint8_t sendShort(int val,boolean ignore_high)
{
  uint8_t checksum;
  uint8_t val_7bit[3]={0};
  val2byte.shortVal = val;
  val_7bit[0] = val2byte.byteVal[0] & 0x7f;
  write_byte_uart0(val_7bit[0]);
  val_7bit[1] = ((val2byte.byteVal[1] << 1) | (val2byte.byteVal[0] >> 7)) & 0x7f;
  write_byte_uart0(val_7bit[1]);
  checksum = val_7bit[0] + val_7bit[1];
  //Send analog can ignored high
  if(ignore_high == false)
  {
    val_7bit[2] = (val2byte.byteVal[1] >> 6) & 0x7f;
    checksum += val_7bit[2];
	checksum = checksum & 0x7f;
    write_byte_uart0(val_7bit[2]);
  }
  return checksum;
}

uint8_t sendFloat(float val)
{
  uint8_t checksum;
  uint8_t val_7bit[5]={0};
  val4byte.floatVal = val;
  val_7bit[0] = val4byte.byteVal[0] & 0x7f;
  write_byte_uart0(val_7bit[0]);
  val_7bit[1] = ((val4byte.byteVal[1] << 1) | (val4byte.byteVal[0] >> 7)) & 0x7f;
  write_byte_uart0(val_7bit[1]);
  val_7bit[2] = ((val4byte.byteVal[2] << 2) | (val4byte.byteVal[1] >> 6)) & 0x7f;
  write_byte_uart0(val_7bit[2]);
  val_7bit[3] = ((val4byte.byteVal[3] << 3) | (val4byte.byteVal[2] >> 5)) & 0x7f;
  write_byte_uart0(val_7bit[3]);
  val_7bit[4] = (val4byte.byteVal[3] >> 4) & 0x7f;
  write_byte_uart0(val_7bit[4]);
  checksum = (val_7bit[0] + val_7bit[1] + val_7bit[2] + val_7bit[3] + val_7bit[4]) & 0x7f;
  return checksum;
}

static void assign_dev_id_response(void *arg)
{
  uint8_t device_id_temp1 = sysex.val.value[0];
  uint8_t device_id_temp2 = sysex.val.value[1];
  uint8_t checksum;
  if(((ALL_DEVICE + CTL_ASSIGN_DEV_ID + device_id_temp1) & 0x7f) != device_id_temp2)
  {
    SendErrorUart0(WRONG_INPUT_DATA);
    return;
  }
  device_id = device_id_temp1 + 1;
  //response mesaage to UART0
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(device_id);
  write_byte_uart0(CTL_ASSIGN_DEV_ID);
  write_byte_uart0(device_type);
  checksum = (device_id + CTL_ASSIGN_DEV_ID + device_type) & 0x7f;
  write_byte_uart0(checksum);
  write_byte_uart0(END_SYSEX);

  //forward mesaage to UART1
  write_byte_uart1(START_SYSEX);
  write_byte_uart1(ALL_DEVICE);
  write_byte_uart1(CTL_ASSIGN_DEV_ID);
  write_byte_uart1(device_id);
  checksum = (ALL_DEVICE + CTL_ASSIGN_DEV_ID + device_id) & 0x7f;
  write_byte_uart1(checksum);
  write_byte_uart1(END_SYSEX);
}

static void set_baud_rate(void *arg)
{
  uint8_t baudrate = sysex.val.value[0];
  SendErrorUart0(PROCESS_SUC);
  if(baudrate == 0)
  {
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
  }
  else if(baudrate == 1)
  {
    /* Init UART0 for printf */
    UART0_Init(921600);
	  
    /* Init UART1*/
    UART1_Init(921600);
	
    /* Set UART Configuration */
    UART_SetLine_Config(UART0, 921600, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);
	  
    /* Set UART Configuration */
    UART_SetLine_Config(UART1, 921600, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);
	  
    /* UART sample function */
    UART_Function_Init();
  }
  else
  {
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
  }
}

static void cmd_test_response(void *arg)
{
  if((sysex.val.value[0] == 0x01) &&
  	 (sysex.val.value[1] == 0x02) &&
  	 (sysex.val.value[2] == 0x03) &&
  	 (sysex.val.value[3] == 0x04) &&
  	 (sysex.val.value[4] == 0x05))
  {
    SendErrorUart0(PROCESS_SUC);
  }
  else
  {
    SendErrorUart0(PROCESS_ERROR);
  }
}

static void system_reset_response(void *arg)
{

}

static void shake_hand_response(void *arg)
{
  shake_hand_flag = true;
  blink_count = 0;
  SendErrorUart0(PROCESS_SUC);
}

static void write_digital_response(void *arg)
{

}

static void write_analog_response(void *arg)
{

}

static void report_digital_response(void *arg)
{

}

static void report_analog_response(void *arg)
{

}

static void set_pin_mode_response(void *arg)
{

}

static void read_digital_response(void *arg)
{

}

static void read_analog_response(void *arg)
{

}

static void pwm_rgb_led_response(void *arg)
{

}

static void ws2812_display_response(void *arg)
{

}

static void monochrome_led_response(void *arg)
{

}

static void segment_dis_response(void *arg)
{

}

static void infrared_send_response(void *arg)
{

}

static void motor_dc_response(void *arg)
{

}

static void motor_step_response(void *arg)
{

}

static void motor_encoder_response(void *arg)
{

}
static void motor_servo_response(void *arg)
{

}

static void voice_buzzer_tone_response(void *arg)
{

}

static void laser_device_response(void *arg)
{

}


static void temperature_18b20_response(void *arg)
{

}

static void mpu6050_response(void *arg)
{

}

static void limit_switch_response(void *arg)
{

}

static void rtc_clock_response(void *arg)
{

}

static void send_smart_pos(short pos_value)
{
  uint8_t checksum;
  //response mesaage to UART0
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(device_id);
  write_byte_uart0(SMART_SERVO);
  write_byte_uart0(GET_SERVO_CUR_POS);
  checksum = (device_id + SMART_SERVO + GET_SERVO_CUR_POS);
  checksum += sendShort(pos_value,false);
  checksum = checksum & 0x7f;
  write_byte_uart0(checksum);
  write_byte_uart0(END_SYSEX);
}

static void send_smart_temp(short temp_value)
{
  uint8_t checksum;
  //response mesaage to UART0
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(device_id);
  write_byte_uart0(SMART_SERVO);
  write_byte_uart0(GET_SERVO_TEMPERATURE);
  checksum = (device_id + SMART_SERVO + GET_SERVO_TEMPERATURE);
  checksum += sendShort(temp_value,false);
  checksum = checksum & 0x7f;
  write_byte_uart0(checksum);
  write_byte_uart0(END_SYSEX);
}

static void send_smart_speed(int16_t speed_value)
{
  uint8_t checksum;
  float speed_temp = speed_value;
  speed_temp = ((speed_temp * (1000.0/SAMPLING_INTERVAL_TIME))/4396) * 60;
  //uart_printf(UART0,"speed_temp(%d)",speed_val);

  //response mesaage to UART0
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(device_id);
  write_byte_uart0(SMART_SERVO);
  write_byte_uart0(GET_SERVO_SPEED);
  checksum = (device_id + SMART_SERVO + GET_SERVO_SPEED);
  checksum += sendFloat(speed_temp);
  checksum = checksum & 0x7f;
  write_byte_uart0(checksum);
  write_byte_uart0(END_SYSEX);
}

static void send_smart_voltage(int16_t vol_value)
{
  uint8_t checksum;
  //response mesaage to UART0
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(device_id);
  write_byte_uart0(SMART_SERVO);
  write_byte_uart0(GET_SERVO_VOLTAGE);
  checksum = (device_id + SMART_SERVO + GET_SERVO_VOLTAGE);
  checksum += sendShort(vol_value,false);
  checksum = checksum & 0x7f;
  write_byte_uart0(checksum);
  write_byte_uart0(END_SYSEX);
}

static void send_smart_current(int16_t cur_value)
{
  uint8_t checksum;
  //response mesaage to UART0
  write_byte_uart0(START_SYSEX);
  write_byte_uart0(device_id);
  write_byte_uart0(SMART_SERVO);
  write_byte_uart0(GET_SERVO_ELECTRIC_CURRENT);
  checksum = (device_id + SMART_SERVO + GET_SERVO_ELECTRIC_CURRENT);
  checksum += sendShort(cur_value,false);
  checksum = checksum & 0x7f;
  write_byte_uart0(checksum);
  write_byte_uart0(END_SYSEX);
}

static void smart_servo_cmd_process(void *arg)
{
  short pos_value;
  float speed;
  uint8_t cmd;
  uint8_t status;
  uint8_t led_r;
  uint8_t led_g;
  uint8_t led_b;
  cmd = sysex.val.value[0];
  switch(cmd)
  {
    case SET_SERVO__PID:
      break;
    case SET_SERVO_ABSOLUTE_POS:
    {
      pos_value = readShort(sysex.val.value,1,false);
      if(sysexBytesRead > 5)
      {
        speed = readFloat(sysex.val.value,4);
      }
      else
      {
        speed = SMART_SERVO_PER_SPPED_MAX;
      }
      smart_servo_move_to(pos_value,speed);
      SendErrorUart0(PROCESS_SUC);
      break;
    }
    case SET_SERVO_RELATIVE_POS:
    {
      pos_value = readShort(sysex.val.value,1,false);
      if(sysexBytesRead > 5)
      {
        speed = readFloat(sysex.val.value,4);
      }
      else
      {
        speed = SMART_SERVO_MAX_OUTPUT;
      }
      smart_servo_move(pos_value,speed);
      SendErrorUart0(PROCESS_SUC);
      break;
    }
    case SET_SERVO_BREAK:
    {
      status = sysex.val.value[1];
      smart_servo_break(status);
      SendErrorUart0(PROCESS_SUC);
      break;
    }
    case SET_SERVO_RGB_LED:
    {
      led_r = readbyte(sysex.val.value,1);
      led_g = readbyte(sysex.val.value,3);
      led_b = readbyte(sysex.val.value,5);
      smart_servo_led(led_r,led_g,led_b);
      SendErrorUart0(PROCESS_SUC);
      break;
    }
    case GET_SERVO_STATUS:
      break;
    case GET_SERVO_PID:
      break;
    case GET_SERVO_CUR_POS:
    {
      report_smart_pos_mode = sysex.val.value[1];
      report_mode += report_smart_pos_mode;
      send_smart_pos(smart_servo_pos_val);
      break;
    }
    case GET_SERVO_SPEED:
    {
      report_smart_speed_mode = sysex.val.value[1];
      report_mode += report_smart_speed_mode;
      send_smart_speed(smart_servo_speed_val);
      break;
    }
    case GET_SERVO_TEMPERATURE:
    {
      report_smart_temp_mode = sysex.val.value[1];
      report_mode += report_smart_temp_mode;
      send_smart_temp(smart_servo_temp_val);
      break;
    }
    case GET_SERVO_ELECTRIC_CURRENT:
      report_smart_current_mode = sysex.val.value[1];
      report_mode += report_smart_current_mode;
      send_smart_current(smart_servo_current_val);
      break;
    case GET_SERVO_VOLTAGE:
      report_smart_voltage_mode = sysex.val.value[1];
      report_mode += report_smart_voltage_mode;
      send_smart_voltage(smart_servo_voltage_val);
      break;			
    default:
      SendErrorUart0(PROCESS_ERROR);
      break;				
  }
}

const Cmd_list_tab_type cmd_list_tab[]={
{CTL_ASSIGN_DEV_ID,NULL,assign_dev_id_response},
{CTL_SYSTEM_RESET,NULL,system_reset_response},
{CTL_SHAKE_HANDS,NULL,shake_hand_response},
{CTL_SET_BAUD_RATE,NULL,set_baud_rate},
{CTL_CMD_TEST,NULL,cmd_test_response},
{CTL_DIGITAL_MESSAGE,NULL,write_digital_response},
{CTL_ANALOG_MESSAGE,NULL,write_analog_response},
{CTL_REPORT_DIGITAL,NULL,report_digital_response},
{CTL_REPORT_ANALOG,NULL,report_analog_response},
{CTL_SET_PIN_MODE,NULL,set_pin_mode_response},
{CTL_READ_DIGITAL,NULL,read_digital_response},
{CTL_READ_ANALOG,NULL,read_analog_response},
{DIS_RGB_LED_PWM,NULL,pwm_rgb_led_response},
{DIS_RGB_LED_WS2812,NULL,ws2812_display_response},
{DIS_MONOCHROME_LED,NULL,monochrome_led_response},
{DIS_SEGMENT_DIS,NULL,segment_dis_response},
{INFRARED_SEND,NULL,infrared_send_response},
{MOTOR_DC,NULL,motor_dc_response},
{MOTOR_STEP,NULL,motor_step_response},
{MOTOR_ENCODER,NULL,motor_encoder_response},
{MOTOR_SERVO,NULL,motor_servo_response},
{VOICE_BUZZER_TONE,NULL,voice_buzzer_tone_response},
{LASER_DEVICE,NULL,laser_device_response},
{TEMPERATURE_18B20,NULL,temperature_18b20_response},
{MPU6050,NULL,mpu6050_response},
{LIMITSWITCH,NULL,limit_switch_response},
{RTC_CLOCK,NULL,rtc_clock_response},
{SMART_SERVO,NULL,smart_servo_cmd_process},
};

#define CMD_LIST_TAB_ARRAY_SIZE sizeof(cmd_list_tab)/ sizeof(cmd_list_tab[0])
/*******************************************************************************
* Function Name  : SendErroruart0
* Description    : .....
* Input          :
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void SendErrorUart0(uint8_t errorcode)
{
  uint8_t checksum;
  if(command_mode == G_CODE_MODE)
  {
    uart0_printf("G%d R%d\r\n",device_id,errorcode);
  }
  else
  {
    write_byte_uart0(START_SYSEX);
    write_byte_uart0(device_id);
    write_byte_uart0(SMART_SERVO);
    write_byte_uart0(CTL_ERROR_CODE);
    write_byte_uart0(errorcode);
    checksum = (device_id + CTL_ERROR_CODE + SMART_SERVO + errorcode) & 0x7f;
    write_byte_uart0(checksum);
    write_byte_uart0(END_SYSEX);
  }
}

/*******************************************************************************
* Function Name  : processSysexMessage
* Description    : .....
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void processSysexMessage(void)
{
  uint8_t i;
  if(parsingSysexComplete == false)
  {
    SendErrorUart0(PROCESS_BUSY);
    parsingSysexComplete = true;
    return;
  }
  parsingSysexComplete = false;
  if(sysex.val.dev_id == ALL_DEVICE || sysex.val.dev_id == device_id)
  {
    //CTL_ASSIGN_DEV_ID should processed one by one
    if((sysex.val.dev_id == ALL_DEVICE) &&
       (sysex.val.srv_id != CTL_ASSIGN_DEV_ID))
    {
      write_byte_uart1(START_SYSEX);
      flush_uart0_forward_buffer();
      write_byte_uart1(END_SYSEX);
    }
    for(i=0;i<CMD_LIST_TAB_ARRAY_SIZE;i++)
    {
      if(cmd_list_tab[i].service_id == sysex.val.srv_id)
      {
        if((device_type != CUSTOM_TYPE) &&
           (sysex.val.srv_id != device_type) &&
           ((sysex.val.srv_id >> 4) != 1))
        {
          SendErrorUart0(WRONG_TYPE_OF_SERVICE);
          parsingSysexComplete = true;
          return;
        }
        else if(cmd_list_tab[i].response_fun != NULL)
        {
          cmd_list_tab[i].response_fun((void*)NULL);
        }
        break;
      }
    }
  }
  else
  {
    write_byte_uart1(START_SYSEX);
    flush_uart0_forward_buffer();
    write_byte_uart1(END_SYSEX);
  }
  parsingSysexComplete = true;
}

/*******************************************************************************
* Function Name  : flush_uart0_forward_buffer
* Description    : .....
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart0_forward_buffer(void)
{
  uint8_t i;
  for(i = 0;i < sysexBytesRead;i++)
  {
    UART_WRITE(UART1, sysex.storedInputData[i]);
    UART_WAIT_TX_EMPTY(UART0);
  }
  sysexBytesRead = 0;
}

/*******************************************************************************
* Function Name  : flush_uart1_forward_buffer
* Description    : flush the uart1 forward data on UART0
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart1_forward_buffer(void)
{
  uint8_t inputData = 0xFF;
  uint16_t tmp;
  tmp = Uart1RevRtail;
  flush_uart1_forward_time = millis();
  while(Uart1Revhead != tmp)
  {
    tmp = Uart1RevRtail;
    inputData = Uart1RecData[Uart1Revhead];
    if(((command_mode == FIRMATA_DATA_MODE) && (inputData == START_SYSEX)) ||
       ((command_mode == G_CODE_MODE) && (inputData == 'G')))
    {
      isForwardOutput = true;
    }
    UART_WRITE(UART0, inputData);
    UART_WAIT_TX_EMPTY(UART0);
    Uart1Revhead = (Uart1Revhead == (UART1_REV_BUF_SIZE - 1)) ? 0 : (Uart1Revhead + 1);
    ForwardBytesRead--;
    if(((command_mode == FIRMATA_DATA_MODE) && (inputData == END_SYSEX)) ||
       ((command_mode == G_CODE_MODE) && (inputData == '\n')))
    {
      isForwardOutput = false;
      break;
    }
	else if((millis() - flush_uart1_forward_time) > 2000)
	{
      isForwardOutput = false;
      break;
	}
  }
}

/*******************************************************************************
* Function Name  : flush_uart0_local_buffer
* Description    : flush the uart0 send buffer
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart0_local_buffer(void)
{
  uint8_t inputData = 0xFF;
  uint16_t tmp;
  tmp = Uart0SendRtail;
  flush_uart0_local_time = millis();
  while(Uart0Sendhead != tmp)
  {
    tmp = Uart0SendRtail;
    inputData = Uart0SendData[Uart0Sendhead];
    if(((command_mode == FIRMATA_DATA_MODE) && (inputData == START_SYSEX)) ||
       ((command_mode == G_CODE_MODE) && (inputData == 'G')))

    {
      isLocalOutput = true;
    }
    UART_WRITE(UART0, inputData);
    UART_WAIT_TX_EMPTY(UART0);
    Uart0Sendhead = (Uart0Sendhead == (UART0_SEND_BUF_SIZE - 1)) ? 0 : (Uart0Sendhead + 1);
    Uart0SendBytes--;
    if(((command_mode == FIRMATA_DATA_MODE) && (inputData == END_SYSEX)) ||
       ((command_mode == G_CODE_MODE) && (inputData == '\n')))

    {
      isLocalOutput = false;
      break;
    }
    else if((millis() - flush_uart0_local_time) > 2000)
	{
      isForwardOutput = false;
      break;
	}
  }
}

/*******************************************************************************
* Function Name  : write_byte_uart0
* Description    : write a byte to the uart0
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void write_byte_uart0(uint8_t inputData)
{
  if(Uart0SendBytes < UART0_SEND_BUF_SIZE - 1)
  {
    /* Enqueue the character */
    Uart0SendData[Uart0SendRtail] = inputData;
    Uart0SendRtail = (Uart0SendRtail == (UART0_SEND_BUF_SIZE - 1)) ? 0 : (Uart0SendRtail + 1);
    Uart0SendBytes++;
  }
  else
  {
    Uart0SendBytes = 0;
    Uart0SendRtail = 0;
    Uart0Sendhead = 0;
  }
}

/*******************************************************************************
* Function Name  : send_uart0_string
* Description    : send string to the uart0 buffer
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void send_uart0_string(char *str)
{
  while(*str)
  {
    write_byte_uart0(*str++);
  }
}

/*******************************************************************************
* Function Name  : uart0_printf
* Description    : send format string to the uart0 buffer
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void uart0_printf(char *fmt,...)
{
  va_list ap;
  char string[64];
  va_start(ap,fmt);
  vsprintf(string,fmt,ap);
  send_uart0_string(string);
  va_end(ap);
}

/*******************************************************************************
* Function Name  : write_byte_uart1
* Description    : write a byte to the uart1
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void write_byte_uart1(uint8_t inputData)
{
  UART_WRITE(UART1, inputData);
  UART_WAIT_TX_EMPTY(UART1);
}

/*******************************************************************************
* Function Name  : send_uart1_string
* Description    : send string to the uart1 buffer
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void send_uart1_string(char *str)
{
  while(*str)
  {
    write_byte_uart1(*str++);
  }
}

/*******************************************************************************
* Function Name  : uart1_printf
* Description    : send format string to the uart1 buffer
* Input          : ....
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void uart1_printf(char *fmt,...)
{
  va_list ap;
  char string[64];
  va_start(ap,fmt);
  vsprintf(string,fmt,ap);
  send_uart1_string(string);
  va_end(ap);
}

void check_digital_report(void)
{

}

void check_analog_report(void)
{

}

void send_sensor_report(void)
{
  if(report_mode != 0x00)
  {
    switch(device_type)
    {
      case SMART_SERVO:
      {
        if(((report_smart_pos_mode == 0x01) &&
            (abs(pre_smart_servo_pos_val - smart_servo_pos_val) > 5)) ||
            (report_smart_pos_mode == 0x02))
        {
          pre_smart_servo_pos_val = smart_servo_pos_val;
          send_smart_pos(smart_servo_pos_val);
        }

        if(((report_smart_temp_mode == 0x01) &&
            (abs(pre_smart_servo_temp_val - smart_servo_temp_val) > 5)) ||
            (report_smart_temp_mode == 0x02))
        {
          pre_smart_servo_temp_val = smart_servo_temp_val;
          send_smart_temp(smart_servo_temp_val);
        }

        if(((report_smart_speed_mode == 0x01) &&
            (abs(pre_smart_servo_speed_val - smart_servo_speed_val) > 2)) ||
            (report_smart_speed_mode == 0x02))
        {
          pre_smart_servo_speed_val = smart_servo_speed_val;
          send_smart_speed(smart_servo_speed_val);
        }

        if(((report_smart_voltage_mode == 0x01) &&
            (abs(pre_smart_servo_voltage_val - smart_servo_voltage_val) > 10)) ||
            (report_smart_voltage_mode == 0x02))
        {
          pre_smart_servo_voltage_val = smart_servo_voltage_val;
          send_smart_voltage(smart_servo_voltage_val);
        }

        if(((report_smart_current_mode == 0x01) &&
            (abs(pre_smart_servo_current_val - smart_servo_current_val) > 10)) ||
            (report_smart_current_mode == 0x02))
        {
          pre_smart_servo_current_val = smart_servo_current_val;
          send_smart_current(smart_servo_current_val);
        }
        break;
      }
      default:
        break;
    }
  }
}

float K_P  =  1.0;
float varP = 0.0004;
float varM = 0.25;
float K = 1.0;
float Kalmanq = 0.0;

int Kalman(float inByte)
{
  K_P = K_P+ varP;
  K = K_P/ (K_P+ varM);
  Kalmanq = K * inByte + (1 - K) * Kalmanq;
  K_P = (1 - K) * K_P; 
  return (int)Kalmanq;
}

void device_neep_loop_in_sampling(void)
{
  int16_t speed,cur_pos;
  cur_pos = adc_get_position_value();
  smart_servo_cur_pos = cur_pos;
  smart_servo_speed_val = normalize_position_difference(cur_pos - pre_pos);
  //smart_servo_speed_val = Kalman(smart_servo_speed_val);
  //uart_printf(UART0,"%d\r\n",smart_servo_speed_val);
  pre_pos = cur_pos;

  if(device_type == SMART_SERVO)
  {
//    if((smart_servo_target_pos != smart_servo_cur_pos) && 
//       (smart_servo_target_pos != -1))
//    {
//      speed = pid_position_to_pwm();
//      smart_servo_speed_update(speed);
//    }
    speed = pid_position_to_pwm();
    smart_servo_speed_update(speed);
  }
}

static void cmd_assign_dev_id(char *cmd)
{
  char * tmp;
  char * str;
  int8_t dev_id;
  str = strtok_r(cmd, " ", &tmp);
	if(str != NULL)
  {
    str = strtok_r(0, " ", &tmp);
  }
  if(str[0]=='D')
  {
    dev_id = atof(str+1);
  }
  device_id = dev_id + 1;
  uart0_printf("G%d M10 T%d\r\n",device_id,device_type);

  uart_printf(UART1,"G%d M10 D%d\r\n",ALL_DEVICE,device_id);
}

static void cmd_set_command_mode(char *cmd)
{
  
}

static void cmd_shake_hand(char *cmd)
{
  shake_hand_flag = true;
  blink_count = 0;
  uart0_printf("G%d M20\r\n",device_id);
}

static void cmd_absolute_pos(char *cmd)
{
  char * tmp;
  char * str;
  int16_t angle;

  float speed = SMART_SERVO_PER_SPPED_MAX;
//  uart_printf(UART0,"cmd_absolute_pos 1:%c",cmd[0]);
//  uart_printf(UART0," %c",cmd[1]);
//  uart_printf(UART0," %c",cmd[2]);
//  uart_printf(UART0," %c",cmd[3]);
//  uart_printf(UART0," %c",cmd[4]);
//  uart_printf(UART0," %c",cmd[5]);
//  uart_printf(UART0," %c",cmd[6]);
//  uart_printf(UART0," %c",cmd[7]);
//  uart_printf(UART0," %c",cmd[8]);
//  uart_printf(UART0," %c\r\n",cmd[9]);
  str = strtok_r(cmd, " ", &tmp);

//  xxx = atoi(str+1);
//  uart_printf(UART0,"cmd_absolute_pos 2:%d\r\n",xxx);

  while(str != NULL)
  {
    str = strtok_r(0, " ", &tmp);
    if(str[0]=='P')
    {
      angle = atof(str+1);
    }
    else if(str[0]=='S')
    {
      speed = atof(str+1);
    }
  }
  smart_servo_move_to(angle,speed);
  uart0_printf("G%d M1 P%d, S%.2f\r\n",device_id,angle,speed);
}

static void cmd_relative_pos(char *cmd)
{
  char * tmp;
  char * str;
  int16_t angle;
  float speed = SMART_SERVO_PER_SPPED_MAX;
  str = strtok_r(cmd, " ", &tmp);
  while(str != NULL)
  {
    str = strtok_r(0, " ", &tmp);
    if(str[0]=='P')
    {
      angle = atof(str+1);
    }
    else if(str[0]=='S')
    {
      speed = atof(str+1);
    }
  }
  smart_servo_move(angle,speed);
  uart0_printf("G%d M2 P%d, S%.2f\r\n",device_id,angle,speed);
}

static void cmd_set_servo_break(char *cmd)
{
  char * tmp;
  char * str;
  int8_t break_value;
  str = strtok_r(cmd, " ", &tmp);
	if(str != NULL)
  {
    str = strtok_r(0, " ", &tmp);
  }
  if(str[0]=='B')
  {
    break_value = atof(str+1);
  }
  smart_servo_break(break_value);
  uart0_printf("G%d M3 B%d\r\n",device_id,break_value);
}

static void cmd_set_rgb_led(char *cmd)
{
  char * tmp;
  char * str;
  uint8_t r_value;
  uint8_t g_value;
  uint8_t b_value;
  str = strtok_r(cmd, " ", &tmp);
  while(str != NULL)
  {
    str = strtok_r(0, " ", &tmp);
    if(str[0]=='R')
    {
      r_value = atof(str+1);
    }
    else if(str[0]=='G')
    {
      g_value = atof(str+1);
    }
    else if(str[0]=='B')
    {
      b_value = atof(str+1);
    }
  }
  smart_servo_led(r_value,g_value,b_value);
  uart0_printf("G%d M4 R%d, G%d, B%d\r\n",device_id,r_value,g_value,b_value);
}

static void cmd_get_servo_pos(char *cmd)
{
  uart0_printf("G%d M5 P%d\r\n",device_id,smart_servo_pos_val);
}

static void cmd_get_servo_speed(char *cmd)
{
  uart0_printf("G%d M6 S%.2f\r\n",device_id,smart_servo_speed_val);
}

static void cmd_get_servo_temperature(char *cmd)
{
  uart0_printf("G%d M7 T%.2f\r\n",device_id,calculate_temp(smart_servo_temp_val));
}

static void cmd_get_servo_current(char *cmd)
{
  uart0_printf("G%d M8 C%.2f\r\n",device_id,calculate_current(smart_servo_current_val));
}

static void cmd_get_servo_voltage(char *cmd)
{
  uart0_printf("G%d M9 V%.2f\r\n",device_id,calculate_voltage(smart_servo_voltage_val));
}

static void parse_mcode(char *cmd)
{
  uint8_t code;
//  uart_printf(UART0,"parse_mcode: ");
//  uart_printf(UART0,"%c",cmd[0]);
//  uart_printf(UART0,"%c",cmd[1]);
//  uart_printf(UART0,"%c",cmd[2]);
//  uart_printf(UART0,"%c",cmd[3]);
//  uart_printf(UART0,"%c",cmd[4]);
//  uart_printf(UART0,"%c",cmd[5]);
//  uart_printf(UART0,"%c",cmd[6]);
//  uart_printf(UART0,"%c",cmd[7]);
//  uart_printf(UART0,"%c",cmd[8]);
//  uart_printf(UART0,"%c",cmd[9]);
//  uart_printf(UART0,"%c",cmd[10]);
//  uart_printf(UART0,"%c",cmd[11]);
//  uart_printf(UART0,"%c",cmd[12]);
//  uart_printf(UART0,"%c",cmd[13]);
//  uart_printf(UART0,"%c",cmd[14]);
//  uart_printf(UART0,"%c",cmd[15]);
//  uart_printf(UART0,"%c",cmd[16]);
  code = atoi((const char *)cmd+1);

  switch(code)
  {
    case 1:
      cmd_absolute_pos(cmd);
      break;
    case 2:
      cmd_relative_pos(cmd);
      break;
    case 3:
      cmd_set_servo_break(cmd);
      break;
    case 4:
      cmd_set_rgb_led(cmd);
      break;
    case 5:
      cmd_get_servo_pos(cmd);
      break;
    case 6:
      cmd_get_servo_speed(cmd);
      break;
    case 7:
      cmd_get_servo_temperature(cmd);
      break;
    case 8:
      cmd_get_servo_current(cmd);
      break;
    case 9:
      cmd_get_servo_voltage(cmd);
      break;
    case 10:
      cmd_assign_dev_id(cmd);
      break;
    case 20:
      cmd_shake_hand(cmd);
      break;
    default:
      break;
  }
}

void parse_gcommand(char *cmd)
{
  uint8_t devid_temp;
  char srv_type = 0;
  devid_temp = atoi((const char *)cmd);
//  uart_printf(UART0,"parse_gcommand %d\r\n",parsingGcodeComplete);
  if(parsingGcodeComplete == false)
  {
//    SendErrorUart0(PROCESS_BUSY);
    parsingGcodeComplete = true;
    return;
  } 
  parsingGcodeComplete = false;
//  uart_printf(UART0,"mcode_0: ");
//  uart_printf(UART0,"%c",sysex.gcode_buffer[0]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[1]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[2]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[3]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[4]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[5]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[6]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[7]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[8]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[9]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[10]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[11]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[12]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[13]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[14]);
//  uart_printf(UART0,"%c",sysex.gcode_buffer[15]);
//  uart_printf(UART0,"%c\r\n",sysex.gcode_buffer[16]);
  if((devid_temp == ALL_DEVICE) || (devid_temp == device_id))
  {
    int p  = strcspn(cmd, "M");
    if(p)
    {
      srv_type = atoi(sysex.gcode_buffer + p + 1);
//      uart_printf(UART0,"srv_type:%d\r\n",srv_type);
      if((devid_temp == ALL_DEVICE) && (srv_type != 10))
      {
        write_byte_uart1('G');
        flush_uart0_forward_buffer();
        write_byte_uart1('\n');
      }
      parse_mcode(sysex.gcode_buffer + p);
//      uart_printf(UART0,"exit 0");
    }
    else
    {
      SendErrorUart0(WRONG_TYPE_OF_SERVICE);
    }
  }
  else
  {
//    uart_printf(UART0,"exit 1");
    write_byte_uart1('G');
    flush_uart0_forward_buffer();
    write_byte_uart1('\n');
  }
  parsingGcodeComplete = true;
//  uart_printf(UART0,"exit 2");
}

