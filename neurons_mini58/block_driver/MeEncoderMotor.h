#ifndef __MEENCODERMOTOR__
#define __MEENCODERMOTOR__
#include "mygpio.h"

#define PWM_A_PIN                           P2_5
#define PWM_B_PIN                           P2_6
#define ENCODER_A_PIN                       P3_2
#define ENCODER_B_PIN                       P5_2

#define RATIO                               50

#define MOTOR_FREQUENCE                     10000

#define ENCODER_POSITION_DEADBAND           10
#define ENCODER_PRECISION                   4

#define DECELERATION_DISTANCE_PITCH         6

#define PWM_MIN_OFFSET                      25

#define MAX_ENCODER_SPEED                   360  // rpm
#define MIN_ENCODER_SPEED                   -360 // rpm


typedef enum
{
	DIRECTION_CW  = 0, // Clockwise
	DIRECTION_CCW     // Counter-Clockwise
} Direction;

typedef enum
{
    PWM_MODE = 0,
    SPEED_MODE,
    POSITION_SPEED_MODE
} ENCODER_MODE;

typedef struct
{
  float P, I, D;
  float Setpoint, Output, Integral, error, last_error;
} PID;

typedef struct
{
  float ratio; // gear ratio.
  float target_position;
  float current_position;
  float current_speed;
  float target_speed;
  ENCODER_MODE mode;
  uint8_t lock_flag;
  uint8_t direction_flag;
  PID  PID_position;
  PID  PID_speed;
  int16_t encoder_output;
}Me_Encoder;

void encoder_motor_init(void);

void encoder_motor_update_position_speed(void);

void encoder_motor_set_ratio(float ratio);

void encoder_motor_move(int32_t turns, int16_t speed);

void encoder_motor_move_to(int32_t turns, int16_t speed);

void encoder_motor_set_speed_pid(float p, float i, float d);

void encoder_motro_set_position_pid(float p, float i, float d);

void encoder_motor_set_mode(ENCODER_MODE mode);

void encoder_motor_move_speed(int16_t speed);

void encoder_motor_move_with_pwm(int16_t pwm);

void reset_encoder_motor(void);

int16_t get_encoder_motor_speed(void);

int32_t get_encoder_motor_position(void);

void encoder_motor_run_loop(void);
#endif/* __MEENCODERMOTOR__ */
