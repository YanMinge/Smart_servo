#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

#include "M051Series.h"
#include "mygpio.h"
#include "fmc.h"

#define DATA_FULL_RANK              0xffffffff
#define DATA_FLASH_ADDR_BASE        (FMC->DFBADR)
#define DATA_FLASH_ADDR_END         (FMC->DFBADR + 0x1000)
#define DATA_FLASH_PAGE_SIZE        512
#define DATA_FLASH_PAGE_1_OFFSET           512
#define DATA_FLASH_PAGE_2_OFFSET           512 * 2
#define DATA_FLASH_PAGE_3_OFFSET           512 * 3

//command mode: G code mode and Firmata data mode.
#define G_CODE_MODE             0x01
#define FIRMATA_DATA_MODE       0x00


#define DATA_FLASH_START_POS           DATA_FLASH_ADDR_BASE
#define USER_DATA_AREA                 DATA_FLASH_START_POS

#define USER_DATA_CMD_MODE_CHECK_START 0x00000011
#define USER_DATA_CMD_MODE_CHECK_END   (DATA_FULL_RANK - USER_DATA_CMD_MODE_CHECK_START)
#define USER_DATA_CMD_MODE_START_ADDR  USER_DATA_AREA
#define USER_DATA_CMD_MODE             USER_DATA_CMD_MODE_START_ADDR + 4
#define USER_DATA_CMD_MODE_END_ADDR    USER_DATA_CMD_MODE + 4

typedef enum {
    SPI_FLASH_RESULT_ERR  = -1,
    SPI_FLASH_RESULT_OK   = 0,
    SPI_FLASH_RESULT_TIMEOUT = 1,
} SpiFlashOpResult;

typedef struct
{
  uint32_t cmd_mode_start;
  uint32_t cmd_mode;
  uint32_t cmd_mode_end;
}cmd_mode_data_struct_type;

extern SpiFlashOpResult set_data_flash_fix_4k(void);
extern uint8_t flash_checksum(const uint8_t *buffer, uint8_t size, uint8_t sum);
extern SpiFlashOpResult spi_flash_read(uint32_t src_addr,uint32_t *des_addr,uint16_t size);
extern SpiFlashOpResult spi_flash_write(uint32_t des_addr,uint32_t *src_addr,uint16_t size);
extern uint32_t flash_read_data_mode(void);
extern SpiFlashOpResult flash_write_data_mode(uint32_t cmd_mode);

#endif

