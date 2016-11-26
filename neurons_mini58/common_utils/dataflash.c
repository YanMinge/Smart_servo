#include <stdio.h>
#include "dataflash.h"
#include "uart_printf.h"
#include <string.h>

// Register values.

uint8_t flash_checksum(const uint8_t *buffer, uint8_t size, uint8_t sum)
// Adds the buffer to the checksum passed in returning the updated sum.
{
  // Keep going while bytes in the buffer.
  while (size)
  {
    // Add the buffer to the sum.
    sum += *buffer;

    // One less byte left.
    ++buffer;
    --size;
  }

  // Return the new sum.
  return sum;
}

int32_t spi_flash_erase_sector(int8_t sec)
{
  int32_t result;
  uart_printf(UART0,"spi_flash_erase_sector(%d)\r\n",sec);
  if((sec < 0) || (sec > 7))
  {
    result = -1;
  }
  SYS_UnlockReg();
  FMC_Open();
  result = FMC_Erase(DATA_FLASH_ADDR_BASE+sec*DATA_FLASH_PAGE_SIZE);
  FMC_Close();
  SYS_LockReg();
  return result;
}

int32_t spi_flash_read(uint32_t src_addr,uint32_t *des_addr,uint16_t size)
{
  int32_t result = 1;
  //int32_t Rdata = 0;
  uint16_t index;
  uint16_t index_max;

  if((src_addr < DATA_FLASH_ADDR_BASE) || 
     (src_addr > DATA_FLASH_ADDR_END) || 
     (size + ((src_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE) > DATA_FLASH_PAGE_SIZE))
  {
    uart_printf(UART0,"src_addr wrong(0x%x)\r\n",src_addr);
    return -1;
  }

  if((size % 4) == 0)
  {
    index_max = size/4;
  }
  else
  {
    index_max = size/4 + 1;
  }

  SYS_UnlockReg();
  FMC_Open();
  uart_printf(UART0,"spi_flash_read \r\n");
  for(index = 0; index < index_max;index++)
  {
    uart_printf(UART0,"spi_flash_read addr: 0x%x\r\n",src_addr+index*4);
    *des_addr = FMC_Read(src_addr+index*4);
    uart_printf(UART0,"spi_flash_read addr: 0x%x=(0x%x)\r\n",src_addr+index*4,*des_addr);
    des_addr++;
  }
  FMC_Close();
  SYS_LockReg();

  return result;
}

int32_t spi_flash_write(uint32_t des_addr,uint32_t *src_addr,uint16_t size)
{
  uint16_t index;
  int32_t result;
  int32_t sec;
  uint32_t temp[128];

  if((des_addr < DATA_FLASH_ADDR_BASE) || 
     (des_addr > DATA_FLASH_ADDR_END) || 
     (size + ((des_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE) > DATA_FLASH_PAGE_SIZE))
  {
    return -1;
  }

  sec = (des_addr - DATA_FLASH_ADDR_BASE)/DATA_FLASH_PAGE_SIZE;
  index = (des_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE;

  spi_flash_read(DATA_FLASH_ADDR_BASE+sec*DATA_FLASH_PAGE_SIZE,temp,DATA_FLASH_PAGE_SIZE);

  uart_printf(UART0,"memcpy,index(%d)\r\n",index);
  memcpy(temp+(index/4),src_addr,size);

  spi_flash_erase_sector(sec);

  SYS_UnlockReg();
  FMC_Open();

  for(index = 0; index < DATA_FLASH_PAGE_SIZE/4;index++)
  {
    uart_printf(UART0,"spi_flash_write sec(%d) addr: 0x%x=0x%x\r\n",index,DATA_FLASH_ADDR_BASE+index*4,temp[index]);
    FMC_Write(DATA_FLASH_ADDR_BASE+index*4,temp[index]);
  }
  FMC_Close();
  SYS_LockReg();

  return result;
}

int32_t spi_flash_restore_registers(void)
{
  uint32_t *MagicWord;

  spi_flash_read(DATA_FLASH_ADDR_BASE,MagicWord,4);

  uart_printf(UART0,"MagicWord word is (0x%x)\r\n",*MagicWord);

  // Does the version match?
  if(*MagicWord != FLASH_MAGIC_WORD)
  { 
    uart_printf(UART0,"MagicWord wrong!\r\n");
    return 0;
  }
  //spi_flash_read(DATA_FLASH_ADDR_BASE+4,registers,8);
  // Return success.
  return 1;
}

