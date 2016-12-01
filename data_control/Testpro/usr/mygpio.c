#include "mygpio.h"
#include "uart_printf.h"

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void pinMode(uint8_t pin,uint8_t mode)
{
  uint8_t port,pin_num;
  uint16_t gpio_pin = 0;
  GPIO_InitTypeDef GPIO_InitStructure;

  port = (pin & 0xf0) >> 4;
  pin_num = (pin & 0x0f);
  gpio_pin = 1 << pin_num;

  GPIO_InitStructure.GPIO_Pin = gpio_pin;

  if(mode == INPUT)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  }
  else if(mode == OUTPUT)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  }
  else if(mode == INPUT_PULLUP)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  }
  else if(mode == INPUT_PULLDOWN)
  {
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  }
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  switch(port)
  {
    case 0:
      GPIO_Init(GPIOA, &GPIO_InitStructure);
      break;
    case 1:
      GPIO_Init(GPIOB, &GPIO_InitStructure);
      break;
    default:
      break;
  }
}

void digitalWrite(uint8_t pin,int val)
{
  uint8_t port,pin_num;
  uint16_t gpio_pin;
  port = (pin&0xf0) >> 4;
  pin_num = pin&0x0f;
  gpio_pin = 1 << pin_num;

  switch(port)
  {
    case 0:
      if(val == HIGH)
      {
        GPIO_SetBits(GPIOA, gpio_pin);
      }
      else
      {
        GPIO_ResetBits(GPIOA, gpio_pin);
      }
      break;
    case 1:
      if(val == HIGH)
      {
        GPIO_SetBits(GPIOB, gpio_pin);
      }
      else
      {
        GPIO_ResetBits(GPIOB, gpio_pin);
      }
      break;
    default:
      break;
  }
}

int digitalRead(uint8_t pin)
{
  uint8_t port, pin_num;
  uint16_t gpio_pin;
  uint16_t bitstatus = 0x00;
  port = (pin & 0xf0) >> 4;
  pin_num =  (pin & 0x0f);
  gpio_pin = 1 << pin_num;
  switch(port)
  {
    case 0:
      bitstatus = GPIO_ReadInputDataBit(GPIOA, gpio_pin);
      break;
    case 1:
      bitstatus = GPIO_ReadInputDataBit(GPIOB, gpio_pin);
      break;
    default:
      break;
  }
  return bitstatus;
}

//int analogRead(uint8_t pin)
//{
//  if(!is_pin_analog(pin))
//  {
//    return -1;
//  }
//  else
//  {
//    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_ADC_S_Msk) | CLK_CLKSEL1_ADC_S_HXT;//Select ADC clock source
//    CLK->CLKDIV |= CLK_CLKDIV_ADC(1);    //select divider
//    CLK->APBCLK |= CLK_APBCLK_ADC_EN_Msk;//Enable ADC clock source
//    ADC->ADCR = (ADC_ADCR_ADMD_SINGLE | ADC_ADCR_DIFFEN_SINGLE_END | ADC_ADCR_ADEN_CONVERTER_ENABLE);//Set ADC mode
//    switch(pin)
//    {
//      case 0x10:
//        SYS->P1_MFP |= SYS_MFP_P10_AIN0;
//        ADC_SET_INPUT_CHANNEL(ADC,1<<0);
//        break;
//      case 0x12:
//        SYS->P1_MFP |= SYS_MFP_P12_AIN2;
//        ADC_SET_INPUT_CHANNEL(ADC,1<<2);
//        break;
//      case 0x13:
//        SYS->P1_MFP |= SYS_MFP_P13_AIN3;
//        ADC_SET_INPUT_CHANNEL(ADC,1<<3);
//        break;
//      case 0x14:
//        SYS->P1_MFP |= SYS_MFP_P14_AIN4;
//        ADC_SET_INPUT_CHANNEL(ADC,1<<4);
//        break;
//      case 0x15:
//        SYS->P1_MFP |= SYS_MFP_P15_AIN5;
//        ADC_SET_INPUT_CHANNEL(ADC,1<<5);
//        break;
//      default:
//        break;
//    }
//    ADC_CLR_INT_FLAG(ADC, ADC_ADSR_ADF_Msk);//clear the converte end flag for safety
//    ADC_START_CONV(ADC);//start conversion 
//    while(!ADC_GET_INT_FLAG(ADC,ADC_ADF_INT));
//    ADC_STOP_CONV(ADC);
//    ADC_CLR_INT_FLAG(ADC,ADC_ADF_INT);
//    return ADC->ADDR[pin - 0x10] & 0x0fff;
//  }
//}
//void analogWrite(uint8_t pin, int val)
//{

//}
