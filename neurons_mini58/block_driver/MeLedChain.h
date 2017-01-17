#ifndef __MELEDCHAIN_H__
#define __MELEDCHAIN_H__

#include "Mini58Series.h"
#include "mygpio.h"

#define MAX_RGB_LED_NUM         (1+35)
#define RGB_LED_SLOT1           0
#define RGB_LED_SLOT2           1
#define RGB_LED_SLOT_MAX        2
#define ALL_LED			        0

#define DISPLAY_MODE_STATIC         0x00
#define DISPLAY_MODE_ROLL           0x01
#define DISPLAY_MODE_REPEAT         0x02
#define DISPLAY_MODE_MARQUEE        0x03

#define COLOR_NUMS                  (8u)
#define COLOR_SCALE                 (32)

typedef __packed struct 
{
 uint8_t red;
 uint8_t green;
 uint8_t blue;
}RGB_STRUCT;

typedef enum 
{
 COLOR_RED_INDEX=0,
 COLOR_ORANGE_INDEX,
 COLOR_YELL0_INDEX,
 COLOR_GREEN_INDEX,
 COLOR_CYAN_INDEX,
 COLOR_BLUE_INDEX,
 COLOR_PURPLE_INDEX,
 COLOR_WHITE_INDEX
}COLORS_INDEX;

typedef RGB_STRUCT (*LUM_TO_RGB_FUNC_PTR) (RGB_STRUCT color_base,uint8_t sensor_value);

typedef __packed struct 
{
    COLORS_INDEX        color_index;
    RGB_STRUCT          color_baae_value;
    LUM_TO_RGB_FUNC_PTR lum_to_rgg_value_fun;
}COLOR_PROFILE;


struct rgb_value
{ 
     uint8_t r; 
     uint8_t g; 
     uint8_t b; 
};

void rgb_init(void);
void rgb_clear(void);
void rgb_show(uint8_t led_quantity);
        
COLOR_PROFILE Color_Table_Query(COLORS_INDEX color_index);
void led_chain_all_set(uint8_t led_quantity, uint8_t* led_array);
boolean setColor(uint8_t index, uint8_t red,uint8_t green,uint8_t blue);
boolean setColorAll(uint8_t red,uint8_t green,uint8_t blue);
void Set_SevenColor_Regular(uint8_t sensor_value);
static RGB_STRUCT Lum_To_RGB(COLORS_INDEX color_type,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Red(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Orange(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Yellow(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Green(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Cyan(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Blue(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_Purple(RGB_STRUCT color_base,uint8_t sensor_value);
static RGB_STRUCT Color_LumToRGB_White(RGB_STRUCT color_base,uint8_t sensor_value);

#endif



