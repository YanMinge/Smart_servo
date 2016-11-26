#include "systime.h"

unsigned long millis(void)
{
  return system_time;
}

void delay(unsigned long ms)
{
  unsigned long start_time = millis();
  while ((millis() - start_time) <= ms) 
  {
    //do nothing
  }
}


