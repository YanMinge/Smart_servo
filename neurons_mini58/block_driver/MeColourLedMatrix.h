#ifndef __MECOLOURLEDMATRIX_H__
#define __MECOLOURLEDMATRIX_H__
#include "Mini58Series.h"
#include "mygpio.h"

extern volatile uint8_t hwSPI_Tx_Fifo[200];
extern volatile uint8_t bSPI_MasEndFlag;
extern volatile uint32_t	wSPI_NBytes;
extern volatile uint32_t wSPI_Send_Pointer;

#define SDB_PIN               		P5_3
#define COLOUR_MATRIX_CS_PIN	  	P0_0
#define COLOUR_LED_QUANTITY			64
#define ALL_LED						0
#define BREATH_ON					1
#define BREATH_OFF					0

#define __LED_SetShutDownPinLow			digitalWrite(SDB_PIN, 0);
#define __LED_SetShutDownPinHigh		digitalWrite(SDB_PIN, 1);
#define __SPI0_SET_SEL1                 digitalWrite(COLOUR_MATRIX_CS_PIN, 1);
#define __SPI0_CLR_SEL1					digitalWrite(COLOUR_MATRIX_CS_PIN, 0);		
	
void led_colour_matrix_init(void);

/*****************************************************************************
* Function		: single_led_set
* Description	: set single led or all led with the same kind of colour.
* Input			: uint8_t index: 0-64; 0: light all leds with the set colour. 1-64, light the index led with the set colour.
				  uint8_t r_value:set red value.
				  uint8_t g_value: set green value.
				  uint8_t b_value: set blue value. 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void single_led_set(uint8_t index, uint8_t r_value, uint8_t g_value, uint8_t b_value);

void MeColourLEDMatrix_DrawBitMap(uint8_t led_matrix[], uint8_t r_value, uint8_t g_value, uint8_t b_value);

void MeColourLEDMatrix_setBreath(uint8_t breath_set);
	
#endif
