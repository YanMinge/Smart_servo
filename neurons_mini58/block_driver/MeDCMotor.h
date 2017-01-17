#ifndef __MEMOTOR__
#define __MEMOTOR__
#include "Mini58Series.h"
#include "mygpio.h"

#if defined D_DOUBLE_MOTOR  // for double motor
// CCW abbreviation  to counterclockwise, CW abbreviation to clockwise. 
#define MOTOR1_PWM_CW_PIN                 P0_5
#define MOTOR1_PWM_CCW_PIN				  P0_4
#define MOTOR1_SOURCE_MEASURE_PIN		  P3_1
#define MOTOR2_PWM_PIN					  P2_5
#define MOTOR2_CW_CONTROL_PIN			  P0_6  
#define MOTOR2_CCW_CONTROL_PIN            P2_6
#define MOTOR2_SOURCE_MEASURE_PIN         P5_3
#define MOTOR_SOURCE_MEASURE_PIN		  P3_0

#elif defined D_SINGLE_MOTOR // for single motor.
#define MOTOR1_PWM_CW_PIN                 P2_5
#define MOTOR1_PWM_CCW_PIN				  P2_6
#define MOTOR1_SOURCE_MEASURE_PIN		  P3_1
#define MOTOR_SOURCE_MEASURE_PIN		  P3_0

#else
	#error please define the motor type.
#endif

#define MOTOR_SPEED_MIN  0
#define MOTOR_SPEED_MAX  100
#define MOTOR_FREQUENCE  16000

#define MOTOR_1 0
#define MOTOR_2 1

#define DETECT_RESISTANCE         4.7
#define SHORT_THRESHOLD_VALUE     20
#define SHORT_THRESHOLD_TIMES     50

void dc_run(uint8_t dc_num, int8_t speed);
void dc_motor_init(void);
uint8_t motor_short_detect(void);
void dc_motor_stop(uint8_t motor_num);
void dc_motor_run(uint8_t motor_num);

#endif
