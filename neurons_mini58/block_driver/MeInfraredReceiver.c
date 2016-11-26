#include "MeInfraredReceiver.h"
#include "uart_printf.h"
#include "Interrupt.h"

volatile IR_STATE g_ir_state = IR_READY;
volatile unsigned char ir_recv_bit[32];
volatile uint16_t  g_inframed_address;
volatile uint8_t  g_inframed_data;
volatile uint32_t  g_test_time = 0;

static boolean parse_inframe_code(uint16_t* address, uint8_t* data)
{
	uint8_t data_reverse;
	uint8_t* p_temp;
	
	// parse address.
	int i = 0;
	for(i = 0; i < 16; i++)
	{
		*address =  *address >> 1;
		if(ir_recv_bit[i] > 140&&ir_recv_bit[i]<190)
		{
			*address |= 0x080;
		}
		else if(ir_recv_bit[i] > 40&&ir_recv_bit[i]<90)
		{
			*address &= 0x7f;	
		}
		else
		{
			return false;
		}
	}
	
	// parse data.
	p_temp = ir_recv_bit + 16;
	for(i = 0; i< 8; i++)
	{
		*data =  *data >> 1;
		if(p_temp[i] > 140&&p_temp[i]<190)
		{
			*data |= 0x080;
		}
		else if(p_temp[i] > 40&&p_temp[i]<90)
		{
			*data &= 0x7f;	
		}
		else
		{
			return false;
		}
	}
	
	// parse data reverse.
	p_temp += 8;
	for(i = 0; i< 8; i++)
	{
		data_reverse =  data_reverse >> 1;
		if(p_temp[i] > 140&&p_temp[i]<190)
		{
			data_reverse |= 0x080;
		}
		else if(p_temp[i] > 40&&p_temp[i]<90)
		{
			data_reverse &= 0x7f;	
		}
		else
		{
			return false;
		}	
	}
	if(((*data)&data_reverse) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

boolean ir_recv_poll(void)
{
	static boolean s_led_flag = 0;
	static uint32_t current_time = 0;
	static uint32_t previous_time = 0;
	
	// blank out the led.
	if(s_led_flag == 1)
	{
		current_time = millis(); 
		if(current_time - previous_time > 20)
		{
			digitalWrite(LED_PIN, 1);
			s_led_flag = 0;
		}
	}
	
	if(g_ir_state == IR_FRAME_END)
	{
		s_led_flag = 1;
		previous_time = millis();
		digitalWrite(LED_PIN, 0);
		g_ir_state = IR_READY;
		parse_inframe_code(&g_inframed_address, &g_inframed_data);
		return true;
	}
	else if(g_ir_state == IR_FRAME_REPEAT)
	{
		g_ir_state = IR_READY;
		return false;
	}
	else if(g_ir_state == IR_ERROR)
	{
		g_ir_state = IR_READY;
		return false;
	}
	else
	{
		return false;
	}
}

void InfraredReceiverInit(void)
{
	GPIO_SetMode(P5, BIT2, GPIO_MODE_INPUT);
	GPIO_EnableInt(P5, 2, GPIO_INT_BOTH_EDGE);
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_HCLK, GPIO_DBCTL_DBCLKSEL_4096);
	GPIO_ENABLE_DEBOUNCE(P5, BIT2);
    NVIC_EnableIRQ(EINT1_IRQn);
	
	// for state led.
	pinMode(LED_PIN, GPIO_MODE_OUTPUT);
	digitalWrite(LED_PIN, 1);
}
