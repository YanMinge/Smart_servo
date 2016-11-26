#include "MeTftDisplay.h"
#include "MeTftFontLib.h"
#include "systime.h"

//----------------------------------------------------------------------
void WriteComm(unsigned int i)
{
	digitalWrite(CS0, 0);
	digitalWrite(RD0, 1);
	digitalWrite(RS, 0);	

	//DBH=i>>8; 
	//WR0=0;  
	//WR0=1; 
		
	GPIO_SET_OUT_DATA(DB, i); 
	digitalWrite(WR0, 0);
	delayMicroseconds(5);
	digitalWrite(WR0, 1);
	delayMicroseconds(5);
	
	digitalWrite(CS0, 1);
}

void WriteData(unsigned int i)
{
	digitalWrite(CS0, 0);
	digitalWrite(RD0, 1);
	digitalWrite(RS, 1); 

	//DBH=i>>8; 
	//WR0=0;  
	//WR0=1; 
		
	GPIO_SET_OUT_DATA(DB, i); 
	digitalWrite(WR0, 0);
	delayMicroseconds(5);
	digitalWrite(WR0, 1);
	delayMicroseconds(5);
	
	digitalWrite(CS0, 1);
}

void WriteDispData(unsigned char DataH,unsigned char DataL)
{
	 //CS0=0;
	 //RD0=1;
	 //RS=1;

//	 DBL=DataH;
//	 WR0=0;
//	 WR0=1; 
//	 
//	 DBL=DataL;
//	 WR0=0;
//	 WR0=1; 
	 

	 //CS0=1;
}

void WriteOneDot(unsigned int color)
{ 
	
	digitalWrite(CS0, 0);
	digitalWrite(RD0, 1);
	digitalWrite(RS, 1); 

	GPIO_SET_OUT_DATA(DB, color>>8); 
	digitalWrite(WR0, 0);
	delayMicroseconds(5);
	digitalWrite(WR0, 1);
	delayMicroseconds(5);

	GPIO_SET_OUT_DATA(DB, color); 
	digitalWrite(WR0, 0);
	delayMicroseconds(5);
	digitalWrite(WR0, 1);
	delayMicroseconds(5);

	digitalWrite(CS0, 1);
}

void LCD_Init(void)
{
	pinMode(CS0, GPIO_MODE_OUTPUT);
	pinMode(RST, GPIO_MODE_OUTPUT);
	pinMode(RS, GPIO_PMD_OUTPUT);
	pinMode(WR0, GPIO_MODE_OUTPUT);
	pinMode(RD0, GPIO_PMD_INPUT);
	DB->PMD = PORT_OUTPUT_MODE;
	//CS0=0;
	
	digitalWrite(RST, 1); 
	delay(1);
	
	digitalWrite(RST, 0); 
	delay(20);

	digitalWrite(RST, 1); 
	delay(130);
/*	
//---------------------------------------------------------------------------------------------------// 
	WriteComm(0x11); 
	delay(120);        //Delay 120ms
//--------------------------------ST7789S Frame rate setting----------------------------------// 
	WriteComm(0xb2); 
	WriteData(0x0c); 
	WriteData(0x0c); 
	WriteData(0x00); 
	WriteData(0x33); 
	WriteData(0x33); 

	WriteComm(0xb7);  //vgh vgl setting
	WriteData(0x35); 
//---------------------------------ST7789S Power setting--------------------------------------//
	WriteComm(0xbb); //vcom setting
	WriteData(0x2f); 

	WriteComm(0xc3); 
	WriteData(0x1a); 

	WriteComm(0xc0); 
	WriteData(0x2c);

	WriteComm(0xc2); 
	WriteData(0x01);

	WriteComm(0xc4); 
	WriteData(0x20); 

	WriteComm(0xc6); 
	WriteData(0x0f);

	WriteComm(0xca); 
	WriteData(0x0f); 

	WriteComm(0xc8); 
	WriteData(0x08); 

	WriteComm(0x55); 
	WriteData(0x90); 

	WriteComm(0xd0); 
	WriteData(0xa4); 
	WriteData(0xa1);
//--------------------------------ST7789S gamma setting---------------------------------------//
	WriteComm(0xe0);
	WriteData(0xf0); 
	WriteData(0x00); 
	WriteData(0x0a); 
	WriteData(0x10); 
	WriteData(0x12); 
	WriteData(0x1b); 
	WriteData(0x35); 
	WriteData(0x44); 
	WriteData(0x47); 
	WriteData(0x28); 
	WriteData(0x12); 
	WriteData(0x10); 
	WriteData(0x16); 
	WriteData(0x1b);

	WriteComm(0xe1); 
	WriteData(0xf0); 
	WriteData(0x00); 
	WriteData(0x0a); 
	WriteData(0x10); 
	WriteData(0x11); 
	WriteData(0x1a); 
	WriteData(0x35); 
	WriteData(0x34); 
	WriteData(0x4e); 
	WriteData(0x3a); 
	WriteData(0x17); 
	WriteData(0x16); 
	WriteData(0x21); 
	WriteData(0x22); 

	WriteComm(0x36);
	WriteData(0x00); 

	WriteComm(0x3a);
	WriteData(0x55); 
 
	WriteComm(0x21);
	WriteComm(0x29);
	delay(50);*/
//---------------------------------------------------------------------------------------------------//
/*	WriteComm(0x11);
	delay(120); //Delay 120ms
//--------------------------------------Display Setting------------------------------------------//
	WriteComm(0x36);
	WriteData(0x00);
	WriteComm(0x3a);
	WriteData(0x06);
	WriteComm(0x21);
	WriteComm(0x2a);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xef);
	WriteComm(0x2b);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xef);
//------------------------------------Frame rate setting-------------------------------------//
	WriteComm(0xb2);
	WriteData(0x0c);
	WriteData(0x0c);
	WriteData(0x00);
	WriteData(0x33);
	WriteData(0x33);
	WriteComm(0xb7);
	WriteData(0x35);
//----------------------------------------Power setting-----------------------------------------//
	WriteComm(0xbb);
	WriteData(0x1f);
	WriteComm(0xc0);
	WriteData(0x2c);
	WriteComm(0xc2);
	WriteData(0x01);
	WriteComm(0xc3);
	WriteData(0x12);
	WriteComm(0xc4);
	WriteData(0x20);
	WriteComm(0xc6);
	WriteData(0x0f);
	WriteComm(0xd0);
	WriteData(0xa4);
	WriteData(0xa1);
//------------------------------------------gamma setting----------------------------------------//
	WriteComm(0xe0);
	WriteData(0xd0);
	WriteData(0x08);
	WriteData(0x11);
	WriteData(0x08);
	WriteData(0x0c);
	WriteData(0x15);
	WriteData(0x39);
	WriteData(0x33);
	WriteData(0x50);
	WriteData(0x36);
	WriteData(0x13);
	WriteData(0x14);
	WriteData(0x29);
	WriteData(0x2d);
	WriteComm(0xe1);
	WriteData(0xd0);
	WriteData(0x08);
	WriteData(0x10);
	WriteData(0x08);
	WriteData(0x06);
	WriteData(0x06);
	WriteData(0x39);
	WriteData(0x44);
	WriteData(0x51);
	WriteData(0x0b);
	WriteData(0x16);
	WriteData(0x14);
	WriteData(0x2f);
	WriteData(0x31);
	WriteComm(0x29);
//----------------------------------------2 data lane enable--------------------------------------//
	WriteComm(0xe7);
	WriteData(0x10);*/
   
//---------------------------------------------------------------------------------------------------//
	WriteComm(0x11);
	delay(120); //delay 120ms
//--------------------------------------Display Setting------------------------------------------//
	WriteComm(0x36);
	WriteData(0x00);
	WriteComm(0x3a);
	WriteData(0x05);
	WriteComm(0x21);
	WriteComm(0x2a);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xef);
	WriteComm(0x2b);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0x00);
	WriteData(0xef);
//--------------------------------ST7789V Frame rate setting----------------------------------//
	WriteComm(0xb2);
	WriteData(0x0c);
	WriteData(0x0c);
	WriteData(0x00);
	WriteData(0x33);
	WriteData(0x33);
	WriteComm(0xb7);
	WriteData(0x35);
//---------------------------------ST7789V Power setting--------------------------------------//
	WriteComm(0xbb);
	WriteData(0x1f);
	WriteComm(0xc0);
	WriteData(0x2c);
	WriteComm(0xc2);
	WriteData(0x01);
	WriteComm(0xc3);
	WriteData(0x12);
	WriteComm(0xc4);
	WriteData(0x20);
	WriteComm(0xc6);
	WriteData(0x0f);
	WriteComm(0xd0);
	WriteData(0xa4);
	WriteData(0xa1);
//--------------------------------ST7789V gamma setting--------------------------------------//
	WriteComm(0xe0);
	WriteData(0xd0);
	WriteData(0x08);
	WriteData(0x11);
	WriteData(0x08);
	WriteData(0x0c);
	WriteData(0x15);
	WriteData(0x39);
	WriteData(0x33);
	WriteData(0x50);
	WriteData(0x36);
	WriteData(0x13);
	WriteData(0x14);
	WriteData(0x29);
	WriteData(0x2d);
	WriteComm(0xe1);
	WriteData(0xd0);
	WriteData(0x08);
	WriteData(0x10);
	WriteData(0x08);
	WriteData(0x06);
	WriteData(0x06);
	WriteData(0x39);
	WriteData(0x44);
	WriteData(0x51);
	WriteData(0x0b);
	WriteData(0x16);
	WriteData(0x14);
	WriteData(0x2f);
	WriteData(0x31);
	WriteComm(0x29);

}

void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{
	//ILI9327

	WriteComm(0x2a);   
	WriteData(Xstart>>8);
	WriteData(Xstart&0xff);
	WriteData(Xend>>8);
	WriteData(Xend&0xff);

	WriteComm(0x2b);   
	WriteData(Ystart>>8);
	WriteData(Ystart&0xff);
	WriteData(Yend>>8);
	WriteData(Yend&0xff);

	WriteComm(0x2c);  //Display ON?
}

void DispColor(unsigned int color)
{
	unsigned int i,j;

	BlockWrite(0,COL-1,0,ROW-1);

	digitalWrite(CS0, 0);
	digitalWrite(RD0, 1);
	digitalWrite(RS, 1); 

	for(i=0;i<ROW;i++)
	{
	    for(j=0;j<COL;j++)
		{    
			GPIO_SET_OUT_DATA(DB, color>>8); 
			digitalWrite(WR0, 0);
			delayMicroseconds(5);
			digitalWrite(WR0, 1);
			delayMicroseconds(5);
	
			GPIO_SET_OUT_DATA(DB, color); 
			digitalWrite(WR0, 0);
			delayMicroseconds(5);
			digitalWrite(WR0, 1);
			delayMicroseconds(5);
		}
	}
	
	digitalWrite(CS0, 1);
}

void DispBand(void)	 
{
	unsigned int i,j,k;
	//unsigned int color[8]={0x001f,0x07e0,0xf800,0x07ff,0xf81f,0xffe0,0x0000,0xffff};
	unsigned int color[8]={0xf800,0xf800,0x07e0,0x07e0,0x001f,0x001f,0xffff,0xffff};//0x94B2
	//unsigned int gray16[]={0x0000,0x1082,0x2104,0x3186,0x42,0x08,0x528a,0x630c,0x738e,0x7bcf,0x9492,0xa514,0xb596,0xc618,0xd69a,0xe71c,0xffff};

   	BlockWrite(0,COL-1,0,ROW-1);
    	
	digitalWrite(CS0, 0);
	digitalWrite(RD0, 1);
	digitalWrite(RS, 1); 																								  

	for(i=0;i<8;i++)
	{
		for(j=0;j<ROW/8;j++)
		{
	        for(k=0;k<COL;k++)
			{
				GPIO_SET_OUT_DATA(DB, color[i]>>8); 
				digitalWrite(WR0, 0);
				delayMicroseconds(5);
				digitalWrite(WR0, 1);
				delayMicroseconds(5);
	
				GPIO_SET_OUT_DATA(DB, color[i]); 
				digitalWrite(WR0, 0);
				delayMicroseconds(5);
				digitalWrite(WR0, 1);
				delayMicroseconds(5);
			} 
		}
	}
	
	digitalWrite(CS0, 1);
}


void DispFrame(void)
{
	unsigned int i,j;
	
	BlockWrite(0,COL-1,0,ROW-1);

	
	for(i=0;i<ROW;i++)
	{ 
	  for(j=0;j<COL;j++)
	 
	  if(i==0||i==(ROW-1)||j==0||j==(COL-1))
	  {WriteData(0xff);WriteData(0xff);}
	  else
	 {WriteData(0x00);WriteData(0x00);}
	}
}

#ifdef EVAL_PIC
void DispPic(unsigned int code *picture)
{
    unsigned int *p;
	unsigned int  i,j; //i-row,j-col
	unsigned char n,k; //n-row repeat count,k-col repeat count

	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RS =1;
	RD0=1;

	for(n=0;n<ROW/PIC_HEIGHT;n++)         //n-row repeat count
	{
		for(i=0;i<PIC_HEIGHT;i++)
		{
			p=picture;
			for(k=0;k<COL/PIC_WIDTH;k++) //k-col repeat count
		    {
				for(j=0;j<PIC_WIDTH;j++)
		    	{
				    DBH=(*(p+i*PIC_HEIGHT+j))>>8; 
					WR0=0;
					WR0=1; 
		
					DBH=*(p+i*PIC_HEIGHT+j);  
					WR0=0;
					WR0=1;
				}

		  	}

			p=picture;
			for(j=0;j<COL%PIC_WIDTH;j++)
		    {
				    DBH=(*(p+i*PIC_HEIGHT+j))>>8; 
					WR0=0;
					WR0=1; 
		
					DBH=*(p+i*PIC_HEIGHT+j);  
					WR0=0;
					WR0=1;
		  	}
		}
	}

	for(i=0;i<ROW%PIC_HEIGHT;i++)
	{
		p=picture;
		for(k=0;k<COL/PIC_WIDTH;k++) //k-col repeat count
	    {
			for(j=0;j<PIC_WIDTH;j++)
	    	{
			    DBH=(*(p+i*PIC_HEIGHT+j))>>8; 
				WR0=0;
				WR0=1; 
	
				DBH=*(p+i*PIC_HEIGHT+j);  
				WR0=0;
				WR0=1;
			}

	  	}

		p=picture;
		for(j=0;j<COL%PIC_WIDTH;j++)
	    {
			    DBH=(*(p+i*PIC_HEIGHT+j))>>8; 
				WR0=0;
				WR0=1; 
	
				DBH=*(p+i*PIC_HEIGHT+j);  
				WR0=0;
				WR0=1;
	  	}
	}

	CS0=1;
}

#endif

#ifdef SD_PIC_DEMO
void DispPicFromSD(unsigned char PicNum)
{
	unsigned long page_s,page_e;
	unsigned int  i;
	unsigned long page_num;

	SD_Start();

	CLKSEL = 0x03;

    BlockWrite(0,COL-1,0,ROW-1);

 	CS0=0;
	//RD0=1;
 	RS=1;

	page_num=(unsigned long)ROW*(unsigned long)COL*2/512;	   
	page_s = PicNum*page_num+1;   	  		
	page_e = (PicNum+1)*page_num+1;
    for(;page_s<page_e;page_s++)
    {  
	    MMC_SD_ReadSingleBlock(page_s);

        for(i=0;i<512;i+=2)
        {	 
            WriteDispData(buffer[i],buffer[i+1]);
        } 

    }  

	CS0=1;
	CLKSEL = 0x00;
} 
#endif


#ifdef SD_PIC_TEST_MODE
unsigned int GetPageNum(unsigned int row,unsigned int col)
{
	unsigned long page_num;

	page_num=(unsigned long)row*(unsigned long)col*2/512;

	return page_num;

}

void DispPicFromSD(unsigned char PicNum)
{
	unsigned long page_s,page_e;
	unsigned long page_offset=0;
	unsigned long page_num;
	unsigned int  i;
	
 	//128128-64p		   
	//128160-80p
	//176220-151.25p
	//240320-300p
	//240400-375p
	//272480-510p

	#ifdef QQVGA1
		page_offset=0;	
	#endif
	#ifdef QQVGA2
		page_offset=GetPageNum(128,128);
	#endif
	#ifdef QCIF
		page_offset=GetPageNum(128,128)+GetPageNum(128,160);	
	#endif
	#ifdef QVGA
		page_offset=GetPageNum(128,128)+GetPageNum(128,160)+GetPageNum(220*4,176);
	#endif
	#ifdef WQVGA
		page_offset=GetPageNum(128,128)+GetPageNum(128,160)+GetPageNum(220*4,176)+GetPageNum(320,240);
	#endif
	#ifdef HVGA1
		page_offset=GetPageNum(128,128)+GetPageNum(128,160)+GetPageNum(220*4,176)+GetPageNum(320,240)+GetPageNum(400,240);
	#endif
	#ifdef HVGA2
		page_offset=GetPageNum(128,128)+GetPageNum(128,160)+GetPageNum(220*4,176)+GetPageNum(320,240)+GetPageNum(400,240)+GetPageNum(272,480);	
	#endif
	#ifdef WVGA
		page_offset=GetPageNum(128,128)+GetPageNum(128,160)+GetPageNum(220*4,176)+GetPageNum(320,240)+GetPageNum(400,240)+GetPageNum(272,480)+GetPageNum(480,320);	
	#endif

	SD_Start();

	CLKSEL = 0x03;

    BlockWrite(0,COL-1,0,ROW-1);

 	CS0=0;
	//RD0=1;
 	RS=1;

	page_num=(unsigned long)ROW*(unsigned long)COL*2/512;	   
	page_s = PicNum*page_num+1;   	  		
	page_e = (PicNum+1)*page_num+1;

    for(;page_s<page_e;page_s++)
    {  
	    MMC_SD_ReadSingleBlock(page_s+page_offset);

        for(i=0;i<512;i+=2)
        {	 
            WriteDispData(buffer[i],buffer[i+1]);
        } 

    }  

	CS0=1;
	CLKSEL = 0x00;
} 

#endif

#ifdef EXTEND_TEST
void DispScaleHor1(void)
{
	unsigned int i,j,k;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;

	// balck -> red	
	for(k=0;k<ROW/4;k++)
	{
		for(i=0;i<COL%32;i++){DBH=0x00;WR0=0;WR0=1;DBH=0x00;WR0=0;WR0=1;}
		for(i=0;i<32;i++)
		{
			for(j=0;j<COL/32;j++)	 
			{
				DBH=i<<3;
				WR0=0;
				WR0=1;

				DBH=0;
				WR0=0;
				WR0=1;
			}
		}
		
	}


	// balck -> green
	for(k=0;k<ROW/4;k++)
	{
		for(i=0;i<COL%32;i++){DBH=0x00;WR0=0;WR0=1;DBH=0x00;WR0=0;WR0=1;}
		for(i=0;i<32;i++)
		{
			for(j=0;j<COL/32;j++)
			{
				DBH=(i*2)>>3;
				WR0=0;
				WR0=1;

				DBH=(i*2)<<5;
				WR0=0;
				WR0=1;
			}
		}
	}

	// balck -> blue
	for(k=0;k<ROW/4;k++)
	{
		for(i=0;i<COL%32;i++){DBH=0x00;WR0=0;WR0=1;DBH=0x00;WR0=0;WR0=1;}
		for(i=0;i<32;i++)
		{
			for(j=0;j<COL/32;j++)
			{
				DBH=0;
				WR0=0;
				WR0=1;

				DBH=i;
				WR0=0;
				WR0=1;
			}
		}
	}

	// black -> white
	for(k=0;k<(ROW/4+ROW%4);k++)
	{
		for(i=0;i<COL%32;i++){DBH=0x00;WR0=0;WR0=1;DBH=0x00;WR0=0;WR0=1;}
		for(i=0;i<32;i++)
		{
			for(j=0;j<COL/32;j++)
			{
				DBH=(i<<3)|((i*2)>>3);
				WR0=0;
				WR0=1;

				DBH=((i*2)<<5)|i;
				WR0=0;
				WR0=1;
			}
		}
	}

	CS0=1;
}

void DispScaleVer(void)
{
	unsigned int i,j,k;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;

	// BLACK
	for(k=0;k<ROW%32;k++)
	{
		for(j=0;j<COL;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;
		}			
	}

	for(k=0;k<32;k++)
	{	
	
		for(i=0;i<ROW/32;i++)
		{	
			// RED
			for(j=0;j<COL/4;j++)
			{
				DBH=k<<3;
				WR0=0;
				WR0=1;

				DBH=0;
				WR0=0;
				WR0=1;
			}
			// GREEN
			for(j=0;j<COL/4;j++)
			{
				DBH=(k*2)>>3;
				WR0=0;
				WR0=1;

				DBH=(k*2)<<5;
				WR0=0;
				WR0=1;
			}
			// BLUE
			for(j=0;j<COL/4;j++)
			{
				DBH=0;
				WR0=0;
				WR0=1;

				DBH=k;
				WR0=0;
				WR0=1;
			}
			// GRAY										  
			for(j=0;j<COL/4;j++)
			{
				DBH=(k<<3)|((k*2)>>3);
				WR0=0;
				WR0=1;

				DBL=((k*2)<<5)|k;
				WR0=0;
				WR0=1;
			}

		}
	
	}

	CS0=1;
}

void DispScaleVer_Red(void)
{
	unsigned int i,j,k;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;

	// BLACK
	for(k=0;k<ROW%32;k++)
	{
		for(j=0;j<COL;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;

		}			
	}
	
	for(k=0;k<32;k++)
	{	
		for(i=0;i<ROW/32;i++)
		{	
			// RED
			for(j=0;j<COL;j++)
			{
				DBH=k<<3;
				WR0=0;
				WR0=1;

				DBH=0;
				WR0=0;
				WR0=1;
			}
		}
	
	}

	CS0=1;
}

void DispScaleVer_Green(void)
{
	unsigned int i,j,k;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;

	// BLACK
	for(k=0;k<ROW%64;k++)
	{
		for(j=0;j<COL;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;

		}			
	}
	
	for(k=0;k<64;k++)
	{	
		for(i=0;i<ROW/64;i++)
		{	
			// GREEN
			for(j=0;j<COL;j++)
			{
				DBH=k>>3;
				WR0=0;
				WR0=1;

				DBH=k<<5;
				WR0=0;
				WR0=1;
			}
		}
	
	}

	CS0=1;
}

void DispScaleVer_Blue(void)
{
	unsigned int i,j,k;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;

	// BLACK
	for(k=0;k<ROW%32;k++)
	{
		for(j=0;j<COL;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;

		}			
	}
	
	for(k=0;k<32;k++)
	{	
		for(i=0;i<ROW/32;i++)
		{	
			// BLUE
			for(j=0;j<COL;j++)
			{
				DBH=0;
				WR0=0;
				WR0=1;

				DBH=k;
				WR0=0;
				WR0=1;
			}
		}
	
	}

	CS0=1;
}

void DispScaleVer_Gray(void)
{
	unsigned int i,j,k;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;

	// BLACK
	for(k=0;k<ROW%32;k++)
	{
		for(j=0;j<COL;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;

		}			
	}
	
	for(k=0;k<32;k++)
	{	
		for(i=0;i<ROW/32;i++)
		{	
			// GRAY										  
			for(j=0;j<COL;j++)
			{
				DBH=(k<<3)|((k*2)>>3);
				WR0=0;
				WR0=1;

				DBH=((k*2)<<5)|k;
				WR0=0;
				WR0=1;
			}
		}
	
	}

	CS0=1;
}

void DispGrayHor16(void)	 
{
	unsigned int i,j,k;

   	BlockWrite(0,COL-1,0,ROW-1);
    	
	CS0=0;
	RD0=1;
	RS=1;																									  

	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COL%16;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;
		}
		
		for(j=0;j<16;j++)
		{
	        for(k=0;k<COL/16;k++)
			{		
				DBH=((j*2)<<3)|((j*4)>>3);	
				WR0=0;
				WR0=1;
					 
				DBH=((j*4)<<5)|(j*2);
				WR0=0;
				WR0=1;
			} 
		}
	}
	
	CS0=1;
}

void DispGrayHor32(void)	 
{
	unsigned int i,j,k;

   	BlockWrite(0,COL-1,0,ROW-1);
    	
	CS0=0;
	RD0=1;
	RS=1;																									  

	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COL%32;j++)
		{
			DBH=0;
			WR0=0;
			WR0=1;

			DBH=0;
			WR0=0;
			WR0=1;
		}
		
		for(j=0;j<32;j++)
		{
	        for(k=0;k<COL/32;k++)
			{		
				DBH=(j<<3)|((j*2)>>3);	
				WR0=0;
				WR0=1;	 

				DBH=((j*2)<<5)|j;
				WR0=0;
				WR0=1;
			} 
		}
	}
	
	CS0=1;
}

void DispScaleHor2(void)
{
	unsigned int i,j;
	
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
	RD0=1;
	RS=1;
for(j=0;j<160;j++)
	{	for(i=0;i<15;i++)
		{ DispColor(0x0000); }	//1		

		for(i=15;i<30;i++)
		{ DispColor(0x1082); }	//2		

		for(i=30;i<45;i++)
		{ DispColor(0x2104); }	//3	

		for(i=45;i<60;i++)
		{ DispColor(0x3186); }	//4		

		for(i=60;i<75;i++)
		{ DispColor(0x4208); }  	//5		

		for(i=75;i<90;i++)
		{ DispColor(0x528a); }	//6	

		for(i=90;i<105;i++)
		{ DispColor(0x630c); }	//7	

		for(i=105;i<120;i++)
		{ DispColor(0x738e); }	//8	

		for(i=120;i<135;i++)
		{ DispColor(0x7bcf); }	//9	

		for(i=135;i<150;i++)
		{ DispColor(0x9492); }	//10		

		for(i=150;i<165;i++)
		{ DispColor(0xa514); }	//11		

		for(i=165;i<180;i++)
		{ DispColor(0xb596); }	//12	

		for(i=180;i<195;i++)
		{ DispColor(0xc618); }	//13	

		for(i=195;i<210;i++)
		{ DispColor(0xd69a); }	//14	

		for(i=210;i<225;i++)
		{ DispColor(0xe71c); }	//15	

		for(i=225;i<240;i++)        
		{ DispColor(0xffff); }	//16	
	}

	for(j=160;j<320;j++)
	{	for(i=0;i<120;i++)
		{ DispColor(0x0000); }		
		
		for(i=120;i<240;i++)       
		{ DispColor(0xffff); }
	}
   
}

void DispSnow(void)
{
	unsigned int i,j;

	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
   	RS=1;
	RD0=1;

	DBH = 0;
	//DBL = 0;

	for(i=0;i<ROW;i++)
	{
		for(j=0;j<COL/2;j++) 
		{
			WR0 = 0; WR0 = 1;
			WR0 = 0; WR0 = 1;

			DBH = ~DBH;
			WR0 = 0; WR0 = 1;
			WR0 = 0; WR0 = 1;

			DBH = ~DBH;
		} 
        
		DBH = ~DBH;
		//DBL = ~DBL;
	}
    
	CS0 = 1;
}

void DispBlock(void)
{
    unsigned int i,j,k;
    k = ROW /4;
    
	BlockWrite(0,COL-1,0,ROW-1);

	CS0=0;
   	RS=1;
	RD0=1;	

	
	for(i=0;i<k;i++)
	{
        for(j=0;j<COL;j++)
		{
			DBH=GRAY50>>8;
			WR0=0;  
		    WR0=1; 
			
			DBH=GRAY50;
			WR0=0;  
		    WR0=1;  
		}
	}

	for(i=0;i<k*2;i++)
	{
		
		for(j=0;j<COL/4;j++)
		{
			DBH=GRAY50>>8;
			WR0=0;  
		    WR0=1;  

			DBH=GRAY50;
			WR0=0;  
		    WR0=1;  
		}

		
		for(j=0;j<COL/2;j++)
		{	
			DBH=BLACK>>8;
			WR0=0;  
		    WR0=1; 

			DBH=BLACK; 
			WR0=0;  
		    WR0=1;  
		}

		
		for(j=0;j<COL/4;j++)
		{
			DBH=GRAY50>>8;
			WR0=0;  
		    WR0=1; 

			DBH=GRAY50; 
			WR0=0;  
		    WR0=1;  
		}
	}

	
	for(i=0;i<k;i++)
	{
        for(j=0;j<COL;j++)
		{
			DBH=GRAY50>>8;
			WR0=0;  
		    WR0=1; 

			DBH=GRAY50;
			WR0=0;  
		    WR0=1;  
		}
	}

	CS0=1;
}

#endif

//ascii 32~90(!~Z), (32~47)¿Õ¸ñ~/,(48~57)0~9,(58~64):~@,(65~126)A~~
//ord 0~95, (48~57)0~9,(65~126)A~z,(33~47)!~/,(58~64):~@
unsigned char ToOrd(unsigned char ch)
{
	if(ch<32)
	{
		ch=95;
	}
	else if((ch>=32)&&(ch<=47)) //(32~47)¿Õ¸ñ~/
	{
		ch=(ch-32)+10+62;
	}
	else if((ch>=48)&&(ch<=57))//(48~57)0~9
	{
		ch=ch-48;
	}
	else if((ch>=58)&&(ch<=64))//(58~64):~@
	{
		ch=(ch-58)+10+62+16;
	}
	else if((ch>=65)&&(ch<=126))//(65~126)A~~
	{
		ch=(ch-65)+10;
	}
	else if(ch>126)
	{		
		ch=95;
	}

	return ch;
}

void  DispOneChar(unsigned char ord,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor)	 // ord:0~95
{													  
   unsigned char i,j;
   unsigned char  *p;
   unsigned char dat;
   unsigned int index;

   BlockWrite(Xstart,Xstart+(FONT_W-1),Ystart,Ystart+(FONT_H-1));

   index = ord;

   if(index>95)	   //95:ASCII CHAR NUM
   		index=95;

   index = index*((FONT_W/8)*FONT_H);	 

   p = ascii;
   p = p+index;

   for(i=0;i<(FONT_W/8*FONT_H);i++)
    {
       dat=*p++;
       for(j=0;j<8;j++)
        {
           if((dat<<j)&0x80)
             {
                WriteOneDot(TextColor);
             }      
           else 
             {
                WriteOneDot(BackColor);	  
             }
         }
     }
}

void DispStr(unsigned char *str,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor)
{

	while(!(*str=='\0'))
	{
		DispOneChar(ToOrd(*str++),Xstart,Ystart,TextColor,BackColor);

		if(Xstart>((COL-1)-FONT_W))
		{
			Xstart=0;
		    Ystart=Ystart+FONT_H;
		}
		else
		{
			Xstart=Xstart+FONT_W;
		}

		if(Ystart>((ROW-1)-FONT_H))
		{
			Ystart=0;
		}
	}	
	BlockWrite(0,COL-1,0,ROW-1);
}

void DispInt(unsigned int i,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor)
{
	if(Xstart>((COL-1)-FONT_W*4))
	{
		Xstart=(COL-1)-FONT_W*4;
	}
	if(Ystart>((ROW-1)-FONT_H))
	{
		Ystart=(Ystart-1)-FONT_H;
	}
			
	DispOneChar((i>>12)%16,Xstart,Ystart,TextColor,BackColor); //ID value
	DispOneChar((i>>8)%16,Xstart+FONT_W,Ystart,TextColor,BackColor);
	DispOneChar((i>>4)%16,Xstart+FONT_W*2,Ystart,TextColor,BackColor);
	DispOneChar(i%16,Xstart+FONT_W*3,Ystart,TextColor,BackColor); 

	BlockWrite(0,COL-1,0,ROW-1);
}


#ifdef READ_FUNCTION
unsigned int ReadData(void)
{
	unsigned char dataH;
	//unsigned char dataL;

	////////////////
	P2MDIN |= 0xFF;
	P2MDOUT |= 0x00;
	
	P4MDIN |= 0xFF;
	P4MDOUT |= 0x00;
		
	DBH=0xFF;
	DBL=0xFF;	 

	CS0 = 0;
	RS  = 1;        
	WR0 = 1;

	RD0 = 0;				 
	Delay(800);
	dataH=DBH;
	//dataL=DBL;
	RD0 = 1; 

 	CS0=1;

	////////////////
	P2MDIN |= 0xFF;
	P2MDOUT |= 0xFF;
	
	P4MDIN |= 0xFF;
	P4MDOUT |= 0xFF;

	//return (dataH<<8)|dataL;
	return (dataH);
}
#endif

#ifdef READ_REG
void DispRegValue(unsigned int RegIndex,unsigned char ParNum)
{
	unsigned char i;
	xdata unsigned int reg_data[20];	

	WriteComm(RegIndex);	 

	if(ParNum>20)
		ParNum=20;

	for(i=0;i<ParNum;i++)
	{
		reg_data[i]=ReadData();
	}


	//READ REG
	DispStr("READ ID ",0,0,BLACK,WHITE);
	//DispStr("READ REG:0X",0,0,BLACK,WHITE);

	DispInt(RegIndex,0,(FONT_H+1),GREEN,BLACK);
    for(i=0;i<ParNum;i++)
	{
		//DispStr("0X",0,(FONT_H+1)*(i+1),BLUE,WHITE);
		//DispInt(reg_data[i],FONT_W*2,(FONT_H+1)*(i+1),BLUE,WHITE);

		DispInt(reg_data[i],0,(FONT_H+1)*(i+2),BLUE,WHITE);

	}
}

#endif

#ifdef SLEEP_TEST
void EnterSLP(void)
{
	//FOR OTM3225A

	WriteComm(0x0007);WriteData(0x0131); // Set D1=0, D0=1
    Delay(10);
    WriteComm(0x0007);WriteData(0x0130); // Set D1=0, D0=0
    Delay(10);
	WriteComm(0x0007);WriteData(0x0000); // display OFF
	//************* Power OFF sequence **************//
	WriteComm(0x0010);WriteData(0x0080); // SAP, BT[3:0], APE, AP, DSTB, SLP
	WriteComm(0x0011);WriteData(0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
	WriteComm(0x0012);WriteData(0x0000); // VREG1OUT voltage
	WriteComm(0x0013);WriteData(0x0000); // VDV[4:0] for VCOM amplitude
	Delay(200);                          // Dis-charge capacitor power voltage
	WriteComm(0x0010);WriteData(0x0082); // SAP, BT[3:0], APE, AP, DSTB, SLP
}

void ExitSLP(void)
{
	//FOR OTM3225A

	//Power On sequence
	WriteComm(0x0010);WriteData(0x0080); // SAP, BT[3:0], AP, DSTB, SLP
	WriteComm(0x0011);WriteData(0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
	WriteComm(0x0012);WriteData(0x0000); // VREG1OUT voltage
	WriteComm(0x0013);WriteData(0x0000); // VDV[4:0] for VCOM amplitude
	Delay(200);                          // Dis-charge capacitor power voltage
	WriteComm(0x0010);WriteData(0x1490); // SAP, BT[3:0], AP, DSTB, SLP, STB
	WriteComm(0x0011);WriteData(0x0227); // R11h=0x0221 at VCI=3.3V DC1[2:0], DC0[2:0], VC[2:0]
	Delay(50);                           // Delay 50ms
	WriteComm(0x0012);WriteData(0x009B); // External reference voltage =Vci;
	Delay(50);                           // Delay 50ms
	WriteComm(0x0013);WriteData(0x1800); // R13h=0x1200 when R12=009D VDV[4:0] for VCOM amplitude
	WriteComm(0x0029);WriteData(0x0027); // R29h=0x000C when R12=009D VCM[5:0] for VCOMH
	Delay(50);                           // Delay 50ms
	WriteComm(0x0007);WriteData(0x0133); // 262K color and display ON
}

#endif
#ifdef DEBUG
void Pause(void)
{  
	while(KEY_STEP);
	Delay(500);
	while(!KEY_STEP);

}

void DispDebugCode(void)
{
	unsigned int i,j;

	i=0;j=0;
	while(i<DEBUG_X)
	{		
		j=0;
		while((j<DEBUG_Y)&&(debug_code[i][j]!=END))
		{
			if(j==0)
			{
				WriteComm(debug_code[i][j]);
			}
			else
			{
				WriteData(debug_code[i][j]);
			}
			j++;		
		} 

		j=0;
		while((j<DEBUG_Y)&&(debug_code[i][j]!=END))
		{
			if(j==0)
			{	
				//WriteComm(debug_code[i][j]);
				DispStr("0X",0,(FONT_H+1)*(j+1),RED,WHITE);
				DispInt(debug_code[i][j],FONT_W*2,(FONT_H+1)*(j+1),RED,WHITE);							
			}
			else
			{
				//WriteData(debug_code[i][j]);
				DispStr("0X",0,(FONT_H+1)*(j+1),BLUE,WHITE);
				DispInt(debug_code[i][j],FONT_W*2,(FONT_H+1)*(j+1),BLUE,WHITE);
			}
			j++;		
		}
		i++;

		Pause(); Delay(800);

		if((j==0)&&(debug_code[i][j]!=END))
			break;			
	}
}


void Debug(void)
{
	DispStr("DEBUG CODE",0,0,BLACK,WHITE);

	while(1)
	{
		DispDebugCode();
	}
}

#endif


#ifdef DRAW_FUNCTION
void PutPixel(unsigned int x,unsigned int y,unsigned int color)
{
	unsigned char i;

	#if 0
	BlockWrite(x,x+1,y,y+1);

	CS0=0;
	RD0=1;
	RS=1;
	
	DBH=color>>8;
	WR0=0;  
	WR0=1; 

	DBH=color;  
	WR0=0;  
	WR0=1; 

	DBH=color>>8;
	WR0=0;  
	WR0=1; 

	DBH=color;  
	WR0=0;  
	WR0=1; 

	DBH=color>>8;
	WR0=0;  
	WR0=1; 

	DBH=color;  
	WR0=0;  
	WR0=1; 

	DBH=color>>8;
	WR0=0;  
	WR0=1; 

	DBH=color;  
	WR0=0;  
	WR0=1; 
  
	CS0=1; 	

	#endif

	#if 1
//	unsigned char i;

 	for (i=0;i<3;i++)
 	{
       BlockWrite(x,COL-1,y+i,ROW-1);
	   WriteOneDot(color);
	   WriteOneDot(color);
    } 

	#endif 
}

void DrawLine(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend,unsigned int color)
{
	unsigned int i,j;

	BlockWrite(Xstart,Xend,Ystart,Yend);

	for(i=Ystart;i<Yend+1;i++)
	{
		for(j=Xstart;j<Xend+1;j++)
		{
			WriteOneDot(color);
		}
	}
}

void DrawGird(unsigned int color)
{
	unsigned int i;

	for(i=15;i<ROW-1;i=i+16)
	{
		DrawLine(0,COL-1,i,i,color);

	}

	for(i=15;i<COL-1;i=i+16)
	{
		DrawLine(i,i,0,ROW-1,color);
	}

}
#endif

//ARRAY DIFINE
//-------------------------------------------------------------------------
#ifdef DEBUG
unsigned int code debug_code[50][15]=
{
//ILI9481
{0xd1,0x00,0x1d,0x0c,END},
{0xd1,0x00,0x1d,0x02,END},
{0xd1,0x00,0x1d,0x03,END},
{0xd1,0x00,0x1d,0x04,END},
{0xd1,0x00,0x1d,0x05,END},
{0xd1,0x00,0x1d,0x06,END},
{0xd1,0x00,0x1d,0x07,END},
{0xd1,0x00,0x1d,0x08,END},
{0xd1,0x00,0x1d,0x09,END},
{0xd1,0x00,0x1d,0x0a,END},
{0xd1,0x00,0x1d,0x0b,END},
{0xd1,0x00,0x1d,0x0c,END},

{END},//end
};
#endif
