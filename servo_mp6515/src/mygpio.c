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
     (pin == 0x14) || (pin == 0x15))
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
      PIN_Mode(3,pin_num,mode);
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
  if(!is_pin_analog(pin))
  {
    return -1;
  }
  else
  {
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_ADC_S_Msk) | CLK_CLKSEL1_ADC_S_HXT;//Select ADC clock source
    CLK->CLKDIV |= CLK_CLKDIV_ADC(1);    //select divider
    CLK->APBCLK |= CLK_APBCLK_ADC_EN_Msk;//Enable ADC clock source
    ADC->ADCR = (ADC_ADCR_ADMD_SINGLE | ADC_ADCR_DIFFEN_SINGLE_END | ADC_ADCR_ADEN_CONVERTER_ENABLE);//Set ADC mode
    switch(pin)
    {
      case 0x10:
        SYS->P1_MFP |= SYS_MFP_P10_AIN0;
        ADC_SET_INPUT_CHANNEL(ADC,1<<0);
        break;
      case 0x12:
        SYS->P1_MFP |= SYS_MFP_P12_AIN2;
        ADC_SET_INPUT_CHANNEL(ADC,1<<2);
        break;
      case 0x13:
        SYS->P1_MFP |= SYS_MFP_P13_AIN3;
        ADC_SET_INPUT_CHANNEL(ADC,1<<3);
        break;
      case 0x14:
        SYS->P1_MFP |= SYS_MFP_P14_AIN4;
        ADC_SET_INPUT_CHANNEL(ADC,1<<4);
        break;
      case 0x15:
        SYS->P1_MFP |= SYS_MFP_P15_AIN5;
        ADC_SET_INPUT_CHANNEL(ADC,1<<5);
        break;
      default:
        break;
    }
    ADC_CLR_INT_FLAG(ADC, ADC_ADSR_ADF_Msk);//clear the converte end flag for safety
    ADC_START_CONV(ADC);//start conversion 
    while(!ADC_GET_INT_FLAG(ADC,ADC_ADF_INT));
    ADC_STOP_CONV(ADC);
    ADC_CLR_INT_FLAG(ADC,ADC_ADF_INT);
    return ADC->ADDR[pin - 0x10] & 0x0fff;
  }
}
//void analogWrite(uint8_t pin, int val)
//{

//}
