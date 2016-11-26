#ifndef __METFTDISPLAY_H__
#define __METFTDISPLAY_H__

#include "Mini58Series.h"
#include "mygpio.h"

/*****************************************************
IO for LCM.
8bit mode: IM1 set to 0.
*****************************************************/

#define DB   P2

#define	CS0  P3_2
#define	RST  P0_5
#define	RS   P3_3
#define	WR0  P1_7
#define	RD0  P1_6

#define ROW  240			//显示的行、列数
#define COL  240

#define RED    0xF800		 //定义颜色常量 
#define GREEN  0x07E0
#define BLUE   0x001F
#define WHITE  0xFFFF
#define BLACK  0x0000

#define CHAR_FONT_W8_H16

#ifdef  CHAR_FONT_W8_H16	
	#define  FONT_W  8
	#define  FONT_H  16
#endif
#ifdef  CHAR_FONT_W16_H21
	#define  FONT_W  16
	#define  FONT_H  21
#endif

//---------------------------------------------------------------------
void LCD_Init(void);
void WriteComm(unsigned int i);
void WriteData(unsigned int i);
void WriteDispData(unsigned char DataH,unsigned char DataL);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void DispBand(void);
void DispColor(unsigned int color);
void DispFrame(void);
void DispPic(unsigned int *picture);
void DispPicFromSD(unsigned char PicNum);

void DispScaleHor1(void);
void DispScaleVer(void);
void DispScaleVer_Red(void);
void DispScaleVer_Green(void);
void DispScaleVer_Blue(void);
void DispScaleVer_Gray(void);
void DispGrayHor16(void);
void DispGrayHor32(void);
void DispScaleHor2(void);
void DispSnow(void);
void DispBlock(void);

void WriteOneDot(unsigned int color);
unsigned char ToOrd(unsigned char ch); 
void DispOneChar(unsigned char ord,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor);	 // ord:0~95
void DispStr(unsigned char *str,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor);
void DispInt(unsigned int i,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor);

unsigned int ReadData(void);
void DispRegValue(unsigned int RegIndex,unsigned char ParNum);

void Debug(void);

void PutPixel(unsigned int x,unsigned int y,unsigned int color);
void DrawLine(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend,unsigned int color);
void DrawGird(unsigned int color);


#endif
