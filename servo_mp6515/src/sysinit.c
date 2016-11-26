#include "systime.h"
#include "sysinit.h"

volatile static uint32_t s_while_count = 0;

void TMR0_Init(uint32_t u32Freq)
{
  /* Configure timer to operate in periodic mode and frequency = 1000Hz*/
  TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, u32Freq);

  /* Enable the Timer time-out interrupt */
  TIMER_EnableInt(TIMER0);

  /* Start Timer counting */
  TIMER_Start(TIMER0);
 
  /* Enable TMR0 Interrupt */
  NVIC_EnableIRQ(TMR0_IRQn);
}

void SYS_Init(void)
{
  /*---------------------------------------------------------------------------------------------------------*/
  /* Init System Clock                                                                                       */
  /*---------------------------------------------------------------------------------------------------------*/
  /* Enable Internal RC 22.1184MHz clock */
  CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

  /* Waiting for Internal RC clock ready */
  CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

//  /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
//  CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

  /* Enable external XTAL 12MHz clock */
  CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

  /* Waiting for external XTAL clock ready */
  CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

  /* Switch HCLK clock source to HXT and HCLK source divide 1 */
  CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));

  /* Set core clock as PLL_CLOCK from PLL */
  CLK_SetCoreClock(PLL_CLOCK);

  /* Enable UART0 and UART1 module clock */
  CLK_EnableModuleClock(UART0_MODULE);
  CLK_EnableModuleClock(UART1_MODULE);

  CLK_EnableModuleClock(TMR0_MODULE);

  /* Select UART0 and UART1 module clock source */
  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
  CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
//  CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
//  CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));

  /* Select HCLK as the clock source of SPI1 */
  CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL1_SPI1_S_HCLK, MODULE_NoMsk);

  CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HXT, 0);


  /* Enable SPI1 peripheral clock */
  CLK_EnableModuleClock(SPI1_MODULE);

  /* Update System Core Clock */
  /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
  SystemCoreClockUpdate();

  /*---------------------------------------------------------------------------------------------------------*/
  /* Init I/O Multi-function                                                                                 */
  /*---------------------------------------------------------------------------------------------------------*/

  /* Set P3 multi-function pins for UART0 RXD and TXD */
  SYS->P3_MFP &= ~(SYS_MFP_P30_Msk | SYS_MFP_P31_Msk);
  SYS->P3_MFP |= (SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0);
		
  /* Set P0 multi-function pins for UART1 RXD and TXD */
  SYS->P0_MFP &= ~(SYS_MFP_P00_Msk | SYS_MFP_P01_Msk);
  SYS->P0_MFP |= (SYS_MFP_P00_TXD1 | SYS_MFP_P01_RXD1);

  /* Set multi function pin for SPI1 */
  SYS->P0_MFP |= SYS_MFP_P04_SPISS1 | SYS_MFP_P05_MOSI_1 | SYS_MFP_P06_MISO_1 | SYS_MFP_P07_SPICLK1;
}

void UART_Function_Init(void)
{
  /* Enable Interrupt and install the call back function */
  UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk);
  NVIC_EnableIRQ(UART0_IRQn);

  /* Enable Interrupt and install the call back function */
  UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk);
  NVIC_EnableIRQ(UART1_IRQn);
}

/* Configure P2.3 external interrupt */
void GPIO_P23_INT_init(void)
{
  GPIO_SetMode(P2, BIT3, GPIO_PMD_INPUT);
  GPIO_EnableEINT0(P2, 3, GPIO_INT_FALLING);
  NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
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
    s_while_count = 0;
    return 1;
  }
  else
  {
    return 0;
  }
}

