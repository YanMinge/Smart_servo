#ifndef __MEST7735_H__
#define __MEST7735_H__

#include "mygpio.h"

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        128

#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111                 


#define LCD_SCL        	P0_7
#define LCD_SDA        	P0_5
#define LCD_CS        	P0_4

#define LCD_LED        	P2_2
#define LCD_RS         	P2_3
#define LCD_RST     	P2_4

//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
#define	LCD_SCL_SET  	digitalWrite(LCD_SCL, 1)
#define	LCD_SDA_SET  	digitalWrite(LCD_SDA, 1)   
#define	LCD_CS_SET  	digitalWrite(LCD_CS, 1)  

    
#define	LCD_LED_SET  	digitalWrite(LCD_LED, 1)   
#define	LCD_RS_SET  	digitalWrite(LCD_RS, 1) 
#define	LCD_RST_SET  	digitalWrite(LCD_RST, 1)  
//液晶控制口置0操作语句宏定义
#define	LCD_SCL_CLR  	digitalWrite(LCD_SCL, 0)  
#define	LCD_SDA_CLR  	digitalWrite(LCD_SDA, 0)  
#define	LCD_CS_CLR  	digitalWrite(LCD_CS, 0) 
    
#define	LCD_LED_CLR  	digitalWrite(LCD_LED, 0)
#define	LCD_RS_CLR  	digitalWrite(LCD_RS, 0) 
#define	LCD_RST_CLR  	digitalWrite(LCD_RST, 0) 

void LCD_GPIO_Init(void);
void Lcd_WriteIndex(uint8_t Index);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
void LCD_WriteData_16Bit(uint16_t Data);
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);
void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s);

#endif

