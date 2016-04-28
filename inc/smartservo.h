#ifndef __SMARTSERVO_H__
#define __SMARTSERVO_H__

#include "M051Series.h"
#include "mygpio.h"
#include "MePwm.h"

typedef enum
{
  SMART_SERVO_DIR_CW = 0,    ///< Clockwise
  SMART_SERVO_DIR_CCW  = 1   ///< Counter-Clockwise
} smart_servo_dir;

//GPIO define
#define SMART_SERVO_LED_B     P2_4     //P2.4
#define SMART_SERVO_LED_R     P2_6     //P2.5
#define SMART_SERVO_LED_G     P2_5

#define SMART_SERVO_SLEEP     P3_6
#define SMART_SERVO_PW1       P2_2
#define SMART_SERVO_PW2       P2_3
#define SMART_SERVO_DIR_AD    P1_0     //P1.4
#define SMART_SERVO_TEMP_AD   P1_3
#define SMART_SERVO_VOL_AD    P1_5
#define SMART_SERVO_CURR_AD   P1_4     //P1.0
#define SMART_SERVO_NFAULT    P0_4

#define SAMPLING_INTERVAL_TIME    10

#define SMART_SERVO_MIN_POSITION  (0)
#define SMART_SERVO_MAX_POSITION  (4096)

#define SMART_SERVO_MIN_LIM_POS  (200)
#define SMART_SERVO_MAX_LIM_POS  (3900)

#define SMART_SERVO_PER_SPPED_MAX  (40)    //(rpm/60)*(4096*SamplingTime/1000)  ---- 0.35sec/60   (30rpm ~ 20/10ms)

// The minimum and maximum output.
#define SMART_SERVO_MAX_OUTPUT              (255)
#define SMART_SERVO_MIN_OUTPUT              (-SMART_SERVO_MAX_OUTPUT)
#define SMART_SERVO_POS_DEADBAND            (5)
#define SMART_SERVO_MINIMUM_SPEED           (5)

typedef struct
{
  float P, I, D;
  float Setpoint, Output, Integral, differential, last_error;
} PID;

extern volatile boolean pos_lock_flag;
extern int16_t smart_servo_pre_pos;
extern volatile int16_t pre_pos;
extern volatile int16_t smart_servo_cur_pos;
extern int16_t smart_servo_target_pos;
extern int16_t smart_servo_pre_target_pos;
extern float smart_servo_target_speed;
extern int16_t smart_servo_output;
extern volatile uint16_t blink_count;

extern void samrt_servo_init(void);
extern long smart_servo_distance_togo(void);
extern void smart_servo_led(uint8_t led_r,uint8_t led_g,uint8_t led_b);
extern void smart_led_change(uint8_t led_color);
extern void smart_led_blink(uint16_t blink_time,uint8_t led_r,uint8_t led_g,uint8_t led_b);
extern void smart_servo_break(uint8_t status);
extern int16_t adc_get_position_value(void);
extern int16_t adc_get_temperature_value(void);
extern int16_t adc_get_voltage_value(void);
extern int16_t adc_get_current_value(void);
extern int16_t io_get_nfault_value(void);
extern void smart_servo_move_to(int16_t absolute_angle,float speed);
extern void smart_servo_move(int16_t angle,float speed);
extern int16_t normalize_position_difference(int16_t posdiff);
extern int16_t pid_position_to_pwm(void);
extern void smart_servo_speed_update(int16_t speed);
extern void servo_move_test(float speed);
extern float calculate_temp(int16_t In_temp);
extern float calculate_current(int16_t In_cur);
extern float calculate_voltage(int16_t In_vol);
extern void motor_protection(void);

#endif/* __SMARTSERVO_H__ */
