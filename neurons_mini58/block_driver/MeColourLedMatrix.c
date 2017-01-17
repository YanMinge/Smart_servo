#include "MeColourLedMatrix.h"
#include "systime.h"
#include "MeColourLedMatrixDef.h"

#define LED_AMOUNT  64

#define DIRECT_SHOW_MODE                    0x00
#define ERASE_SHOW_MODE                     0x01
#define LEFT_SHOW_MODE                      0x02
#define RIGHT_SHOW_MODE                     0x03

#define DINAMIC_REFRESH_INTERVAL            200  // ms

//------------SPI---------------
volatile uint8_t hwSPI_Tx_Fifo[200];
volatile uint8_t bSPI_MasEndFlag = 0;
volatile uint32_t	wSPI_NBytes = 0;
volatile uint32_t wSPI_Send_Pointer = 0;

uint8_t display_matrix[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
struct rgb_value light_panel[] = {{0x00, 0x00, 0x00},{0xff, 0x00, 0x00}, {0xff, 0xaf, 0x00}, {0xff, 0xff, 0x00},{0x00, 0xff, 0x00},  \
                                  {0x00, 0xff, 0xff},{0x00, 0x00, 0xff},{0xd4, 0x00, 0xff},{0xff, 0xff, 0xff}
	                             };

static uint32_t s_dinamic_pre_millis;
static uint32_t s_animation_pre_millis;
static uint8_t  s_colour_block_matrix[LED_AMOUNT*2]; // store the current set frame, and the previous frame.
static uint8_t  s_animation_store_array[LED_AMOUNT*4]; // store the 4 animation frames.
static uint8_t  s_show_over_flag = 0;
static uint8_t  s_dinamic_count = 0;
static uint8_t  s_animation_frame_flag_arry[4] = {0, 0, 0, 0}; // a frame set, the corresponding flag set to 1, else 0.

static void spi_driver_init(void)
{
	CLK_EnableModuleClock(SPI0_MODULE);
	CLK_SetModuleClock(SPI0_MODULE,CLK_CLKSEL1_SPISEL_HCLK,0);
	
	SYS->P0_MFP &= ~(SYS_MFP_P05_Msk | SYS_MFP_P06_Msk|SYS_MFP_P07_Msk); 	
	SYS->P0_MFP |= (SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO|SYS_MFP_P07_SPI0_CLK); 
	SPI_Open(SPI0, SPI_MASTER, SPI_MODE_1, 8, 2000000); // default MSB first. master. 
	__SPI0_SET_SEL1;
}


void SPI_W_3BYTE(uint8_t bPage, uint8_t bAddr, uint8_t bData)
{
	wSPI_NBytes = 3; 
	hwSPI_Tx_Fifo[0] = (0x20|bPage);
	hwSPI_Tx_Fifo[1] = bAddr;
	hwSPI_Tx_Fifo[2] = bData;	
	__SPI0_CLR_SEL1;
	wSPI_Send_Pointer = 0;
	while(bSPI_MasEndFlag == 0)
	{
		SPI0->TX  = hwSPI_Tx_Fifo[wSPI_Send_Pointer++];
		SPI_TRIGGER(SPI0);
		while(SPI_IS_BUSY(SPI0));
		if(wSPI_Send_Pointer == wSPI_NBytes)
		{
			wSPI_Send_Pointer = 0;
			bSPI_MasEndFlag =1 ;
		}	
	}
	__SPI0_SET_SEL1;
	bSPI_MasEndFlag = 0;
}

void SPI_W_NBYTE(uint8_t bPage, uint8_t bStart_Addr, uint8_t blength)
{
	hwSPI_Tx_Fifo[0] = (0x20|bPage);
	hwSPI_Tx_Fifo[1] = bStart_Addr;
	wSPI_NBytes =	blength+2;
	
	__SPI0_CLR_SEL1;
	wSPI_Send_Pointer = 0;
	while(bSPI_MasEndFlag == 0)
	{
		SPI0->TX  = hwSPI_Tx_Fifo[wSPI_Send_Pointer++];
		SPI_TRIGGER(SPI0);
		while(SPI_IS_BUSY(SPI0));
		if(wSPI_Send_Pointer == wSPI_NBytes)
		{
			wSPI_Send_Pointer = 0;
			bSPI_MasEndFlag =1 ;
		}	
	}
	bSPI_MasEndFlag = 0;
	__SPI0_SET_SEL1;
}

/*****************************************************************************
* Function		: LED_SPIType3ClearFrame1Page
* Description	: Clear Type3 Frame 1 Page 
* Input			: None
* Output		: None
* Return		: None
* Note			: Type3 Frame Page Address and Length different than Type1/2/4
*****************************************************************************/
void LED_SPIType3ClearFrame1Page(void)
{
	uint32_t i;
	//Setting SLED1735 Ram Page to Frame 1 Page
	//Write Data=0x00 to hwSPI_Tx_Fifo[] FIFO to CLR SLED1735 Ram	
	for( i = 0; i< TYPE3_FRAME1PAGE_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	//Clear SLED1735 Ram Page to Frame 1 Page
	//send 0xB3 bytes length Data From address 0x00 
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR&0x00), TYPE3_FRAME1PAGE_LENGTH);
}

/*****************************************************************************
* Function		: LED_SPIType3ClearFrame2Page
* Description	: Clear Type3 Frame 2 Page 
* Input			: None
* Output		: None
* Return		: None
* Note			: Type3 Frame Page Address and Length different than Type1/2/4
*****************************************************************************/
void LED_SPIType3ClearFrame2Page(void)
{
	uint32_t i;
	//Setting SLED1735 Ram Page to Frame 2 Page	

	//Write Data=0x00 to hwSPI_Tx_Fifo[] FIFO to CLR SLED1735 Ram	
		for( i = 0; i< TYPE3_FRAME2PAGE_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	//Clear SLED1735 Ram Page to Frame 1 Page
	//send 0xB3 bytes length Data From address 0x00 
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME_REG_ADDR&0x00), TYPE3_FRAME2PAGE_LENGTH);
}


/*****************************************************************************
* Function		: LED_SledType4Init
* Description	: Set Initial Value for SLED1735 (Type3 Matrix) 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3Init(void)
{
	uint32_t i;
	//Setting SLED1735 Ram Page to Function Page		
	// System must go to SW shutdowm mode when initialization
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);		
	//Setting Matrix Type = Type3	
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, PICTURE_DISPLAY_REG, mskMATRIX_TYPE_TYPE3);		
	//Setting Staggered Delay 			
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, STAGGERED_DELAY_REG, ((mskSTD4 & CONST_STD_GROUP4)|(mskSTD3 & CONST_STD_GROUP3)|(mskSTD2 & CONST_STD_GROUP2)|(mskSTD1 & CONST_STD_GROUP1)));
	//Enable Slew Rate control 
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SLEW_RATE_CTL_REG, mskSLEW_RATE_CTL_EN);
	//===============================================================
	//VAF Control settings base on the LED type.
	//================================================================
	#if ((TYPE3_VAF_OPTION == TYPE3_CATHODE_COLOR_RGB)||(TYPE3_VAF_OPTION == TYPE3_ANODE_COLOR_RGB))
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, VAF_CTL_REG, (mskVAF2|mskVAF1));
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, VAF_CTL_REG2, (mskFORCEVAFCTL_VAFTIMECTL|(mskFORCEVAFTIME_CONST & 0x01)|mskVAF3));
	#else
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, VAF_CTL_REG, (mskVAF2|mskVAF1));
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, VAF_CTL_REG2, (mskFORCEVAFCTL_ALWAYSON|mskVAF3));
	#endif
	//================================================================
	
	//Setting LED driving current = 20mA and Enable current control	
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, CURRENT_CTL_REG, (mskCURRENT_CTL_EN|0));
	
	//Init Frame1Page and Frame2Page(Clear all Ram)
	LED_SPIType3ClearFrame1Page();	
	LED_SPIType3ClearFrame2Page();		
	
	//======================================================//
	//Init Type3 FrameVAFPage : Single R/G/B ,              //
	//Anode RGB or Cathode RGB have different VAF settings  //
	//which can choice by " TYPE2_VAF_OPTION ".             //
	//======================================================//	
	for( i = 0; i< TYPE3_VAF_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3Vaf[i];
	}
	SPI_W_NBYTE(SPI_FRAME_LED_VAF_PAGE, TYPE3_VAF_FRAME_FIRST_ADDR, TYPE3_VAF_FRAME_LENGTH);	
	
	//After initialization , system back to SW Normal mode.
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_NORMAL_MODE);					
}

/*****************************************************************************
* Function		: LED_SledType3Fun1 : Column Scan
* Description	: Column by Column Light on LED Panel 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3Fun1(void)
{
		uint32_t i;
		
	// System must go to SW shutdowm mode
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);	
	
	//=====================================//
	//Clear LED CTL Registers (Frame1Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI		
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	

	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame1Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_PWM_FRAME_FIRST_ADDR, TYPE3_PWM_FRAME_LENGTH);

	
	//=====================================//
	//Clear LED CTL Registers (Frame2Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI			for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame2Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_PWM_FRAME2_FIRST_ADDR, TYPE3_PWM_FRAME2_LENGTH);

	//System back to SW Normal mode.		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_NORMAL_MODE);	
	
	//===============================//
	//Start Function1 light on Flow  //
	//===============================//
	//Write Frame1Page LED CTL Register:
	//mskLED_FRAME_REG_ADDR means Frame1Page Location address ,
	//mskLED_FRAME_REG_DATA means each bits mapping to one LED. 
	
	//Turn On column 1 of LED panel	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x00), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x01), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 1 of LED panel, and turn on current column 2 	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x00), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x01), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x02), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x03), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 2 of LED panel, and turn on current column 3 	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x02), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x03), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x04), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x05), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 3 of LED panel, and turn on current column 4 		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x04), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x05), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x06), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x07), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);
	
	//Turn Off  column 4 of LED panel, and turn on current column 5 	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x06), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x07), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x08), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x09), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);
	
	//Turn Off  column 5 of LED panel, and turn on current column 6 		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x08), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x09), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0A), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0B), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 6 of LED panel, and turn on current column 7 	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0A), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0B), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0C), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0D), (mskLED_FRAME_REG_DATA & 0xFF));
	UT_DelayNms(500);	
	
	//Turn Off  column 7 of LED panel, and turn on current column 8 	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0C), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0D), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0E), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0F), (mskLED_FRAME_REG_DATA & 0xFF));
	UT_DelayNms(500);
	
	//Turn Off  column 8 of LED panel, and turn on current column 9 	
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0E), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, (mskLED_FRAME_REG_ADDR & 0x0F), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));

	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x00), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x01), (mskLED_FRAME_REG_DATA & 0xFF));
	UT_DelayNms(500);	
	
	//Turn Off  column 9 of LED panel, and turn on current column 10 	
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x00), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x01), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x02), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x03), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 10 of LED panel, and turn on current column 11 	
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x02), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x03), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x04), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x05), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 11 of LED panel, and turn on current column 12 	
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x04), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x05), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x06), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x07), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);
	
	//Turn Off  column 12 of LED panel, and turn on current column 13	
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x06), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x07), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x08), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x09), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);
	
	//Turn Off  column 13 of LED panel, and turn on current column 14		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x08), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x09), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0A), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0B), (mskLED_FRAME_REG_DATA & 0xFF));	
	UT_DelayNms(500);	
	
	//Turn Off  column 14 of LED panel, and turn on current column 15		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0A), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0B), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0C), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0D), (mskLED_FRAME_REG_DATA & 0xFF));
	UT_DelayNms(500);

	//Turn Off  column 15 of LED panel, and turn on current column 16	
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0C), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0D), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0E), (mskLED_FRAME_REG_DATA & 0xFF));
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0F), (mskLED_FRAME_REG_DATA & 0xFF));
	UT_DelayNms(500);

	//Turn Off  column 16 of LED panel	
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0E), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));		
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, (mskLED_FRAME2_REG_ADDR & 0x0F), (mskLED_FRAME_REG_DATA & TYPE3_LED_FRAME_CLR_DATA));
	UT_DelayNms(500);	
}

/*****************************************************************************
* Function		: LED_SledType3Fun2 : Blink
* Description	: All LED of the Panel Blink 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3Fun2(void)
{
	uint32_t i;
	
	// System must go to SW shutdowm mode when Enable Blink Registers  
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);	
	
	//==============================//
	//Turn On Blink Funtion					//
	//==============================//		
	//mskBLINK_EN bit: Enable /Disable Blink Function.
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, DISPLAY_OPTION_REG, (mskBLINK_FRAME_300|mskBLINK_EN|(mskBLINK_PERIOD_TIME_CONST & 0x03)));	

	//==============================//
	//Turn Off All LED  (Frame1Page)//
	//==============================//		
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	

	//=============================================//
	//Turn On All Blink Control bits  (Frame1Page) //
	//=============================================//	
	for( i = 0; i< TYPE3_BLINK_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_BLINK_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_BLINK_FRAME_FIRST_ADDR, TYPE3_BLINK_FRAME_LENGTH);	

	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame1Page) //
	//=========================================================//	
	for( i = 0; i< TYPE3_PWM_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_PWM_FRAME_FIRST_ADDR, TYPE3_PWM_FRAME_LENGTH);
	
	
	//==============================//
	//Turn Off All LED  (Frame2Page)//
	//==============================//
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	

	//=============================================//
	//Turn On All Blink Control bits  (Frame2Page) //
	//=============================================//		
	for( i = 0; i< TYPE3_BLINK_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_BLINK_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_BLINK_FRAME2_FIRST_ADDR, TYPE3_BLINK_FRAME2_LENGTH);	

	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame2Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_PWM_FRAME2_FIRST_ADDR, TYPE3_PWM_FRAME2_LENGTH);


	//After Setting Blink Registers,system back to SW Normal mode.		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_NORMAL_MODE);	
	
	//=================================//
	//Delay for Blink Frame Data ready //
	//=================================//
	UT_DelayNms(5);
	
	//==============================//
	//Turn On All LED  (Frame1Page) //
	//==============================//		
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	

	
	//==============================//
	//Turn On All LED  (Frame2Page) //
	//==============================//		
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	

	//===================================//	
	//Delay for observing Blink function //
	//===================================//	
	UT_DelayNms(5800);	
	
	//==============================//
	//Turn Off Blink Funtion				//
	//==============================//	
	//Disable Blink	
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, DISPLAY_OPTION_REG, (mskBLINK_FRAME_300|mskBLINK_DIS|(mskBLINK_PERIOD_TIME_CONST & 0x03)));
}	

/*****************************************************************************
* Function		: LED_SledType3Fun3 : Single Scan
* Description	: Single LED move on the Panel  
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3Fun3(void)
{
	uint32_t i, j , x;

	// System must go to SW shutdowm mode
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);	
	
	//==============================================//
	//Clear Fram1 and Frame2 Registers (Frame1Page) //
	//==============================================//	
	LED_SPIType3ClearFrame1Page();	
	LED_SPIType3ClearFrame2Page();			
	
	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame1Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_PWM_FRAME_FIRST_ADDR, TYPE3_PWM_FRAME_LENGTH);

	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame2Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_PWM_FRAME2_FIRST_ADDR, TYPE3_PWM_FRAME2_LENGTH);

	//System back to SW Normal mode.		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_NORMAL_MODE);	
	
	//===============================//
	//		Start Function3  Flow 		 //
	//===============================//	
	//i loop for the Type3 LED control Frame1 Registers' address		
	for(i=0; i<TYPE3_LED_FRAME_LENGTH ; i++)
	{
		x = 0x01;	
		//j loop for 8 LEDs 		
		for(j=0; j<8 ; j++)
		{
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, i, x);			//turn on one LED
			x <<= 1;
			UT_DelayNms(100);				//delay 100ms		
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, i, 0);			//turn off current LED
		}
	}	
	
	//i loop for the Type3 LED control Frame2 Registers' address		
	for(i=0; i<TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		x = 0x01;	
		//j loop for 8 LEDs 		
		for(j=0; j<8 ; j++)
		{
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, i, x);			//turn on one LED
			x <<= 1;
			UT_DelayNms(100);				//delay 100ms	
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, i, 0);			//turn off current LED			
		}
	}	
}

/*****************************************************************************
* Function		: LED_SledType3Fun4 : Breath
* Description	: All LEDs of the Panel breathe
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3Fun4(void)
{
	uint32_t i;
	
	// System must go to SW shutdowm mode when Enable Breath Registers  		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);	
	
	//==============================//
	//Turn On Breath Funtion				//
	//==============================//	
	//Breath Control 2, Extinguish Time is 7,	Continuous  Enable,	Breath Enable	
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG2, (mskBC_CONTINUOUS_BREATH_EN|mskBC_BREATH_EN|(mskBC_EXTINGUISH_TIME_CONST&0x07)));		
	//Breath Control,Fade Out/In Time
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG, ((mskBC_FADE_IN_TIME_CONST&0x07)|(mskBC_FADE_OUT_TIME_CONST&0x70)));		
			
	//==============================//
	//Turn On All LED  (Frame1Page) //
	//==============================//			
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	

	//=============================================//
	//Turn Off All Blink Control bits  (Frame1Page)//
	//=============================================//			
	for( i = 0; i< TYPE3_BLINK_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_BLINK_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_BLINK_FRAME_FIRST_ADDR, TYPE3_BLINK_FRAME_LENGTH);	

	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame1Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_PWM_FRAME_FIRST_ADDR, TYPE3_PWM_FRAME_LENGTH);

	//==============================//
	//Turn On All LED  (Frame2Page) //
	//==============================//				
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	

	
	//=============================================//
	//Turn Off All Blink Control bits  (Frame2Page)//
	//=============================================//	
	for( i = 0; i< TYPE3_BLINK_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_BLINK_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_BLINK_FRAME2_FIRST_ADDR, TYPE3_BLINK_FRAME2_LENGTH);	

	
	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame2Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_PWM_FRAME2_FIRST_ADDR, TYPE3_PWM_FRAME2_LENGTH);

	
	//After Setting Breath Registers,system back to SW Normal mode.		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_NORMAL_MODE);		
	
	//====================================//	
	//Delay for observing Breath function //
	//====================================//		
	UT_DelayNms(23000);	

	//System back to SW Normal mode.		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);	
	
	//==============================//
	//Turn Off Breath Funtion				//
	//==============================//		
	//Breath Control 2, Extinguish Time is 0,	Continuous Disable, Breath Disable
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG2, (mskBC_CONTINUOUS_BREATH_DIS|mskBC_BREATH_DIS));
	//Contorl Fade Out/In Time=0
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG, ((mskBC_FADE_IN_TIME_CONST&0x00)|(mskBC_FADE_OUT_TIME_CONST&0x00)));	
}				

/*****************************************************************************
* Function		: LED_SledType3RGBAnodeFun1
* Description	: RGB LED Turn On
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3RGBAnodeFun1(void)
{
	
	uint32_t i;
		
	//=====================================//
	//Clear LED CTL Registers (Frame1Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI		
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	
	//=====================================//
	//Clear LED CTL Registers (Frame2Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI			for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	

	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame1Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_PWM_FRAME_FIRST_ADDR, TYPE3_PWM_FRAME_LENGTH);
	
	//=========================================================//
	//SET PWM CTL Registers Value =0xFF(Max Value, Frame2Page) //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_PWM_FRAME2_FIRST_ADDR, TYPE3_PWM_FRAME2_LENGTH);
	
	//=======================================//
	//Start Function1 light on RED LED Flow  //
	//=======================================//
	//Write Frame1Page LED CTL Register:
	//mskLED_FRAME_REG_ADDR means Frame1Page Location address ,
	//mskLED_FRAME_REG_DATA means each bits mapping to one LED. 	

	//=======================================//
	//			Turn On Red LED Panel					   //
	//=======================================//	
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLAnodeRed[i];
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLAnodeRed[i+16];
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//=========================//	
	//			Delay for 3s		   //
	//=========================//		
	UT_DelayNms(3000);	

	//=======================================//
	//			Turn On Green LED Panel				   //
	//=======================================//	
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLAnodeGreen[i];
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLAnodeGreen[i+16];
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//=========================//	
	//			Delay for 3s		   //
	//=========================//		
	UT_DelayNms(3000);		

	//=======================================//
	//			Turn On Blue LED Panel				   //
	//=======================================//	
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLAnodeBlue[i];
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLAnodeBlue[i+16];
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//=========================//	
	//			Delay for 3s		   //
	//=========================//	
	UT_DelayNms(3000);	

	//=======================================//
	//			Turn On RGB LED Panel				   //
	//=======================================//	
		for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = (tabLED_Type3CTLAnodeRed[i]|tabLED_Type3CTLAnodeGreen[i]|tabLED_Type3CTLAnodeBlue[i]);
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	
		for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = (tabLED_Type3CTLAnodeRed[i+16]|tabLED_Type3CTLAnodeGreen[i+16]|tabLED_Type3CTLAnodeBlue[i+16]);
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//=========================//	
	//			Delay for 3s		   //
	//=========================//	
	UT_DelayNms(3000);		
}

/*****************************************************************************
* Function		: LED_SledType3RGBAnodeFun2 : Single Scan
* Description	: RGB LED move on the Panel  
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3RGBAnodeFun2(void)
{

	uint32_t i;
		
	//=====================================//
	//Clear LED CTL Registers (Frame1Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI		
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	

	//=====================================//
	//Clear LED CTL Registers (Frame2Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI			for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//=========================================================//
	//SET PWM CTL Registers Value =0x00(Frame1Page) 		   //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_PWM_FRAME_FIRST_ADDR, TYPE3_PWM_FRAME_LENGTH);
	
	//=========================================================//
	//SET PWM CTL Registers Value =0x00(Frame2Page) 		   //
	//=========================================================//		
	for( i = 0; i< TYPE3_PWM_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_PWM_FRAME2_FIRST_ADDR, TYPE3_PWM_FRAME2_LENGTH);
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[52] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[52] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
	SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[52] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
	//===============================//
	//		Start Function3  Flow 		 //
	//===============================//	
	//i loop for the Type1 RED LED control Registers' address		
//	for(i=0; i<TYPE3_RGB_ANODE_PWM_LENGTH ; i++)
//	{
//		if(i < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1)
//		{
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			UT_DelayNms(1000);				//delay 100ms	
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] ,	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED	
//		}
//		else if(i < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1_2)
//		{
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			UT_DelayNms(1000);				//delay 100ms	
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
//			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] ,	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED		
//		}	
//		else//TYPE3_RGB_ANODE_PWM_LENGTH > TYPE3_RGB_ANODE_PWM_LENGTH_FRAME2
//		{	
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
//			UT_DelayNms(1000);				//delay 100ms	
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] ,	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
//			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED			
//		}	
//	}
}


/*****************************************************************************
* Function		: LED_SledType3ShutDownFun
* Description	: LED Shutdown
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3ShutDownFun(void)
{
	
	uint32_t i;
	
	//System into S/W Shutdown	mode		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_SHUT_DOWN_MODE);		
	
	//===============================//
	//Turn Off Frame1 LED(Frame1Page) //
	//===============================//	
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	
	//===============================//
	//Turn Off Frame2 LED(Frame2Page) //
	//===============================//	
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_CLR_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	
	//into H/W Shutdown			
	__LED_SetShutDownPinLow	
	
	//==============================//
	//H/W shutdown mode							//
	//==============================//
	UT_DelayNms(1000);				//delay 1000ms			
	//leave H/W Shutdown
	__LED_SetShutDownPinHigh	
	
	//===============================//
	//Turn On Frame1 LED(Frame1Page) //
	//===============================//	
	/*
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	*/
	
	//===============================//
	//Turn On Frame2 LED(Frame2Page) //
	//===============================//	
	/*
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_LED_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
	*/
	
	//System leave S/W Shutdown	mode, return Normal mode		
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, SW_SHUT_DOWN_REG, mskSW_NORMAL_MODE);		
}

void LED_SledType3ControlOn(void)
{
	int i = 0;
	//=====================================//
	//Clear LED CTL Registers (Frame1Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI		
	for( i = 0; i< TYPE3_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);	

	//=====================================//
	//Clear LED CTL Registers (Frame2Page) //
	//=====================================//		
	//Write LED Control Data=0x00 to hwSPI_Tx_Fifo[] , then send N bytes length Data by SPI			for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = TYPE3_PWM_FRAME_MAX_DATA;
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	
}

/*****************************************************************************
* Function		: single_led_set
* Description	: set single led or all led with the same kind of colour.
* Input			: uint8_t index: 0-64; 0: light all leds with the set colour. 1-64, light the index led with the set colour.
				  uint8_t r_value£ºset red value.
				  uint8_t g_value: set green value.
				  uint8_t b_value: set blue value. 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void single_led_set(uint8_t index, uint8_t r_value, uint8_t g_value, uint8_t b_value)
{	
	
	if(index > COLOUR_LED_QUANTITY)
	{
		return;
	}
	else if(index == 0) // light all led with set colour
	{
		int i = 0;
		for( ; i <  COLOUR_LED_QUANTITY; i++)
		{
			if(i < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1)
			{
				SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , r_value);
				SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , g_value);
				SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , b_value);
			}
			else if(i < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1_2)
			{
				SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , r_value);
				SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , g_value);
				SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , b_value);
			}	
			else
			{	
				SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , r_value);
				SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , g_value);
				SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , b_value);	
			}	
		}
	}
	else
	{
		index--;
		if(index < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1)
		{
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[index] , 	r_value);
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[index] , g_value);
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeBlue[index] , 	b_value);
		}
		else if(index < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1_2) //0~63
		{
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[index] , 	r_value);
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[index] , g_value);
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[index] ,  b_value);
		}
		else
		{
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[index] , r_value);
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[index] , g_value);
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[index] , b_value);	
		}
	}
}

void MeColourLEDMatrix_DrawBitMap(uint8_t led_matrix[], uint8_t r_value, uint8_t g_value, uint8_t b_value)
{
	uint32_t  index = 1;
	uint32_t row = 0;
	uint32_t column = 0;
	for( ; index <= COLOUR_LED_QUANTITY; index++)
	{
		row = (index-1)/8;
		column = (index -1)%8;
		if((led_matrix[row]>>column)&0x01) 
		{
			single_led_set(index, r_value, g_value, b_value);
		}
		else
		{
			single_led_set(index, 0, 0, 0);
		}
	}
	
}

void led_matrix_clear(void)
{
    single_led_set(0, 0, 0, 0);
}

void MeColourLEDMatrix_setBreath(uint8_t breath_set)
{
	if(BREATH_ON == breath_set)
	{
		//Breath Control 2, Extinguish Time is 7,	Continuous  Enable,	Breath Enable	
		SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG2, (mskBC_CONTINUOUS_BREATH_DIS|mskBC_BREATH_EN|(mskBC_EXTINGUISH_TIME_CONST&0x07)));		
		//Breath Control,Fade Out/In Time
		SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG, ((mskBC_FADE_IN_TIME_CONST&0x07)|(mskBC_FADE_OUT_TIME_CONST&0x70)));	
	}
	else
	{
		SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, BREATH_CTL_REG2, (mskBC_CONTINUOUS_BREATH_DIS|mskBC_BREATH_DIS|(mskBC_EXTINGUISH_TIME_CONST&0x07)));	
	}
}

static void colour_block_show_direct(void)
{
    struct rgb_value colour_value;
    for(int i = 0; i < LED_AMOUNT; i++)
    {
       colour_value =  light_panel[s_colour_block_matrix[i]];
       single_led_set(i+ 1, colour_value.r, colour_value.g, colour_value.b);
    }
    s_show_over_flag = 0;
    s_animation_pre_millis = millis();
}

static void colour_block_show_erase(void)
{
    uint8_t led_index;
    struct rgb_value colour_value;
    uint32_t current_time = millis();
    if((current_time - s_dinamic_pre_millis) > DINAMIC_REFRESH_INTERVAL)
    {
        s_dinamic_pre_millis = current_time;
        
        for(int j = 0; j < 8; j++)
        {
            led_index = (j<<3) + s_dinamic_count;
            colour_value = light_panel[s_colour_block_matrix[led_index]];    
            single_led_set(led_index + 1, colour_value.r, colour_value.g, colour_value.b);
        }
        s_dinamic_count++;
        if(s_dinamic_count >= 8)
        {
            s_show_over_flag = 0;
            s_dinamic_count = 0;
            s_animation_pre_millis = millis();
        }
    }
}


static void colour_block_show_left(void)
{
    uint8_t led_index, i , j;
    struct rgb_value colour_value;
    uint32_t current_time = millis();
    if((current_time - s_dinamic_pre_millis) > DINAMIC_REFRESH_INTERVAL)
    {
        s_dinamic_pre_millis = current_time;
        
        for(j = 0; j < (8 - s_dinamic_count-1); j++) // column.
        {
            for(i = 0; i < 8; i++) // row
            {
                led_index = (i<<3) + j;
                colour_value = light_panel[s_colour_block_matrix[(led_index + LED_AMOUNT + 1 + s_dinamic_count)]];    
                single_led_set(led_index + 1 , colour_value.r, colour_value.g, colour_value.b);
            }
        }
        for(j = (8 - s_dinamic_count -1); j < 8; j++) //column
        {
            for(i = 0; i < 8; i++) // row
            {
                led_index = (i<<3) + j; 
                colour_value = light_panel[s_colour_block_matrix[(led_index  - (8 - s_dinamic_count -1))]];    
                single_led_set(led_index + 1, colour_value.r, colour_value.g, colour_value.b);
            }
        }
        s_dinamic_count++;
        if(s_dinamic_count >= 8)
        {
            s_show_over_flag = 0;
            s_dinamic_count = 0;
            s_animation_pre_millis = millis();
        }
    }
}

static void colour_block_show_right(void)
{
    uint8_t led_index, i , j;
    struct rgb_value colour_value;   
    
    uint32_t current_time = millis();
    if((current_time - s_dinamic_pre_millis) > DINAMIC_REFRESH_INTERVAL)
    {
        s_dinamic_pre_millis = current_time;
        
        for(j = 0; j < s_dinamic_count + 1; j++) //column
        {
            for(i = 0; i < 8; i++) // row
            {
                led_index = (i<<3) + j; 
                colour_value = light_panel[s_colour_block_matrix[led_index+7-s_dinamic_count]];                
                single_led_set(led_index + 1, colour_value.r, colour_value.g, colour_value.b);
            }
        }              
        for(j = s_dinamic_count + 1; j < 8; j++) // column.
        {
            for(i = 0; i < 8; i++) // row
            {
                led_index = (i<<3) + j;
                colour_value = light_panel[s_colour_block_matrix[(led_index - (s_dinamic_count + 1)+ LED_AMOUNT)]];              
                single_led_set(led_index + 1 , colour_value.r, colour_value.g, colour_value.b);
            }
        }                       
        s_dinamic_count++;
        if(s_dinamic_count >= 8)
        {
            s_show_over_flag = 0;
            s_dinamic_count = 0;
            s_animation_pre_millis = millis();
        }
    }
}

void set_leds_by_colour_block(uint8_t led_quantity, uint8_t* led_array)
{ 
    s_show_over_flag = 1;
    s_dinamic_count = 0;
    
    int i = 0;
    // copy the old map to store area 2.
    for(i = 0; i < LED_AMOUNT; i++) 
    {
        s_colour_block_matrix[i + 64] = s_colour_block_matrix[i];
    }
    // copy the new map to store area 1.
    for(i = 0; i < led_quantity; i++)
    {
        s_colour_block_matrix[i] = led_array[i];
    }
    for(i = led_quantity; i < LED_AMOUNT; i++)
    {
        s_colour_block_matrix[i] = 0;
    }   
}

void set_led_by_colour_block_clear(void)
{
    for(int i=0;i<2*LED_AMOUNT;i++)
    {
        s_colour_block_matrix[i] = 0;
    }
}

void colour_block_show(uint8_t mode)
{
    if(s_show_over_flag == 0)
    {
        return;
    }    
    switch(mode)
    {
        case DIRECT_SHOW_MODE:
            colour_block_show_direct();
        break;
        case ERASE_SHOW_MODE:
            colour_block_show_erase();
        break;
        case LEFT_SHOW_MODE:
            colour_block_show_left();
        break;
        case RIGHT_SHOW_MODE:
            colour_block_show_right();
        break;
        default:
        break;
    }
}

void set_animation_frame(uint8_t sequence, uint8_t led_quantity, uint8_t* led_array)
{
    if(sequence > 3) // only store 4 frames.
    {
        return;
    }
    
    int i;
    for(i = 0; i < led_quantity; i++)
    {
        s_animation_store_array[i + sequence*64] = led_array[i];
    }
    for(i = led_quantity; i< LED_AMOUNT; i++)
    {
        s_animation_store_array[i + sequence*64] = 0;
    }
    
    s_animation_frame_flag_arry[sequence] = 1;
}

void animation_show(uint8_t mode, uint32_t interval)
{
    static uint8_t s_frame_index;
    if(s_show_over_flag == 0)
    {
        uint32_t current_time = millis();
        if((current_time - s_animation_pre_millis) > interval)
        {   
            for( ; s_frame_index < 4; s_frame_index++)
            {
                if(s_animation_frame_flag_arry[s_frame_index] == 1)
                {
                    set_leds_by_colour_block(LED_AMOUNT, (s_animation_store_array + (64*s_frame_index)));
                    s_frame_index++;
                    break;
                } 
            }
            if(s_frame_index >= 4)
            {
                s_frame_index = 0;
            }
        }
    }
    colour_block_show(mode);
}

void animation_clear(void)
{
    int i;
    // clear animation store array.
    for(i = 0; i < (LED_AMOUNT*4); i++)
    {
        s_animation_store_array[i] = 0;
    }
    // clear animation set flag.
    for(i = 0; i < 4; i++)
    {
        s_animation_frame_flag_arry[i] = 0;
    }
}

void led_colour_matrix_init(void)
{
	pinMode(SDB_PIN, GPIO_MODE_OUTPUT);
	pinMode(COLOUR_MATRIX_CS_PIN, GPIO_MODE_OUTPUT);
	
	__LED_SetShutDownPinHigh;
	spi_driver_init();
	
	UT_DelayNms(10);
	LED_SledType3Init();
	LED_SledType3ControlOn();
}
