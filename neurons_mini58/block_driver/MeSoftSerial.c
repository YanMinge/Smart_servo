#include "MeSoftSerial.h"
#include "mygpio.h"
#include "uart_printf.h"

/*******************************************************************
NOTE: this software serial dirver is only suitable for 9600 baudrate
********************************************************************/

#define MAX_RECEIVE_BUF           64
#define FIRST_SAMPLE_INTERVAL     400
#define SAMPLE_INTERVAL           1250
#define TIMER_COUNT_STOP          TIMER1->CTL &= ~TIMER_CTL_CNTEN_Msk
#define TIMER_COUNT_START         TIMER1->CTL |= TIMER_CTL_CNTEN_Msk

static uint32_t* rx_pin_address;
static uint32_t* tx_pin_address;
static uint16_t  s_tx_delay;
volatile static uint8_t s_read_bits_count = 0;
volatile static uint8_t s_write_index = 0;
volatile static uint8_t s_in_index = 0;
volatile static uint8_t s_out_index = 0;
volatile static uint8_t s_pop_bytes = 0;
volatile static uint8_t s_push_bytes = 0;
volatile static uint8_t s_rev_data = 0;
volatile static uint8_t s_rev_array[MAX_RECEIVE_BUF]= {0};

void TMR1_IRQHandler(void)
{
    uint8_t temp = 0x00;
    
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
        if(s_read_bits_count ==0) // start bit.
        {
             TIMER1->CMP = SAMPLE_INTERVAL;
             s_read_bits_count++;
        }
        else if(s_read_bits_count == 9) // stop bit.
        {
            s_read_bits_count = 0;
            TIMER_COUNT_STOP;
            GPIO_CLR_INT_FLAG(P5, BIT4);
            NVIC_EnableIRQ(GPIO5_IRQn);    
            
            if(s_push_bytes - s_pop_bytes < MAX_RECEIVE_BUF)
            {
                s_rev_array[s_in_index] = s_rev_data;
                s_in_index = (s_in_index == (MAX_RECEIVE_BUF -1))? 0:(s_in_index + 1);
                s_push_bytes++;
                s_rev_data = 0;
            }
            
        }
        else
        {
            temp = *rx_pin_address;
            s_rev_data |= (temp << (s_read_bits_count-1));
            s_read_bits_count++;
        }
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  GPIO IRQ                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
/* P5.4 IRQ Handler */
void GPIO5_IRQHandler(void)
{
    /* To check if P5.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(P5, BIT4))
    {
        /* Clear P5.2 interrupt flag */
        GPIO_CLR_INT_FLAG(P5, BIT4);
        NVIC_DisableIRQ(GPIO5_IRQn);
        /* Start Timer counting */
        TIMER1->CMP = FIRST_SAMPLE_INTERVAL;
        TIMER_COUNT_START;
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



static void BitDelay(uint16_t delay)
{
	SysTick->LOAD = delay;
    SysTick->VAL  =  (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    
    /* Waiting for down-count to zero */
    while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}

uint8_t SoftwareSerialReadAvaiable(void)
{
    return (s_push_bytes - s_pop_bytes);
}

uint8_t SoftwareSerialReadByte(void)
{
    uint8_t ret_value;
    s_pop_bytes++;
    ret_value = s_rev_array[s_out_index];
    s_out_index = (s_out_index == (MAX_RECEIVE_BUF -1))? 0:(s_out_index + 1);
    return ret_value;
}
	
// initial the software serial.
void SoftwareSerialBegin(uint8_t pin, uint32_t baud_rate)
{
	pinMode(pin, GPIO_MODE_OUTPUT);
	tx_pin_address =Pin2Addr(pin) ;
	digitalWrite(pin, 1);
	s_tx_delay = PLL_CLOCK/baud_rate;
}

void SoftwareSerialBeginInterrupt(uint32_t baud_rate)
{
    // serial input config, FALLING edge triger serial sampling.
    GPIO_SetMode(P5, BIT4, GPIO_MODE_INPUT);
    rx_pin_address =Pin2Addr(P5_4) ;
	GPIO_EnableInt(P5, 4, GPIO_INT_FALLING);

    NVIC_SetPriority(GPIO5_IRQn, 2);
    NVIC_EnableIRQ(GPIO5_IRQn);
    
    // timer config for serial sampling.
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_XTAL, 0);
	CLK_EnableModuleClock(TMR1_MODULE);
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, baud_rate>>1);
    TIMER_EnableInt(TIMER1); /* Enable the Timer time-out interrupt */
	NVIC_SetPriority(TMR1_IRQn, 2);
    NVIC_EnableIRQ(TMR1_IRQn); /* Enable TMR1 Interrupt */ 
}
 
// write data.
void SoftwareSerialWrite(uint8_t data)
{
	uint16_t delay = s_tx_delay;

	// start bit.
	*tx_pin_address = 0;
	BitDelay(delay);

	// Write each of the 8 bits
	for (uint8_t i = 8; i > 0; --i)
	{
		if (data & 1) // choose bit
		{
			*tx_pin_address = 1; // send 1.
		}
		else
		{
			*tx_pin_address = 0; // send 0.
		}

		BitDelay(delay);
		data >>= 1;
	}
	
	// stop bit
	*tx_pin_address = 1;
	BitDelay(s_tx_delay);
	
	*tx_pin_address = 1;
}
