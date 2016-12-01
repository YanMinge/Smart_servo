#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__
#include "M051Series.h"
#include "mygpio.h"

#define DEFAULT_UART_BUF_SIZE      128
#define UART0_SEND_BUF_SIZE        128
#define UART1_REV_BUF_SIZE         128
#define UART0_REV_BUF_SIZE         128


/* Define the Service ID*/

/* 0x00-0x0F reserved for user-defined commands */

// extended command set using sysex (0-127/0x00-0x7F)
/* 0x00-0x0F reserved for user-defined commands */

/* Device control&management command */
#define CTL_ASSIGN_DEV_ID       0x10 // Assignment device ID
#define CTL_SYSTEM_RESET        0x11 // reset from host
#define CTL_READ_DEV_VERSION    0x12 // read the firmware version
#define CTL_SET_BAUD_RATE       0x13 // Set the bandrate
#define CTL_CMD_TEST            0x14 // Just for test
#define CTL_ERROR_CODE          0x15 // error code

#define CTL_ASSIGN_DEV_ID_2     0x16 // Assignment device ID 2
#define CTL_DIGITAL_MESSAGE     0x18 // send data for a digital pin
#define CTL_ANALOG_MESSAGE      0x19 // send data for an analog pin (or PWM)
#define CTL_REPORT_DIGITAL      0x1a // enable digital input by port pair
#define CTL_REPORT_ANALOG       0x1b // enable analog input by pin
#define CTL_SET_PIN_MODE        0x1c // set a pin to INPUT/OUTPUT/PWM/etc
#define CTL_READ_DIGITAL        0x1d // If only need read digital once
#define CTL_READ_ANALOG         0x1e // If only need read analog once
#define CTL_SHAKE_HANDS         0x20 // used for shake hands.

/* Control equipment command */
#define DIS_RGB_LED_PWM         0x21 // Common RGB LED lights
#define DIS_RGB_LED_WS2812      0x22 // WS2812 RGB LED lights
    #define SET_RGB_LED_WS2812_S1  0xF0 // WS2812 Compound commands
    #define SET_RGB_LED_WS2812_S2  0xF1 // WS2812 Compound commands
#define DIS_MONOCHROME_LED      0x24    // Monochrome LED lights

#define DIS_SEGMENT_DIS         0x28 // 7 Segment display
#define INFRARED_SEND           0x29 // IR send module
/* motor device command */
#define MOTOR_DC                0x2a // DC motor
#define MOTOR_STEP              0x2b // step motor
    /* Secondary command */

#define MOTOR_ENCODER           0x2c // encoder motor
#define MOTOR_SERVO             0x2d // servo motor
/* Voice service message command */
#define VOICE_BUZZER_TONE       0x2e // buzzer tone
#define LASER_DEVICE            0x2f // laser device



/* Sensor service message command */
#define TEMPERATURE_18B20       0x40
#define MPU6050                 0x41
#define LIMITSWITCH             0x42
#define JOYSTICK                0x43
#define INFRARED_RECEIVER       0x44
#define MIC_SENSOR              0x45
#define LIGHT_SENSOR            0x46
#define RTC_CLOCK               0x47
#define BUTTON_KEY_1            0x48
#define VARIABLE_RESISTOR       0x49


/* It can be replaced for makeblock own device */

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

#define START_SYSEX             0xF0 // start a MIDI Sysex message
#define VERSION_READ_SYSEX      0xFF // read version Sysex message
#define END_SYSEX               0xF7 // end a MIDI Sysex message

/* report error code */
#define PROCESS_SUC             0x0F
#define PROCESS_BUSY            0x10
#define PROCESS_ERROR           0x11
#define WRONG_TYPE_OF_SERVICE   0x12
#define WRONG_INPUT_DATA        0x13

//pin modes
#define INPUT                   0x00
#define OUTPUT                  0x01
#define ANALOG                  0x02 // analog pin in analogInput mode
#define PWM                     0x03 // digital pin in PWM output mode
#define SERVO                   0x04 // digital pin in Servo output mode
#define SHIFT                   0x05 // shiftIn/shiftOut mode
#define I2C                     0x06 // pin included in I2C setup
#define TOTAL_PIN_MODES         7

#define ALL_DEVICE              0xff    // pin included in I2C setup
#define CUSTOM_TYPE             0x00    // 0x00 indicates no external module

//command mode: G code mode and Firmata data mode.
#define G_CODE_MODE             0x01
#define FIRMATA_DATA_MODE       0x00

extern volatile uint8_t device_id;    // hardware ID, Uniqueness in a link.
extern volatile uint8_t device_id_2;  //hardware ID, Uniqueness in a link.
extern volatile uint8_t device_type;  // device type, Indicates the type of module, 0x00 indicates no external module
extern volatile uint8_t command_mode; // Command mode.

extern volatile uint8_t Uart0SendData[UART0_SEND_BUF_SIZE];
extern volatile uint8_t Uart0RecData[UART0_REV_BUF_SIZE];
extern volatile uint8_t Uart1RecData[UART1_REV_BUF_SIZE];

extern volatile boolean parsingSysex;
extern volatile boolean parsingVersionSysex;
extern volatile boolean parsingSysexComplete;
extern volatile boolean parsingGcode;
extern volatile boolean parsingGcodeComplete;
extern volatile boolean isLocalOutput;
extern volatile boolean isForwardOutput;

extern volatile boolean shake_hand_flag;

extern volatile uint16_t sysexBytesRead;
extern volatile int16_t ForwardBytesRead;
extern volatile int16_t Uart0SendBytes;
extern volatile uint16_t Uart1Revhead;
extern volatile uint16_t Uart1RevRtail;
extern volatile uint16_t Uart0Sendhead;
extern volatile uint16_t Uart0SendRtail;
extern volatile int16_t InputBytesRead;
extern volatile uint16_t Uart0Revhead;
extern volatile uint16_t Uart0RevRtail;

extern char mVersion[10];

//Sensor value
extern volatile int16_t report_mode;
extern volatile int16_t report_smart_pos_mode;
extern volatile int16_t report_smart_angle_mode;
extern volatile int16_t report_smart_temp_mode;
extern volatile int16_t report_smart_speed_mode;
extern volatile int16_t report_smart_voltage_mode;
extern volatile int16_t report_smart_current_mode;

extern volatile int16_t smart_servo_pos_val;
extern volatile int16_t pre_smart_servo_pos_val;

extern volatile int16_t smart_servo_temp_val;
extern volatile int16_t pre_smart_servo_temp_val;

extern volatile int16_t smart_servo_voltage_val;
extern volatile int16_t pre_smart_servo_voltage_val;

extern volatile int16_t smart_servo_current_val;
extern volatile int16_t pre_smart_servo_current_val;

extern volatile float smart_servo_speed_val;
extern volatile float pre_smart_servo_speed_val;

extern volatile long smart_servo_angle_val;
extern volatile long pre_smart_servo_angle_val;

extern volatile unsigned long flush_uart1_forward_time;
extern volatile unsigned long flush_uart0_local_time;

typedef struct{
  uint8_t dev_id;
  uint8_t srv_id;
  uint8_t value[DEFAULT_UART_BUF_SIZE - 2];
}sysex_message_type;

typedef struct{
    uint8_t service_id;
    void (*request_fun)(void *arg);
    void (*response_fun)(void *arg);
}Cmd_list_tab_type;

union sysex_message{
  uint8_t storedInputData[DEFAULT_UART_BUF_SIZE];
  sysex_message_type val;
  char gcode_buffer[DEFAULT_UART_BUF_SIZE];
};

extern union sysex_message sysex;
extern const Cmd_list_tab_type cmd_list_tab[];

extern void SendErrorUart0(uint8_t errorcode);
extern void processSysexMessage(void);
extern uint8_t readbyte(uint8_t *argv,int idx);
extern short readShort(uint8_t *argv,int idx,boolean ignore_high);
extern float readFloat(uint8_t *argv,int idx);
extern long readLong(uint8_t *argv,int idx);
extern uint8_t sendbyte(uint8_t val);
extern uint8_t sendShort(int val,boolean ignore_high);
extern uint8_t sendFloat(float val);
extern uint8_t sendLong(long val);
extern void flush_uart0_forward_buffer(void);
extern void flush_uart1_forward_buffer(void);
extern void flush_uart0_local_buffer(void);
extern void parse_uart0_recv_buffer(void);
extern void write_byte_uart0(uint8_t inputData);
extern void send_uart0_string(char *str);
extern void uart0_printf(char *fmt,...);
extern void send_uart1_string(char *str);
extern void uart1_printf(char *fmt,...);
extern void write_byte_uart1(uint8_t inputData);
extern void check_digital_report(void);
extern void check_analog_report(void);
extern void send_sensor_report(void);
extern void device_neep_loop(void);
extern void device_neep_loop_in_sampling(void);
extern void parse_gcommand(char *cmd);

#endif

