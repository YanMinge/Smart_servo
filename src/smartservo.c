#include "smartservo.h"
#include "systime.h"
#include "uart_printf.h"
#include "math.h"

#define FILTER_SHIFT 3

PID  PID_pos, PID_speed, PID_speed_lock;
smart_led_type smart_led = {0,0,255};

volatile boolean pos_lock_flag = false;
volatile boolean dir_lock_flag = false;
volatile boolean protect_flag = false;
volatile boolean blink_flag = false;

volatile int16_t pre_pos = 0;
volatile int16_t smart_servo_cur_pos = 0;
int16_t smart_servo_pre_pos = 0;
int16_t smart_servo_target_pos = -1;
int16_t smart_servo_pre_target_pos = -1;
int16_t smart_servo_max_speed = 0;
int16_t smart_servo_output = 0;
float smart_servo_target_speed = 250;
float filter_pos = 0;

static int16_t TEMPERATURENOMINAL     = 25;    //Nominl temperature depicted on the datasheet
static int16_t SERIESRESISTOR         = 10000; // Value of the series resistor
static int16_t BCOEFFICIENT           = 3377;  // Beta value for our thermistor(3350-3399)
static int16_t TERMISTORNOMINAL       = 10000; // Nominal temperature value for the thermistor

volatile uint16_t cur_debounced_count = 0;
volatile uint16_t cur_match_max_count = 0;
volatile uint16_t cur_match_min_count = 0;
volatile uint16_t temp_debounced_count = 0;
volatile uint16_t temp_match_max_count = 0;
volatile uint16_t temp_match_min_count = 0;
volatile uint16_t blink_count = 0;
volatile unsigned long pre_blink_time = 0;

static void smart_servo_ccw(uint8_t speed);

void samrt_servo_init(void)
{
  pinMode(SMART_SERVO_LED_R,GPIO_PMD_OUTPUT);
  pinMode(SMART_SERVO_LED_G,GPIO_PMD_OUTPUT);
  pinMode(SMART_SERVO_LED_B,GPIO_PMD_OUTPUT);   
  pinMode(SMART_SERVO_SLEEP,GPIO_PMD_OUTPUT);

	//digitalWrite(SMART_SERVO_SLEEP,1);
  digitalWrite(SMART_SERVO_SLEEP,0);
	
  pwm_init(SMART_SERVO_LED_R,1000);
  pwm_init(SMART_SERVO_LED_G,1000);
  pwm_init(SMART_SERVO_LED_B,1000);
  pwm_init(SMART_SERVO_PW1,20000);  //20000
  pwm_init(SMART_SERVO_PW2,20000);

  smart_servo_led(0,0,255);

  smart_servo_cur_pos = adc_get_position_value();
  smart_servo_pre_pos = smart_servo_cur_pos;
  smart_servo_target_pos = smart_servo_cur_pos;
  filter_pos = smart_servo_cur_pos;
  smart_servo_target_speed = SMART_SERVO_PER_SPPED_MAX;

  protect_flag = false;

  PID_pos.P = 2.0;  //1.3
  PID_pos.I = 0.6;
  PID_pos.D = 1.5;
  PID_pos.Setpoint = smart_servo_target_pos;
  PID_pos.Integral = 0;
	
  PID_speed.P = 3.2;
  PID_speed.I = 0;
  PID_speed.D = 0;
  PID_speed.Setpoint = smart_servo_target_speed;
  PID_speed.Integral = 0;

  PID_speed_lock.P = 1.8;
  PID_speed_lock.I = 0;
  PID_speed_lock.D = 0;
  PID_speed_lock.Setpoint = 0;
  PID_speed_lock.Integral = 0;

  smart_servo_break(true);
}

void smart_servo_led(uint8_t led_r,uint8_t led_g,uint8_t led_b)
{
  pwm_write(SMART_SERVO_LED_R,(255-led_r),0,255);
  pwm_write(SMART_SERVO_LED_G,(255-led_g),0,255);
  pwm_write(SMART_SERVO_LED_B,(255-led_b),0,255);  
}

void smart_led_change(uint8_t led_color)
{
  switch(led_color)
  {
    case 0:
      smart_servo_led(255,0,0);
      break;
    case 1:
      smart_servo_led(0,255,0);
      break;
    case 2:
      smart_servo_led(0,0,255);
      break;
    default:
      smart_servo_led(0,0,0);
      break;
  }
}

void smart_led_blink(uint16_t blink_time,uint8_t led_r,uint8_t led_g,uint8_t led_b)
{
  if(millis() - pre_blink_time > blink_time)
  {
    if(blink_flag == false)
    {
      blink_flag = true;
    }
    else
    {
      blink_flag = false;
    }
	blink_count++;
    pre_blink_time = millis();
  }
  if(blink_flag == true)
  {
    smart_servo_led(led_r,led_g,led_b);
  }
  else
  {
    smart_servo_led(0,0,0);
  }
}

void smart_servo_break(boolean status)
{
  if(status == true)
  {
//    pwm_write(SMART_SERVO_PW1,0,0,255);
//    pwm_write(SMART_SERVO_PW2,0,0,255);
    digitalWrite(SMART_SERVO_SLEEP,0);
    smart_servo_target_pos = -1;
  }
  else
  {
    digitalWrite(SMART_SERVO_SLEEP,1);
//    pwm_write(SMART_SERVO_PW1,255,0,255);
//    pwm_write(SMART_SERVO_PW2,255,0,255);
  }
}

static void smart_servo_ccw(uint8_t speed)
{
  pwm_write(SMART_SERVO_PW1,255,0,255);
  pwm_write(SMART_SERVO_PW2,255-speed,0,255);
//  pwm_write(SMART_SERVO_PW1,speed,0,255);
//  pwm_write(SMART_SERVO_PW2,0,0,255);
}

static void smart_servo_cw(uint8_t speed)
{
  pwm_write(SMART_SERVO_PW1,255-speed,0,255);
  pwm_write(SMART_SERVO_PW2,255,0,255);
//  pwm_write(SMART_SERVO_PW1,0,0,255);
//  pwm_write(SMART_SERVO_PW2,speed,0,255);
}

void smart_servo_speed_update(int16_t pwm)
{
  int16_t speed_temp = pwm;
 
  if(smart_servo_target_pos == -1)
  {
    return;
  }

  if(speed_temp > 0)
  {
    smart_servo_cw((uint8_t)speed_temp);
  }
  else
  {
    smart_servo_ccw((uint8_t)abs(speed_temp));
  }
}

void smart_servo_move_to(int16_t absolute_angle,float speed)
{
  if((absolute_angle < SMART_SERVO_MIN_LIM_POS) ||
     (absolute_angle > SMART_SERVO_MAX_LIM_POS))
  {
    return;
  }

  digitalWrite(SMART_SERVO_SLEEP,1);
  pos_lock_flag = false;
  PID_speed.Integral = 0;
  PID_pos.Integral = 0;
  smart_servo_target_pos = absolute_angle;
  smart_servo_target_speed = (float)abs(speed);
//  smart_servo_target_speed = SMART_SERVO_PER_SPPED_MAX;

  if(abs(smart_servo_distance_togo()) >= smart_servo_target_speed * 5)
  {
    PID_speed.Output = (255.0/SMART_SERVO_PER_SPPED_MAX) * smart_servo_target_speed;

    if(smart_servo_distance_togo() > 0)
    {
      dir_lock_flag = true;
      PID_speed.Output = max(abs(PID_speed.Output),abs(smart_servo_output));
    }
    else
    {
      dir_lock_flag = false;
      PID_speed.Output = min(-abs(PID_speed.Output),-abs(smart_servo_output));
    }
  }
  else
  {
    PID_speed.Output = (255.0/SMART_SERVO_PER_SPPED_MAX)* abs(smart_servo_distance_togo())/5.0;
    if(smart_servo_distance_togo() > 0)
    {
      dir_lock_flag = true;
      PID_speed.Output = max(abs(PID_speed.Output),abs(smart_servo_output));
    }
    else
    {
      dir_lock_flag = false;
      PID_speed.Output = min(-abs(PID_speed.Output),-abs(smart_servo_output));
    }
  }
	smart_servo_output = PID_speed.Output;
}

void smart_servo_move(int16_t angle,float speed)
{
  smart_servo_move_to((smart_servo_cur_pos + angle),speed);
}

int16_t normalize_position_difference(int16_t posdiff)
{
  if (posdiff > ((SMART_SERVO_MAX_POSITION - SMART_SERVO_MIN_POSITION) / 2))
  {
    posdiff -= (SMART_SERVO_MAX_POSITION - SMART_SERVO_MIN_POSITION);
  }

  if (posdiff < -((SMART_SERVO_MAX_POSITION - SMART_SERVO_MIN_POSITION) / 2))
  {
    posdiff += (SMART_SERVO_MAX_POSITION - SMART_SERVO_MIN_POSITION);
  }
  return posdiff;
}

long smart_servo_distance_togo(void)
{
  if(smart_servo_target_pos != -1)
  {
    return smart_servo_target_pos - smart_servo_cur_pos;
  }
  else
  {
    return 0;
  }
}

int16_t filter_position(int16_t input)
{
  filter_pos = 0.5 * filter_pos + 0.5 * input;
	return (int16_t)filter_pos;
}

int16_t pid_position_to_pwm(void)
{
  float speed_error;
  float pos_error;
  float d_component;
  float cur_speed;
  float cur_speed_temp;
  float seek_velocity;
	
  int cur_pos;

  if(pos_lock_flag == true)
  {
    cur_pos = adc_get_position_value();
		cur_pos = filter_position(cur_pos);
	}
	else
  {
    cur_pos = adc_get_position_value();
  }
  cur_speed_temp = cur_pos - smart_servo_pre_pos;
  cur_speed_temp = constrain(cur_speed_temp,-SMART_SERVO_PER_SPPED_MAX,SMART_SERVO_PER_SPPED_MAX);
  filter_position(cur_pos);
  smart_servo_cur_pos = cur_pos;
  smart_servo_pre_pos = smart_servo_cur_pos;
  cur_speed = (cur_speed_temp * (1000.0/SAMPLING_INTERVAL_TIME))/4396 * 60;

  //speed pid;
  if(abs(smart_servo_distance_togo()) >= smart_servo_target_speed * 5)
  {
    speed_error = abs(cur_speed) - smart_servo_target_speed;

    if((speed_error != 0) && (smart_servo_distance_togo() > 0))
    {
      if(abs(speed_error) > PID_speed.P)
      {
        PID_speed.Output -= PID_speed.P * (speed_error/abs(speed_error));
      }
      else
      {
        PID_speed.Output -= abs(speed_error) * (speed_error/abs(speed_error));
      }
    }
    else if((speed_error !=0) && (smart_servo_distance_togo() < 0))
    {
      if(abs(speed_error) > PID_speed.P)
      {
        PID_speed.Output += PID_speed.P * (speed_error/abs(speed_error));
      }
      else
      {
        PID_speed.Output += abs(speed_error) * (speed_error/abs(speed_error));
      }
    }

    PID_speed.Output = constrain(PID_speed.Output,SMART_SERVO_MIN_OUTPUT,SMART_SERVO_MAX_OUTPUT);
		smart_servo_output = PID_speed.Output;
//    uart_printf(UART0,"cur_speed:%.2f, tar_speed:%.2f, Output:%.2f, Pos:%d\r\n",cur_speed,smart_servo_target_speed,PID_speed.Output,cur_pos);
  }

  //position pid;
  else if(abs(smart_servo_distance_togo()) < smart_servo_target_speed * 5)
  {
    pos_error = smart_servo_target_pos - cur_pos;

    smart_servo_output = (255.0/SMART_SERVO_PER_SPPED_MAX)* smart_servo_distance_togo()/5.0;
    if(abs(pos_error) > SMART_SERVO_POS_DEADBAND)
    {
      if(pos_error > 0)
      {
        if((pos_lock_flag == true) && (dir_lock_flag == false))
        {
          PID_pos.Integral = 0;
        }
        dir_lock_flag = true;
        seek_velocity = sqrt(abs(smart_servo_target_speed * 5 * pos_error))/5.0;
        d_component = cur_speed - seek_velocity;
      }
      else
      {
        if((pos_lock_flag == true) && (dir_lock_flag == true))
        {
          PID_pos.Integral = 0;
        }
        dir_lock_flag = false;
        seek_velocity = -sqrt(abs(smart_servo_target_speed * 5 * pos_error))/5.0;
        d_component = cur_speed - seek_velocity;
      }
      PID_pos.Integral += d_component;
      if(pos_lock_flag == true)
      {
        PID_pos.Output = smart_servo_output + PID_pos.P * pos_error - PID_pos.D * d_component;
      }
      else
      {
        PID_pos.Output = smart_servo_output - PID_pos.D * d_component - PID_pos.I * PID_pos.Integral;
      }
    }
    else
    {
      pos_lock_flag = true;
      PID_pos.Integral = 0;
      seek_velocity = 0;
      d_component = cur_speed - seek_velocity;
      PID_pos.Output = smart_servo_output + PID_pos.P * pos_error;
    }
    PID_pos.Output = constrain(PID_pos.Output,SMART_SERVO_MIN_OUTPUT,SMART_SERVO_MAX_OUTPUT);
//    uart_printf(UART0,"pos_error:%.2f, PID_pos.Integral:%.2f, d_component:%.2f, Output:%.2f, \r\n",pos_error, PID_pos.Integral, d_component, PID_pos.Output);
    smart_servo_output = PID_pos.Output;
  }
  return smart_servo_output;
}

int16_t adc_get_position_value(void)
{
  int value = -1;
  value = analogRead(SMART_SERVO_DIR_AD);
  return value;
}

int16_t adc_get_temperature_value(void)
{
  int value = -1;
  value = analogRead(SMART_SERVO_TEMP_AD);
  return value;
}

int16_t adc_get_voltage_value(void)
{
  int value = -1;
  value = analogRead(SMART_SERVO_VOL_AD);
  return value;
}

int16_t adc_get_current_value(void)
{
  int value = -1;
  value = analogRead(SMART_SERVO_CURR_AD);
  return value;
}

int16_t io_get_nfault_value(void)
{
  int value = -1;
  value = digitalRead(SMART_SERVO_NFAULT);
  return value;
}

void servo_move_test(float speed)
{
  digitalWrite(SMART_SERVO_SLEEP,1);
  smart_servo_ccw(speed);
}

float calculate_temp(int16_t In_temp)
{
  float media;
  float temperatura;
  media = (float)In_temp;
  // Convert the thermal stress value to resistance
  media = 4095.0 / media - 1;
  media = SERIESRESISTOR / media;
  //Calculate temperature using the Beta Factor equation

  temperatura = media / TERMISTORNOMINAL;              // (R/Ro)
  temperatura = log(temperatura); // ln(R/Ro)
  temperatura /= BCOEFFICIENT;                         // 1/B * ln(R/Ro)
  temperatura += 1.0 / (TEMPERATURENOMINAL + 273.15);  // + (1/To)
  temperatura = 1.0 / temperatura;                     // Invert the value
  temperatura -= 273.15;                               // Convert it to Celsius
  return temperatura;
}

float calculate_current(int16_t In_cur)
{
  float current_temp;
  current_temp = (In_cur * 3000.0)/4096.0;
//  uart_printf(UART0,"current_temp:%.2f\r\n",current_temp);
  return current_temp;
}

float calculate_voltage(int16_t In_vol)
{
  float voltage_temp;
  voltage_temp = In_vol * 0.008;
//  uart_printf(UART0,"oltage_temp:%.2f\r\n",voltage_temp);
  return voltage_temp;
}

void motor_protection(void)
{
  float temp = adc_get_temperature_value();
  float current = adc_get_current_value();
  float voltage = adc_get_voltage_value();
  cur_debounced_count ++;
  temp_debounced_count ++;
  temp = calculate_temp(temp);
  current = calculate_current(current);
  voltage = calculate_voltage(voltage);

  //If the temperature is above 65 degrees
  if(temp > 65)
  {
    temp_match_max_count ++;
  }

  if(temp_debounced_count == (6000/SAMPLING_INTERVAL_TIME))
  {
    temp_debounced_count = 0;
    if(temp_match_max_count > (3000/SAMPLING_INTERVAL_TIME))
    {
      smart_servo_break(true);
      digitalWrite(SMART_SERVO_SLEEP,0);
//      uart_printf(UART0,"temp:%d\r\n",temp_match_max_count);
      protect_flag = true;
    }
    temp_match_max_count = 0;
  }

  if(protect_flag == true)
  {
    smart_led_blink(500,255,0,0);
    return;
  }
	
//  if(io_get_nfault_value() == 0)
//  {
//    smart_servo_break(true);
//    digitalWrite(SMART_SERVO_SLEEP,0);
//    uart_printf(UART0,"nfault:%d\r\n",io_get_nfault_value());
//    protect_flag = true;
//  }

  //Whether the voltage is lower than 5.5V or greater than 9V
  if((voltage < 5.5) || (voltage > 9))
  {
    smart_servo_break(true);
    digitalWrite(SMART_SERVO_SLEEP,0);
    protect_flag = true;
  }

  //If current is greater than 1.5A
  if(current > 1500)
  {
    cur_match_max_count ++;
  }

  if(cur_debounced_count == (6000/SAMPLING_INTERVAL_TIME))
  {
    cur_debounced_count = 0;
    if(cur_match_max_count > (3000/SAMPLING_INTERVAL_TIME))
    {
      smart_servo_break(true);
      digitalWrite(SMART_SERVO_SLEEP,0);
      protect_flag = true;
    }
    cur_match_max_count = 0;
  }
//	uart_printf(UART0,"temp: %.2f, cur: %.2f, vol: %.2f, pos:%d\r\n",temp,current,voltage,adc_get_position_value());
}
