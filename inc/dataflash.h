#ifndef __DATAFLASH_H__
#define __DATAFLASH_H__

#include "M051Series.h"
#include "mygpio.h"
#include "fmc.h"

#define FLASH_MAGIC_WORD            0x5A5A5A5A
#define DATA_FLASH_ADDR_BASE        (FMC->DFBADR)
#define DATA_FLASH_ADDR_END         (FMC->DFBADR + 0x1000)
#define DATA_FLASH_PAGE_SIZE        512

extern uint8_t flash_checksum(const uint8_t *buffer, uint8_t size, uint8_t sum);
extern int32_t spi_flash_read(uint32_t src_addr,uint32_t *des_addr,uint16_t size);
extern int32_t spi_flash_write(uint32_t des_addr,uint32_t *src_addr,uint16_t size);
#endif

