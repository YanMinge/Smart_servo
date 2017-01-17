#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include "Mini58Series.h"
#include "mygpio.h"
#include <stdio.h>

// the version.
#define DEVICE_TYPE     	21 // neurons device.
#define PROTOCOL_TYPE   	01 // firmata protocol.

// uart buffer size.
#define DEFAULT_UART_BUF_SIZE      128
#define UART0_REV_BUF_SIZE         256
#define UART0_SEND_BUF_SIZE        256
#define UART1_REV_BUF_SIZE         256

// device mode.
#define OFF_LINE 0
#define ON_LINE  1

// protocol, start index, end index.
#define START_SYSEX             0xF0 // online start
#define END_SYSEX               0xF7 // online end
#define START_SYSEX_OFFLINE     0xF1 // offline start
#define END_SYSEX_OFFLINE      	0xF6 // offline end

// data type for firmata.
#define BYTE_8                     0x01
#define BYTE_16                    0x02
#define SHORT_16                   0x03
#define SHORT_24                   0x04
#define LONG_40                    0x05
#define FLOAT_40                   0x06
#define DOUBLE_72                  0x07
#define UNIFORM_16                 0x0f

/* general command for system. */
#define CTL_ASSIGN_DEV_ID                0x10 // Assignment device ID.
#define CTL_SYSTEM_RESET                 0x11 // reset from host.
#define CTL_QUERY_FIRMWARE_VERSION       0x12
#define CTL_SET_BAUDRATE                 0x13
#define CTL_COMMUNICATION_TEST           0x14

/* general response */
#define GENERAL_RESPONSE                 0x15 // general response to host.

#define PROCESS_SUC             0x0F
#define PROCESS_BUSY            0x10
#define EXECUTE_ERROR           0x11
#define WRONG_TYPE              0x12
#define CHECK_ERROR             0x13

/* general command for module */
#define CTL_GENERAL						 0x61

#define CTL_SET_FEEDBACK                 0x01 // set feedback.
#define CTL_SET_RGB_LED                  0x02
#define CTL_FIND_BLOCK                   0x03

/* Block type. */
#define NO_SUBTYPE_BOUND        0x60 // type <=0x60, no subtype.

/* MOTOR */
#define CLASS_MOTOR                     0x62

#define BLOCK_SINGLE_DC_MOTOR           0x01
#define BLOCK_DOUBLE_DC_MOTOR           0x02
#define BLOCK_9G_SERVO                  0x03
#define BLOCK_ENCODER_MOTOR             0x04

/* SENSOR */
#define CLASS_SENSOR                    0x63

#define BLOCK_TEMPERATURE               0x01
#define BLOCK_LIGHT_SENSOR              0x02
#define BLOCK_ULTRASONIC_SENSOR         0x03
#define BLOCK_LINE_FOLLOWER             0x04
#define BLOCK_COLOUR_SENSOR             0x05
#define BLOCK_ACCELEROMETER_GYRO        0x06
#define BLOCK_TEMPERATURE_HUMIDITY      0x07
#define BLOCK_TENSIOMETER               0x08
#define BLOCK_RAINING_SENSOR            0x09
#define BLOCK_WIND_SPEED_SENSOR         0x0a
#define BLOCK_PM_SENSOR                 0x0b
#define BLOCK_PIR_SENSOR                0x0c
#define BLOCK_VOLUME                    0x0d

/* CONTROL */
#define CLASS_CONTROL                   0x64

#define BLOCK_POTENTIOMETER             0x01
#define BLOCK_BUTTON                    0x02
#define BLOCK_ANGLE_SENSOR              0x03
#define BLOCK_MAKEY_MAKEY               0x04
#define BLOCK_TOUCH_KEY                 0x05
#define BLOCK_SELF_LOCKING_SWITCH       0x06
#define BLOCK_JOYSTICK                  0x07
#define BLOCK_SLIGHT_TOUCH_BUTTON       0x08

/* DISPLAY */
#define CLASS_DISPLAY                   0x65

#define BLOCK_NUMERIC_DISPLAY           0x01
#define BLOCK_SINGLE_COLOUR_LED         0x02
#define BLOCK_LIGHT_BAR                 0x03
#define BLOCK_LED_COLOUR_MATRIX_8_8     0x04
#define BLOCK_OLED_DISPLAY              0x05
#define BLOCK_COOL_LIGHT                0x06


/* AUDIO */
#define CLASS_AUDIO                     0x66

#define BLOCK_MP3                       0x01
#define BLOCK_BUZZER                    0x02



/* report mode */
#define REPORT_MODE_REQ         		   0x00 // report when host request current value.
#define REPORT_MODE_DIFF        		   0x01 // report when current vale is differnt from previous value.
#define REPORT_MODE_CYCLE       		   0x02 // report cycle.
#define MIN_REPORT_PERIOD_ON_LINE		   10
#define DEFAULT_REPORT_PERIOD_ON_LINE  40
#define OFF_LINE_REPORT_PERIOD  		   40

#define ALL_DEVICE              		   0xff    // pin included in I2C setup

#define UNIFORM_MAX            		     1023
#define UNIFORM_MIN						         0

extern volatile uint8_t g_block_no;
extern volatile uint8_t g_block_type;
extern volatile uint8_t g_block_sub_type;
extern volatile uint8_t g_handshake_flag;
extern uint16_t g_firmware_version;

extern uint8_t Uart0RecData[UART0_REV_BUF_SIZE]; // uart0 receive buffer.
extern uint8_t Uart1RecData[UART1_REV_BUF_SIZE]; // uart1 receive buffer.

extern volatile uint16_t Uart0Revhead;
extern volatile uint16_t Uart0RevRtail;
extern volatile uint16_t Uart1Revhead;
extern volatile uint16_t Uart1RevRtail;

extern volatile uint16_t Uart0RecvBufferPushBytes;
extern volatile uint16_t Uart0RecvBufferPopBytes;

extern volatile uint16_t Uart1RecvBufferPushBytes;
extern volatile uint16_t Uart1RecvBufferPopBytes;

typedef void(* uart0_recv_cb)(void);

/*******************************************************************************
* Function Name  : send_sysex_return_code
* Description    : return the message result to host.
* Input          : code: the result code. It could be:PROCESS_SUC, PROCESS_BUSY, 
				   PROCESS_ERROR, WRONG_TYPE, CHECK_ERROR
* Output         : None
* Return         : None
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
void send_sysex_return_code(uint8_t code);

void send_sysex_error_code(uint8_t code);
	
/*******************************************************************************
* Function Name  : send_analog_signal_to_host
* Description    : used in on-line mode.
* Input          : type: block type.
				   sub_type: block sub_type.
				   singal_type: used to distinguish the different signal from the same block.
				   value: the read analog value.
* Output         : None
* Return         : None
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
void send_analog_signal_to_host(uint8_t type, uint8_t sub_type, uint8_t signal_type, uint8_t value);

/*******************************************************************************
* Function Name  : send_digital_signal_to_hosts
* Description    : used in on-line mode.
* Input          : type: block type.
				   sub_type: block sub_type.
				   singal_type: used to distinguish the different signal from the same block.
				   value: the read digital value.
* Output         : None
* Return         : None
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
void send_digital_signal_to_host(uint8_t type, uint8_t sub_type, uint8_t signal_type, boolean value);
/*******************************************************************************
* Function Name  : read_uniform_value
* Description    : read uniform value from the off-line package.
* Input          : none
* Output         : node 
* Return         : the uniform value.
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
uint16_t read_uniform_value(void);

/*******************************************************************************
* Function Name  : uniform_convert_to_real
* Description    : convert uniform data to real value.
* Input          : uniform_value: the uniform value.
				   value_min: the minimum value of the real data.
				   value_max: the maximum value of the real data.
* Output         : none.
* Return         : the real value.
* Attention      : 
*******************************************************************************/
float uniform_convert_to_real(int16_t uniform_value, float value_min, float value_max);

/*******************************************************************************
* Function Name  : real_convert_to_uinform
* Description    : convert real value to uniform value.
* Input          : real_data: the real value.
				   value_min: the minimum value of the real data.
				   value_max: the maximum value of the real data.
* Output         : none.
* Return         : the uniform value.
* Attention      : 
*******************************************************************************/
int16_t real_convert_to_uinform(float real_data, float value_min, float value_max);

/*******************************************************************************
* Function Name  : send_analog_signal_to_block
* Description    : used in off-line mode.
* Input          : type, block type.
				   sub_type, block sub_type.
				   value, the read analog value.
* Output         : None
* Return         : None
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
void send_analog_signal_to_block(uint8_t type, uint8_t sub_type, uint8_t real_value, uint16_t uniform_value);

/*******************************************************************************
* Function Name  : send_digital_signal_to_block
* Description    : used in off-line mode.
* Input          : type, block type.
				   sub_type, block sub_type.
				   value, the read digital value.
* Output         : None
* Return         : None
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
void send_digital_signal_to_block(uint8_t type, uint8_t sub_type, boolean value);

/*******************************************************************************
* Function Name  : init_sysex_to_send
* Description    : initialize type and sub type of sending package.
* Input          : type, block type.
				   sub_type, block sub_type.
				   mode, which decided by the sending package is used in on-line or off-line package.
				   it could be ON_LINE, OFF_LINE.
* Output         : None
* Return         : None
* Attention      : if no sub_type, you can give null value.
*******************************************************************************/
void init_sysex_to_send(uint8_t type, uint8_t sub_type, boolean mode);

/*******************************************************************************
* Function Name  : add_sysex_data
* Description    : add a data to the sending package.
* Input          : type, data type, it could be BYTE_8, BYTE_16, SHORT_16, SHORT_24, LONG_40, FLOAT_40, UNIFORM_16 
				   sub_type, block sub_type.
				   value, the pointer which stores the data value, the type of  value should be in accord with the first parameter type!!!!.
				   mode, which decided by the sending package is used in on-line or off-line package.
				   it could be ON_LINE, OFF_LINE.
* Output         : None
* Return         : return true when adding successfully, and return false when adding failed.
* Attention      :
*******************************************************************************/
int8_t add_sysex_data(uint8_t type, void* value, boolean mode);

/*******************************************************************************
* Function Name  : flush_sysex_to_send
* Description    : send the package through the uart0.
* Input          : mode, which decided by the sending package is used in on-line or off-line package.
				   it could be ON_LINE, OFF_LINE.
* Output         : none
* Return         : none
* Attention      : called when adding data finished.
*******************************************************************************/
void flush_sysex_to_send(boolean mode);

/*******************************************************************************
* Function Name  : read_sysex_type
* Description    : read sysex type from received package.
* Input          : mode, which decided by the sending package is used in on-line or off-line package.
				   it could be ON_LINE, OFF_LINE.
* Output         : type, the block type.
				   sub_type, the block type.
* Return         : return true when reading successfully, and return false when reading failed.
* Attention      : 
*******************************************************************************/
int8_t read_sysex_type(uint8_t* type, uint8_t* sub_type, boolean mode);

/*******************************************************************************
* Function Name  : read_next_sysex_data
* Description    : read data from received package.
* Input          : type, on-line mode, it could be BYTE_8, BYTE_16, SHORT_16, SHORT_24, LONG_40, FLOAT_40, UNIFORM_16£¬ 
				   delivered through pointer.
				   mode, which decided by the sending package is used in on-line or off-line package.
				   it could be ON_LINE, OFF_LINE.
* Output         : type, the data type in off-line mode.
				   value, the received data value, delivered through pointer, which type should be accord with the first parameter type in on line mode.
* Return         : return true when reading successfully, and return false when reading failed.
* Attention      : in on-line mode, the type should be assigned with the read data type. but in off-line mode, the type is read from the package.
*******************************************************************************/
int8_t read_next_sysex_data(uint8_t* type, void* value, boolean mode);

/*******************************************************************************
* Function Name  : parse_uart0_recv_buffer
* Description    : parse the uart0 received data
* Input          : none
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void parse_uart0_recv_buffer(void);

/*******************************************************************************
* Function Name  : flush_uart1_to_uart0
* Description    : flush the uart1 received data to uart0.
* Input          : none
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart1_to_uart0(void);
	
/*******************************************************************************
* Function Name  : flush_uart0_to_uart1
* Description    : flush uart0 received data to uart1.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart0_to_uart1(boolean mode);


/*******************************************************************************
* Function Name  : flush_uart0_local_buffer
* Description    : flush local data through uart0.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void flush_uart0_local_buffer(void);


/*******************************************************************************
* Function Name  : uart0_recv_attach
* Description    : register uart0 received sysex processing function.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void uart0_recv_attach(uart0_recv_cb online_recv_cb, uart0_recv_cb offline_recv_cb);
	

#endif

