#include "MeDCMotor.h"
static int s_motor1_speed;
static int s_motor2_speed;

static uint8_t speed_to_duty(int speed)
{
	return speed*100/(MOTOR_SPEED_MAX - MOTOR_SPEED_MIN);
}

void dc_run(uint8_t dc_num, int speed)
{
    int speed_temp = (int)abs(speed);
    speed_temp = constrain(speed_temp,0,255);
	uint8_t speed_duty;
	speed_duty = speed_to_duty(speed_temp);
    if(dc_num == MOTOR_1)
    {
		s_motor1_speed = speed;
		
        if(speed < 1)
        {
            pwm_write(MOTOR1_PWM_CCW_PIN, MOTOR_FREQUENCE, speed_duty);
			pwm_write(MOTOR1_PWM_CW_PIN, MOTOR_FREQUENCE, 0);
        }
        else
        {
            pwm_write(MOTOR1_PWM_CCW_PIN, MOTOR_FREQUENCE, 0);
			pwm_write(MOTOR1_PWM_CW_PIN, MOTOR_FREQUENCE, speed_duty);
        }
    }
    else if(dc_num == MOTOR_2)
    {
#ifdef D_DOUBLE_MOTOR
		s_motor2_speed = speed;
		
        if(speed < 1)
        {
            digitalWrite(MOTOR2_CCW_CONTROL_PIN, 1);
			digitalWrite(MOTOR2_CW_CONTROL_PIN, 0);
        }
        else
        {
            digitalWrite(MOTOR2_CCW_CONTROL_PIN, 0);
			digitalWrite(MOTOR2_CW_CONTROL_PIN, 1); 
        }
		pwm_write(MOTOR2_PWM_PIN, MOTOR_FREQUENCE, speed_duty);
#endif
    }
}

void dc_motor_stop(uint8_t motor_num)
{
	if(motor_num == MOTOR_1)
    {
		pwm_write(MOTOR1_PWM_CCW_PIN, MOTOR_FREQUENCE, 0);
		pwm_write(MOTOR1_PWM_CW_PIN, MOTOR_FREQUENCE, 0);
    }
    else if(motor_num == MOTOR_2)
    {   
#ifdef D_DOUBLE_MOTOR
		pwm_write(MOTOR2_PWM_PIN, MOTOR_FREQUENCE, 0);
#endif
    }
}

void dc_motor_run(uint8_t motor_num)
{
	if(motor_num == MOTOR_1)
	{
		dc_run(MOTOR_1, s_motor1_speed);
	}
	else if(motor_num == MOTOR_2)
	{
		dc_run(MOTOR_2, s_motor2_speed);
	}
}

void dc_motor_init(void)
{
#ifdef D_DOUBLE_MOTOR
	pinMode(MOTOR2_CCW_CONTROL_PIN,GPIO_MODE_OUTPUT);
	pinMode(MOTOR2_CW_CONTROL_PIN,GPIO_MODE_OUTPUT);
#endif
}

#if defined D_DOUBLE_MOTOR
uint8_t motor_short_detect(void)
{
	static uint8_t s_short_count  = 0;
	uint16_t motor1_short_measure_value = 0;
	uint16_t motor2_short_measure_value = 0;
	uint16_t motor_source_measure_value = 0;
	uint16_t motor_1_current = 0;
	uint16_t motor_2_current = 0;
	
	motor1_short_measure_value = analogRead(MOTOR1_SOURCE_MEASURE_PIN);
	motor2_short_measure_value = analogRead(MOTOR2_SOURCE_MEASURE_PIN);
	motor_source_measure_value = analogRead(MOTOR_SOURCE_MEASURE_PIN);
	motor_1_current = (motor_source_measure_value - motor1_short_measure_value)>>1;
	motor_2_current = (motor_source_measure_value - motor2_short_measure_value)>>1;
	if((motor_1_current > SHORT_THRESHOLD_VALUE) || (motor_2_current > SHORT_THRESHOLD_VALUE))
	{
		s_short_count++;
	}
	if(s_short_count > SHORT_THRESHOLD_TIMES)
	{
		s_short_count = 0;
		return 1;
	}
	return 0;
}

#elif defined D_SINGLE_MOTOR
uint8_t motor_short_detect(void)
{
	static uint8_t s_short_count  = 0;
	uint16_t motor1_short_measure_value = 0;
	uint16_t motor_source_measure_value = 0;
	uint16_t motor_1_current = 0;
	
	motor1_short_measure_value = analogRead(MOTOR1_SOURCE_MEASURE_PIN);
	motor_source_measure_value = analogRead(MOTOR_SOURCE_MEASURE_PIN);
	motor_1_current = (motor_source_measure_value - motor1_short_measure_value)>>1;
	if(motor_1_current > SHORT_THRESHOLD_VALUE)
	{
		s_short_count++;
	}
	if(s_short_count > SHORT_THRESHOLD_TIMES)
	{
		s_short_count = 0;
		return 1;
	}
	return 0;
}
#else 
	#error no motor type defined!
#endif

