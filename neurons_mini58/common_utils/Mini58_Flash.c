
#include  "Mini58_Flash.h"
#include  "uart_printf.h"
#include  "string.h"


#define   DEVICE_INFO_FLASH_ADDR   (FMC_APROM_END-(FMC_FLASH_PAGE_SIZE*1))  // the device info is  saved  to last page of APROM

static uint8_t  Device_Info_FlashArea_Set(void)
{
     uint32_t   au32Config[2];

    if (FMC_ReadConfig(au32Config, 2) < 0) {
        DEBUG_MSG(UART0,"\nRead User Config failed!\n");
        return 1;
    }

    if ((!(au32Config[0] & 0x1)) && (au32Config[1] == DEVICE_INFO_FLASH_ADDR))
    {
      DEBUG_MSG(UART0,"data flash addr config successfully!\n");
      return 0;
    }
      

    FMC_ENABLE_CFG_UPDATE();

    au32Config[0] &= ~0x1;
    au32Config[1] = DEVICE_INFO_FLASH_ADDR;

    if (FMC_WriteConfig(au32Config, 2) < 0)
        return 1;
    // Perform chip reset to make new User Config take effect
    SYS->IPRST0 = SYS_IPRST0_CHIPRST_Msk;     
}


uint8_t    Device_Info_Write(uint8_t* src,uint16_t len)
{
  int result=1;
  uint32_t temp_buf[128];
  uint16_t write_len=0;
  memset((uint8_t*)temp_buf,0x00,512);   
  if(len>512)
    write_len=128;
  else
    write_len=(len/4)+1;  
  
  memcpy(temp_buf,src,len); 
  __disable_irq();
  SYS_UnlockReg();
  FMC_Open();
  Device_Info_FlashArea_Set();
  result = FMC_Erase(DEVICE_INFO_FLASH_ADDR);
  if(result==(1))
  {
   DEBUG_MSG(UART0,"device info erase flash failed %r%n");
   return 1;
  }
  for(int i=0;i<write_len;i++)
  {
    FMC_Write(DEVICE_INFO_FLASH_ADDR+i*4,temp_buf[i]);
  }
  
  for(int i=0;i<write_len;i++)
  {
   temp_buf[i]=FMC_Read(DEVICE_INFO_FLASH_ADDR+i*4);     
  }
  
  if(memcmp((uint8_t*)temp_buf,src,len)!=0)
  {
    DEBUG_MSG(UART0,"device info wirte flash failed %r%n");  
    return 1; 
  }

  FMC_Close();
  SYS_LockReg();
  __enable_irq();
 return 0;
}

void   Device_Info_Read (uint8_t* dest,uint16_t len)
{
  uint32_t temp_buf[128]; 
  memset(temp_buf,0x00,512);
  SYS_UnlockReg();
  FMC_Open();
    
  for(int i=0;i<(len/4)+1;i++)
  {
   temp_buf[i]=FMC_Read(DEVICE_INFO_FLASH_ADDR+i*4);     
  }
  memcpy(dest,(uint8_t*)temp_buf,len);
  FMC_Close();
  SYS_LockReg(); 
}
void  Device_Info_Flash_Operation_Test(void)
{
 uint8_t test[128];
 memset(test,0x00,128);
 for(int i=0;i<83;i++)   
  {
    test[i]=i;
  }
 Device_Info_Write(test,83);
 memset((uint8_t*)test,0x00,128);
   DEBUG_MSG(UART0,"read flash start\r\n");
 Device_Info_Read((uint8_t*)&test[1],83);
  DEBUG_MSG(UART0,"read flash finish\r\n");
}
