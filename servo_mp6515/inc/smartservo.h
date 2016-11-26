#ifndef __SMARTSERVO_H__
#define __SMARTSERVO_H__

#include "M051Series.h"
#include "mygpio.h"
#include "MePwm.h"
#include "dataflash.h"

typedef enum
{
  SMART_SERVO_DIR_CW = 0,    ///< Clockwise
  SMART_SERVO_DIR_CCW  = 1   ///< Counter-Clockwise
} smart_servo_dir;

//GPIO define
#define SMART_SERVO_LED_B     P2_4
#define SMART_SERVO_LED_R     P2_6
#define SMART_SERVO_LED_G     P2_5

#define SMART_SERVO_SLEEP     P3_6
#define SMART_SERVO_ENBL      P2_2
#define SMART_SERVO_PHASE     P2_3
#define SMART_SERVO_TEMP_AD   P1_3
#define SMART_SERVO_VOL_AD    P1_5
#define SMART_SERVO_CURR_AD   P1_4
#define SMART_SERVO_NFAULT    P3_2

#define SAMPLING_INTERVAL_TIME    10

#define SMART_SERVO_MIN_POSITION  (0)
#define SMART_SERVO_MAX_POSITION  (4095)

#define SMART_SERVO_PER_SPPED_MAX  (40)    // ---- 0.16sec/60   (28.57 rpm)

// The minimum and maximum output.
#define SMART_SERVO_MAX_OUTPUT              (255)
#define SMART_SERVO_MIN_OUTPUT              (-SMART_SERVO_MAX_OUTPUT)
#define SMART_SERVO_POS_DEADBAND            (12)
#define SMART_SERVO_MINIMUM_SPEED           (5)

#define SMART_SERVO_COMMON_MODE             (0)
#define SMART_SERVO_PWM_MODE                (1)


typedef struct
{
  float P, I, D;
  float Setpoint, Output, Integral, differential, last_error;
} PID;

typedef struct
{
  uint8_t R, G, B;
} smart_led_type;

extern volatile boolean pos_lock_flag;
extern volatile boolean protect_flag;
extern volatile boolean release_state_flag;


extern volatile long smart_servo_cur_pos;
extern volatile long smart_servo_target_pos;
extern volatile long smart_servo_pre_target_pos;
extern volatile long smart_servo_circular_turn_count;

extern volatile unsigned long period_time;

extern volatile float smart_servo_target_speed;
extern volatile float smart_servo_cur_speed;


extern volatile int16_t pre_pos;
extern volatile int16_t smart_servo_output;
extern volatile int16_t angle_pos_offset;
extern volatile int16_t motion_mode;
extern volatile int16_t smart_servo_pwm;



extern volatile uint16_t blink_count;
extern smart_led_type smart_led;

extern void samrt_servo_init(void);
extern long smart_servo_distance_togo(void);
extern void smart_servo_led(uint8_t led_r,uint8_t led_g,uint8_t led_b);
extern void smart_led_change(uint8_t led_color);
extern void smart_led_blink(uint16_t blink_time,uint8_t led_r,uint8_t led_g,uint8_t led_b);
extern void smart_servo_break(boolean status);
extern void smart_servo_speed_update(int16_t speed);
extern void smart_servo_move_to(long absolute_angle,float speed);
extern void smart_servo_move(long angle,float speed);
extern SpiFlashOpResult samrt_servo_set_current_angle_zero_degrees(void);
extern void smart_servo_circular_turn_calc(int16_t cur_pos, int16_t pre_pos, float speed_input);
extern void smart_servo_update(void);
extern int16_t normalize_position_difference(int16_t posdiff);
extern int16_t pid_position_to_pwm(void);
extern int16_t adc_get_position_value(void);
extern int16_t adc_get_temperature_value(void);
extern int16_t adc_get_voltage_value(void);
extern int16_t adc_get_current_value(void);
extern int16_t io_get_nfault_value(void);
extern void servo_move_test(float speed);
extern float calculate_temp(int16_t In_temp);
extern float calculate_current(int16_t In_cur);
extern float calculate_voltage(int16_t In_vol);
extern void motor_protection(void);

#endif/* __SMARTSERVO_H__ */
