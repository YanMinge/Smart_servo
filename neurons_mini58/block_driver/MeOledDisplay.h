#ifndef __MEOLEDDISPLAY_H__
#define __MEOLEDDISPLAY_H__
#include "Mini58Series.h"

//OLED Mode setting
//0:   Four wire serial mode
//1:   The parallel pattern 8080
#define OLED_MODE 0
#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	

#define OLED_CMD    0  //Write command
#define OLED_DATA   1  //Write data

#define DISPLAY  0x01
#define CLEAR    0x00

//-----------------OLED Port definitions----------------  					   
#define OLED_CS_Clr()  digitalWrite(P0_4,0) //CS
#define OLED_CS_Set()  digitalWrite(P0_4,1)

#define OLED_RST_Clr() digitalWrite(P0_1,0) //RES
#define OLED_RST_Set() digitalWrite(P0_1,1)

#define OLED_DC_Clr()  digitalWrite(P0_0,0) //DC
#define OLED_DC_Set()  digitalWrite(P0_0,1)

#define OLED_SCLK_Clr() digitalWrite(P0_7,0)//CLK
#define OLED_SCLK_Set() digitalWrite(P0_7,1)

#define OLED_SDIN_Clr() digitalWrite(P0_5,0)//DIN
#define OLED_SDIN_Set() digitalWrite(P0_5,1)

#define High_level 1
#define Low_level  0

void OLED_Init(void);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t cmd);
void LCD_Draw_Circle(uint8_t x,uint8_t  y,uint8_t r,uint8_t cmd);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t cmd);
void DrawPoint(uint8_t x,uint8_t y);
void DrawPoint_clear(uint8_t x,uint8_t y);
void OLED_DrawPoint_value(uint8_t x,uint8_t y,uint8_t t);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);  
void OLED_ShowString(char x,uint8_t y,uint8_t *chr); 
void OLED_ShowString_Emoji(uint8_t x,uint8_t y,uint8_t *chr);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Refresh_Gram(void);   
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot); 

static void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Clear(void);  
static void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
//static void OLED_Clear_all(void); 
static void SPI_Init(void);
 
unsigned char OLED_Calculat_X(unsigned char circle_x,unsigned char Length,unsigned char Angle); 
unsigned char OLED_Calculat_Y(unsigned char circle_y,unsigned char Length,unsigned char Angle);
 
void OLED_DrawBMP_Image(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]);
void OLED_DrawBMP_Offset(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[]); 
void OLED_Clear_Display_GRAM(void);   
void Offline_Display_Emoji_String(uint8_t emoji_no,char * string);
void Offline_Display_Two_Emoji_String(uint8_t emoji_no_1,char * string_1,uint8_t emoji_no_2,char * string_2);
void OLED_Other_type_ShowString(char x,uint8_t y,uint8_t *chr);

void Display_Face_Blink_Flash_Clear_20PX(uint8_t face_type_value);
void Display_Face(uint8_t face_type_value);
void boot_animation(uint8_t face_type,uint8_t repeat_tiems);
#endif






