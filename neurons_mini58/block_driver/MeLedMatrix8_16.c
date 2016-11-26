#include "MeLEDMatrix8_16.h"
#include "MeLEDMatrixData.h"
#include "systime.h"
#include "string.h"

#define BRIGHTNESS_1		0x88
#define BRIGHTNESS_2		0x89
#define BRIGHTNESS_4		0x8A
#define BRIGHTNESS_10		0x8B
#define BRIGHTNESS_11      	0x8c
#define BRIGHTNESS_12       0x8D
#define BRIGHTNESS_13		0x8E
#define BRIGHTNESS_14		0x8F

#define POINT_ON   1
#define POINT_OFF  0

#define LED_BUFFER_SIZE   16

//Define Data Command Parameters
#define Mode_Address_Auto_Add_1  0x40     //0100 0000 B
#define Mode_Permanent_Address   0x44     //0100 0100 B

//Define Address Command Parameters
#define ADDRESS(addr)  (0xC0 | addr)

// local variable
static boolean b_Color_Reverse;
static uint8_t u8_Display_Buffer[LED_BUFFER_SIZE] = {
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00
};
static int16_t i16_Str_Display_X_Position;
static int8_t i8_Str_Display_Y_Position;
static int16_t i16_Number_of_Character_of_Str;

// local function.
static void writeByte(uint8_t data);
static void clearScreen(void);
static void writeBytesToAddress(const uint8_t *P_data, uint8_t count_of_data);

char i8_Str_Display_Buffer[STRING_DISPLAY_BUFFER_SIZE] = {0};

void MeLEDMatrixShowStr(void);

static void tm1640_start(void)
{
	digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SDA_PIN, HIGH);
	delayMicroseconds(1);
	digitalWrite(SDA_PIN, LOW);
	delayMicroseconds(1);
}

static void tm1640_stop(void)
{
	digitalWrite(SDA_PIN, LOW);
	digitalWrite(SCK_PIN, HIGH);
	delayMicroseconds(1);
	digitalWrite(SDA_PIN, HIGH);
	delayMicroseconds(1);
}

void led_matrix8_16_init(void)
{
    pinMode(SCK_PIN, GPIO_MODE_OUTPUT);
    pinMode(SDA_PIN, GPIO_MODE_OUTPUT);
    digitalWrite(SCK_PIN,HIGH);
    digitalWrite(SDA_PIN,HIGH);

	tm1640_start();
    writeByte(Mode_Address_Auto_Add_1);
	tm1640_stop();
	clearScreen();
}

static void writeByte(uint8_t data)
{
    //Start
    digitalWrite(SCK_PIN, LOW);
    digitalWrite(SDA_PIN, LOW);

    for(char i=0;i<8;i++)
    {
        digitalWrite(SCK_PIN, LOW);
        digitalWrite(SDA_PIN, (data & 0x01));
		delayMicroseconds(1);
        digitalWrite(SCK_PIN, HIGH);
        data = data >> 1;
    }

    //End
    digitalWrite(SCK_PIN, LOW);
    digitalWrite(SDA_PIN, LOW);
}

static void writeBytesToAddress(const uint8_t *P_data, uint8_t count_of_data)
{
    uint8_t T_data;
	
	//Start
	tm1640_start();
    //write start address
    writeByte(0xC0);
    //write data
    for(uint8_t k=0; k<count_of_data; k++)
    {
		T_data = *(P_data + k);
		// start.
		digitalWrite(SCK_PIN, LOW);
		digitalWrite(SDA_PIN, LOW);

		for(char i=0;i<8;i++)
		{
			digitalWrite(SCK_PIN, LOW);
			digitalWrite(SDA_PIN, ((T_data & 0x80) > 0 ? 1:0));
			delayMicroseconds(1);
			digitalWrite(SCK_PIN, HIGH);
			T_data = T_data << 1;
		}

		// end.
		digitalWrite(SCK_PIN, LOW);
		digitalWrite(SDA_PIN, LOW);
	}
	
	tm1640_stop();
	tm1640_start();
    writeByte(BRIGHTNESS_1);
	tm1640_stop();
}


static void clearScreen(void)
{
    for(uint8_t i=0;i<LED_BUFFER_SIZE;i++)
    {
        u8_Display_Buffer[i] = 0x00;
    }

    b_Color_Reverse = 1;
	
    writeBytesToAddress(u8_Display_Buffer, LED_BUFFER_SIZE);
}

void MeLEDMatrix_drawBitmap(int8_t x, int8_t y, uint8_t Bitmap_Width, uint8_t *Bitmap)
{

    if(x>15 || y>7 || Bitmap_Width==0)
        return;


    if(b_Color_Reverse == 1)
    {
        for(uint8_t k=0;k<Bitmap_Width;k++)
        {
          if(x+k>=0){
            u8_Display_Buffer[x+k] = (u8_Display_Buffer[x+k] & (0xff << (8-y))) | (y>0?(Bitmap[k] >> y):(Bitmap[k] << (-y)));
          }
        }
    }
    else if(b_Color_Reverse == 0)
    {
        for(uint8_t k=0;k<Bitmap_Width;k++)
        {
            if(x+k>=0){
              u8_Display_Buffer[x+k] = (u8_Display_Buffer[x+k] & (0xff << (8-y))) | (y>0?(~Bitmap[k] >> y):(~Bitmap[k] << (-y)));
            }
        }
    }

    writeBytesToAddress(u8_Display_Buffer, LED_BUFFER_SIZE);
}



void MeLEDMatrix_DrawStr(int16_t X_position, int8_t Y_position, const char *str)
{
	// copy str to string buffer. strcpy.
    for(i16_Number_of_Character_of_Str = 0; str[i16_Number_of_Character_of_Str] != '\0'; i16_Number_of_Character_of_Str++)
    {
        if(i16_Number_of_Character_of_Str < STRING_DISPLAY_BUFFER_SIZE - 1)
        {
            i8_Str_Display_Buffer[i16_Number_of_Character_of_Str] = str[i16_Number_of_Character_of_Str];
        }
        else
        {
            break;
        }
    }
    i8_Str_Display_Buffer[i16_Number_of_Character_of_Str] = '\0';


    if(X_position < -(i16_Number_of_Character_of_Str * 6))
    {
        X_position = -(i16_Number_of_Character_of_Str * 6);
    }
    else if(X_position > 16)
    {
        X_position = 16;
    }
    i16_Str_Display_X_Position = X_position;


    if(Y_position < -1)
    {
        Y_position = -1;
    }
    else if(Y_position >15)
    {
        Y_position = 15;
    }
    i8_Str_Display_Y_Position = Y_position;

    MeLEDMatrixShowStr();

}


void MeLEDMatrixShowStr(void)
{

    uint8_t display_buffer_label = 0;

    if(i16_Str_Display_X_Position > 0)
    {
		//clear the position before set x positon.
        for(display_buffer_label = 0; display_buffer_label < i16_Str_Display_X_Position && display_buffer_label < LED_BUFFER_SIZE; display_buffer_label++)
        {
            u8_Display_Buffer[display_buffer_label] = 0x00;
        }

        if(display_buffer_label < LED_BUFFER_SIZE)
        {
            uint8_t num;

            for(uint8_t k=0;display_buffer_label < LED_BUFFER_SIZE && k < i16_Number_of_Character_of_Str;k++)
            {
                for(num=0; Character_font_6x8[num].Character[0] != '@'; num++)
                {
                    if(i8_Str_Display_Buffer[k] == Character_font_6x8[num].Character[0])
                    {
                        for(uint8_t j=0;j<6;j++)
                        {
                            u8_Display_Buffer[display_buffer_label] = Character_font_6x8[num].data[j];
                            display_buffer_label++;

                            if(display_buffer_label >= LED_BUFFER_SIZE)
                            {
                                break;
                            }
                        }
                        break;
                    }
                }
				
                if(Character_font_6x8[num].Character[0] == '@')
                {
                    i8_Str_Display_Buffer[k] = ' ';
                    k--;
                }
            }
			
			// clear the position behind the show block.
            if(display_buffer_label < LED_BUFFER_SIZE)
            {
                for(; display_buffer_label < LED_BUFFER_SIZE; display_buffer_label++)
                {
                    u8_Display_Buffer[display_buffer_label] = 0x00;
                }
            }
        }
    }
	else if(i16_Str_Display_X_Position <= 0)
    {
		if(i16_Str_Display_X_Position == -(i16_Number_of_Character_of_Str * 6))
        {
			for(; display_buffer_label < LED_BUFFER_SIZE; display_buffer_label++)
            {
                u8_Display_Buffer[display_buffer_label] = 0x00;
            }
        }
        else
        {
            int8_t j = (-i16_Str_Display_X_Position) % 6;
            uint8_t num;

            i16_Str_Display_X_Position = -i16_Str_Display_X_Position;

            for(int16_t k=i16_Str_Display_X_Position/6; display_buffer_label < LED_BUFFER_SIZE && k < i16_Number_of_Character_of_Str;k++)
            {
                for(num=0; Character_font_6x8[num].Character[0] != '@'; num++)
                {
                    if(i8_Str_Display_Buffer[k] == Character_font_6x8[num].Character[0])
                    {
                        for(;j<6;j++)
                        {
                            u8_Display_Buffer[display_buffer_label] = Character_font_6x8[num].data[j];
                            display_buffer_label++;

                            if(display_buffer_label >= LED_BUFFER_SIZE)
                            {
                                break;
                            }
                        }
                        j=0;
                        break;
                    }
                }

                if(Character_font_6x8[num].Character[0] == '@')
                {
                    i8_Str_Display_Buffer[k] = ' ';
                    k--;
                }

            }

            if(display_buffer_label < LED_BUFFER_SIZE)
            {
                for(; display_buffer_label < LED_BUFFER_SIZE; display_buffer_label++)
                {
                    u8_Display_Buffer[display_buffer_label] = 0x00;
                }
            }

            i16_Str_Display_X_Position = -i16_Str_Display_X_Position;
        }
    }

	// Y_Position: 7 is the normal position.
    if(7 - i8_Str_Display_Y_Position >= 0)
    {
        for(uint8_t k=0; k<LED_BUFFER_SIZE; k++)
        {
            u8_Display_Buffer[k] = u8_Display_Buffer[k] << (7 - i8_Str_Display_Y_Position);
        }
    }
    else
    {
        for(uint8_t k=0; k<LED_BUFFER_SIZE; k++)
        {
            u8_Display_Buffer[k] = u8_Display_Buffer[k] >> (i8_Str_Display_Y_Position - 7);
        }
    }

    // reverse.
    if(b_Color_Reverse == 0)
    {
        for(uint8_t k=0; k<LED_BUFFER_SIZE; k++)
        {
            u8_Display_Buffer[k] = ~u8_Display_Buffer[k];
        }
    }

    writeBytesToAddress(u8_Display_Buffer,LED_BUFFER_SIZE);

}


void MeLEDMatrix_DrawTime(uint8_t hour, uint8_t minute, boolean point_flag)
{
    u8_Display_Buffer[0]  = Clock_Number_font_3x8[hour/10].data[0];
    u8_Display_Buffer[1]  = Clock_Number_font_3x8[hour/10].data[1];
    u8_Display_Buffer[2]  = Clock_Number_font_3x8[hour/10].data[2];
 
    u8_Display_Buffer[3]  = 0x00;
 
    u8_Display_Buffer[4]  = Clock_Number_font_3x8[hour%10].data[0];
    u8_Display_Buffer[5]  = Clock_Number_font_3x8[hour%10].data[1];
    u8_Display_Buffer[6]  = Clock_Number_font_3x8[hour%10].data[2];
 
    u8_Display_Buffer[9]  = Clock_Number_font_3x8[minute/10].data[0];
    u8_Display_Buffer[10] = Clock_Number_font_3x8[minute/10].data[1];
    u8_Display_Buffer[11] = Clock_Number_font_3x8[minute/10].data[2];

    u8_Display_Buffer[12] = 0x00;

    u8_Display_Buffer[13] = Clock_Number_font_3x8[minute%10].data[0];
    u8_Display_Buffer[14] = Clock_Number_font_3x8[minute%10].data[1];
    u8_Display_Buffer[15] = Clock_Number_font_3x8[minute%10].data[2];


    if(point_flag == POINT_ON)
    {
        u8_Display_Buffer[7] = 0x28;
        //u8_Display_Buffer[8] = 0x28;
    }
    else
    {
        u8_Display_Buffer[7] = 0x00;
        u8_Display_Buffer[8] = 0x00;
    }

    if(b_Color_Reverse == 0)
    {
        for(uint8_t k=0; k<LED_BUFFER_SIZE; k++)
        {
            u8_Display_Buffer[k] = ~u8_Display_Buffer[k];
        }
    }

    writeBytesToAddress(u8_Display_Buffer,LED_BUFFER_SIZE);
}

void MeLEDMatrix_DrawBitMap(uint8_t matrix[])
{
	memcpy(u8_Display_Buffer, matrix, sizeof(u8_Display_Buffer));
	writeBytesToAddress(u8_Display_Buffer,LED_BUFFER_SIZE);
}
