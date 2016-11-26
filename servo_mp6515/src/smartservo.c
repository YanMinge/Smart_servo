#include "smartservo.h"
#include "systime.h"
#include "uart_printf.h"
#include "math.h"
#include "mp9960.h"

#define FILTER_SHIFT 3

PID  PID_pos, PID_speed, PID_speed_lock;
smart_led_type smart_led = {0,0,255};

volatile boolean pos_lock_flag = false;
volatile boolean protect_flag = false;
volatile boolean release_state_flag = false;

volatile long smart_servo_cur_pos = 0;
volatile long smart_servo_target_pos = 0;
volatile long smart_servo_pre_target_pos = 0;
volatile long smart_servo_circular_turn_count = 0;

volatile unsigned long period_time = 1;

volatile float smart_servo_target_speed = SMART_SERVO_PER_SPPED_MAX;
volatile float smart_servo_cur_speed = 0;

volatile int16_t pre_pos = 0;
volatile int16_t smart_servo_output = 0;
volatile int16_t angle_pos_offset = 0;
volatile int16_t motion_mode = SMART_SERVO_COMMON_MODE;
volatile int16_t smart_servo_pwm = 0;

volatile uint16_t blink_count = 0;
volatile long pre_blink_time = 0;

static volatile boolean blink_flag = false;
static volatile boolean dir_lock_flag = false;

static volatile uint16_t vol_debounced_count = 0;
static volatile uint16_t vol_match_max_count = 0;
static volatile uint16_t vol_match_min_count = 0;

static volatile uint16_t cur_debounced_count = 0;
static volatile uint16_t cur_match_max_count = 0;
static volatile uint16_t cur_match_min_count = 0;

static volatile uint16_t temp_debounced_count = 0;
static volatile uint16_t temp_match_max_count = 0;
static volatile uint16_t temp_match_min_count = 0;

static volatile long samrt_servo_speed_integral = 0;

static int16_t TEMPERATURENOMINAL     = 25;    //Nominl temperature depicted on the datasheet
static int16_t SERIESRESISTOR         = 10000; // Value of the series resistor
static int16_t BCOEFFICIENT           = 3377;  // Beta value for our thermistor(3350-3399)
static int16_t TERMISTORNOMINAL       = 10000; // Nominal temperature value for the thermistor

static float filter_pos = 0.0;
static void smart_servo_ccw(uint8_t speed);

void samrt_servo_init(void)
{
  pinMode(SMART_SERVO_LED_R,GPIO_PMD_OUTPUT);
  pinMode(SMART_SERVO_LED_G,GPIO_PMD_OUTPUT);
  pinMode(SMART_SERVO_LED_B,GPIO_PMD_OUTPUT);   
  pinMode(SMART_SERVO_SLEEP,GPIO_PMD_OUTPUT);
  pinMode(SMART_SERVO_PHASE,GPIO_PMD_OUTPUT);

//	digitalWrite(SMART_SERVO_SLEEP,1);
//  digitalWrite(SMART_SERVO_SLEEP,0);

  pwm_init(SMART_SERVO_LED_R,1000);
  pwm_init(SMART_SERVO_LED_G,1000);
  pwm_init(SMART_SERVO_LED_B,1000);
  pwm_init(SMART_SERVO_ENBL,20000);  //20000

  smart_servo_led(0,0,255);

  smart_servo_cur_pos = adc_get_position_value();
  pre_pos = smart_servo_cur_pos;
  smart_servo_target_pos = smart_servo_cur_pos;
  filter_pos = smart_servo_cur_pos;
  smart_servo_target_speed = SMART_SERVO_PER_SPPED_MAX;
  samrt_servo_speed_integral = 0;

  protect_flag = false;

  PID_pos.P = 1.3;  //1.3    //2.0
  PID_pos.I = 0.6;  //0.6   //0.75
  PID_pos.D = 1.5;
  PID_pos.Setpoint = smart_servo_target_pos;
  PID_pos.Integral = 0;

  PID_speed.P = 3.2;
  PID_speed.I = 0;
  PID_speed.D = 0;
  PID_speed.Setpoint = smart_servo_target_speed;
  PID_speed.Integral = 0;

  PID_speed_lock.P = 1.8;   //1.8
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
	release_state_flag = true;
  }
  else
  {
    digitalWrite(SMART_SERVO_SLEEP,1);
	release_state_flag = false;
    smart_servo_cur_pos = adc_get_position_value();
    pre_pos = smart_servo_cur_pos;
    smart_servo_target_pos = smart_servo_cur_pos;
//    pwm_write(SMART_SERVO_PW1,255,0,255);
//    pwm_write(SMART_SERVO_PW2,255,0,255);
  }
}

static void smart_servo_ccw(uint8_t speed)
{
  pwm_write(SMART_SERVO_ENBL,speed,0,255);
  digitalWrite(SMART_SERVO_PHASE,1);
//  pwm_write(SMART_SERVO_PW1,speed,0,255);
//  pwm_write(SMART_SERVO_PW2,0,0,255);
}

static void smart_servo_cw(uint8_t speed)
{
  pwm_write(SMART_SERVO_ENBL,speed,0,255);
  digitalWrite(SMART_SERVO_PHASE,0);
//  pwm_write(SMART_SERVO_PW1,0,0,255);
//  pwm_write(SMART_SERVO_PW2,speed,0,255);
}

void smart_servo_speed_update(int16_t pwm)
{
  int16_t speed_temp = pwm;

  if((SMART_SERVO_PWM_MODE != motion_mode) && (release_state_flag == true))
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

void smart_servo_move_to(long absolute_angle,float speed)
{
  digitalWrite(SMART_SERVO_SLEEP,1);
  motion_mode = SMART_SERVO_COMMON_MODE;
  pos_lock_flag = false;
  PID_speed.Integral = 0;
  PID_pos.Integral = 0;
  filter_pos = 0.0;
  smart_servo_target_pos = absolute_angle;
  smart_servo_target_speed = (float)abs(speed);
  release_state_flag = false;
  if(abs(smart_servo_distance_togo()) >= smart_servo_target_speed * 4)
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
    PID_speed.Output = (255.0/SMART_SERVO_PER_SPPED_MAX) * abs(smart_servo_distance_togo())/4.0;
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

void smart_servo_move(long angle,float speed)
{
  smart_servo_move_to((smart_servo_cur_pos + angle),speed);
}

SpiFlashOpResult samrt_servo_set_current_angle_zero_degrees(void)
{
  int value = -1;
  SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
  value = 4095 - mp9960_read_raw_angle();
  if(flash_write_angle_offset((uint32_t)value) == SPI_FLASH_RESULT_OK)
  {
    angle_pos_offset = value;
    result = SPI_FLASH_RESULT_OK;
  }
  else
  {
    result = SPI_FLASH_RESULT_ERR;
  }
  smart_servo_cur_pos = 0;
  smart_servo_target_pos = smart_servo_cur_pos;
  smart_servo_circular_turn_count = 0;
  pre_pos = 0;
  samrt_servo_speed_integral = 0;
  return result;
}

void smart_servo_circular_turn_calc(int16_t cur_pos, int16_t pre_pos, float speed_input)
{
  samrt_servo_speed_integral += speed_input;
  if(abs(cur_pos - pre_pos) > 2048)
  {
    if((pre_pos > 2048) && (cur_pos < 2048))
    {
      smart_servo_circular_turn_count = smart_servo_circular_turn_count + 1;
//      uart_printf(UART0,"%ld\r\n",smart_servo_circular_turn_count);
    }
    else if((pre_pos < 2048) && (cur_pos > 2048))
    {
      smart_servo_circular_turn_count = smart_servo_circular_turn_count - 1;
//      uart_printf(UART0,"%ld\r\n",smart_servo_circular_turn_count);
    }
  }
}

void smart_servo_update(void)
{
  int16_t cur_pos;
  int16_t speed_temp;
  cur_pos = adc_get_position_value();
  speed_temp = normalize_position_difference(cur_pos - pre_pos);
  //speed_temp = Kalman(speed_temp);
  smart_servo_circular_turn_calc(cur_pos,pre_pos,speed_temp);
  pre_pos = cur_pos;
  smart_servo_cur_speed = ((speed_temp * (1000.0/period_time))/4096.0) * 60;
  smart_servo_cur_pos = smart_servo_circular_turn_count * 4096 + cur_pos;
}

long smart_servo_distance_togo(void)
{
  if(release_state_flag == false)
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
  filter_pos = 0.65 * filter_pos + 0.35 * input;
  return (int16_t)filter_pos;
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

int16_t pid_position_to_pwm(void)
{
  float speed_error;
  float pos_error;
  float d_component;
  float seek_velocity;

  //speed pid;
  if(abs(smart_servo_distance_togo()) >= smart_servo_target_speed * 4)
  {
    speed_error = abs(smart_servo_cur_speed) - smart_servo_target_speed;

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
//    uart_printf(UART0,"cur_speed:%d, Output:%.2f, Pos:%d\r\n",(int)smart_servo_cur_speed,PID_speed.Output,smart_servo_cur_pos);
  }

  //position pid;
  else if(abs(smart_servo_distance_togo()) < smart_servo_target_speed * 4)
  {
    pos_error = smart_servo_target_pos - smart_servo_cur_pos;
    smart_servo_output = (255.0/SMART_SERVO_PER_SPPED_MAX)* smart_servo_distance_togo()/4.0;
    if(abs(pos_error) > SMART_SERVO_POS_DEADBAND)
    {
      if(pos_error > 0)
      {
        if((pos_lock_flag == true) && (dir_lock_flag == false))
        {
          PID_pos.Integral = 0;
        }
        dir_lock_flag = true;
        seek_velocity = sqrt(abs(smart_servo_target_speed * 4 * pos_error))/4.0;
        d_component = smart_servo_cur_speed - seek_velocity;
      }
      else
      {
        if((pos_lock_flag == true) && (dir_lock_flag == true))
        {
          PID_pos.Integral = 0;
        }
        dir_lock_flag = false;
        seek_velocity = -sqrt(abs(smart_servo_target_speed * 4 * pos_error))/4.0;
        d_component = smart_servo_cur_speed - seek_velocity;
      }
      PID_pos.Integral += d_component;
      if(pos_lock_flag == true)
      {
        PID_pos.Output = smart_servo_output + 2* PID_pos.P * pos_error - PID_pos.D * d_component;
        //PID_pos.Output = smart_servo_output + PID_pos.P * pos_error * 2 - PID_pos.I * PID_pos.Integral;
      }
      else
      {
        PID_pos.Output = smart_servo_output - PID_pos.D * d_component - PID_pos.I * PID_pos.Integral;
       //PID_pos.Output = smart_servo_output + PID_pos.P * pos_error - PID_pos.D * d_component - PID_pos.I * PID_pos.Integral;
      }
    }
    else
    {
      pos_error = filter_position(pos_error);
      pos_lock_flag = true;
      PID_pos.Integral = 0;
      seek_velocity = 0;
      d_component = smart_servo_cur_speed - seek_velocity;
      PID_pos.Output = smart_servo_output + PID_pos.P * pos_error;
    }
    PID_pos.Output = constrain(PID_pos.Output,SMART_SERVO_MIN_OUTPUT,SMART_SERVO_MAX_OUTPUT);
    //uart_printf(UART0,"%.2f, %.2f, %.2f, %.2f, %d\r\n",pos_error, d_component, PID_pos.Integral, seek_velocity,smart_servo_output);
    smart_servo_output = PID_pos.Output;
  }
  return smart_servo_output;
}

int16_t adc_get_position_value(void)
{
  int value = -1;
  value = 4095 - mp9960_read_raw_angle();
  value = value - angle_pos_offset;
  if(value < 0)
  {
    value = 4096 + value;
  }
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
//  int i;
  digitalWrite(SMART_SERVO_SLEEP,1);
  if(speed > 0)
  {
    smart_servo_cw((uint8_t)speed);
  }
  else
  {
    smart_servo_ccw((uint8_t)abs(speed));
  }
//  smart_servo_move_to(1400,50);
//	for(i=0;i<50;i++)
//	{
//    delay(10);
//    device_neep_loop_in_sampling(); 
//  }
//  delay(200);
//  smart_servo_move_to(2100,50);
//	for(i=0;i<50;i++)
//	{
//    delay(10);
//    device_neep_loop_in_sampling(); 
//  }
//  smart_servo_ccw(0);
//  delay(200);	
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
  current_temp = (In_cur * 3300.0)/4096.0;
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
  vol_debounced_count ++;
  temp = calculate_temp(temp);
  current = calculate_current(current);
  voltage = calculate_voltage(voltage);

  //If the temperature is above 80 degrees
  if(temp > 80)
  {
    temp_match_max_count ++;
  }

  if(temp_debounced_count == (8000/SAMPLING_INTERVAL_TIME))
  {
    temp_debounced_count = 0;
    if(temp_match_max_count > (4000/SAMPLING_INTERVAL_TIME))
    {
      smart_servo_break(true);
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

  //Whether the voltage is lower than 5.5 V or greater than 10 V
  if((voltage < 5.0) || (voltage > 10))
  {
    vol_match_max_count++;
  }

  if(vol_debounced_count == (8000/SAMPLING_INTERVAL_TIME))
  {
    vol_debounced_count = 0;
    if(vol_match_max_count > (4000/SAMPLING_INTERVAL_TIME))
    {
      smart_servo_break(true);
      protect_flag = true;
    }
    vol_match_max_count  = 0;
  }

  //If current is greater than 1.5A
  if(current > 1500)
  {
    cur_match_max_count ++;
  }

  if(cur_debounced_count == (8000/SAMPLING_INTERVAL_TIME))
  {
    cur_debounced_count = 0;
    if(cur_match_max_count > (4000/SAMPLING_INTERVAL_TIME))
    {
      smart_servo_break(true);
      protect_flag = true;
    }
    cur_match_max_count = 0;
  }
//	uart_printf(UART0,"temp: %.2f, cur: %.2f, vol: %.2f, pos:%d\r\n",temp,current,voltage,adc_get_position_value());
}
