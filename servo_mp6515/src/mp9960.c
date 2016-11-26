#include "mp9960.h"
#include "uart_printf.h"
#include <string.h>
#include <math.h>
#include "Interrupt.h"
#include "systime.h"
#include "Sysinit.h"

void mp9960_init(void)
{
  /*---------------------------------------------------------------------------------------------------------*/
  /* Init SPI                                                                                                */
  /*---------------------------------------------------------------------------------------------------------*/
  /* Configure as a master, clock idle low, 16-bit transaction, drive output on falling clock edge and latch input on rising edge. */
  /* Set IP clock divider. SPI clock rate = 6MHz */
  SPI_Open(SPI1, SPI_MASTER, SPI_MODE_0, 16, 6000000);

  /* Enable the automatic hardware slave select function. Select the SS pin and configure as low-active. */
  SPI_EnableAutoSS(SPI1, SPI_SS, SPI_SS_ACTIVE_LOW);
  //  SPI_SET_SS_LOW(SPI1);
}

int16_t mp9960_read_raw_angle(void)
{
  int raw_angle;
  SPI_SET_SS_LOW(SPI1);
  SPI_WRITE_TX0(SPI1, 0x0000);
  SPI_TRIGGER(SPI1);
  while(SPI_IS_BUSY(SPI1));
  raw_angle = SPI_READ_RX0(SPI1);
  raw_angle = (raw_angle >> 4) * 2;
  SPI_SET_SS_HIGH(SPI1);
  return raw_angle;
}
