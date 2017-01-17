
#ifndef  MINI58_FLASH_H_

#define  MINI58_FLASH_H_

#include "Mini58Series.h"

static      uint8_t     Device_Info_FlashArea_Set(void);
uint8_t     Device_Info_Write(uint8_t* src,uint16_t len);
void        Device_Info_Read (uint8_t* dest,uint16_t len);
void        Device_Info_Flash_Operation_Test(void);
#endif
