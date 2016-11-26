#include "systime.h"
#include "sysinit.h"
#include "mygpio.h"
#include "uart_printf.h"
#include "protocol.h"

#define R_PIN  	P2_2 
#define G_PIN	P2_3
#define B_PIN	P2_4
#define LED_PWM_FREQUENCE 1000

static uint32_t s_while_count = 0;
static uint8_t s_r_value;
static uint8_t s_g_value;
static uint8_t s_b_value;


void set_rgb_led(uint8_t r_value, uint8_t g_value, uint8_t b_value)
{
	s_r_value = 100 - line_map(r_value, 0, 100, 0, 255);
	s_g_value = 100 - line_map(g_value, 0, 100, 0, 255);
	s_b_value = 100 - line_map(b_value, 0, 100, 0, 255);
	
	pwm_write(R_PIN, LED_PWM_FREQUENCE, s_r_value);
	pwm_write(G_PIN, LED_PWM_FREQUENCE, s_g_value);
	pwm_write(B_PIN, LED_PWM_FREQUENCE, s_b_value);
}

uint8_t blink_rgb_led(uint16_t interval)
{
	static uint32_t previous_time = 0;
	static uint8_t  s_count = 0; // blink times.
	static boolean s_blink_flag = 0;
	
	if(s_count < 3)
	{
		uint32_t now_time = millis();
		if(now_time - previous_time > interval) //blink
		{
			if(s_blink_flag == 0) // led off, led common anode.
			{
				pwm_write(R_PIN, LED_PWM_FREQUENCE, 100);
				pwm_write(G_PIN, LED_PWM_FREQUENCE, 100);
				pwm_write(B_PIN, LED_PWM_FREQUENCE, 100);
				s_blink_flag = 1;
			}
			else // led on.
			{
				pwm_write(R_PIN, LED_PWM_FREQUENCE, s_r_value);
				pwm_write(G_PIN, LED_PWM_FREQUENCE, s_g_value);
				pwm_write(B_PIN, LED_PWM_FREQUENCE, s_b_value);
				s_blink_flag = 0;
				s_count++; // blink one time.
			}
			
			previous_time = now_time;
		}
		
		return 0;
	}

	else // bink end
	{
		s_count = 0;
		return 1; 
	}
}

void poll_led_request(void)
{
	// blink the led.
	if(g_handshake_flag)
	{
		if(blink_rgb_led(500))
		{
			g_handshake_flag = 0;
		}
	}
}

void led_indicate_error(void)
{
	pwm_write(R_PIN, LED_PWM_FREQUENCE, 0);
	pwm_write(G_PIN, LED_PWM_FREQUENCE, 100);
	pwm_write(B_PIN, LED_PWM_FREQUENCE, 100);
}

void led_indicate_normal(void)
{
	pwm_write(R_PIN, LED_PWM_FREQUENCE, s_r_value);
	pwm_write(G_PIN, LED_PWM_FREQUENCE, s_g_value);
	pwm_write(B_PIN, LED_PWM_FREQUENCE, s_b_value);
}

void TMR0_Init(uint32_t u32Freq)
{
    /* Configure timer to operate in periodic mode and frequency = 1000Hz*/
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, u32Freq);

    /* Enable the Timer time-out interrupt */
    TIMER_EnableInt(TIMER0);

    /* Start Timer counting */
    TIMER_Start(TIMER0);
	
	NVIC_SetPriority(TMR0_IRQn, 1);

    /* Enable TMR0 Interrupt */
    NVIC_EnableIRQ(TMR0_IRQn);
}

void TMR1_Init(uint32_t u32Freq)
{
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_XTAL, 0);
	
	CLK_EnableModuleClock(TMR1_MODULE);
	
    /* Configure timer to operate in periodic mode and frequency = 1000Hz*/
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, u32Freq);

    /* Enable the Timer time-out interrupt */
    TIMER_EnableInt(TIMER1);

    /* Start Timer counting */
    TIMER_Start(TIMER1);
	
	NVIC_SetPriority(TMR1_IRQn, 1);

    /* Enable TMR0 Interrupt */
    NVIC_EnableIRQ(TMR1_IRQn);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Read User Config to select internal high speed RC */
    SystemInit();

    /* Set P5 multi-function pins for crystal output/input */
    SYS->P5_MFP &= ~(SYS_MFP_P50_Msk | SYS_MFP_P51_Msk);
    SYS->P5_MFP |= (SYS_MFP_P50_XT1_IN | SYS_MFP_P51_XT1_OUT);
	
	/* Enable external 12MHz XTAL, HIRC */
    CLK->PWRCTL |= CLK_PWRCTL_XTL12M | CLK_PWRCTL_HIRCEN_Msk;
	
    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_XTLSTB_Msk | CLK_STATUS_HIRCSTB_Msk);
	
	 /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART0 and UART1 module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(UART1_MODULE);

    CLK_EnableModuleClock(TMR0_MODULE);

	/* Select UART0 and UART1 module clock source */
	CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UARTSEL_XTAL, CLK_CLKDIV_UART(1));
	CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UARTSEL_XTAL, CLK_CLKDIV_UART(1));

	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_XTAL, 0);	

	/* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

	/*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set P1 multi-function pins for UART1 RXD and TXD  */
    SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
    SYS->P1_MFP |= (SYS_MFP_P12_UART0_RXD | SYS_MFP_P13_UART0_TXD);

    /* Set P4 multi-function pins for UART1 RXD and TXD  */
    SYS->P4_MFP &= ~(SYS_MFP_P46_Msk | SYS_MFP_P47_Msk);
    SYS->P4_MFP |= (SYS_MFP_P46_UART1_RXD | SYS_MFP_P47_UART1_TXD);

}

void init_neuron_system(void)
{
    /* Set UART Configuration */
    UART_SetLine_Config(UART0, 115200, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);

    /* Set UART Configuration */
    UART_SetLine_Config(UART1, 115200, UART_WORD_LEN_8, UART_PARITY_NONE, UART_STOP_BIT_1);

    /* UART sample function */
    UART_Function_Init();

    TMR0_Init(1000);
}

void UART_Function_Init(void)
{
    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART0, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk ));
    NVIC_EnableIRQ(UART0_IRQn);

    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    NVIC_EnableIRQ(UART1_IRQn);
}

void clearWhileCount(void)
{
	s_while_count = 0;
}

boolean whileCoutOver_us(uint32_t time)
{
	s_while_count++;
	if(s_while_count>time)
	{
		DEBUG_MSG(UART0, "over time\n");
		s_while_count = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
