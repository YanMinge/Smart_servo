#include "MeOledDisplay.h"
#include <math.h>
#include "mygpio.h"
#include "systime.h"
#include "oledfont.h"
#include "uart_printf.h"
#include "bmp.h"
#include "face.h"

#define  SPI_RST  P0_1
#define  SPI_DC   P0_0
#define  SPI_CS   P0_4
#define  SPI_IN   P0_5
#define  SPI_CLK  P0_7

float PAI=314;

void Offline_Display_Emoji_String(uint8_t emoji_no,char * string)
{
    OLED_Clear_Display_GRAM();  
    
    switch(emoji_no)
    {
        case 1: OLED_DrawBMP(8,4,(8+24),(4+3),icon_1);break;
        case 2: OLED_DrawBMP(8,4,(8+24),(4+3),icon_2);break;
        case 3: OLED_DrawBMP(8,4,(8+24),(4+3),icon_3);break;
        case 4: OLED_DrawBMP(8,4,(8+24),(4+3),icon_4);break;
        case 5: OLED_DrawBMP(8,4,(8+24),(4+3),icon_5);break;
        case 6: OLED_DrawBMP(8,4,(8+24),(4+3),icon_6);break;
        case 7: OLED_DrawBMP(8,4,(8+24),(4+3),icon_7);break;
        case 8: OLED_DrawBMP(8,4,(8+24),(4+3),icon_8);break;
        case 9: OLED_DrawBMP(8,4,(8+24),(4+3),icon_9);break;
        case 10: OLED_DrawBMP(8,4,(8+24),(4+3),icon_10);break;
        case 11: OLED_DrawBMP(8,4,(8+24),(4+3),icon_11);break;
        case 12: OLED_DrawBMP(8,4,(8+24),(4+3),icon_12);break;
        case 13: OLED_DrawBMP(8,4,(8+24),(4+3),icon_13);break;
        case 14: OLED_DrawBMP(8,4,(8+24),(4+3),icon_14);break;
        default:break;
    }  
    OLED_ShowString(40,4,(uint8_t *)string);
    OLED_Refresh_Gram();
}

void Offline_Display_Two_Emoji_String(uint8_t emoji_no_1,char * string_1,uint8_t emoji_no_2,char * string_2)
{
    OLED_Clear_Display_GRAM();  
    
    switch(emoji_no_1)
    {
        case 1: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_1);break;
        case 2: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_2);break;
        case 3: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_3);break;
        case 4: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_4);break;
        case 5: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_5);break;
        case 6: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_6);break;
        case 7: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_7);break;
        case 8: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_8);break;
        case 9: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_9);break;
        case 10: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_10);break;
        case 11: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_11);break;
        case 12: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_12);break;
        case 13: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_13);break;
        case 14: OLED_DrawBMP_Offset(8,2,(8+24),(2+3),icon_14);break;
        default:break;
    }  
    OLED_ShowString(40,2,(uint8_t *)string_1);
    
    switch(emoji_no_2)
    {
        case 1: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_1);break;
        case 2: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_2);break;
        case 3: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_3);break;
        case 4: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_4);break;
        case 5: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_5);break;
        case 6: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_6);break;
        case 7: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_7);break;
        case 8: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_8);break;
        case 9: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_9);break;
        case 10: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_10);break;
        case 11: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_11);break;
        case 12: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_12);break;
        case 13: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_13);break;
        case 14: OLED_DrawBMP_Offset(8,5,(8+24),(5+3),icon_14);break;
        default:break;
    }  
    OLED_ShowString(40,5,(uint8_t *)string_2);
    
    OLED_Refresh_Gram(); 
}

uint8_t OLED_GRAM[128][8];

/**
  * @brief      Calculating the X axis coordinates of the pointer
  * @param      circle_x:The center abscissa
  *             Length  :
  *             Angle   :
  * @return     X axis
  * @details    None       
  */
unsigned char OLED_Calculat_X(unsigned char circle_x,unsigned char Length,unsigned char Angle)
{
    unsigned char x; 

    x=circle_x-Length*cos(Angle*PAI/18000);      
    return x;    
}

/**
  * @brief      Calculating the Y axis coordinates of the pointer
  * @param      circle_Y:The center coordinates
  *             Length  :
  *             Angle   :
  * @return     Y axis
  * @details    None       
  */
unsigned char OLED_Calculat_Y(unsigned char circle_y,unsigned char Length,unsigned char Angle)
{
    unsigned char y;
        
    y=circle_y+Length*sin(Angle*PAI/18000);	 
    return y;     
}

/**
  * @brief      Painting to GRAM(The reverse)
  * @param      x:0-127
  *             y:0-7
  *             t:1:fill   0:empty
  * @return     None
  * @details    字符取模方式画点      
  */			   
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//Beyond the scope 
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

/**
  * @brief      Painting to GRAM(The order)
  * @param      x:0-127
  *             y:0-7
  *             t:1:fill   0:empty
  * @return     None
  * @details    图片取模方式画点     
  */
void OLED_DrawPoint_value(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>127||y>63)return;//Beyond the scope 
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

/**
  * @brief      Draw Point function
  * @param      x:0-127
  *             y:0-7         
  * @return     None
  * @details    None      
  */
void DrawPoint(uint8_t x,uint8_t y)
{
    uint8_t data1;  //data1 ：The current data
    
    data1 = 0x01<<(y%8); 
    OLED_WR_Byte(0xb0+(y>>3),OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f)|0x00,OLED_CMD);
    OLED_WR_Byte(data1,OLED_DATA);
}

/**
  * @brief      Point to remove the original pattern
  * @param      x:0-127
  *             y:0-7         
  * @return     None
  * @details    None      
  */
void DrawPoint_clear(uint8_t x,uint8_t y)
{   
    OLED_WR_Byte(0xb0+(y>>3),OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f)|0x00,OLED_CMD);
    OLED_WR_Byte(0,OLED_DATA);
}

/**
  * @brief      To write a byte SSD1106.
  * @param      dat:To write data or command
  *             cmd:Data/command 0,indicates that the command    1,indicates that the data
  * @return     None
  * @details    None       
  */
static void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
{	      
    if(cmd)
        OLED_DC_Set();
    else 
        OLED_DC_Clr();	
        
    OLED_CS_Clr();	 
        
    SPI_WRITE_TX(SPI0,dat);    
    SPI_TRIGGER(SPI0);    
         
    while(SPI_IS_BUSY(SPI0)==1)
    {
    }

    OLED_CS_Set();
    OLED_DC_Set();      
} 

///**
//  * @brief      Screen clearing function
//  * @param      None
//  *             
//  * @return     None
//  * @details    Clear the screen,The entire screen is black,And not as bright      
//  */
//static void OLED_Clear_all(void)  
//{  
//    uint8_t i,n;
//    
//    for(i=0;i<8;i++)  
//    {  
//        OLED_WR_Byte (0xb0+i,OLED_CMD);    //Set the page address(0~7)
//        OLED_WR_Byte (0x00,OLED_CMD);      //Set the display position - low column address
//        OLED_WR_Byte (0x10,OLED_CMD);      //Set the display position - high column address 
//        for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
//    }   //Update the display
//}

/**
  * @brief      Screen clearing function
  * @param      None
  *             
  * @return     None
  * @details    Clear the screen,The entire screen is black,And not as bright      
  */  
void OLED_Clear(void)  
{  
    uint8_t i,n;  
    for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
    OLED_Refresh_Gram();//Update the display
}

/**
  * @brief      Set OLED screen display Coordinate point function
  * @param      None
  *             
  * @return     None
  * @details    None      
  */
static void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
    OLED_WR_Byte(0xb0+y,OLED_CMD);
    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
    OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}  

/**
  * @brief      Display a string in the location specified,Including some characters
  * @param      x:0~127
  *             y:0~63
  *             mode:0,The white shows   1,The normal display
  *             size:Select the font 12/16/24
  * @return     None
  * @details    None      
  */
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode)
{      			    
    uint8_t temp,t,t1;
    uint8_t y0=y;
    uint8_t csize=(size/8+((size%8)?1:0))*(size/2);		//Get the font a set of corresponding lattice character of bytes
    chr=chr-' ';//After get the offset value	 
    for(t=0;t<csize;t++)
    {   
        if(size==12)temp=asc2_1608[chr][t]; 	 	//Call 1206 font
        else if(size==16)temp=asc2_1608[chr][t];	//Call 1608 font   
        else return;								//There is no the word stock
        for(t1=0;t1<8;t1++)
        {
            if(temp&0x80)OLED_DrawPoint_value(x,y,mode);
            else OLED_DrawPoint_value(x,y,!mode);
            temp<<=1;
            y++;
            if((y-y0)==size)
            {
                y=y0;
                x++;
                break;
            }
        }  	 
    }          
}

/**
  * @brief      Display a string in OLED function
  * @param      x:0~127
  *             y:0~63
  *             *chr:To display the string
  * @return     None
  * @details    None      
  */
void OLED_ShowString(char x,uint8_t y,uint8_t *chr)
{
    static uint8_t count=0;
    static uint8_t times=0; 
    unsigned char j=0;
        
    while (chr[j]!='\0')
    {      
        if(SIZE ==12)
        {
            if(chr[j]=='*')
            {
                count=1;
            }
            else
            {
                count=0;
            }
                    
            if(count==0)
            {
                OLED_ShowChar(x,y,chr[j],12,1);
            }
                    
            if(count==1)
            {
                x=x-6;
                y+=1;
                count=0;
            }
                   
            x+=6;
            if(x>120)
            {
                x=0;
                y+=1;
            }
            j++;
        }
        else
        {
            if((chr[j]=='\r')||(count==1))
            {
                count=1;
                times=1;
                                
                if((times==1)&&(chr[j]=='\n'))
                {
                    count=2;
                }
                else
                {
                    x=x-8;
                    count=1;
                    times=0;
                }
            }
            else
            {
                count=0;
            }
                    
            if(count==0)
            {
                if(y==2)
                {
                    OLED_ShowChar(x,y*8-1,chr[j],16,1);
                }
                else if(y==4)
                {
                    OLED_ShowChar(x,y*8+4,chr[j],16,1);
                }
                else if(y==5)
                {
                    OLED_ShowChar(x,y*8+1,chr[j],16,1);
                }
                else
                {
                    OLED_ShowChar(x,y*8,chr[j],16,1);
                }
            }
                    
            if(count==2)
            {
                x=x-8;
                y+=2;
                count=0;
            }
                   
            x+=8;
            if(x>120)
            {
                x=0;
                y+=2;
            }
            j++;
        }
    }
}

/**
  * @brief      Display a Other type string in OLED function
  * @param      x:0~127
  *             y:0~63
  *             *chr:To display the string
  * @return     None
  * @details    None      
  */
void OLED_Other_type_ShowString(char x,uint8_t y,uint8_t *chr)
{
    static uint8_t count=0;
    static uint8_t times=0; 
    unsigned char j=0;
        
    while (chr[j]!='\0')
    {      
        if(SIZE ==12)
        {
            if(chr[j]=='*')
            {
                count=1;
            }
            else
            {
                count=0;
            }
                    
            if(count==0)
            {
                OLED_ShowChar(x,y,chr[j],12,1);
            }
                    
            if(count==1)
            {
                x=x-6;
                y+=1;
                count=0;
            }
                   
            x+=6;
            if(x>120)
            {
                x=0;
                y+=1;
            }
            j++;
        }
        else
        {
            if((chr[j]=='\r')||(count==1))
            {
                count=1;
                times=1;
                                
                if((times==1)&&(chr[j]=='\n'))
                {
                    count=2;
                }
                else
                {
                    x=x-8;
                    count=1;
                    times=0;
                }
            }
            else
            {
                count=0;
            }
                    
            if(count==0)
            {
                    OLED_ShowChar(x,y*8,chr[j],16,1);
            }
                    
            if(count==2)
            {
                x=x-8;
                y+=2;
                count=0;
            }
                   
            x+=8;
            if(x>120)
            {
                x=0;
                y+=2;
            }
            j++;
        }
    }
}

/**
  * @brief      Display a string and emoji in OLED function
  * @param      x:0~127
  *             y:0~63
  *             *chr:To display the string and emoji
  * @return     None
  * @details    None      
  */
void OLED_ShowString_Emoji(uint8_t x,uint8_t y,uint8_t *chr)
{
    int8_t  cal_emoji_no_flag=0;      
    int8_t  count=0;
    int8_t  emoji_flag=0;
    uint8_t number=0;
    int16_t emoji_no=0;
    int8_t  j=0;  
       
    uint8_t  a=0;
        
    while(chr[j]!='\0')
    {                 
        if((count==2)&&(chr[j]=='e'))
        {
            emoji_flag=1;
            count=0;
        }
        else if(count==2)
        {
            x=x-16;
            count=0;    
        }
         
        if((count==1)&&(chr[j]=='u'))
        {
            count=2;    
        }
        else if(count==1)
        {
            x=x-8;
            count=0;    
        }
                      
        if((count==0)&&(chr[j]=='/'))
        {
            count=1;                               
        }
            
        if((emoji_flag==0)&&(count==0))
        {
            if(y==2)
            {
                OLED_ShowChar(x,y*8-1,chr[j],16,1);
            } 
            else if(y==5)
            {
                OLED_ShowChar(x,y*8+1,chr[j],16,1);
            } 
            else
            {
                OLED_ShowChar(x,y*8,chr[j],16,1);
            }    
        }
                     
        if(emoji_flag==1)
        { 
            if(cal_emoji_no_flag==0)
            {    
                cal_emoji_no_flag=1;
                a=j;
            }
                
            number++;
                
            if(number>=4)
            {                       
                number=0;
                emoji_flag=0;
                    
                if(x==56)
                {
                     x=x-34;
                }
                else
                {
                     x=x-40;
                }                       
                    
                emoji_no=(((chr[a+1]-0x30)*100)+((chr[a+2]-0x30)*10)+(chr[a+3]-0x30)-800);                           
                    
                switch(emoji_no)
                {
                    case 1: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_1);break;
                    case 2: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_2);break;
                    case 3: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_3);break;
                    case 4: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_4);break;
                    case 5: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_5);break;
                    case 6: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_6);break;
                    case 7: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_7);break;
                    case 8: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_8);break;
                    case 9: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_9);break;
                    case 10: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_10);break;
                    case 11: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_11);break;
                    case 12: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_12);break;
                    case 13: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_13);break;
                    case 14: OLED_DrawBMP_Offset(x,y,(x+24),(y+3),icon_14);break;
                    default:break;
                }
                    
                x=x+24;         
            }                    
        }
                                  
        x=x+8;
        if(x>120)
        {
            x=0;
            if(y==2)
            {
              y+=3;
            } 
            else
            {
              return;
            }            
        }
        j++;
    }  
}

/**
  * @brief      drawing line function
  * @param      x1,x2:Starting point coordinates
  *             y1,y2:End point coordinates
  *             dispaly:0x01  clear:0x00
  * @return     None
  * @details    None      
  */
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t cmd)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
    
	delta_x=x2-x1; //Calculating coordinates increment
	delta_y=y2-y1; 
    
	uRow=x1; 
	uCol=y1; 
    
	if(delta_x>0)incx=1; //Set the step direction 
	else if(delta_x==0)incx=0;//Vertical line
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//A horizontal line
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //Select basic incremental axes
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//Line drawing output
	{  
        if(cmd==DISPLAY)
        {
            OLED_DrawPoint(uRow,uCol,1);
        }
        else
        {
            OLED_DrawPoint(uRow,uCol,0);//Remove paint points    
        }
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

/**
  * @brief      drawing Circle function
  * @param      x:Center coordinates X
  *             y:Center coordinates Y
  *             r:The radius
  *             cmd：DISPLAY/CLEAR
  * @return     None
  * @details    None      
  */
void LCD_Draw_Circle(uint8_t x,uint8_t  y,uint8_t r,uint8_t cmd)
{
    int a,b;
    int di;
    a=0;b=r;	  
    di=3-(r<<1);//Determine the coordinates of a point location
    while(a<=b)
    {
        if(cmd==DISPLAY)
        {
            OLED_DrawPoint(x-b,y-a,1);//3           
            OLED_DrawPoint(x+b,y-a,1);//0           
            OLED_DrawPoint(x-a,y+b,1);//1       
            OLED_DrawPoint(x-b,y-a,1);//7           
            OLED_DrawPoint(x-a,y-b,1);//2             
            OLED_DrawPoint(x+b,y+a,1);//4               
            OLED_DrawPoint(x+a,y-b,1);//5
            OLED_DrawPoint(x+a,y+b,1);//6 
            OLED_DrawPoint(x-b,y+a,1);             
            a++;
            //Using Bresenham algorithm painting circle     
            if(di<0)di +=4*a+6;	  
            else
            {
                di+=10+4*(a-b);   
                b--;
            } 
            OLED_DrawPoint(x+a,y+b,1);
        }
        else
        {         
            OLED_DrawPoint(x-b,y-a,0);//3           
            OLED_DrawPoint(x+b,y-a,0);//0           
            OLED_DrawPoint(x-a,y+b,0);//1       
            OLED_DrawPoint(x-b,y-a,0);//7           
            OLED_DrawPoint(x-a,y-b,0);//2             
            OLED_DrawPoint(x+b,y+a,0);//4               
            OLED_DrawPoint(x+a,y-b,0);//5
            OLED_DrawPoint(x+a,y+b,0);//6 
            OLED_DrawPoint(x-b,y+a,0);             
            a++;
            //Using Bresenham algorithm painting circle      
            if(di<0)di +=4*a+6;	  
            else
            {
                di+=10+4*(a-b);   
                b--;
            } 
            OLED_DrawPoint(x+a,y+b,0);  
        }
    }
}

/**
  * @brief      drawing rectangular function
  * @param      (x1,y1),(x2,y2):Diagonal coordinates of filled areas
  *             cmd：DISPLAY/CLEAR
  * @return     None
  * @details    None      
  */
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint8_t cmd)
{
    if(cmd==DISPLAY)
    {
        LCD_DrawLine(x1,y1,x2,y1,DISPLAY);
        LCD_DrawLine(x1,y1,x1,y2,DISPLAY);
        LCD_DrawLine(x1,y2,x2,y2,DISPLAY);
        LCD_DrawLine(x2,y1,x2,y2,DISPLAY);
    }
    else
    {
        LCD_DrawLine(x1,y1,x2,y1,CLEAR);
        LCD_DrawLine(x1,y1,x1,y2,CLEAR);
        LCD_DrawLine(x1,y2,x2,y2,CLEAR);
        LCD_DrawLine(x2,y1,x2,y2,CLEAR);
    }
}

/**
  * @brief      Painting to GRAM(The reverse)
  * @param      x1,y1,x2,y2:Diagonal coordinates of filled areas
  *             Make sure that:x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	
  *             dot:0,empty;1,fill
  * @return     None
  * @details    None      
  */	
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot)  
{  
	uint8_t x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//Update the display
}

/**
  * @brief      OLED clear display GRAM
  * @param      x1,y1,x2,y2:Diagonal coordinates of filled areas
  *             Make sure that:x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	
  *             dot:0,empty;1,fill
  * @return     None
  * @details    None      
  */	
void OLED_Clear_Display_GRAM(void)  
{  
	uint8_t x,y;  
	for(x=0;x<=127;x++)
	{
		for(y=0;y<=63;y++)OLED_DrawPoint(x,y,0);
	}													    
}


/**
  * @brief      Update the display to the OLED
  * @param      None        
  * @return     None
  * @details    None      
  */
void OLED_Refresh_Gram(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //Set the page address（0~7）
		OLED_WR_Byte (0x10,OLED_CMD);      //Set the display position—Low column address
		OLED_WR_Byte (0x00,OLED_CMD);      //Set the display position—High column address
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

/**
  * @brief      BMP image display function(128 * 64) 
  * @param      x0,x1:0~127
  *             y0,y1:0~7
  *             BMP[]:The location of the images in the character array number
  * @return     None
  * @details    None    
  */
void OLED_DrawBMP_Offset(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[])
{ 	
    unsigned int j=0;
    unsigned char x=0;
    unsigned char y=0;
    unsigned char z=0;
    uint8_t t=0;
    uint8_t temp=0;  
    
    if(y1%8==0) y=y1/8;      
    else y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        for(x=x0;x<x1;x++)
        {     
            temp=BMP[j++];
            for(t=0;t<8;t++)
            {
                if(temp&0x80)
                {
                    if(y==2)
                    {
                        OLED_DrawPoint_value(x,y*8+z-4,1); 
                    }
                    else if(y==5)
                    {
                        OLED_DrawPoint_value(x,y*8+z-3,1); 
                    }
                    else
                    {
                        OLED_DrawPoint_value(x,y*8+z-4,1); 
                    }        
                }
                else 
                {
                    if(y==2)
                    {
                        OLED_DrawPoint_value(x,y*8+z-4,0); 
                    }
                    else if(y==5)
                    {
                        OLED_DrawPoint_value(x,y*8+z-3,0); 
                    }
                    else
                    {
                        OLED_DrawPoint_value(x,y*8+z-4,0); 
                    }   
                }
                temp<<=1;
                z++;
                if(z>=8)
                {
                    z=0;
                }
            }
        }
    }
}

/**
  * @brief      BMP image display function(128 * 64) 
  * @param      x0,x1:0~127
  *             y0,y1:0~7
  *             BMP[]:The location of the images in the character array number
  * @return     None
  * @details    None    
  */
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[])
{ 	
    unsigned int j=0;
    unsigned char x=0;
    unsigned char y=0;
    unsigned char z=0;
    uint8_t t=0;
    uint8_t temp=0;  
    
    if(y1%8==0)
        y=y1/8;      
    else 
        y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        for(x=x0;x<x1;x++)
        {     
            temp=BMP[j++];
            for(t=0;t<8;t++)
            {
                if(temp&0x80)
                {
                        OLED_DrawPoint_value(x,y*8+z,1);      
                }
                else 
                {
                        OLED_DrawPoint_value(x,y*8+z,0); 
                }
                temp<<=1;
                z++;
                if(z>=8)
                {
                    z=0;
                }
            }
        }
    }
}

/**
  * @brief      BMP image display function(128 * 64) 
  * @param      x0,x1:0~127
  *             y0,y1:0~7
  *             BMP[]:The location of the images in the character array number
  * @return     None
  * @details    None    
  */
void OLED_DrawBMP_Image(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,const unsigned char BMP[])
{ 	
    unsigned int j=0;
    unsigned char x=0;
    unsigned char y=0;
    unsigned char z=0;
    uint8_t t=0;
    uint8_t temp=0;  
    
    if(y1%8==0)
        y=y1/8;      
    else
        y=y1/8+1;
    for(y=y0;y<y1;y++)
    {
        for(x=x0;x<x1;x++)
        {     
            temp=BMP[j++];
            for(t=0;t<8;t++)
            {
                if(temp&0x80)
                {
                        OLED_DrawPoint_value(x1-x+x0,y*8+z,1);       
                }
                else 
                {
                        OLED_DrawPoint_value(x1-x+x0,y*8+z,0);   
                }
                temp<<=1;
                z++;
                if(z>=8)
                {
                    z=0;
                }
            }
        }
    }
}

/**
  * @brief      OLED  SPI initialization function
  * @param      None
  *             
  * @return     None
  * @details    None      
  */
static void SPI_Init(void)
{ 
    CLK_SetModuleClock(SPI0_MODULE,CLK_CLKSEL1_SPISEL_HCLK,0);
    CLK_EnableModuleClock(SPI0_MODULE);   
    SYS->P0_MFP &= ~(SYS_MFP_P05_Msk | SYS_MFP_P06_Msk|SYS_MFP_P07_Msk);    
    SYS->P0_MFP |= (SYS_MFP_P05_SPI0_MOSI | SYS_MFP_P06_SPI0_MISO|SYS_MFP_P07_SPI0_CLK);
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000); // default MSB first. master
        
    pinMode(SPI_DC, GPIO_MODE_OUTPUT);
    pinMode(SPI_RST, GPIO_MODE_OUTPUT);
    pinMode(SPI_CS, GPIO_MODE_OUTPUT);
        
    digitalWrite(SPI_DC,High_level);
    digitalWrite(SPI_RST,High_level);
    digitalWrite(SPI_CS,Low_level);
        
    OLED_RST_Set();
    delay(100);
    OLED_RST_Clr();
    delay(100);
    OLED_RST_Set(); 
                          
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//--set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//--set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD);//--Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0:About the inverted          0xa1:normal
    OLED_WR_Byte(0xC8,OLED_CMD);//--Set COM/Row Scan Direction   0xc0:Up and down the inverted    0xc8:normal
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//--set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//--not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//--Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//--Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//--Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);//--Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);//--Disable Inverse Display On (0xa6/a7) 
    OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
     
//    OLED_WR_Byte(0xAE,OLED_CMD);
//    OLED_WR_Byte(0xD5,OLED_CMD);
//    OLED_WR_Byte(0x80,OLED_CMD);
//    OLED_WR_Byte(0xA8,OLED_CMD);
//    OLED_WR_Byte(0x3F,OLED_CMD);
//    OLED_WR_Byte(0xD3,OLED_CMD);
//    OLED_WR_Byte(0x00,OLED_CMD);
//    OLED_WR_Byte(0x10,OLED_CMD);
//    OLED_WR_Byte(0x8D,OLED_CMD);
//    OLED_WR_Byte(0x00,OLED_CMD);
//    OLED_WR_Byte(0xA1,OLED_CMD);
//    OLED_WR_Byte(0xC8,OLED_CMD);
//    OLED_WR_Byte(0xDA,OLED_CMD);
//    OLED_WR_Byte(0x12,OLED_CMD);
//    OLED_WR_Byte(0x81,OLED_CMD);
//    OLED_WR_Byte(0xFF,OLED_CMD);
//    OLED_WR_Byte(0xD9,OLED_CMD);
//    OLED_WR_Byte(0xF1,OLED_CMD);
//    OLED_WR_Byte(0xDB,OLED_CMD);
//    OLED_WR_Byte(0xFF,OLED_CMD);
//    OLED_WR_Byte(0xA4,OLED_CMD);
//    OLED_WR_Byte(0xA6,OLED_CMD);
//    OLED_WR_Byte(0xAF,OLED_CMD);    

    OLED_WR_Byte(0xAF,OLED_CMD);/*display ON*/ 
    OLED_Clear();
    OLED_Set_Pos(0,0); 
}

/**
  * @brief      OLED initialization function
  * @param      None
  *             
  * @return     None
  * @details    None      
  */
void OLED_Init(void)
{
    SPI_Init();
}

/**
  * @brief      OLED display face blink flash clear 20px function
  * @param      face_type_value:face number
  *             
  * @return     None
  * @details    None      
  */
void Display_Face_Blink_Flash_Clear_20PX(uint8_t face_type_value)
{
    switch(face_type_value)
    {
        case 1:
        {
            OLED_Fill(0,7,127,29,0);   
            OLED_Fill(0,32,127,63,0); 
            break;
        }
        case 2:
        {
            OLED_Fill(0,7,127,20,0);   
            OLED_Fill(0,25,127,63,0); 
            break;
        }
        case 3:
        {
            OLED_Fill(0,7,127,20,0);   
            OLED_Fill(0,33,127,63,0);  
            break;
        }
        case 4:
        {
            OLED_Fill(0,7,127,20,0);  
            OLED_Fill(0,33,127,63,0);   
            break;
        }
        case 5:
        {
            OLED_Fill(0,7,127,29,0);   
            OLED_Fill(0,45,127,63,0);    
            break;
        }
        case 6:
        {
            OLED_Fill(0,7,127,27,0);   
            OLED_Fill(0,30,127,63,0);     
            break;
        }
        case 7:
        {
            OLED_Fill(0,7,127,25,0);
            OLED_Fill(0,37,127,63,0);  
            break;
        }
        case 8:
        {
            OLED_Fill(0,7,127,22,0);   
            OLED_Fill(0,35,127,63,0); 
            break;
        }
        default:break;
    }
}

/**
  * @brief      OLED display face function
  * @param      face_type_value:face number
  *             
  * @return     None
  * @details    None      
  */
void Display_Face(uint8_t face_type_value)
{
    switch(face_type_value)
    {
        case 1:
        {
            OLED_DrawBMP(36-20,(36-8+1)/8,36+21,(36+9+1)/8,face_1);     /*angry*/
            OLED_DrawBMP_Image(92-20,(36-8+1)/8,92+21,(36+9+1)/8,face_1);
            break;
        }
        case 2:
        {
            OLED_DrawBMP(36-20,(36-7+6)/8,36+21,(36+8+6)/8,face_2);     /*drowsy*/
            OLED_DrawBMP_Image(92-20,(36-7+6)/8,92+21,(36+8+6)/8,face_2); 
            break;
        }
        case 3:
        {
            OLED_DrawBMP(36-20,(36-24+4)/8,36+21,(36+24+4)/8,face_3);   /*enlarged*/
            OLED_DrawBMP_Image(92-20,(36-24+4)/8,92+21,(36+24+4)/8,face_3);
            break;
        }
        case 4:
        {
            OLED_DrawBMP(36-20,(36-12+1)/8,36+20,(36+12+1)/8,face_4);   /*fixed*/
            OLED_DrawBMP_Image(92-20,(36-12+1)/8,92+20,(36+12+1)/8,face_4);                      
            break;
        }
        case 5:
        {
            OLED_DrawBMP(36-20,(36-12-4)/8,36+20,(36+12-4)/8,face_5);   /*happy*/
            OLED_DrawBMP_Image(92-20,(36-12-4)/8,92+20,(36+12-4)/8,face_5);
            break;
        }
        case 6:
        {
            OLED_DrawBMP(36-20,(36-4-0)/8,36+20,(36+4-0)/8,face_6);     /*mini*/
            OLED_DrawBMP_Image(92-20,(36-4-0)/8,92+20,(36+4-0)/8,face_6);
            break;
        }
        case 7:
        {
            OLED_DrawBMP(36-20,(36-16+1)/8,36+21,(36+16+1)/8,face_7);   /*normal*/
            OLED_DrawBMP_Image(92-20,(36-16+1)/8,92+21,(36+16+1)/8,face_7);
            break;
        }
        case 8:
        {
            OLED_DrawBMP(36-19,(36-10+4)/8,36+19,(36+11+4)/8,face_8);   /*sad*/ 
            OLED_DrawBMP_Image(92-19,(36-10+4)/8,92+19,(36+11+4)/8,face_8);
            break;
        }
        default:break;
    }
}

/**
  * @brief      OLED boot animation function
  * @param      face_type:face number
  *             repeat_tiems:repeat tiems
  * @return     None
  * @details    None      
  */
void boot_animation(uint8_t face_type,uint8_t repeat_tiems)
{
    uint32_t currentMillis = 0;               
    uint32_t boot_animation_previousMillis = 0;  
    uint8_t  boot_animation_flag = 0;
    uint8_t  boot_animation_count = 0;
    uint8_t  times = 0;
    
    while(1)
    {               
        if(boot_animation_flag == 0)
        {
            currentMillis = millis();
            if(currentMillis - boot_animation_previousMillis > 67)
            {
                boot_animation_previousMillis = currentMillis; 
                boot_animation_count++; 
                if(boot_animation_count > 5)
                {
                    boot_animation_count = 0;
                }                   
                                
                if(boot_animation_count == 1)
                {               
                    Display_Face(face_type);
                    OLED_Refresh_Gram();             
                }
                else if(boot_animation_count == 2)
                {
                    Display_Face_Blink_Flash_Clear_20PX(face_type);
                }
                else if(boot_animation_count == 3)
                {
                    OLED_Fill(0,0,127,63,0); 
                }
                else if(boot_animation_count == 4)
                {
                    Display_Face(face_type);
                    Display_Face_Blink_Flash_Clear_20PX(face_type);
                    OLED_Refresh_Gram();                         
                }
                else if(boot_animation_count == 5)
                {
                    Display_Face(face_type);
                    OLED_Refresh_Gram();                     
                    boot_animation_flag = 1; 
                    
                    if(times == repeat_tiems-1)
                    {
                        OLED_Fill(0,0,127,63,0); 
                        return;
                    }          
                }
            }
        }
        if(boot_animation_flag == 1)
        {
            currentMillis = millis();
            if(currentMillis - boot_animation_previousMillis > 1933)
            {
                boot_animation_previousMillis = currentMillis; 
                boot_animation_flag = 0;
                times++;
            }    
        }
    }
}
