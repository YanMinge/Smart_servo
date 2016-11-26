/******************** (C) COPYRIGHT 2016 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2016/03
* AUTHOR:				SA1
* IC:				SN32F240/230/220 
* DESCRIPTION:	SLED1735 Demo Code
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.0				2016/03/17	SA1			1. First release
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/
/*_____ I N C L U D E S ____________________________________________________*/
#include <SN32F240.h>
#include "..\Interface\SPI\SPI.h"
#include "LED_Function.h"
#include "LED_RamSetting.h"
#include "LED_Type3.h"
#include "LED_Type3RGBTable.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
#define TYPE3_VAF_OPTION	TYPE3_SINGLE_COLOR_BLUE
		#define TYPE3_SINGLE_COLOR_RED	1 
		#define TYPE3_SINGLE_COLOR_GREEN	2 
		#define TYPE3_SINGLE_COLOR_BLUE	3 
		#define TYPE3_CATHODE_COLOR_RGB	4 
		#define TYPE3_ANODE_COLOR_RGB	5 		
/*_____ D E F I N I T I O N S ______________________________________________*/
//VAF Setting
//Each LED color type has an option of following to avoid LED blur.  
#if (TYPE3_VAF_OPTION == TYPE3_SINGLE_COLOR_RED)
const uint8_t tabLED_Type3Vaf[64] = { //Reference SLED1735 Datasheet Type3 Circuit Map
																		//Frame 1
																		0x55, 0x55, 0x55, 0x55, //C1-A ~ C1-P
																		0x55, 0x55, 0x55, 0x55, //C2-A ~ C2-P 
																		0x55, 0x55, 0x55, 0x55, //C3-A ~ C3-P  
																		0x55, 0x55, 0x55, 0x55, //C4-A ~ C4-P 
																		0x55, 0x55, 0x55, 0x55, //C5-A ~ C5-P  
																		0x55, 0x55, 0x55, 0x55, //C6-A ~ C6-P 
																		0x55, 0x55, 0x55, 0x55, //C7-A ~ C7-P  
																		0x55, 0x55, 0x55, 0x55, //C8-A ~ C8-P
																		//Frame 2
																		0x55, 0x55, 0x55, 0x55, //C9-A ~ C9-P 
																		0x55, 0x55, 0x55, 0x55, //C10-A ~ C10-P 
																		0x55, 0x55, 0x55, 0x55, //C11-A ~ C11-P  
																		0x55, 0x55, 0x55, 0x55, //C12-A ~ C12-P 
																		0x55, 0x55, 0x55, 0x55, //C13-A ~ C13-P  
																		0x55, 0x55, 0x55, 0x55, //C14-A ~ C14-P 
																		0x55, 0x55, 0x55, 0x55, //C15-A ~ C15-P 
																		0x55, 0x55, 0x55, 0x55, //C16-A ~ C16-P 
																		};

#elif (TYPE3_VAF_OPTION == TYPE3_SINGLE_COLOR_GREEN)
const uint8_t tabLED_Type3Vaf[64] = { //Reference SLED1735 Datasheet Type3 Circuit Map
																		//Frame 1
																		0x00, 0x00, 0x00, 0x00, //C1-A ~ C1-P
																		0x00, 0x00, 0x00, 0x00, //C2-A ~ C2-P 
																		0x00, 0x00, 0x00, 0x00, //C3-A ~ C3-P  
																		0x00, 0x00, 0x00, 0x00, //C4-A ~ C4-P 
																		0x00, 0x00, 0x00, 0x00, //C5-A ~ C5-P  
																		0x00, 0x00, 0x00, 0x00, //C6-A ~ C6-P 
																		0x00, 0x00, 0x00, 0x00, //C7-A ~ C7-P  
																		0x00, 0x00, 0x00, 0x00, //C8-A ~ C8-P
																		//Frame 2
																		0x00, 0x00, 0x00, 0x00, //C9-A ~ C9-P 
																		0x00, 0x00, 0x00, 0x00, //C10-A ~ C10-P 
																		0x00, 0x00, 0x00, 0x00, //C11-A ~ C11-P  
																		0x00, 0x00, 0x00, 0x00, //C12-A ~ C12-P 
																		0x00, 0x00, 0x00, 0x00, //C13-A ~ C13-P  
																		0x00, 0x00, 0x00, 0x00, //C14-A ~ C14-P 
																		0x00, 0x00, 0x00, 0x00, //C15-A ~ C15-P 
																		0x00, 0x00, 0x00, 0x00, //C16-A ~ C16-P 
																		};

#elif (TYPE3_VAF_OPTION == TYPE3_SINGLE_COLOR_BLUE)
const uint8_t tabLED_Type3Vaf[64] = { //Reference SLED1735 Datasheet Type3 Circuit Map
																		//Frame 1
																		0xAA, 0xAA, 0xAA, 0xAA, //C1-A ~ C1-P
																		0xAA, 0xAA, 0xAA, 0xAA, //C2-A ~ C2-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C3-A ~ C3-P  
																		0xAA, 0xAA, 0xAA, 0xAA, //C4-A ~ C4-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C5-A ~ C5-P  
																		0xAA, 0xAA, 0xAA, 0xAA, //C6-A ~ C6-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C7-A ~ C7-P  
																		0xAA, 0xAA, 0xAA, 0xAA, //C8-A ~ C8-P
																		//Frame 2
																		0xAA, 0xAA, 0xAA, 0xAA, //C9-A ~ C9-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C10-A ~ C10-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C11-A ~ C11-P  
																		0xAA, 0xAA, 0xAA, 0xAA, //C12-A ~ C12-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C13-A ~ C13-P  
																		0xAA, 0xAA, 0xAA, 0xAA, //C14-A ~ C14-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C15-A ~ C15-P 
																		0xAA, 0xAA, 0xAA, 0xAA, //C16-A ~ C16-P 
																		};

#elif (TYPE3_VAF_OPTION == TYPE3_CATHODE_COLOR_RGB)
const uint8_t tabLED_Type3Vaf[64] = { //Reference SLED1735 Datasheet Type3 Circuit Map
																		//Frame 1
																		0x41, 0x10, 0x04, 0x01, //C1-A ~ C1-P
																		0x40, 0x10, 0x04, 0x01, //C2-A ~ C2-P 
																		0x44, 0x10, 0x04, 0x01, //C3-A ~ C3-P  
																		0x44, 0x10, 0x04, 0x01, //C4-A ~ C4-P 
																		0x04, 0x10, 0x04, 0x01, //C5-A ~ C5-P  
																		0x04, 0x11, 0x04, 0x01, //C6-A ~ C6-P 
																		0x04, 0x11, 0x04, 0x01, //C7-A ~ C7-P  
																		0x04, 0x01, 0x04, 0x01, //C8-A ~ C8-P
																		//Frame 2
																		0x04, 0x41, 0x04, 0x01, //C9-A ~ C9-P 
																		0x04, 0x41, 0x04, 0x01, //C10-A ~ C10-P 
																		0x04, 0x41, 0x00, 0x01, //C11-A ~ C11-P  
																		0x04, 0x41, 0x10, 0x01, //C12-A ~ C12-P 
																		0x04, 0x41, 0x10, 0x01, //C13-A ~ C13-P  
																		0x04, 0x41, 0x10, 0x00, //C14-A ~ C14-P 
																		0x04, 0x41, 0x10, 0x04, //C15-A ~ C15-P 
																		0x04, 0x41, 0x10, 0x04, //C16-A ~ C16-P 
																		};

#elif (TYPE3_VAF_OPTION == TYPE3_ANODE_COLOR_RGB)
const uint8_t tabLED_Type3Vaf[64] = { //Reference SLED1735 Datasheet Type3 Circuit Map
																		//Frame 1
																		0x50, 0x55, 0x55, 0x55, //C1-A ~ C1-P
																		0x00, 0x00, 0x00, 0x00, //C2-A ~ C2-P ,
																		0x00, 0x00, 0x00, 0x00, //C3-A ~ C3-P  
																		0x15, 0x54, 0x55, 0x55, //C4-A ~ C4-P 
																		0x00, 0x00, 0x00, 0x00, //C5-A ~ C5-P  
																		0x00, 0x00, 0x00, 0x00, //C6-A ~ C6-P 
																		0x55, 0x05, 0x55, 0x55, //C7-A ~ C7-P  
																		0x00, 0x00, 0x00, 0x00, //C8-A ~ C8-P
																		//Frame 2
																		0x00, 0x00, 0x00, 0x00, //C9-A ~ C9-P 
																		0x55, 0x55, 0x41, 0x55, //C10-A ~ C10-P 
																		0x00, 0x00, 0x00, 0x00, //C11-A ~ C11-P  
																		0x00, 0x00, 0x00, 0x00, //C12-A ~ C12-P 
																		0x55, 0x55, 0x55, 0x50, //C13-A ~ C13-P  
																		0x00, 0x00, 0x00, 0x00, //C14-A ~ C14-P 
																		0x00, 0x00, 0x00, 0x00, //C15-A ~ C15-P 
																		0x00, 0x00, 0x00, 0x00, //C16-A ~ C16-P 
																		};
#endif

/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/

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
	SPI_W_3BYTE(SPI_FRAME_FUNCTION_PAGE, CURRENT_CTL_REG, (mskCURRENT_CTL_EN|CONST_CURRENT_STEP_20mA));
	
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
	
	//===============================//
	//		Start Function3  Flow 		 //
	//===============================//	
	//i loop for the Type1 RED LED control Registers' address		
	for(i=0; i<TYPE3_RGB_ANODE_PWM_LENGTH ; i++)
	{
		if(i < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1)
		{
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			UT_DelayNms(100);				//delay 100ms	
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] ,	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED	
		}
		else if(i < TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1_2)
		{
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			UT_DelayNms(100);				//delay 100ms	
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] ,	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED		
		}	
		else//TYPE3_RGB_ANODE_PWM_LENGTH > TYPE3_RGB_ANODE_PWM_LENGTH_FRAME2
		{	
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			UT_DelayNms(100);				//delay 100ms	
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeGreen[i] ,	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLAnodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED			
		}	
	}
}

/*****************************************************************************
* Function		: LED_SledType3RGBCathodeFun1
* Description	: RGB LED Turn On 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3RGBCathodeFun1(void)
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
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLCathodeRed[i];
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLCathodeRed[i+16];
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
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLCathodeGreen[i];
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLCathodeGreen[i+16];
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
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLCathodeBlue[i];
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = tabLED_Type3CTLCathodeBlue[i+16];
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);
	
	//=========================//	
	//			Delay for 3s		   //
	//=========================//	
	UT_DelayNms(3000);	

	//=======================================//
	//			Turn On RGB LED Panel				   //
	//=======================================//	
		for( i = 0; i< TYPE1_LED_FRAME_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = (tabLED_Type3CTLCathodeRed[i]|tabLED_Type3CTLCathodeGreen[i]|tabLED_Type3CTLCathodeBlue[i]);
	}
	SPI_W_NBYTE(SPI_FRAME_ONE_PAGE, TYPE3_LED_FRAME_FIRST_ADDR, TYPE3_LED_FRAME_LENGTH);
	
		for( i = 0; i< TYPE3_LED_FRAME2_LENGTH ; i++)
	{
		hwSPI_Tx_Fifo[i+2] = (tabLED_Type3CTLCathodeRed[i+16]|tabLED_Type3CTLCathodeGreen[i+16]|tabLED_Type3CTLCathodeBlue[i+16]);
	}
	SPI_W_NBYTE(SPI_FRAME_TWO_PAGE, TYPE3_LED_FRAME2_FIRST_ADDR, TYPE3_LED_FRAME2_LENGTH);	

	//=========================//	
	//			Delay for 3s		   //
	//=========================//	
	UT_DelayNms(3000);	
}

/*****************************************************************************
* Function		: LED_SledType3RGBCathodeFun2 : Single Scan
* Description	: RGB LED move on the Panel  
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void LED_SledType3RGBCathodeFun2(void)
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
	
	//===============================//
	//		Start Function3  Flow 		 //
	//===============================//	
	//i loop for the Type3 RED LED control Registers' address	
	for(i=0; i<TYPE3_RGB_CATHODE_PWM_FRAME1_LENGTH ; i++)
	{
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLCathodeRed[i] ,	 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLCathodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLCathodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			UT_DelayNms(100);				//delay 100ms	
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLCathodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLCathodeGreen[i] , TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_ONE_PAGE, tabLED_Type3PWMCTLCathodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED	
	}	
	//i loop for the Type3 RED LED control Registers' address		
	for(i=TYPE3_RGB_CATHODE_PWM_FRAME1_LENGTH; i<TYPE3_RGB_CATHODE_PWM_FRAME2_LENGTH ; i++)
	{
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLCathodeRed[i] ,	 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLCathodeGreen[i] , TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLCathodeBlue[i] , 	TYPE3_PWM_FRAME_MAX_DATA);		//turn on one LED
			UT_DelayNms(100);				//delay 100ms	
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLCathodeRed[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLCathodeGreen[i] , TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED
			SPI_W_3BYTE(SPI_FRAME_TWO_PAGE, tabLED_Type3PWMCTLCathodeBlue[i] , 	TYPE3_PWM_FRAME_CLR_DATA);		//turn off one LED	
	}	
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
