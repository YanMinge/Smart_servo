#include <stdio.h>
#include "dataflash.h"
#include "uart_printf.h"
#include <string.h>
#include <stdlib.h>

//#define DATA_FLASH_DEBUG

#define CONFIG0_DFEN                0x01
#define CONFIG0_DFVSEN              0x04

cmd_mode_data_struct_type cmd_mode_data_struct = {0};

SpiFlashOpResult set_data_flash_base(uint32_t u32DFBA)
{
  uint32_t au32Config[2];

  /* Read current User Configuration */
  FMC_ReadConfig(au32Config, 1);

  /* Just return when Data Flash has been enabled */
  if(!(au32Config[0] & 0x1))
  {
    return SPI_FLASH_RESULT_OK;
  }

  /* Enable User Configuration Update */
  FMC_EnableConfigUpdate();

  /* Erase User Configuration */
  FMC_Erase(FMC_CONFIG_BASE);

  /* Write User Configuration to Enable Data Flash */
  /* Note: DFVSEN = 1, DATA Flash Size is 4K bytes
           DFVSEN = 0, DATA Flash Size is based on CONFIG1 */
  au32Config[0] &= ~(CONFIG0_DFEN | CONFIG0_DFVSEN);
  au32Config[1] = u32DFBA;

  if(FMC_WriteConfig(au32Config, 2))
  {
    return SPI_FLASH_RESULT_ERR;
  }
#ifdef DATA_FLASH_DEBUG
  uart_printf(UART0,"\nSet Data Flash base as 0x%x.\n", FMC_ReadDataFlashBaseAddr());
#endif /* DATA_FLASH_DEBUG */

  /* Perform chip reset to make new User Config take effect */
  SYS->IPRSTC1 |= SYS_IPRSTC1_CHIP_RST_Msk;

  return SPI_FLASH_RESULT_OK;
}

SpiFlashOpResult set_data_flash_fix_4k(void)
{
  uint32_t au32Config[2];

  /* Read current User Configuration */
  FMC_ReadConfig(au32Config, 1);

  /* Just return when Data Flash has been enabled */
  if(!(au32Config[0] & 0x1))
  {
#ifdef DATA_FLASH_DEBUG
    uart_printf(UART0,"\nData flash has been enabled addr:0x%x.\n", FMC_ReadDataFlashBaseAddr());
#endif /* DATA_FLASH_DEBUG */
    return SPI_FLASH_RESULT_OK;
  }

  /* Enable User Configuration Update */
  FMC_EnableConfigUpdate();

  /* Erase User Configuration */
  FMC_Erase(FMC_CONFIG_BASE);

  /* Write User Configuration to Enable Data Flash */
  /* Note: DFVSEN = 1, DATA Flash Size is 4K bytes
           DFVSEN = 0, DATA Flash Size is based on CONFIG1 */
  au32Config[0] |= (CONFIG0_DFEN | CONFIG0_DFVSEN);

  if(FMC_WriteConfig(au32Config, 2))
  {
    return SPI_FLASH_RESULT_ERR;
  }
#ifdef DATA_FLASH_DEBUG
  uart_printf(UART0,"\nSet Data Flash base as 0x%x.\n", FMC_ReadDataFlashBaseAddr());
#endif /* DATA_FLASH_DEBUG */

  /* Perform chip reset to make new User Config take effect */
  SYS->IPRSTC1 |= SYS_IPRSTC1_CHIP_RST_Msk;

  return SPI_FLASH_RESULT_OK;
}

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

SpiFlashOpResult spi_flash_erase_sector(int8_t sec)
{
  SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
  //uart_printf(UART0,"spi_flash_erase_sector(%d)\r\n",sec);
  if((sec < 0) || (sec > 7))
  {
    return SPI_FLASH_RESULT_ERR;
  }
  SYS_UnlockReg();
  FMC_Open();
  result = (SpiFlashOpResult)FMC_Erase(DATA_FLASH_ADDR_BASE+sec*DATA_FLASH_PAGE_SIZE);
  FMC_Close();
  SYS_LockReg();
  return result;
}

SpiFlashOpResult spi_flash_read(uint32_t src_addr,uint32_t *des_addr,uint16_t size)
{
  SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
  //int32_t Rdata = 0;
  uint16_t index;
  uint16_t index_max;

  if((src_addr < DATA_FLASH_ADDR_BASE) || 
     (src_addr > DATA_FLASH_ADDR_END) || 
     (size + ((src_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE) > DATA_FLASH_PAGE_SIZE))
  {

#ifdef DATA_FLASH_DEBUG
    uart_printf(UART0,"src_addr wrong(0x%x)\r\n",src_addr);
#endif /* DATA_FLASH_DEBUG */

    return SPI_FLASH_RESULT_ERR;
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
  for(index = 0; index < index_max;index++)
  {
    *des_addr = FMC_Read(src_addr+index*4);

#ifdef DATA_FLASH_DEBUG
    uart_printf(UART0,"spi_flash_read addr: 0x%x=(0x%x) (%d)\r\n",src_addr+index*4,*des_addr,index);
#endif /* DATA_FLASH_DEBUG */

    des_addr++;
  }
  FMC_Close();
  SYS_LockReg();

  return result;
}

SpiFlashOpResult spi_flash_write(uint32_t des_addr,uint32_t *src_addr,uint16_t size)
{
  SpiFlashOpResult result = SPI_FLASH_RESULT_OK;
  uint16_t index;
  int32_t sec;
  uint32_t temp[128];
  memset(temp,0,128);

#ifdef DATA_FLASH_DEBUG
  uart_printf(UART0,"spi_flash_write,addr(0x%x),size(%d)\r\n",des_addr,size + ((des_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE));
#endif /* DATA_FLASH_DEBUG */


  if((des_addr < DATA_FLASH_ADDR_BASE) || 
     (des_addr > DATA_FLASH_ADDR_END) || 
     (size + ((des_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE) > DATA_FLASH_PAGE_SIZE))
  {
    free(temp);
    return SPI_FLASH_RESULT_ERR;
  }

  sec = (des_addr - DATA_FLASH_ADDR_BASE)/DATA_FLASH_PAGE_SIZE;
  index = (des_addr - DATA_FLASH_ADDR_BASE)%DATA_FLASH_PAGE_SIZE;

  spi_flash_read(DATA_FLASH_ADDR_BASE+sec*DATA_FLASH_PAGE_SIZE,temp,DATA_FLASH_PAGE_SIZE);

#ifdef DATA_FLASH_DEBUG
  uart_printf(UART0,"memcpy,index(%d)\r\n",index);
#endif /* DATA_FLASH_DEBUG */

  memcpy(temp+(index/4),src_addr,size);

  spi_flash_erase_sector(sec);

  SYS_UnlockReg();
  FMC_Open();

  for(index = 0; index < DATA_FLASH_PAGE_SIZE/4;index++)
  {

#ifdef DATA_FLASH_DEBUG
    uart_printf(UART0,"spi_flash_write sec(%d) addr: 0x%x=0x%x\r\n",index,DATA_FLASH_ADDR_BASE+sec*DATA_FLASH_PAGE_SIZE+index*4,temp[index]);
#endif /* DATA_FLASH_DEBUG */

    FMC_Write(DATA_FLASH_ADDR_BASE+sec*DATA_FLASH_PAGE_SIZE+index*4,temp[index]);
  }
  FMC_Close();
  SYS_LockReg();
  return result;
}

SpiFlashOpResult write_cmd_mode_to_flash(uint32_t cmd_mode)
{
  SpiFlashOpResult result = SPI_FLASH_RESULT_OK;

  cmd_mode_data_struct.cmd_mode_start = USER_DATA_CMD_MODE_CHECK_START;
  cmd_mode_data_struct.cmd_mode_end = USER_DATA_CMD_MODE_CHECK_END;
  cmd_mode_data_struct.cmd_mode = cmd_mode;
  
#ifdef DATA_FLASH_DEBUG
  uart_printf(UART0,"Write primary partition!\r\n");
#endif /* DATA_FLASH_DEBUG */

  result = spi_flash_write(DATA_FLASH_START_POS,(uint32_t *)&cmd_mode_data_struct.cmd_mode_start,sizeof(cmd_mode_data_struct_type));
  if(result == SPI_FLASH_RESULT_ERR)
  {
    uart_printf(UART0,"Write primary partition failed!\r\n");
  }

#ifdef DATA_FLASH_DEBUG
	uart_printf(UART0,"Write backup partition!\r\n");
#endif /* DATA_FLASH_DEBUG */

  result = spi_flash_write(DATA_FLASH_START_POS + DATA_FLASH_PAGE_1_OFFSET,(uint32_t *)&cmd_mode_data_struct.cmd_mode_start,sizeof(cmd_mode_data_struct_type));
  if(result == SPI_FLASH_RESULT_ERR)
  {
    uart_printf(UART0,"Write backup partition failed!\r\n");
  }
  return result;
}

uint32_t flash_read_data_mode(void)
{
  uint32_t cmd_mode_start;
  uint32_t cmd_mode_end;
  uint32_t cmd_mode = FIRMATA_DATA_MODE;
  if((spi_flash_read(USER_DATA_CMD_MODE_START_ADDR,&cmd_mode_start,4) == SPI_FLASH_RESULT_OK) &&
     (spi_flash_read(USER_DATA_CMD_MODE_END_ADDR,&cmd_mode_end,4) == SPI_FLASH_RESULT_OK))
  {
    if((cmd_mode_start + cmd_mode_end == DATA_FULL_RANK) &&
       (spi_flash_read(USER_DATA_CMD_MODE,&cmd_mode,4) == SPI_FLASH_RESULT_OK))
    {
#ifdef DATA_FLASH_DEBUG
      uart_printf(UART0,"read data mode primary partition,cmd_mode(%d)\r\n",cmd_mode);
#endif /* DATA_FLASH_DEBUG */
    }
    else if((spi_flash_read(USER_DATA_CMD_MODE_START_ADDR  + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_start,4) == SPI_FLASH_RESULT_OK) &&
            (spi_flash_read(USER_DATA_CMD_MODE_END_ADDR + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_end,4) == SPI_FLASH_RESULT_OK))
    {
      if((cmd_mode_start + cmd_mode_end == DATA_FULL_RANK) &&
         (spi_flash_read(USER_DATA_CMD_MODE + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode,4) == SPI_FLASH_RESULT_OK))
      {
#ifdef DATA_FLASH_DEBUG
        uart_printf(UART0,"read data backup partition 1,cmd_mode(%d)\r\n",cmd_mode);
#endif /* DATA_FLASH_DEBUG */
	  }
      else
      {
#ifdef DATA_FLASH_DEBUG
        uart_printf(UART0,"write_default_value_to_flash 1\r\n");
#endif /* DATA_FLASH_DEBUG */
        write_cmd_mode_to_flash(FIRMATA_DATA_MODE);
      }
    }
  }
  else if((spi_flash_read(USER_DATA_CMD_MODE_START_ADDR  + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_start,4) == SPI_FLASH_RESULT_OK) &&
          (spi_flash_read(USER_DATA_CMD_MODE_END_ADDR + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_end,4) == SPI_FLASH_RESULT_OK))
  {
    if((cmd_mode_start + cmd_mode_end == DATA_FULL_RANK) &&
       (spi_flash_read(USER_DATA_CMD_MODE + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode,4) == SPI_FLASH_RESULT_OK))
    {
#ifdef DATA_FLASH_DEBUG
      uart_printf(UART0,"read data backup partition 2,cmd_mode(%d)\r\n",cmd_mode);
#endif /* DATA_FLASH_DEBUG */
    }
  }
  else
  {
#ifdef DATA_FLASH_DEBUG
    uart_printf(UART0,"write_default_value_to_flash 2\r\n");
#endif /* DATA_FLASH_DEBUG */
    write_cmd_mode_to_flash(FIRMATA_DATA_MODE);
  }
  return cmd_mode;
}

SpiFlashOpResult flash_write_data_mode(uint32_t cmd_mode)
{
  SpiFlashOpResult result = SPI_FLASH_RESULT_OK;

  uint32_t cmd_mode_start;
  uint32_t cmd_mode_end;
  uint32_t cmd_mode_temp = FIRMATA_DATA_MODE;

  if((spi_flash_read(USER_DATA_CMD_MODE_START_ADDR,&cmd_mode_start,4) == SPI_FLASH_RESULT_OK) &&
     (spi_flash_read(USER_DATA_CMD_MODE_END_ADDR,&cmd_mode_end,4) == SPI_FLASH_RESULT_OK) &&
     (spi_flash_read(USER_DATA_CMD_MODE,&cmd_mode_temp,4) == SPI_FLASH_RESULT_OK) &&
     (cmd_mode_start + cmd_mode_end == DATA_FULL_RANK) &&
     (cmd_mode_temp == cmd_mode))
  {
#ifdef DATA_FLASH_DEBUG
    uart_printf(UART0,"No need change 1\r\n");
#endif /* DATA_FLASH_DEBUG */
    return result;
  }

  if((spi_flash_read(USER_DATA_CMD_MODE_START_ADDR + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_start,4) == SPI_FLASH_RESULT_OK) &&
     (spi_flash_read(USER_DATA_CMD_MODE_END_ADDR + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_end,4) == SPI_FLASH_RESULT_OK) &&
     (spi_flash_read(USER_DATA_CMD_MODE + DATA_FLASH_PAGE_1_OFFSET,&cmd_mode_temp,4) == SPI_FLASH_RESULT_OK) &&
     (cmd_mode_start + cmd_mode_end == DATA_FULL_RANK) &&
     (cmd_mode_temp == cmd_mode))
  {
#ifdef DATA_FLASH_DEBUG
		  uart_printf(UART0,"No need change 2\r\n");
#endif /* DATA_FLASH_DEBUG */
    return result;
  }

  result = write_cmd_mode_to_flash(cmd_mode);

  return result;
}

