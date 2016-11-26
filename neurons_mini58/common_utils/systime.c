#include "systime.h"
#include "clk.h"

unsigned long millis(void)
{
  return system_time;
}

/**
  * @brief      This function execute delay function.
  * @param      us  Delay time. The Max value is 2^24 / CPU Clock(MHz). Ex:
  *                             50MHz => 335544us, 48MHz => 349525us, 28MHz => 699050us ...
  * @return     None
  * @details    Use the SysTick to generate the delay time and the UNIT is in us.
  *             The SysTick clock source is from HCLK, i.e the same as system core clock.
  */
void delayMicroseconds(uint32_t us)
{
    CLK_SysTickDelay(us);	
}

void delay(unsigned long ms)
{
    unsigned long start_time = millis();
    while ((millis() - start_time) <= ms) 
    {
        //do nothing
    }
}


