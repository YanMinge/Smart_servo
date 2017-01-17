#include "mygpio.h"

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int pin_to_index(int pin)
{
    return (pin&0xf0)*8+pin&0x0f;
}
	
boolean is_pin_servo(int pin)
{
    if((pin&0xf0) == 0x20)
    {
        return true;
    }
    else
    {
        return false;		
    }			
}

boolean is_pin_pwm(int pin)
{
    if((pin&0xf0) == 0x20)
    {
        return true;
    }
    else
    {
        return false;		
    }			
}

boolean is_pin_analog(int pin)
{
    if((pin == 0x10) || (pin == 0x12) || (pin == 0x13) ||
       (pin == 0x14) || (pin == 0x15)||(pin == 0x53))
    {
        return true;		
    }
    else
    {
        return false;		
	}			
}

void pinMode(uint8_t pin,uint8_t mode)
{
    uint8_t port,pin_num;
    port = (pin&0xf0) >> 4;
    pin_num =  1 << (pin&0x0f);
    switch(port)
    {
        case 0:
            PIN_Mode(0,pin_num,mode);
            break;
        case 1:
            PIN_Mode(1,pin_num,mode);
            break;
        case 2:
            PIN_Mode(2,pin_num,mode);
            break;
        case 3:
            PIN_Mode(3,pin_num,mode);
            break;
        case 4:
            PIN_Mode(4,pin_num,mode);
            break;
		case 5:
            PIN_Mode(5,pin_num,mode);
            break;
        default:
            break;
    }				
}

uint32_t* Pin2Addr(uint8_t pin)
{
    uint32_t *addr;
    uint8_t port, pin_num;
    port = (pin&0xf0) >> 4;
    pin_num =  pin&0x0f;
    addr = (uint32_t *)((GPIO_PIN_DATA_BASE+(0x20*(port))) + ((pin_num)<<2));
    return addr;
}

void digitalWrite(uint8_t pin,int val)
{
    *(Pin2Addr(pin)) = val;
}

int digitalRead(uint8_t pin)
{
    uint8_t port, pin_num;
    port = (pin&0xf0) >> 4;
    pin_num =  pin&0x0f;
    return GPIO_PIN_ADDR(port,pin_num);
}

int analogRead(uint8_t pin)
{
	CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_ADCSEL_Msk) | CLK_CLKSEL1_ADCSEL_XTAL;//Select ADC clock source
	CLK->CLKDIV |= CLK_CLKDIV_ADC(1);    //select divider
	CLK->APBCLK |= CLK_APBCLK_ADCCKEN_Msk;//Enable ADC clock source
	ADC->CTL = ADC_CTL_ADCEN_Msk | ADC_CTL_ADCIEN_Msk | ADC_CTL_HWTRGSEL_Msk | ADC_CTL_HWTRGEN_Msk;
	switch(pin)
	{
		case 0x53:
			SYS->P5_MFP |= SYS_MFP_P53_ADC_CH0;
			ADC_SET_INPUT_CHANNEL(ADC,1<<0);
			break;
		case 0x10:
            SYS->P1_MFP |= SYS_MFP_P10_ADC_CH1;
            ADC_SET_INPUT_CHANNEL(ADC,1<<1);
            break;
        case 0x12:
            SYS->P1_MFP |= SYS_MFP_P12_ADC_CH2;
            ADC_SET_INPUT_CHANNEL(ADC,1<<2);
            break;
        case 0x13:
            SYS->P1_MFP |= SYS_MFP_P13_ADC_CH3;
            ADC_SET_INPUT_CHANNEL(ADC,1<<3);
            break;
        case 0x14:
            SYS->P1_MFP |= SYS_MFP_P14_ADC_CH4;
            ADC_SET_INPUT_CHANNEL(ADC,1<<4);
            break;
        case 0x15:
            SYS->P1_MFP |= SYS_MFP_P15_ADC_CH5;
            ADC_SET_INPUT_CHANNEL(ADC,1<<5);
            break;
		case 0x30:
			SYS->P3_MFP |= SYS_MFP_P30_ADC_CH6;
            ADC_SET_INPUT_CHANNEL(ADC,1<<6);
            break;
		case 0x31:
			SYS->P3_MFP |= SYS_MFP_P31_ADC_CH7;
            ADC_SET_INPUT_CHANNEL(ADC,1<<7);
            break;
        default:
            break;
	}
        
	ADC_CLR_INT_FLAG(ADC, ADC_ADIF_INT);//clear the converte end flag for safety
    ADC_START_CONV(ADC);//start conversion 
    while(!ADC_GET_INT_FLAG(ADC,ADC_ADIF_INT));
    ADC_STOP_CONV(ADC);
    ADC_CLR_INT_FLAG(ADC,ADC_ADIF_INT);
    return ((ADC)->DAT & ADC_DAT_RESULT_Msk);
}
//void analogWrite(uint8_t pin, int val)
//{

//}

void pwm_write(int pin,int fq, uint8_t duty)
{
    switch(pin)
    {
		case 0x07:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_PWMCH01SEL_Msk))| CLK_CLKSEL1_PWMCH01SEL_HCLK;
            CLK_EnableModuleClock(PWMCH01_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN0_Msk;                 //Enable PWM Output
            SYS->P0_MFP |= SYS_MFP_P07_PWM0_CH0;               //Set P0 multi-function pins for PWM Channel-0
            PWM_ConfigOutputChannel(PWM, PWM_CH0, fq, duty);   //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN0_Msk;
            break;
			
        case 0x22:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_PWMCH01SEL_Msk))| CLK_CLKSEL1_PWMCH01SEL_HCLK;
            CLK_EnableModuleClock(PWMCH01_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN0_Msk;                 //Enable PWM Output
            SYS->P2_MFP |= SYS_MFP_P22_PWM0_CH0;               //Set P2 multi-function pins for PWM Channel-0
            PWM_ConfigOutputChannel(PWM, PWM_CH0, fq, duty);   //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN0_Msk;
            break;
        case 0x23:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_PWMCH01SEL_Msk))| CLK_CLKSEL1_PWMCH01SEL_HCLK;
            CLK_EnableModuleClock(PWMCH01_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN1_Msk;                 //Enable PWM Output
            SYS->P2_MFP |= SYS_MFP_P23_PWM0_CH1;               //Set P2 multi-function pins for PWM Channel-1
            PWM_ConfigOutputChannel(PWM, PWM_CH1, fq, duty);    //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN1_Msk;
            break;
        case 0x24:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_PWMCH23SEL_Msk))| CLK_CLKSEL1_PWMCH23SEL_HCLK;
            CLK_EnableModuleClock(PWMCH23_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN2_Msk;                 //Enable PWM Output
            SYS->P2_MFP |= SYS_MFP_P24_PWM0_CH2;               //Set P2 multi-function pins for PWM Channel-2
			PWM_ConfigOutputChannel(PWM, PWM_CH2, fq, duty);    //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN2_Msk;
            break;
        case 0x25:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_PWMCH23SEL_Msk))| CLK_CLKSEL1_PWMCH23SEL_HCLK;
            CLK_EnableModuleClock(PWMCH23_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN3_Msk;                 //Enable PWM Output
            SYS->P2_MFP |= SYS_MFP_P25_PWM0_CH3;               //Set P2 multi-function pins for PWM Channel-3
            PWM_ConfigOutputChannel(PWM, PWM_CH3, fq, duty);       //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN3_Msk;
            break;
        case 0x26:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL2_PWMCH45SEL_Msk))| CLK_CLKSEL2_PWMCH45SEL_HCLK;
            CLK_EnableModuleClock(PWMCH45_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN4_Msk;                 //Enable PWM Output
            SYS->P2_MFP |= SYS_MFP_P26_PWM0_CH4;               //Set P2 multi-function pins for PWM Channel-3
            PWM_ConfigOutputChannel(PWM, PWM_CH4, fq, duty);       //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN4_Msk;
            break;
		case 0x05:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL2_PWMCH45SEL_Msk))| CLK_CLKSEL2_PWMCH45SEL_HCLK;
            CLK_EnableModuleClock(PWMCH45_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN4_Msk;                 //Enable PWM Output
            SYS->P0_MFP |= SYS_MFP_P05_PWM0_CH4;               //Set P2 multi-function pins for PWM Channel-4
            PWM_ConfigOutputChannel(PWM, PWM_CH4, fq, duty);       //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN4_Msk;
            break;
		case 0x04:
			CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL2_PWMCH45SEL_HCLK))| CLK_CLKSEL2_PWMCH45SEL_HCLK;
            CLK_EnableModuleClock(PWMCH45_MODULE);        //enable the clock
            PWM->POEN |= PWM_POEN_POEN5_Msk;                 //Enable PWM Output
            SYS->P0_MFP |= SYS_MFP_P04_PWM0_CH5;               //Set P0 multi-function pins for PWM Channel-5
            PWM_ConfigOutputChannel(PWM, PWM_CH5, fq, duty);       //Set PWM frequence and duty.
			PWM->CTL |= PWM_CTL_CNTEN5_Msk;
            break;
        default:
            break;
    }
}


void pwm_canceled(int pin)
{
    switch(pin)
    {
        case 0x22:
		case 0x07:
			PWM->POEN &= ~PWM_POEN_POEN0_Msk;                 //Disable PWM Output
            PWM->CTL &= ~PWM_CTL_CNTEN0_Pos;
            break;
        case 0x23:
			PWM->POEN &= ~PWM_POEN_POEN1_Msk;                 //Disable PWM Output
            PWM->CTL &= ~PWM_CTL_CNTEN1_Pos;
            break;
        case 0x24:
			PWM->POEN &= ~PWM_POEN_POEN2_Msk;                 //Disable PWM Output
            PWM->CTL &= ~PWM_CTL_CNTEN2_Pos;
            break;
        case 0x25:
            PWM->POEN &= ~PWM_POEN_POEN3_Msk;                 //Disable PWM Output
            PWM->CTL &= ~PWM_CTL_CNTEN3_Pos;
            break;
		case 0x04:
			PWM->POEN &= ~PWM_POEN_POEN5_Msk;                 //Disable PWM Output
            PWM->CTL &= ~PWM_CTL_CNTEN5_Pos;
            break;
		default:
            break;
    }
}

