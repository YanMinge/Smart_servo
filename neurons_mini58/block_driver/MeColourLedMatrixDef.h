#ifndef __MECOLOURLEDMATRIXDEF_H__
#define __MECOLOURLEDMATRIXDEF_H__

#define		SPI_FRAME_ONE_PAGE				0x0			//Setting SLED1735 Frame Page 1
#define		SPI_FRAME_TWO_PAGE				0x1			//Setting SLED1735 Frame Page 2	
#define		SPI_FRAME_FUNCTION_PAGE		0xB			//Setting SLED1735 Frame Page 9	
#define		SPI_FRAME_DETECTION_PAGE	0xC			//Setting SLED1735 Frame Page 10
#define		SPI_FRAME_LED_VAF_PAGE		0xD			//Setting SLED1735 Frame Page 11	



/*_____ D E F I N I T I O N S ______________________________________________*/

	/*--------Registers Page Define------------*/
		#define	CONFIGURE_CMD_PAGE 0XFD
		#define	FRAME1_PAGE 0X00
		#define	FRAME2_PAGE 0X01
		#define	FUNCTION_PAGE 0X0B
		#define	DETECTION_PAGE 0X0C
		#define	LED_VAF_PAGE 0X0D
		
	/*--------Registers Define------------*/		
		#define	CONFIGURATION_REG 0X00
		#define	PICTURE_DISPLAY_REG 0X01
		#define	DISPLAY_OPTION_REG 0X05
		#define	BREATH_CTL_REG 0X08
		#define	BREATH_CTL_REG2 0X09
		#define	SW_SHUT_DOWN_REG 0X0A	
		
		#define	AUDIO_GAIN_CTL_REG 0X0B
		#define	STAGGERED_DELAY_REG 0X0D
		#define	SLEW_RATE_CTL_REG 0X0E
		#define	CURRENT_CTL_REG 0X0F
		#define	OPEN_SHORT_REG 0X10				
		#define	OPEN_SHORT_REG2 0X11			
		#define	VAF_CTL_REG 0X14			
		#define	VAF_CTL_REG2 0X15	
		#define	TYPE4_VAF_REG1 0X18	
		#define	TYPE4_VAF_REG2 0X19
		#define	TYPE4_VAF_REG3 0X1A
		
/*--------Function Register: address 00H Configuration Reg------------*/	
		#define	mskDISPLAY_MODE_EN						(0X0<<3)		
		#define	mskDISPLAY_MODE_DIS					(0X1<<3)		//disable PWM output
		#define	mskSHOLD_EN									(0x1<<5)	
		#define	mskSHOLD_DIS								(0x0<<5)	
		#define	mskSYNC_HIGH_IMPEDANCE				(0x0<<6)
		#define	mskSYNC_MASTER_MODE					(0x1<<6)
		#define	mskSYNC_SLAVE_MODE						(0x2<<6)	
		
	/*--------Function Register: address 01H Picture Display Reg------------*/			
		#define	mskMATRIX_TYPE_TYPE1						(0x0<<3)	
		#define	mskMATRIX_TYPE_TYPE2						(0x1<<3)					
		#define	mskMATRIX_TYPE_TYPE3						(0x2<<3)				
		#define	mskMATRIX_TYPE_TYPE4						(0x3<<3)	

	/*--------Function Register: address 05H Display Option Reg------------*/
		#define	mskBLINK_PERIOD_TIME_CONST			(0x7<<0)	
		#define	mskBLINK_EN								(0x1<<3)
		#define	mskBLINK_DIS							(0x0<<3)			
		#define	mskBLINK_FRAME_300					(0x0<<6)			
		#define	mskBLINK_FRAME_600					(0x1<<6)	
		#define	mskBLINK_FRAME_75					(0x2<<6)	
		#define	mskBLINK_FRAME_150					(0x3<<6)	
		
	/*--------Function Register: address 08H Breath Control Reg------------*/
		#define	mskBC_FADE_IN_TIME_CONST					(0x7<<0)
		#define	mskBC_FADE_OUT_TIME_CONST				(0x7<<4)	
		
	/*--------Function Register: address 09H Breath Control Reg2------------*/	
		#define	mskBC_EXTINGUISH_TIME_CONST			(0x7<<0)		
		#define	mskBC_BREATH_EN								(0x1<<4)	
		#define	mskBC_BREATH_DIS								(0x0<<4)			
		#define	mskBC_CONTINUOUS_BREATH_EN			(0x1<<5)	
		#define	mskBC_CONTINUOUS_BREATH_DIS			(0x0<<5)	

	/*--------Function Register: address 0AH Shutdown Reg------------*/	
		#define	mskSW_SHUT_DOWN_MODE					(0x0<<0)
		#define	mskSW_NORMAL_MODE						(0x1<<0)	
	
	/*--------Function Register: address 0BH AGC Control Reg------------*/	
		#define	mskAGS_CONST							(0x7<<0)
		#define	mskAGC_EN									(0x0<<3)
		#define	mskAGC_DIS								(0x1<<3)	
		#define	mskAGC_MODE_SLOW						(0x0<<4)
		#define	mskAGC_MODE_FAST						(0x1<<4)	
		
	/*--------Function Register: address 0DH Staggered Delay Reg------------*/	
		#define	mskSTD1				(0x3<<0)	
		#define	mskSTD2				(0x3<<2)
		#define	mskSTD3				(0x3<<4)	
		#define	mskSTD4				(0x3<<6)
		
		#define	CONST_STD_GROUP1	0x00		
		#define	CONST_STD_GROUP2	0x55	
		#define	CONST_STD_GROUP3	0xAA		
		#define	CONST_STD_GROUP4	0xFF		
	
	/*--------Function Register: address 0EH Slew Rate Control Reg------------*/	
		#define	mskSLEW_RATE_CTL_EN				(0x1<<0)	
		#define	mskSLEW_RATE_CTL_DIS			(0x0<<0)	

	/*--------Function Register: address 0FH Current Control Reg------------*/
		#define	mskCURRENT_STEP_CONST		(0x3F<<0)		
		#define	CONST_CURRENT_STEP_10mA			(0x5<<0)
		#define	CONST_CURRENT_STEP_15mA			(0xF<<0)
		#define	CONST_CURRENT_STEP_20mA			(0x19<<0)
		#define	CONST_CURRENT_STEP_25mA			(0x23<<0)		
		#define	CONST_CURRENT_STEP_32mA			(0x31<<0)
		#define	CONST_CURRENT_STEP_40mA			(0x3F<<0)

		#define	mskCURRENT_CTL_EN				(0x1<<7)	
		#define	mskCURRENT_CTL_DIS				(0x0<<7)	

	/*--------Function Register: address 10H Open/Short Test Reg------------*/	
		#define	mskOPEN_SHORT_DETECT_DUTY	(0x3F<<0)	
		#define	mskSHORT_DETECT_START		(0x1<<6) // HW will clear this bits automatically	
		#define	mskOPEN_DETECT_START			(0x1<<7) // HW will clear this bits automatically		
		
	/*--------Function Register: address 11H Open/Short Test Reg2------------*/	
		#define	mskSHORT_DETECT_INT		(0x1<<6) //when open/short detect done, HW will set this bit as 1 automatically	
		#define	mskOPEN_DETECT_INT			(0x1<<7) //when open/short detect done, HW will set this bit as 1 automatically		

	/*--------Function Register: address 14H VAF Reg1 ------------*/
		#define	Pakeage_Type Pakeage_Type_EV
		//////////////////////////
		#define	Pakeage_Type_EV	0
		#define	Pakeage_Type_MP	1	
		//////////////////////////
	
		#if (Package_Type == Package_Type_MP)	
				#define	mskVAF1										(0x4<<0) 		
				#define	mskVAF2										(0x4<<4)		
		#else 		
				#define	mskVAF1										(0x0<<0) 	
				#define	mskVAF2										(0x0<<4) 		
		#endif
 		
	/*--------Function Register: address 15H VAF Reg2 ------------*/	
		#if (Package_Type == Package_Type_MP)	
				#define	mskVAF3										(0x4<<0) 		
		#else 		
				#define	mskVAF3										(0x7<<0) 				
		#endif
		
		#define	mskTP4VAF3								(0x0<<0) 		
		
		#define	mskFORCEVAFTIME_CONST			(0x0<<3) 			
		#define	mskFORCEVAFCTL_ALWAYSON		(0x0<<6) 	
		#define	mskFORCEVAFCTL_VAFTIMECTL	(0x1<<6) 	
		#define	mskFORCEVAFCTL_DISABLE		(0x2<<6) 	
		
	/*--------Function Register: address 18H Type 4 VAF Reg1 ------------*/	
		#define	mskTYPE4_CB4_VAF2									(0x0<<0) 
		#define	mskTYPE4_CB4_VAF1									(0x1<<0) 	
		#define	mskTYPE4_CB4_VAF3									(0x2<<0) 		
		#define	mskTYPE4_CB4_VAFFLOATING						(0x3<<0) 	
		
		#define	mskTYPE4_CB5_VAF2									(0x0<<2) 
		#define	mskTYPE4_CB5_VAF1									(0x1<<2) 	
		#define	mskTYPE4_CB5_VAF3									(0x2<<2) 		
		#define	mskTYPE4_CB5_VAFFLOATING						(0x3<<2) 	
		
		#define	mskTYPE4_CB6_VAF2									(0x0<<4) 
		#define	mskTYPE4_CB6_VAF1									(0x1<<4) 	
		#define	mskTYPE4_CB6_VAF3									(0x2<<4) 		
		#define	mskTYPE4_CB6_VAFFLOATING						(0x3<<4) 	
	
		#define	mskTYPE4_CB7_VAF2									(0x0<<6) 
		#define	mskTYPE4_CB7_VAF1									(0x1<<6) 	
		#define	mskTYPE4_CB7_VAF3									(0x2<<6) 		
		#define	mskTYPE4_CB7_VAFFLOATING						(0x3<<6) 
		
	/*--------Function Register: address 19H Type 4 VAF Reg2 ------------*/	
		#define	mskTYPE4_CB8_VAF2									(0x0<<0) 
		#define	mskTYPE4_CB8_VAF1									(0x1<<0) 	
		#define	mskTYPE4_CB8_VAF3									(0x2<<0) 		
		#define	mskTYPE4_CB8_VAFFLOATING						(0x3<<0) 	
		
		#define	mskTYPE4_CB9_VAF2									(0x0<<2) 
		#define	mskTYPE4_CB9_VAF1									(0x1<<2) 	
		#define	mskTYPE4_CB9_VAF3									(0x2<<2) 		
		#define	mskTYPE4_CB9_VAFFLOATING						(0x3<<2) 	
		
		#define	mskTYPE4_CC1_VAF2									(0x0<<4) 
		#define	mskTYPE4_CC1_VAF1									(0x1<<4) 	
		#define	mskTYPE4_CC1_VAF3									(0x2<<4) 		
		#define	mskTYPE4_CC1_VAFFLOATING						(0x3<<4) 	
	
		#define	mskTYPE4_CC2_VAF2									(0x0<<6) 
		#define	mskTYPE4_CC2_VAF1									(0x1<<6) 	
		#define	mskTYPE4_CC2_VAF3									(0x2<<6) 		
		#define	mskTYPE4_CC2_VAFFLOATING						(0x3<<6)
 		
	/*--------Function Register: address 1AH Type 4 VAF Reg3 ------------*/	
		#define	mskTYPE4_CC3_VAF2									(0x0<<0) 
		#define	mskTYPE4_CC3_VAF1									(0x1<<0) 	
		#define	mskTYPE4_CC3_VAF3									(0x2<<0) 		
		#define	mskTYPE4_CC3_VAFFLOATING					(0x3<<0) 	
		
		#define	mskTYPE4_CC4_VAF2									(0x0<<2) 
		#define	mskTYPE4_CC4_VAF1									(0x1<<2) 	
		#define	mskTYPE4_CC4_VAF3									(0x2<<2) 		
		#define	mskTYPE4_CC4_VAFFLOATING						(0x3<<2) 	
		
		#define	mskTYPE4_CC5_VAF2									(0x0<<4) 
		#define	mskTYPE4_CC5_VAF1									(0x1<<4) 	
		#define	mskTYPE4_CC5_VAF3									(0x2<<4) 		
		#define	mskTYPE4_CC5_VAFFLOATING						(0x3<<4) 	
	
		#define	mskTYPE4_CC6_VAF2									(0x0<<6) 
		#define	mskTYPE4_CC6_VAF1									(0x1<<6) 	
		#define	mskTYPE4_CC6_VAF3									(0x2<<6) 		
		#define	mskTYPE4_CC6_VAFFLOATING						(0x3<<6) 					
		
/*----------------------------------------------------------*/
/*											Frame1Page													*/
/*----------------------------------------------------------*/
		#define	mskLED_FRAME_REG_ADDR									(0xFF<<0) 
			#define	TYPE3_FRAME1PAGE_LENGTH							((TYPE3_PWM_FRAME_LAST_ADDR-TYPE3_LED_FRAME_FIRST_ADDR)+1)			
			#define	TYPE3_FRAME2PAGE_LENGTH							((TYPE3_PWM_FRAME2_LAST_ADDR-TYPE3_LED_FRAME2_FIRST_ADDR)+1)			
			#define	TYPE1_2_4_FRAME1PAGE_LENGTH					((TYPE1_PWM_FRAME_LAST_ADDR-TYPE1_LED_FRAME_FIRST_ADDR)+1)					
		
			//=======================//
			// LED Control Registers //
			//=======================//
			//-------Frame1----------//
			#define	TYPE1_LED_FRAME_FIRST_ADDR					0x00	
			#define	TYPE1_LED_FRAME_LAST_ADDR						0x11								 	
			#define	TYPE1_LED_FRAME_LENGTH					((TYPE1_LED_FRAME_LAST_ADDR-TYPE1_LED_FRAME_FIRST_ADDR)+1)

			#define	TYPE2_LED_FRAME_FIRST_ADDR					0x00	
			#define	TYPE2_LED_FRAME_LAST_ADDR						0x11								 	
			#define	TYPE2_LED_FRAME_LENGTH					((TYPE2_LED_FRAME_LAST_ADDR-TYPE2_LED_FRAME_FIRST_ADDR)+1)

			#define	TYPE3_LED_FRAME_FIRST_ADDR					0x00	
			#define	TYPE3_LED_FRAME_LAST_ADDR						0x0F								 	
			#define	TYPE3_LED_FRAME_LENGTH					((TYPE3_LED_FRAME_LAST_ADDR-TYPE3_LED_FRAME_FIRST_ADDR)+1)
			
			#define	TYPE4_LED_FRAME_FIRST_ADDR					0x00	
			#define	TYPE4_LED_FRAME_LAST_ADDR						0x11								 	
			#define	TYPE4_LED_FRAME_LENGTH					((TYPE4_LED_FRAME_LAST_ADDR-TYPE4_LED_FRAME_FIRST_ADDR)+1)
			
			//=========================//
			// Blink Control Registers //
			//=========================//	
			//-------Frame1----------//			
		#define	mskBLINK_FRAME_REG_ADDR								(0xFF<<0) 
			#define	TYPE1_BLINK_FRAME_FIRST_ADDR					0x12	
			#define	TYPE1_BLINK_FRAME_LAST_ADDR						0x23								 	
			#define	TYPE1_BLINK_FRAME_LENGTH					((TYPE1_BLINK_FRAME_LAST_ADDR-TYPE1_BLINK_FRAME_FIRST_ADDR)+1)	

			#define	TYPE2_BLINK_FRAME_FIRST_ADDR					0x12	
			#define	TYPE2_BLINK_FRAME_LAST_ADDR						0x23								 	
			#define	TYPE2_BLINK_FRAME_LENGTH					((TYPE2_BLINK_FRAME_LAST_ADDR-TYPE2_BLINK_FRAME_FIRST_ADDR)+1)

			#define	TYPE3_BLINK_FRAME_FIRST_ADDR					0x10	
			#define	TYPE3_BLINK_FRAME_LAST_ADDR						0x1F								 	
			#define	TYPE3_BLINK_FRAME_LENGTH					((TYPE3_BLINK_FRAME_LAST_ADDR-TYPE3_BLINK_FRAME_FIRST_ADDR)+1)	
			
			#define	TYPE4_BLINK_FRAME_FIRST_ADDR					0x12	
			#define	TYPE4_BLINK_FRAME_LAST_ADDR						0x23								 	
			#define	TYPE4_BLINK_FRAME_LENGTH					((TYPE4_BLINK_FRAME_LAST_ADDR-TYPE4_BLINK_FRAME_FIRST_ADDR)+1)	
			
			//=======================//
			// PWM Control Registers //
			//=======================//	
			//-------Frame1----------//					
		#define	mskPWM_FRAME_REG_ADDR									(0xFF<<0) 
			#define	TYPE1_PWM_FRAME_FIRST_ADDR					0x24	
			#define	TYPE1_PWM_FRAME_LAST_ADDR						0xB3								 	
			#define	TYPE1_PWM_FRAME_LENGTH					((TYPE1_PWM_FRAME_LAST_ADDR-TYPE1_PWM_FRAME_FIRST_ADDR)+1)	
	
			#define	TYPE1_RGB_ANODE_PWM_LENGTH					0x24 //Total control 36 RGB LEDs  			
			#define	TYPE1_RGB_CATHODE_PWM_LENGTH				0x24 //Total control 36 RGB LEDs  
			
			#define	TYPE2_PWM_FRAME_FIRST_ADDR					0x24	
			#define	TYPE2_PWM_FRAME_LAST_ADDR						0xB3								 	
			#define	TYPE2_PWM_FRAME_LENGTH					((TYPE2_PWM_FRAME_LAST_ADDR-TYPE2_PWM_FRAME_FIRST_ADDR)+1)

			#define	TYPE2_RGB_ANODE_PWM_LENGTH					0x28 //Total control 40 RGB LEDs  			
			#define	TYPE2_RGB_CATHODE_PWM_LENGTH				0x29 //Total control 41 RGB LEDs  		
			
			#define	TYPE3_PWM_FRAME_FIRST_ADDR					0x20	
			#define	TYPE3_PWM_FRAME_LAST_ADDR						0x9F								 	
			#define	TYPE3_PWM_FRAME_LENGTH							((TYPE3_PWM_FRAME_LAST_ADDR-TYPE3_PWM_FRAME_FIRST_ADDR)+1)	

			#define	TYPE3_RGB_ANODE_PWM_LENGTH								0x46//Total control 70 RGB LEDs  	
			#define	TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1					0x1C 
			#define	TYPE3_RGB_ANODE_PWM_LENGTH_FRAME1_2				0x2A 	  	
																		
			#define	TYPE3_RGB_CATHODE_PWM_FRAME1_LENGTH				0x25  	
			#define	TYPE3_RGB_CATHODE_PWM_FRAME2_LENGTH				0x4B //Total control 75 RGB LEDs  	
			
			#define	TYPE4_PWM_FRAME_FIRST_ADDR					0x24	
			#define	TYPE4_PWM_FRAME_LAST_ADDR						0xB3								 	
			#define	TYPE4_PWM_FRAME_LENGTH					((TYPE4_PWM_FRAME_LAST_ADDR-TYPE4_PWM_FRAME_FIRST_ADDR)+1)
			
			#define	TYPE4_RGB_ANODE_PWM_LENGTH					0x30 //Total control 48 RGB LEDs  			
			#define	TYPE4_RGB_CATHODE_PWM_LENGTH				0x30 //Total control 48 RGB LEDs  		
/*----------------------------------------------------------*/
/*											Frame2Page													*/
/*----------------------------------------------------------*/	
			//=======================//
			// LED Control Registers //
			//=======================//
			//-------Frame2----------//		
		#define	mskLED_FRAME2_REG_ADDR								(0xFF<<0) 		
			#define	TYPE3_LED_FRAME2_FIRST_ADDR					0x00	
			#define	TYPE3_LED_FRAME2_LAST_ADDR					0x0F								 	
			#define	TYPE3_LED_FRAME2_LENGTH				((TYPE3_LED_FRAME2_LAST_ADDR-TYPE3_LED_FRAME2_FIRST_ADDR)+1)
					
			//=========================//
			// Blink Control Registers //
			//=========================//	
			//-------Frame2------------//			
		#define	mskBLINK_FRAME2_REG_ADDR								(0xFF<<0) 	
			#define	TYPE3_BLINK_FRAME2_FIRST_ADDR					0x10	
			#define	TYPE3_BLINK_FRAME2_LAST_ADDR					0x1F								 	
			#define	TYPE3_BLINK_FRAME2_LENGTH				((TYPE3_BLINK_FRAME2_LAST_ADDR-TYPE3_BLINK_FRAME2_FIRST_ADDR)+1)		
			
			//=======================//
			// PWM Control Registers //
			//=======================//	
			//-------Frame2----------//				
		#define	mskPWM_FRAME2_REG_ADDR								(0xFF<<0) 		
			#define	TYPE3_PWM_FRAME2_FIRST_ADDR					0x20	
			#define	TYPE3_PWM_FRAME2_LAST_ADDR					0x9F								 	
			#define	TYPE3_PWM_FRAME2_LENGTH				((TYPE3_PWM_FRAME2_LAST_ADDR-TYPE3_PWM_FRAME2_FIRST_ADDR)+1)		
			
/*----------------------------------------------------------*/
/*											Frame Data 													*/
/*----------------------------------------------------------*/			
	/*--------Frame Register: Data ------------*/	
		#define	mskLED_FRAME_REG_DATA									(0xFF<<0) 
			#define	TYPE1_LED_FRAME_CLR_DATA						0x00	
			#define	TYPE1_LED_FRAME_MAX_DATA						0xFF 	
	
			#define	TYPE2_LED_FRAME_CLR_DATA						0x00	
			#define	TYPE2_LED_FRAME_MAX_DATA						0xFF 

			#define	TYPE3_LED_FRAME_CLR_DATA						0x00	
			#define	TYPE3_LED_FRAME_MAX_DATA						0xFF 	
			
			#define	TYPE4_LED_FRAME_CLR_DATA						0x00	
			#define	TYPE4_LED_FRAME_MAX_DATA						0xFF 
					
		#define	mskBLINK_FRAME_REG_DATA								(0xFF<<0) 	
			#define	TYPE1_BLINK_FRAME_CLR_DATA					0x00	
			#define	TYPE1_BLINK_FRAME_MAX_DATA					0xFF
	
			#define	TYPE2_BLINK_FRAME_CLR_DATA					0x00	
			#define	TYPE2_BLINK_FRAME_MAX_DATA					0xFF 	

			#define	TYPE3_BLINK_FRAME_CLR_DATA					0x00	
			#define	TYPE3_BLINK_FRAME_MAX_DATA					0xFF 
 			
			#define	TYPE4_BLINK_FRAME_CLR_DATA					0x00	
			#define	TYPE4_BLINK_FRAME_MAX_DATA					0xFF 	
						
		#define	mskPWM_FRAME_REG_DATA									(0xFF<<0)
			#define	TYPE1_PWM_FRAME_CLR_DATA						0x00	
			#define	TYPE1_PWM_FRAME_MAX_DATA						0xFF 
			
			#define	TYPE2_PWM_FRAME_CLR_DATA						0x00	
			#define	TYPE2_PWM_FRAME_MAX_DATA						0xFF 			
			
			#define	TYPE3_PWM_FRAME_CLR_DATA						0x00	
			#define	TYPE3_PWM_FRAME_MAX_DATA						0xFF 
			
			#define	TYPE4_PWM_FRAME_CLR_DATA						0x00	
			#define	TYPE4_PWM_FRAME_MAX_DATA						0xFF  		

/*----------------------------------------------------------*/
/*											FrameVAF1Page												*/
/*----------------------------------------------------------*/	
			//-------Frame1----------//
			#define	TYPE1_VAF_FRAME_FIRST_ADDR					0x00	
			#define	TYPE1_VAF_FRAME_LAST_ADDR						0x23								 	
			#define	TYPE1_VAF_FRAME_LENGTH					((TYPE1_VAF_FRAME_LAST_ADDR-TYPE1_VAF_FRAME_FIRST_ADDR)+1)

			#define	TYPE2_VAF_FRAME_FIRST_ADDR					0x00	
			#define	TYPE2_VAF_FRAME_LAST_ADDR						0x23								 	
			#define	TYPE2_VAF_FRAME_LENGTH					((TYPE2_VAF_FRAME_LAST_ADDR-TYPE2_VAF_FRAME_FIRST_ADDR)+1)

			#define	TYPE3_VAF_FRAME_FIRST_ADDR					0x00	
			#define	TYPE3_VAF_FRAME_LAST_ADDR						0x3F								 	
			#define	TYPE3_VAF_FRAME_LENGTH					((TYPE3_VAF_FRAME_LAST_ADDR-TYPE3_VAF_FRAME_FIRST_ADDR)+1)
			
			#define	TYPE4_VAF_FRAME_FIRST_ADDR					0x00	
			#define	TYPE4_VAF_FRAME_LAST_ADDR						0x23								 	
			#define	TYPE4_VAF_FRAME_LENGTH					((TYPE4_VAF_FRAME_LAST_ADDR-TYPE4_VAF_FRAME_FIRST_ADDR)+1)

/*_____ D E C L A R A T I O N S ____________________________________________*/
#define TYPE3_VAF_OPTION	TYPE3_ANODE_COLOR_RGB
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
																		
/*_____ D E F I N I T I O N S ______________________________________________*/


	//---------------------------------------------------------------------
	//															
	//				Type3 RGB Anode Control Table														
	//														
	//---------------------------------------------------------------------
	//Address 0x00 bits[7:2] 			/ 0x01 bits[7:0] 				=> Red	 D1~D14
	//Address 0x02 bits[7:2] 			/ 0x03 bits[7:0] 				=> Green	 D1~D14	
	//Address 0x04 bits[7:2] 			/ 0x05 bits[7:0] 				=> Blue	 D1~D14
	//---------------------------------------------------------------------
	//Address 0x06 bits[7:5][2:0] / 0x07 bits[7:0] 				=> Red	 D15~D28
	//Address 0x08 bits[7:5][2:0] / 0x09 bits[7:0] 				=> Green	 D15~D28	
	//Address 0x0A bits[7:5][2:0] / 0x0B bits[7:0] 				=> Blue	 D15~D28
	//---------------------------------------------------------------------
	//Address 0x0C bits[5:0]			/ 0x0D bits[7:0] 				=> Red	 D29~D42
	//Address 0x0E bits[5:0]			/ 0x0F bits[7:0] 				=> Green	 D29~D42	
	//Address 0x00 bits[5:0]			/ 0x01bits[7:0] 				=> Blue	 D29~D42
	//----------------------------------------------------------------------
	//Address 0x02 bits[7:0] 			/	0x03 bits[7:3][0] 		=> Red	 D43~D56	
	//Address 0x04 bits[7:0] 			/ 0x05 bits[7:3][0] 		=> Green	 D43~D56
	//Address 0x06 					 			/ 0x07 bits[7:3][0] 		=> Blue	 D43~D56
	//----------------------------------------------------------------------
	//Address 0x08 bits[7:0]      / 0x09 bits[7:6][3:0] 	=> Red	 D57~D70	
	//Address 0x0A bits[7:0]     	/ 0x0B bits[7:6][3:0] 	=> Green	 D57~D70	
	//Address 0x0C bits[7:0]      / 0x0D bits[7:6][3:0] 	=> Blue	 D57~D70		
	//----------------------------------------------------------------------
	//---------------------------------------------------------------------
	//LUT for Frame1Page : LED Control and Blink Control Ram 
	//Notice: This Table setting the R/G/B LEDs mapping value,
	//				Setting start Address from 0x00 to 0x11 : LED Control Ram
	//				Setting start Address from 0x12 to 0x23 : Blink Control Ram
	//----------------------------------------------------------------------
const uint8_t tabLED_Type3CTLAnodeRed[32] = { //Reference SLED1735 Datasheet Type3 Map
																0xFC,	0xFF, 			//Address 0x00 bits[7:2] 			/ 0x01 bits[7:0] 				=> Red	 D1~D14
																0x00,	0x00,				//Address 0x02 					 			/ 0x03					  
																0x00,	0x00,				//Address 0x04 				 			 	/ 0x05						  
																0xE7,	0xFF,				//Address 0x06 bits[7:5][2:0] / 0x07 bits[7:0] 				=> Red	 D15~D28
																0x00,	0x00,				//Address 0x08					 			/ 0x09						
																0x00,	0x00,				//Address 0x0A				 	 			/ 0x0B		
																0x3F,	0xFF,				//Address 0x0C bits[5:0]			/ 0x0D bits[7:0] 				=> Red	 D29~D42
																0x00,	0x00,				//Address 0x0E 							 	/ 0x0F					  	
																0x00,	0x00,				//Address 0x00					 			/ 0x01
																0xFF,	0xF9,				//Address 0x02 bits[7:0] 			/	0x03 bits[7:3][0] 		=> Red	 D43~D56		
																0x00,	0x00,				//Address 0x04 								/ 0x05	
																0x00,	0x00,				//Address 0x06					 			/ 0x07	
																0xFF,	0xCF,				//Address 0x08 bits[7:0]      / 0x09 bits[7:6][3:0] 	=> Red	 D57~D70		
																0x00,	0x00,				//Address 0x0A					 			/ 0x0B		
																0x00,	0x00,				//Address 0x0C					 			/ 0x0D		
																0x00,	0x00,				//Address 0x0E					 			/ 0x0F	
															};

const uint8_t tabLED_Type3CTLAnodeGreen[32] = { //Reference SLED1735 Datasheet Type3 Map
																0x00,	0x00, 			//Address 0x00 					 			/ 0x01 
																0xFC,	0xFF,				//Address 0x02 bits[7:2] 			/ 0x03 bits[7:0] 				=> Green	 D1~D14
																0x00,	0x00,				//Address 0x04 								/ 0x05					  
																0x00,	0x00,				//Address 0x06 					 			/ 0x07
																0xE7,	0xFF,				//Address 0x08 bits[7:5][2:0] / 0x09 bits[7:0] 				=> Green	 D15~D28
																0x00,	0x00,				//Address 0x0A 					 			/ 0x0B 	
																0x00,	0x00,				//Address 0x0C				 	 			/ 0x0D		
																0x3F,	0xFF,				//Address 0x0E bits[5:0]			/ 0x0F bits[7:0] 				=> Green	 D29~D42	
																0x00,	0x00,				//Address 0x00 								/ 0x01
																0x00,	0x00,				//Address 0x02					 			/ 0x03		
																0xFF,	0xF9,				//Address 0x04 bits[7:0] 			/ 0x05 bits[7:3][0] 		=> Green	 D43~D56		
																0x00,	0x00,				//Address 0x06 								/ 0x07
																0x00,	0x00,				//Address 0x08							 	/ 0x09			
																0xFF,	0xCF,				//Address 0x0A bits[7:0]     	/ 0x0B bits[7:6][3:0] 	=> Green	 D57~D70			
																0x00,	0x00,				//Address 0x0C 								/ 0x0D		
																0x00,	0x00,				//Address 0x0E					 			/ 0x0F		
															};															
															
const uint8_t tabLED_Type3CTLAnodeBlue[32] = { //Reference SLED1735 Datasheet Type3 Map
																0x00,	0x00,				//Address 0x00					 			/ 0x01
																0x00,	0x00,				//Address 0x02					 			/ 0x03
																0xFC,	0xFF, 			//Address 0x04 bits[7:2] 			/ 0x05 bits[7:0] 				=> Blue	 D1~D14
																0x00,	0x00,				//Address 0x06								/ 0x07
																0x00,	0x00,				//Address 0x08 				 	 			/ 0x09						  
																0xE7,	0xFF,				//Address 0x0A bits[7:5][2:0] / 0x0B bits[7:0] 				=> Blue	 D15~D28
																0x00,	0x00,				//Address 0x0C 								/ 0x0D		
																0x00,	0x00,				//Address 0x0E				 	 			/ 0x0F		
																0x3F,	0xFF,				//Address 0x00 bits[5:0]			/ 0x01bits[7:0] 				=> Blue	 D29~D42
																0x00,	0x00,				//Address 0x02 								/ 0x03		
																0x00,	0x00,				//Address 0x04					 			/ 0x05
																0xFF,	0xF9,				//Address 0x06 					 			/ 0x07 bits[7:3][0] 		=> Blue	 D43~D56		
																0x00,	0x00,				//Address 0x08  							/ 0x09	
																0x00,	0x00,				//Address 0x0A					 			/ 0x0B	
																0xFF,	0xCF,				//Address 0x0C bits[7:0]      / 0x0D bits[7:6][3:0] 	=> Blue	 D57~D70	
																0x00,	0x00,				//Address 0x0E  							/ 0x0F		
															};																

//Table Index means the number of LED, and the data of table means PWM control ram address															
const uint8_t tabLED_Type3PWMCTLAnodeRed[70] = { //Reference SLED1735 Datasheet Type3 Map
																0x22,	0x23, 0x24, 0x25, 0x26, 0x27,	0x28, 0x29, 0x2A,	0x2B, 0x2C, 0x2D, 0x2E, 0x2F, // Red D1~D14 PWM CTL Mapping address
																0x50,	0x51, 0x52, 0x55, 0x56, 0x57,	0x58, 0x59, 0x5A,	0x5B, 0x5C, 0x5D, 0x5E, 0x5F,	// Red D15~D28 PWM CTL Mapping address
																0x80,	0x81, 0x82, 0x83, 0x84, 0x85, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D,	0x8E, 0x8F, // Red D29~D42 PWM CTL Mapping address
																0x30,	0x31,	0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,	0x3B, 0x3C, 0x3D, 0x3E, 0x3F, // Red D43~D56 PWM CTL Mapping address
																0x60, 0x61, 0x62, 0x63,	0x64, 0x65, 0x66,	0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6E, 0x6F, // Red D57~D70 PWM CTL Mapping address
															};							

//Table Index means the number of LED, and the data of table means PWM control ram address																	
const uint8_t tabLED_Type3PWMCTLAnodeGreen[70] = { //Reference SLED1735 Datasheet Type3 Map
																0x32, 0x33, 0x34, 0x35,	0x36, 0x37, 0x38,	0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,	// Green D1~D14 PWM CTL Mapping address
																0x60,	0x61, 0x62, 0x65, 0x66, 0x67,	0x68, 0x69, 0x6A,	0x6B, 0x6C, 0x6D, 0x6E, 0x6F,	// Green D15~D28 PWM CTL Mapping address
																0x90,	0x91, 0x92, 0x93, 0x94, 0x95, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D,	0x9E, 0x9F, // Green D29~D42 PWM CTL Mapping address
																0x40,	0x41,	0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,	0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // Green D43~D56 PWM CTL Mapping address
																0x70, 0x71, 0x72, 0x73,	0x74, 0x75, 0x76,	0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7E, 0x7F, // Green D57~D70 PWM CTL Mapping address
															};															

//Table Index means the number of LED, and the data of table means PWM control ram address																	
const uint8_t tabLED_Type3PWMCTLAnodeBlue[70] = { //Reference SLED1735 Datasheet Type3 Map
																0x42, 0x43, 0x44,	0x45, 0x46, 0x47,	0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, // Blue D1~D14 PWM CTL Mapping address
																0x70,	0x71, 0x72, 0x75, 0x76, 0x77,	0x78, 0x79, 0x7A,	0x7B, 0x7C, 0x7D, 0x7E, 0x7F,	// Blue D15~D28 PWM CTL Mapping address
																0x20,	0x21, 0x22, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,	0x2E, 0x2F, // Blue D29~D42 PWM CTL Mapping address
																0x50,	0x51,	0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,	0x5B, 0x5C, 0x5D, 0x5E, 0x5F, // Blue D43~D56 PWM CTL Mapping address
																0x80, 0x81, 0x82, 0x83,	0x84, 0x85, 0x86,	0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8E, 0x8F, // Blue D57~D70 PWM CTL Mapping address
															};																			

	//---------------------------------------------------------------------
	//															
	//				Type3 RGB Cathode Control Table															
	//														
	//---------------------------------------------------------------------
	//Address 0x00 bits[6][3][0] 		/ 0x01 bits[4][1] 			=> Red	 D1~D5
	//Address 0x00 bits[7][4][1] 		/ 0x01 bits[5][2] 			=> Green D1~D5
	//Address 0x00 bits   [5][2] 		/ 0x01 bits[6][3][0] 	 	=> Blue	 D1~D5	
	//---------------------------------------------------------------------
	//Address 0x02 bits[6][3] 	 		/ 0x03 bits[4][1] 			=> Red	 D6~D9
	//Address 0x02 bits[7][4] 	 		/ 0x03 bits[5][2] 			=> Green D6~D9
	//Address 0x03 bits   [5]	   		/ 0x03 bits[6][3][0] 	 	=> Blue	 D6~D9	
	//----------------------------------------------------------------------
	//Address 0x04 bits[6][3][1] 		/ 0x05 bits[4][1] 			=> Red	 D10~D14
	//Address 0x04 bits[7][4][0] 		/ 0x05 bits[5][2] 			=> Green D10~D14
	//Address 0x04 bits   [5][2] 		/ 0x05 bits[6][3][0] 	 	=> Blue	 D10~D14
	//---------------------------------------------------------------------
	//Address 0x06 bits[6][3][1] 		/ 0x07 bits   [4][1] 	 	=> Red	 D15~D19
	//Address 0x06 bits[7][4][2] 		/ 0x07 bits   [5][2] 	 	=> Green D15~D19
	//Address 0x06 bits   [5] 	 		/ 0x07 bits[7][6][3][0] => Blue	 D15~D19
	//---------------------------------------------------------------------
	//Address 0x08 bits[6][1] 	 		/ 0x09 bits[4][1] 	 		=> Red	 D20~D23
	//Address 0x08 bits[7][2] 	 		/ 0x09 bits[5][2] 	 		=> Green D20~D23
	//Address 0x08 bits   [3] 	 		/ 0x09 bits[6][3][0] 	 	=> Blue	 D20~D23
	//---------------------------------------------------------------------
	//Address 0x0A bits[6][4][1] 		/ 0x0B bits[4][1] 	 	 	=> Red	 D24~D28
	//Address 0x0A bits[7][0][2] 		/ 0x0B bits[5][2] 	 	 	=> Green D24~D28
	//Address 0x0A bits   [5][3] 		/ 0x0B bits[6][3][0] 	 	=> Blue	 D24~D28
	//---------------------------------------------------------------------
	//Address 0x0C bits[6][4][1] 		/ 0x0D bits   [4][1] 	 	=> Red	 D29~D33
	//Address 0x0C bits[7][5][2] 		/ 0x0D bits   [5][2] 	  => Green D29~D33
	//Address 0x0C bits      [3] 		/ 0x0D bits[7][6][3][0] => Blue	 D29~D33
	//---------------------------------------------------------------------
	//Address 0x0E bits[4][1] 			/ 0x0F bits[4][1] 	 	 	=> Red	 D34~D37
	//Address 0x0E bits[5][2] 			/ 0x0F bits[5][2] 	 	 	=> Green D34~D37
	//Address 0x0E bits[6][3] 			/ 0x0F bits[6][3] 	 		=> Blue	 D34~D37	
	//---------------------------------------------------------------------
	//Address 0x00 bits[7][4][1] 		/ 0x01 bits[4][1] 	 	 	=> Red 	 D38~D42
	//Address 0x00 bits[0][5][2] 		/ 0x01 bits[5][2] 	 	 	=> Green D38~D42
	//Address 0x00 bits   [6][3] 		/ 0x01 bits[6][3][0] 	 	=> Blue	 D38~D42	
	//---------------------------------------------------------------------
	//Address 0x02 bits[7][4][1] 		/ 0x03 bits[4][1] 	 		=> Red 	 D43~D47
	//Address 0x02 bits		[5][2] 		/ 0x03 bits[5][2][0]	  => Green D43~D47
	//Address 0x02 bits   [6][3] 		/ 0x03 bits[6][3][7]	  => Blue	 D43~D47		
	//---------------------------------------------------------------------
	//Address 0x04 bits[7][4][1] 		/ 0x05 bits[4] 	 	 			=> Red 	 D48~D51
	//Address 0x04 bits		[5][2] 		/ 0x05 bits[5][0] 	 	 	=> Green D48~D51
	//Address 0x04 bits   [6][3] 		/ 0x05 bits[6][1] 	 		=> Blue	 D48~D51		
	//---------------------------------------------------------------------
	//Address 0x06 bits		[7][4][1] / 0x07 bits[4][2] 	 		=> Red	 D52~D56
	//Address 0x06 bits	[0]	 [5][2] / 0x07 bits[5]	 [0] 		=> Green D52~D56
	//Address 0x06 bits   	 [6][3] / 0x07 bits[6][3][1] 		=> Blue	 D52~D56
	//---------------------------------------------------------------------
	//Address 0x08 bits[7][4][1] 		/ 0x09 bits[4][2] 	 		=> Red	 D57~D61
	//Address 0x08 bits		[5][2] 		/ 0x09 bits[5][3][0] 	  => Green D57~D61
	//Address 0x08 bits   [6][3] 		/ 0x09 bits[6][7][1]	  => Blue	 D57~D61
	//---------------------------------------------------------------------
	//Address 0x0A bits[7][4][1] 		/ 0x0B bits[2] 	 				=> Red	 D62~D65
	//Address 0x0A bits		[5][2] 		/ 0x0B bits[3][0] 			=> Green D62~D65
	//Address 0x0A bits   [6][3] 		/ 0x0B bits[4][1] 			=> Blue	 D62~D65	
	//---------------------------------------------------------------------
	//Address 0x0C bits		[7][4][1] / 0x0D bits[5][2] 	 		=> Red	 D66~D70
	//Address 0x0C bits[0]	 [5][2] / 0x0D bits		[3][0] 	 	=> Green D66~D70
	//Address 0x0C bits      [6][3] / 0x0D bits[6][4][1]	 	=> Blue	 D66~D70
	//---------------------------------------------------------------------
	//Address 0x08 bits[7][4][1] 		/ 0x09 bits[5][2] 	 		=> Red	 D71~D75
	//Address 0x08 bits		[5][2] 		/ 0x09 bits[6][3][0] 	  => Green D71~D75
	//Address 0x08 bits   [6][3] 		/ 0x09 bits[7][4][1]	  => Blue	 D71~D75

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	//LUT for Frame1Page : LED Control and Blink Control Ram 
	//Notice: This Table setting the R/G/B LEDs mapping value,
	//				Setting start Address from 0x00 to 0x11 : LED Control Ram
	//				Setting start Address from 0x12 to 0x23 : Blink Control Ram
	//----------------------------------------------------------------------															
const uint8_t tabLED_Type3CTLCathodeRed[32] = { //Reference SLED1735 Datasheet Type3 Map
																0x49,	0x12, 			//Address 0x00 bits[6][3][0] 		/ 0x01 bits[4][1] 			=> Red	 D1~D5
																0x48,	0x12,				//Address 0x02 bits[6][3] 	 		/ 0x03 bits[4][1] 			=> Red	 D6~D9
																0x4A,	0x12,				//Address 0x04 bits[6][3][1] 		/ 0x05 bits[4][1] 			=> Red	 D10~D14
																0x4A,	0x12,				//Address 0x06 bits[6][3][1] 		/ 0x07 bits   [4][1] 	 	=> Red	 D15~D19
																0x42,	0x12,				//Address 0x08 bits[6][1] 	 		/ 0x09 bits[4][1] 	 		=> Red	 D20~D23
																0x52,	0x12,				//Address 0x0A bits[6][4][1] 		/ 0x0B bits[4][1] 	 	 	=> Red	 D24~D28
																0x52,	0x12,				//Address 0x0C bits[6][4][1] 		/ 0x0D bits   [4][1] 	 	=> Red	 D29~D33
																0x12,	0x12,				//Address 0x0E bits[4][1] 			/ 0x0F bits[4][1] 	 	 	=> Red	 D34~D37
																0x92,	0x12,				//Address 0x00 bits[7][4][1] 		/ 0x01 bits[4][1] 	 	 	=> Red 	 D38~D42
																0x92,	0x12,				//Address 0x02 bits[7][4][1] 		/ 0x03 bits[4][1] 	 		=> Red 	 D43~D47	
																0x92,	0x10,				//Address 0x04 bits[7][4][1] 		/ 0x05 bits[4] 	 	 			=> Red 	 D48~D51	
																0x92,	0x14,				//Address 0x06 bits		[7][4][1] / 0x07 bits[4][2] 	 		=> Red	 D52~D56
																0x92,	0x14,				//Address 0x08 bits[7][4][1] 		/ 0x09 bits[4][2] 	 		=> Red	 D57~D61		
																0x92,	0x04,				//Address 0x0A bits[7][4][1] 		/ 0x0B bits[2] 	 				=> Red	 D62~D65
																0x92,	0x24,				//Address 0x0C bits		[7][4][1] / 0x0D bits[5][2] 	 		=> Red	 D66~D70
																0x92,	0x24,				//Address 0x08 bits[7][4][1] 		/ 0x09 bits[5][2] 	 		=> Red	 D71~D75
															};

const uint8_t tabLED_Type3CTLCathodeGreen[32] = { //Reference SLED1735 Datasheet Type3 Map
																0x92,	0x24, 			//Address 0x00 bits[7][4][1] 		/ 0x01 bits[5][2] 			=> Green D1~D5
																0x90,	0x24,				//Address 0x02 bits[7][4] 	 		/ 0x03 bits[5][2] 			=> Green D6~D9
																0x91,	0x24,				//Address 0x04 bits[7][4][0] 		/ 0x05 bits[5][2] 			=> Green D10~D14
																0x94,	0x24,				//Address 0x06 bits[7][4][2] 		/ 0x07 bits   [5][2] 	 	=> Green D15~D19
																0x84,	0x24,				//Address 0x08 bits[7][2] 	 		/ 0x09 bits[5][2] 	 		=> Green D20~D23
																0x85,	0x24,				//Address 0x0A bits[7][0][2] 		/ 0x0B bits[5][2] 	 	 	=> Green D24~D28
																0xA4,	0x24,				//Address 0x0C bits[7][5][2] 		/ 0x0D bits   [5][2] 	  => Green D29~D33
																0x24,	0x24,				//Address 0x0E bits[5][2] 			/ 0x0F bits[5][2] 	 	 	=> Green D34~D37
																0x25,	0x24,				//Address 0x00 bits[0][5][2] 		/ 0x01 bits[5][2] 	 	 	=> Green D38~D42
																0x24,	0x25,				//Address 0x02 bits		[5][2] 		/ 0x03 bits[5][2][0]	  => Green D43~D47
																0x24,	0x21,				//Address 0x04 bits		[5][2] 		/ 0x05 bits[5][0] 	 	 	=> Green D48~D51	
																0x25,	0x21,				//Address 0x06 bits	[0]	 [5][2] / 0x07 bits[5]	 [0] 		=> Green D52~D56
																0x24,	0x29,				//Address 0x08 bits		[5][2] 		/ 0x09 bits[5][3][0] 	  => Green D57~D61
																0x24,	0x09,				//Address 0x0A bits		[5][2] 		/ 0x0B bits[3][0] 			=> Green D62~D65			
																0x25,	0x09,				//Address 0x0C bits[0]	 [5][2] / 0x0D bits		[3][0] 	 	=> Green D66~D70			
																0x24,	0x49,				//Address 0x08 bits		[5][2] 		/ 0x09 bits[6][3][0] 	  => Green D71~D75
															};															
															
const uint8_t tabLED_Type3CTLCathodeBlue[32] = { //Reference SLED1735 Datasheet Type3 Map
																0x24,	0x49,				//Address 0x00 bits   [5][2] 		/ 0x01 bits[6][3][0] 	 	=> Blue	 D1~D5	
																0x20,	0x49,				//Address 0x03 bits   [5]	   		/ 0x03 bits[6][3][0] 	 	=> Blue	 D6~D9
																0x24,	0x49, 			//Address 0x04 bits   [5][2] 		/ 0x05 bits[6][3][0] 	 	=> Blue	 D10~D14
																0x20,	0xC9,				//Address 0x06 bits   [5] 	 		/ 0x07 bits[7][6][3][0] => Blue	 D15~D19
																0x08,	0x49,				//Address 0x08 bits   [3] 	 		/ 0x09 bits[6][3][0] 	 	=> Blue	 D20~D23
																0x28,	0x49,				//Address 0x0A bits   [5][3] 		/ 0x0B bits[6][3][0] 	 	=> Blue	 D24~D28
																0x08,	0xC9,				//Address 0x0C bits      [3] 		/ 0x0D bits[7][6][3][0] => Blue	 D29~D33
																0x48,	0x48,				//Address 0x0E bits[6][3] 			/ 0x0F bits[6][3] 	 		=> Blue	 D34~D37
																0x48,	0x49,				//Address 0x00 bits   [6][3] 		/ 0x01 bits[6][3][0] 	 	=> Blue	 D38~D42	
																0x48,	0xC8,				//Address 0x02 bits   [6][3] 		/ 0x03 bits[6][3][7]	  => Blue	 D43~D47
																0x48,	0x42,				//Address 0x04 bits   [6][3] 		/ 0x05 bits[6][1] 	 		=> Blue	 D48~D51
																0x48,	0x4A,				//Address 0x06 bits   	 [6][3] / 0x07 bits[6][3][1] 		=> Blue	 D52~D56	
																0x48,	0xC2,				//Address 0x08 bits   [6][3] 		/ 0x09 bits[6][7][1]	  => Blue	 D57~D61		
																0x48,	0x12,				//Address 0x0A bits   [6][3] 		/ 0x0B bits[4][1] 			=> Blue	 D62~D65
																0x48,	0x52,				//Address 0x0C bits      [6][3] / 0x0D bits[6][4][1]	 	=> Blue	 D66~D70	
																0x48,	0x92,				//Address 0x08 bits   [6][3] 		/ 0x09 bits[7][4][1]	  => Blue	 D71~D75	
															};			

//Table Index means the number of LED, and the data of table means PWM control ram address															
const uint8_t tabLED_Type3PWMCTLCathodeRed[75] = { //Reference SLED1735 Datasheet Type3 Map
																0x20,	0x23, 0x26, 0x29, 0x2C, 0x33,	0x36, 0x39, 0x3C,	0x41, 0x43, 0x46, 0x49, 0x4C, 0x51,	// Red D1~D15 PWM CTL Mapping address
																0x53,	0x56, 0x59, 0x5C, 0x61, 0x66,	0x69, 0x6C, 0x71,	0x74, 0x76, 0x79, 0x7C, 0x81, 0x84,	// Red D16~D30 PWM CTL Mapping address
																0x86,	0x89, 0x8C, 0x91, 0x94, 0x99,	0x9C, 0x21, 0x24,	0x27, 0x29, 0x2C,	0x31, 0x34, 0x37, // Red D31~D45 PWM CTL Mapping address
																0x39,	0x3C, 0x41, 0x44, 0x47, 0x4C,	0x51, 0x54, 0x57,	0x5A, 0x5C, 0x61, 0x64, 0x67, 0x6A,	// Red D46~D60 PWM CTL Mapping address
																0x6C,	0x71, 0x74, 0x77, 0x7A, 0x81,	0x84, 0x87, 0x8A,	0x8D, 0x91, 0x94, 0x97, 0x9A, 0x9D	// Red D61~D75 PWM CTL Mapping address
															};							

//Table Index means the number of LED, and the data of table means PWM control ram address																	
const uint8_t tabLED_Type3PWMCTLCathodeGreen[75] = { //Reference SLED1735 Datasheet Type3 Map
																0x21, 0x24, 0x27, 0x2A,	0x2D, 0x34, 0x37,	0x3A, 0x3D, 0x40, 0x44, 0x47, 0x4A, 0x4D, 0x52,	// Green D1~D15 PWM CTL Mapping address
																0x54,	0x57, 0x5A, 0x5D, 0x62,	0x67, 0x6A, 0x6D,	0x72, 0x70, 0x77,	0x7A, 0x7D, 0x82, 0x85, // Green D16~D30 PWM CTL Mapping address
																0x87,	0x8A, 0x8D, 0x92, 0x95, 0x9A,	0x9D, 0x22, 0x25,	0x20, 0x2A, 0x2D,	0x32, 0x35, 0x38, // Green D31~D45 PWM CTL Mapping address
																0x3A,	0x3D, 0x42, 0x45, 0x48, 0x4D,	0x52, 0x55, 0x58,	0x50, 0x5D, 0x62, 0x65, 0x68, 0x6B,	// Green D46~D60 PWM CTL Mapping address
																0x6D,	0x72, 0x75, 0x78, 0x7B, 0x82,	0x85, 0x88, 0x8B,	0x80, 0x92, 0x95, 0x98, 0x9B, 0x9E	// Green D61~D75 PWM CTL Mapping address
															};															

//Table Index means the number of LED, and the data of table means PWM control ram address																	
const uint8_t tabLED_Type3PWMCTLCathodeBlue[75] = { //Reference SLED1735 Datasheet Type3 Map
																0x22, 0x25, 0x28,	0x2B, 0x2E, 0x35,	0x38, 0x3B, 0x3E, 0x42, 0x45, 0x48, 0x4B, 0x4E, 0x5F, // Blue D1~D15 PWM CTL Mapping address
																0x55,	0x58, 0x5B, 0x5E, 0x63,	0x68, 0x6B, 0x6E,	0x73, 0x75, 0x78,	0x7B, 0x7E, 0x83, 0x8F,	// Blue D16~D30 PWM CTL Mapping address
																0x88,	0x8B, 0x8E, 0x93, 0x96, 0x9B,	0x9E, 0x23, 0x26,	0x28, 0x2B, 0x2E,	0x33, 0x36, 0x3F, // Blue D31~D45 PWM CTL Mapping address
																0x3B,	0x3E, 0x43, 0x46, 0x49, 0x4E,	0x53, 0x56, 0x59,	0x5B, 0x5E, 0x63, 0x66, 0x69, 0x6F,	// Blue D46~D60 PWM CTL Mapping address
															  0x6E,	0x73, 0x76, 0x79, 0x7C, 0x83,	0x86, 0x89, 0x8C,	0x8E, 0x93, 0x96, 0x99, 0x9C, 0x9F	// Blue D61~D75 PWM CTL Mapping address
															};					
#define UT_DelayNms   delay
#endif
