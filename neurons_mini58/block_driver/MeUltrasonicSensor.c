#include "MeUltrasonicSensor.h"
#include "Interrupt.h"
#include "systime.h"
#include "uart_printf.h"
#include "interrupt.h"

#define ULTRANSONIC_RECEIVE_ENABLE   pinMode(RECEIVE_ENABLE , GPIO_MODE_INPUT)
#define ULTRANSONIC_RECEIVE_DISABLE  pinMode(RECEIVE_ENABLE , GPIO_MODE_OUTPUT);digitalWrite(RECEIVE_ENABLE, 0)
#define TX_POWER_ENABLE  digitalWrite(T_PWR, 0) // tx power enable.
#define TX_POWER_DISABLE digitalWrite(T_PWR, 1) // tx power disable.
#define TIME_OUT_THRESHOLD 35000*CyclesPerUs  //35ms.

volatile static uint32_t  s_echo_count;
static float s_distance = 0;
volatile static uint8_t s_count = 0;

volatile static  enum ULTRANSONIC_STATE s_ultransonic_state = ULTRANSONIC_READY;

void PWM_IRQHandler(void)
{
	// Clear channel 4 period interrupt flag
	PWM_ClearPeriodIntFlag(PWM, PWM_CH4);
	++s_count;
}

static void count_start(void)
{	
	CLK_EnableModuleClock(TMR1_MODULE);
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HCLK, 0);
	TIMER1->CTL = 0;
	TIMER1->CTL |= TIMER_CTL_RSTCNT_Msk;
	TIMER1->CTL |= TIMER_CONTINUOUS_MODE;
	TIMER_Start(TIMER1);
}

static void count_end(void)
{
	TIMER_Stop(TIMER1);
}

static void transmint8Puls(void)
{
	
	TX_POWER_ENABLE;
	
	PWM_EnablePeriodInt(PWM, PWM_CH4);
	NVIC_SetPriority(PWM_IRQn, 1);
    NVIC_EnableIRQ(PWM_IRQn);
	count_start();
	PWM->CTL |= (PWM_CTL_CNTEN4_Msk|PWM_CTL_CNTEN5_Msk); // pwm start 
	while(s_count < 8)
	{
		if(whileCoutOver_us(1000))
		{
			return;
		}
		clearWhileCount();
    }
	
	PWM->CTL &= ~(PWM_CTL_CNTEN4_Msk | PWM_CTL_CNTEN5_Msk);
	s_count = 0;
	PWM_DisablePeriodInt(PWM, PWM_CH4);
	
	TX_POWER_DISABLE;	// tx power disable.
}

enum ULTRANSONIC_STATE poll_ultrasonic_state(void)
{
	if(s_ultransonic_state == ULTRANSONIC_ECHO_RECV)
	{
		count_end();
		
		s_distance = 0.017*s_echo_count/CyclesPerUs - 3.5; // 0.017us/cm, 3.5 is for calibration.
		s_ultransonic_state = ULTRANSONIC_OVER;
	}
	
	else
	{
		uint32_t current_count = TIMER_GetCounter(TIMER1);
		if(current_count > TIME_OUT_THRESHOLD) // 35ms, time out.
		{
			
			count_end();
			s_distance = 0;
			s_ultransonic_state = ULTRANSONIC_OVER;
		}
	}
	return s_ultransonic_state;
}


void ultrasonic_measure_start(void)
{
	ULTRANSONIC_RECEIVE_DISABLE;
	transmint8Puls();
	ULTRANSONIC_RECEIVE_ENABLE;
	NVIC_EnableIRQ(GPIO234_IRQn);	
	s_ultransonic_state = ULTRANSONIC_TRIGGER;
}

float ultrasonic_get_distance(void)
{
	return s_distance;
}
	
void ultrasonicSensorInit(void)
{
	pinMode(T_PWR, GPIO_MODE_OUTPUT);
	pinMode(RECEIVE_SINGAL, GPIO_MODE_INPUT);
	
	ULTRANSONIC_RECEIVE_DISABLE;
	TX_POWER_DISABLE;
	
	// tx pwm init.
	CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL2_PWMCH45SEL_Msk))| CLK_CLKSEL2_PWMCH45SEL_HCLK;
    CLK_EnableModuleClock(PWMCH45_MODULE);        //enable the clock
	PWM->POEN |= PWM_POEN_POEN4_Msk; // enable pwm4 output.
	PWM->POEN |= PWM_POEN_POEN5_Msk; // enable pwm5 output.
	SYS->P0_MFP |= SYS_MFP_P05_PWM0_CH4|SYS_MFP_P04_PWM0_CH5;
	PWM_ConfigOutputChannel(PWM, PWM_CH4, 40000, 50);
	PWM_ConfigOutputChannel(PWM, PWM_CH5, 40000, 50);
	PWM->CTL |= PWM_CTL_PINV4_Msk; // PWM CHANNEL 4 invert.
	
	s_ultransonic_state = ULTRANSONIC_READY;
	GPIO_EnableInt(P3, 5, GPIO_INT_FALLING);
}

void GPIO234_IRQHandler(void)
{
	if(s_ultransonic_state == ULTRANSONIC_TRIGGER) // rasing edge.
	{
		if(GPIO_GET_INT_FLAG(P3, BIT5))
		{
			GPIO_CLR_INT_FLAG(P1, BIT5);
			s_echo_count = TIMER_GetCounter(TIMER1);
			NVIC_DisableIRQ(GPIO234_IRQn);
			s_ultransonic_state = ULTRANSONIC_ECHO_RECV;
		}
	}
		
	/* Un-expected interrupt. Just clear all PORT2, PORT3 and PORT4 interrupts */
	P2->INTSRC = P2->INTSRC;
	P3->INTSRC = P3->INTSRC;
	P4->INTSRC = P4->INTSRC;
}
