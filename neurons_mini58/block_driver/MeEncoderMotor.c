#include "MeEncoderMotor.h"
#include "systime.h"
#include "mygpio.h"
#include "math.h"
#include "uart_printf.h"

#define SPEED_POSITION_SAMPLE_INTERVAL    20
#define CONTROL_INTERVAL                  40

volatile static  uint32_t s_pulse_count = 0;
static uint32_t s_pre_millis = 0; 
static uint32_t s_pre_move_millis = 0;
static Me_Encoder s_encoder_motor;
volatile static uint32_t* s_encoder_a_pin_address;
volatile static uint32_t* s_encoder_b_pin_address;
    
void encoder_motor_update_position(void)
{
}

void EINT0_IRQHandler(void)
{
    if(GPIO_GET_INT_FLAG(P3, BIT2))
    {
        /* Clear P3.2 interrupt flag */
        GPIO_CLR_INT_FLAG(P3, BIT2);
        if(*s_encoder_a_pin_address)
        {
            if(*s_encoder_b_pin_address)
            {
                s_pulse_count++;
            }
            else
            {
                s_pulse_count--;
            }
        }
        else
        {
            if(*s_encoder_b_pin_address)
            {
                s_pulse_count--;
            }
            else
            {
                s_pulse_count++;
            }
        }
       
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT2, PORT3 and PORT4 interrupts */
		P1->INTSRC = P1->INTSRC;
        P2->INTSRC = P2->INTSRC;
        P3->INTSRC = P3->INTSRC;
        P4->INTSRC = P4->INTSRC;
        P5->INTSRC = P5->INTSRC;
	}
}

void EINT1_IRQHandler(void)
{
    if(GPIO_GET_INT_FLAG(P5, BIT2))
    {
        /* Clear P5.2 interrupt flag */
        GPIO_CLR_INT_FLAG(P5, BIT2);
        if(*s_encoder_b_pin_address)
        {
            if(*s_encoder_a_pin_address)
            {
                s_pulse_count--;
            }
            else
            {
                s_pulse_count++;
            }
        }
        else
        {
            if(*s_encoder_a_pin_address)
            {
                s_pulse_count++;
            }
            else
            {
                s_pulse_count--;
            }
        }
    }
    else
    {
        /* Un-expected interrupt. Just clear all PORT2, PORT3 and PORT4 interrupts */
		P1->INTSRC = P1->INTSRC;
        P2->INTSRC = P2->INTSRC;
        P3->INTSRC = P3->INTSRC;
        P4->INTSRC = P4->INTSRC;
        P5->INTSRC = P5->INTSRC;
	}
}


float encoder_distance_togo(void)
{
    return (s_encoder_motor.target_position - s_encoder_motor.current_position);
}


void encoder_motor_update_position_speed(void)
{
    uint32_t current_millis = 0;
    uint32_t speed_sample_interval = 0; 
    int16_t pulse_count = 0;
    
    current_millis = millis();
    speed_sample_interval = current_millis - s_pre_millis;
    if(speed_sample_interval > 20)
    {
        s_pre_millis = current_millis;
        pulse_count = s_pulse_count;
        s_pulse_count = 0;
        s_encoder_motor.current_position += (((float)pulse_count*360)/(ENCODER_PRECISION*s_encoder_motor.ratio));
        s_encoder_motor.current_speed = (pulse_count*1000*60)/(ENCODER_PRECISION*s_encoder_motor.ratio* speed_sample_interval); // r/min
        //uart_printf(UART0, "speed_sample_interval:%d, s_pulse_count:%d, speed:%d, position:%d\n", speed_sample_interval, pulse_count, (int)(s_encoder_motor.current_speed), (int)(s_encoder_motor.current_position));
        
    }
}


void encoder_motor_set_ratio(float ratio)
{
    s_encoder_motor.ratio = ratio;
}

void encoder_motor_set_speed_pid(float p, float i, float d)
{
    s_encoder_motor.PID_speed.P = p;
    s_encoder_motor.PID_speed.I = i;
    s_encoder_motor.PID_speed.D = d;
}

void encoder_motro_set_position_pid(float p, float i, float d)
{
    s_encoder_motor.PID_position.P = p;
    s_encoder_motor.PID_position.I = i;
    s_encoder_motor.PID_position.D = d;
}

void encoder_motor_set_mode(ENCODER_MODE mode)
{
    s_encoder_motor.mode = mode;
}

// speed mode.
void encoder_motor_move_speed(int16_t speed)
{
    s_encoder_motor.mode = SPEED_MODE;
    s_encoder_motor.target_speed = constrain(speed, MIN_ENCODER_SPEED, MAX_ENCODER_SPEED);
}

void encoder_motor_move(int32_t turns, int16_t speed)
{
    s_encoder_motor.target_position = s_encoder_motor.current_position + turns;
    encoder_motor_move_to(s_encoder_motor.target_position, speed);
}

void encoder_motor_move_to(int32_t turns, int16_t speed)
{
    s_encoder_motor.mode = POSITION_SPEED_MODE;
    s_encoder_motor.target_position = turns;
    speed = constrain(speed, MIN_ENCODER_SPEED, MAX_ENCODER_SPEED);
    s_encoder_motor.target_speed = abs_user(speed);
    if(encoder_distance_togo() > 0)
    {
        s_encoder_motor.direction_flag = true;
    }
    else
    {
        s_encoder_motor.direction_flag = false;
    }
}


int16_t pid_speed_to_pwm(void)
{
    float out_put_offset;
    s_encoder_motor.PID_speed.error = s_encoder_motor.current_speed - s_encoder_motor.target_speed;
    out_put_offset = (s_encoder_motor.PID_speed.P)*(s_encoder_motor.PID_speed.error);
    //uart_printf(UART0, "out_put_offset:%d\n", (int)out_put_offset);
    out_put_offset = constrain(out_put_offset, -25, 25);
    s_encoder_motor.PID_speed.Output = s_encoder_motor.encoder_output;
    s_encoder_motor.PID_speed.Output -= out_put_offset;
    
    //uart_printf(UART0, "pid out:%d\n", (int)(s_encoder_motor.PID_speed.Output));
    //uart_printf(UART0, "pid out:%d\n", (int)(s_encoder_motor.encoder_output));
//    if(s_encoder_motor.lock_flag == true)
//    {
//        s_encoder_motor.PID_speed.Integral += s_encoder_motor.PID_speed.error;
//        out_put_offset = (s_encoder_motor.PID_speed.I)*(s_encoder_motor.PID_speed.Integral);
//        s_encoder_motor.PID_speed.Output = -out_put_offset;
//    }
//    else
//    {
//        s_encoder_motor.PID_speed.Output = 0;
//    }
    //uart_printf(UART0, "pid out:%d\n", s_encoder_motor.PID_speed.Output);
    s_encoder_motor.PID_speed.Output = constrain(s_encoder_motor.PID_speed.Output, -255, 255);
    s_encoder_motor.encoder_output = s_encoder_motor.PID_speed.Output;
    
    //uart_printf(UART0, "pwm:%d\n", s_encoder_motor.encoder_output);
    return s_encoder_motor.encoder_output;
}


int16_t pid_position_speed_to_pwm(void)
{
    float seek_speed;
    float d_component;
    float out_put_offset = 0;
    
    s_encoder_motor.PID_position.error = encoder_distance_togo();
    if((s_encoder_motor.direction_flag == true)&&((s_encoder_motor.PID_position.error)< 0)) // target position is opposite to the current position.
    {
        d_component = s_encoder_motor.current_speed;
        out_put_offset = (s_encoder_motor.PID_position.D)*d_component;
        s_encoder_motor.PID_position.Output = -out_put_offset;
        s_encoder_motor.encoder_output = s_encoder_motor.PID_position.Output;
        return s_encoder_motor.encoder_output;
    }
    else if((s_encoder_motor.direction_flag == false)&&((s_encoder_motor.PID_position.error)> 0)) // target position is opposite to the current position.
    {
        d_component = s_encoder_motor.current_speed;
        out_put_offset = (s_encoder_motor.PID_position.D)*d_component;
        s_encoder_motor.PID_position.Output = -out_put_offset;
        s_encoder_motor.encoder_output = s_encoder_motor.PID_position.Output;
        return s_encoder_motor.encoder_output;
    }
    // /-\ control, - segment, speed pid
    if(abs_user(s_encoder_motor.PID_position.error) >= (s_encoder_motor.target_speed)*DECELERATION_DISTANCE_PITCH)
    {
        s_encoder_motor.PID_speed.error = s_encoder_motor.current_speed - s_encoder_motor.target_speed*(s_encoder_motor.PID_position.error)/abs_user(s_encoder_motor.PID_position.error);
        out_put_offset = (s_encoder_motor.PID_speed.P)*(s_encoder_motor.PID_speed.error);
        out_put_offset = constrain(out_put_offset, -25, 25);
        s_encoder_motor.PID_speed.Output = s_encoder_motor.encoder_output;
        s_encoder_motor.PID_speed.Output -= out_put_offset;
        s_encoder_motor.PID_speed.Output = constrain((s_encoder_motor.PID_speed.Output), -255, 255);
        s_encoder_motor.encoder_output = s_encoder_motor.PID_speed.Output;
    }
    // /-\ control, /\ segmet, position pid
    else
    {
        if(abs_user(s_encoder_motor.PID_position.error) > ENCODER_POSITION_DEADBAND)
        {
            seek_speed = sqrt(abs_user(s_encoder_motor.target_speed * DECELERATION_DISTANCE_PITCH * (s_encoder_motor.PID_position.error - ENCODER_POSITION_DEADBAND))) \
            /DECELERATION_DISTANCE_PITCH; // TODO: - ENCODER_POSITION_DEADBAND need?
            d_component = s_encoder_motor.current_speed - seek_speed*((s_encoder_motor.PID_position.error)/abs_user(s_encoder_motor.PID_position.error));
            out_put_offset = s_encoder_motor.PID_position.D *d_component;
            out_put_offset = constrain(out_put_offset, -20, 20);
            s_encoder_motor.PID_position.Output = s_encoder_motor.encoder_output;
            s_encoder_motor.PID_position.Output -= out_put_offset;
            if(s_encoder_motor.PID_position.error >= 0)
            {
                s_encoder_motor.PID_position.Output = constrain(s_encoder_motor.PID_position.Output, PWM_MIN_OFFSET, 255);
            }
            else
            {
                s_encoder_motor.PID_position.Output = constrain(s_encoder_motor.PID_position.Output, -255, -PWM_MIN_OFFSET);
            }
            s_encoder_motor.encoder_output = s_encoder_motor.PID_position.Output;
        }
        else
        {
            if(s_encoder_motor.lock_flag == true)
            {
                d_component = s_encoder_motor.current_speed;
                out_put_offset = (s_encoder_motor.PID_position).D*d_component;
                out_put_offset = constrain(out_put_offset, -20, 20);
                s_encoder_motor.PID_position.Output = (s_encoder_motor.PID_position.error)*(s_encoder_motor.PID_position.P);
                s_encoder_motor.PID_position.Output -= out_put_offset;
                s_encoder_motor.PID_position.Output = constrain(s_encoder_motor.PID_position.Output, -255, 255);
                s_encoder_motor.encoder_output = s_encoder_motor.PID_position.Output;
            }
            else
            {
                s_encoder_motor.encoder_output = 0;
            }
        }
    }
    return s_encoder_motor.encoder_output;
}

static void driver_encoder_motor_with_pwm(int16_t pwm)
{
    int8_t duty;
    pwm = constrain(pwm, -255, 255);
    duty = line_map(abs_user(pwm), 0, 100, 0, 255);
    if(pwm < 0)
    {
        pwm_write(PWM_A_PIN, MOTOR_FREQUENCE, 0);
        pwm_write(PWM_B_PIN, MOTOR_FREQUENCE, duty);
       
    }
    else
    {
        pwm_write(PWM_A_PIN, MOTOR_FREQUENCE, duty);
        pwm_write(PWM_B_PIN, MOTOR_FREQUENCE, 0);
        
    }
}

void encoder_motor_move_with_pwm(int16_t pwm)
{
    s_encoder_motor.mode = PWM_MODE;
    driver_encoder_motor_with_pwm(pwm);
}

int16_t get_encoder_motor_speed(void)
{
    return (int16_t)(s_encoder_motor.current_speed);
}

int32_t get_encoder_motor_position(void)
{
    return (int32_t)(s_encoder_motor.current_position);
}

void encoder_motor_run_loop(void)
{
    int16_t pwm_value;
    uint32_t current_millis;
    
    if(s_encoder_motor.mode == PWM_MODE)
    {
        return;
    }
    encoder_motor_update_position_speed();
    
    current_millis = millis();
    if((current_millis - s_pre_move_millis) > 40)
    {
        s_pre_move_millis = current_millis;
        if(s_encoder_motor.mode == POSITION_SPEED_MODE)
        {
            pwm_value = pid_position_speed_to_pwm();
        }
        else if(s_encoder_motor.mode == SPEED_MODE)
        {
            pwm_value = pid_speed_to_pwm();
        }
        
        driver_encoder_motor_with_pwm(pwm_value);
    }
}

void reset_encoder_motor(void)
{
    s_encoder_motor.ratio = RATIO;
    s_encoder_motor.mode = PWM_MODE;
    s_encoder_motor.lock_flag = 0;
    s_encoder_motor.direction_flag = 0;
    s_encoder_motor.current_position = 0;
    s_encoder_motor.current_speed = 0;
    s_encoder_motor.direction_flag = 0;
    s_encoder_motor.encoder_output = 0;
    s_encoder_motor.target_position = 0;
    s_encoder_motor.target_speed = 0;
    // position pid
    s_encoder_motor.PID_position.P = 1.8;
    s_encoder_motor.PID_position.I = 0;
    s_encoder_motor.PID_position.D = 1.2;
    s_encoder_motor.PID_position.error = 0;
    s_encoder_motor.PID_position.last_error = 0;
    // speed pid
    s_encoder_motor.PID_speed.P = 0.18;
    s_encoder_motor.PID_speed.I = 0.64;
    s_encoder_motor.PID_speed.D = 0;
    s_encoder_motor.PID_speed.error = 0;
    s_encoder_motor.PID_speed.last_error = 0;
    s_encoder_motor.PID_speed.Output = 0;
}

void encoder_motor_init(void)
{
    pinMode(ENCODER_A_PIN, GPIO_MODE_INPUT);
    pinMode(ENCODER_B_PIN, GPIO_MODE_INPUT);
    
    GPIO_EnableInt(P3, 2, GPIO_INT_BOTH_EDGE);
    NVIC_SetPriority(EINT0_IRQn, 2);
    NVIC_EnableIRQ(EINT0_IRQn);
    
    GPIO_EnableInt(P5, 2, GPIO_INT_BOTH_EDGE);
    NVIC_SetPriority(EINT1_IRQn, 2);
    NVIC_EnableIRQ(EINT1_IRQn);
    
    s_encoder_a_pin_address = Pin2Addr(ENCODER_A_PIN);
    s_encoder_b_pin_address = Pin2Addr(ENCODER_B_PIN);
    
    reset_encoder_motor();
    
}

