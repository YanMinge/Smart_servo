#include "MeLedChain.h"
#include "systime.h"
#include "uart_printf.h"

// note : the time cost of  pwm interrupt program  > 1us, so the clock can't precisely satisfy the require of the ws2812.
#define LED_CTRL_FREQ       500000
#define LED_BIT_0_DUTY      20     
#define LED_BIT_1_DUTY      80

#define LED_PWM_CHANNEL         PWM_CH5
#define LED_PWM_DUTY_REG        *((__IO uint32_t *)((((uint32_t) & ((PWM)->CMPDAT0)) + LED_PWM_CHANNEL * 4)))
#define LED_PWM_PERIOD_REG      *((__IO uint32_t *)((((uint32_t) & ((PWM)->PERIOD0)) + (LED_PWM_CHANNEL) * 4)))
#define LED_PWM_CNT_DISABLE     0xffefffff   // count start enable. ~(1<<(LED_PWM_CHANNEL<<2))
#define PWM_FUNCTION_INIT       CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_PWMCH45SEL_Msk))| CLK_CLKSEL2_PWMCH45SEL_HCLK; \
                                CLK_EnableModuleClock(PWMCH45_MODULE);  \
                                SYS->P0_MFP |= SYS_MFP_P04_PWM0_CH5
#define PWM_CLEAR_INT_FLAG      PWM->INTSTS = 0x20 // (1 << LED_PWM_CHANNEL)

static uint8_t s_bit_0_duty;
static uint8_t s_bit_1_duty;
static uint32_t s_led_bit_count;
static uint32_t s_led_bits_num;

uint8_t pixels_array[MAX_RGB_LED_NUM*3]; // store rgb value of every led.
uint8_t pixels_bits_array[MAX_RGB_LED_NUM*24]; // store duty of every rgb value.
volatile static uint32_t* s_duty_set_address = NULL;

struct rgb_value light_panel[] = {{0x00, 0x00, 0x00},{0xff, 0x00, 0x00}, {0xff, 0xaf, 0x00}, {0xff, 0xff, 0x00},{0x00, 0xff, 0x00},  \
                                  {0x00, 0xff, 0xff},{0x00, 0x00, 0xff},{0xd4, 0x00, 0xff},{0xff, 0xff, 0xff}
};

static const COLOR_PROFILE LED_Chain_Color_Table[COLOR_NUMS]=
{
 {COLOR_RED_INDEX,      {220,000,000},Color_LumToRGB_Red},
 {COLOR_ORANGE_INDEX,   {255,165,000},Color_LumToRGB_Orange},
 {COLOR_YELL0_INDEX,    {255,255,000},Color_LumToRGB_Yellow},
 {COLOR_GREEN_INDEX,    {000,255,000},Color_LumToRGB_Green},
 {COLOR_CYAN_INDEX,     {000,255,255},Color_LumToRGB_Cyan},
 {COLOR_BLUE_INDEX,     {030,000,255},Color_LumToRGB_Blue},
 {COLOR_PURPLE_INDEX,   {255,0,255},Color_LumToRGB_Purple},
 {COLOR_WHITE_INDEX,    {255,255,255},Color_LumToRGB_White}
};

                              
 COLOR_PROFILE Color_Table_Query(COLORS_INDEX color_index)
 {
  return LED_Chain_Color_Table[color_index];
 }  

static void led_param_set(void)
{
    uint32_t i = SystemCoreClock / LED_CTRL_FREQ;
    uint8_t  u8Divider = 1, u8Prescale = 0xFF;
    uint16_t u16CNR = 0xFFFF;

    // clk divider could only be 1, 2, 4, 8, 16
    for(; u8Divider < 17; u8Divider <<= 1) 
    {  
        i = (SystemCoreClock / LED_CTRL_FREQ) / u8Divider;
        // If target value is larger than CNR * prescale, need to use a larger divider
        if(i > (0x10000 * 0x100))
            continue;

        // CNR = 0xFFFF + 1, get a prescaler that CNR value is below 0xFFFF
        u8Prescale = (i + 0xFFFF)/ 0x10000;

        // u8Prescale must at least be 2, otherwise the output stop
        if(u8Prescale < 3)
            u8Prescale = 2;

        i /= u8Prescale;

        if(i <= 0x10000) 
        {
            if(i == 1)
                u16CNR = 1;     // Too fast, and PWM cannot generate expected frequency...
            else
                u16CNR = i;
            break;
        }

    }
    // Store return value here 'cos we're gonna change u8Divider & u8Prescale & u16CNR to the real value to fill into register
    i = SystemCoreClock / (u8Prescale * u8Divider * u16CNR);

    u8Prescale -= 1;
    u16CNR -= 1;
    // convert to real register value
    if(u8Divider == 1)
        u8Divider = 4;
    else if (u8Divider == 2)
        u8Divider = 0;
    else if (u8Divider == 4)
        u8Divider = 1;
    else if (u8Divider == 8)
        u8Divider = 2;
    else // 16
        u8Divider = 3;

    // every two channels share a prescaler
    PWM->CLKPSC = (PWM->CLKPSC & ~(PWM_CLKPSC_CLKPSC01_Msk << ((LED_PWM_CHANNEL >> 1) * 8))) | (u8Prescale << ((LED_PWM_CHANNEL >> 1) * 8));
    PWM->CLKDIV = (PWM->CLKDIV & ~(PWM_CLKDIV_CLKDIV0_Msk << (4 * LED_PWM_CHANNEL))) | (u8Divider << (4 * LED_PWM_CHANNEL));
    PWM->CTL = (PWM->CTL & ~PWM_CTL_CNTTYPE_Msk) | (PWM_CTL_CNTMODE0_Msk << ((4 * LED_PWM_CHANNEL)));

    *((__IO uint32_t *)((((uint32_t) & ((PWM)->PERIOD0)) + (LED_PWM_CHANNEL) * 4))) = u16CNR;
    s_bit_0_duty = (100 - LED_BIT_0_DUTY)*(u16CNR + 1)/100 - 1;
    s_bit_1_duty = (100 - LED_BIT_1_DUTY)*(u16CNR + 1)/100 - 1;
}

void rgb_init(void)
{
    PWM_FUNCTION_INIT;
    PWM->CTL |= (1<<(PWM_CTL_PINV0_Pos + (LED_PWM_CHANNEL<<2))); // inverse.
    led_param_set();
    s_duty_set_address = &(LED_PWM_DUTY_REG);
    LED_PWM_DUTY_REG = 0x66; // set pwm output to low.
    PWM->POEN |= (1<<LED_PWM_CHANNEL); //Enable PWM Output
    NVIC_SetPriority(PWM_IRQn, 0);
    delayMicroseconds(10); // signal is not stable when mcu start, wait a frame intervl time, then clear all led.
    rgb_clear();
}

boolean setColor(uint8_t index, uint8_t red,uint8_t green,uint8_t blue)
{
    uint8_t i,tmp;
    if(index == 0)
    {
        for(i = 1; i <= MAX_RGB_LED_NUM; i++)
        {
			tmp = (i-1) * 3;
            pixels_array[tmp] = green;
            pixels_array[tmp+1] = red;
            pixels_array[tmp+2] = blue;
        }
        return TRUE;
    }
	
    else
    {
         if(index <= MAX_RGB_LED_NUM)
         {
             tmp = (index-1) * 3;
             pixels_array[tmp] = green;
             pixels_array[tmp+1] = red;
             pixels_array[tmp+2] = blue;
             return TRUE;
         }
    }
    return FALSE;
}

boolean setColorAll(uint8_t red,uint8_t green,uint8_t blue)
{
    setColor(0,red,green,blue);
    return TRUE;
}

void led_reset(void)
{
    delayMicroseconds(100);
}


void rgb_clear(void)
{
    uint8_t curbyte;
   
    for(curbyte = 0;curbyte < (3*MAX_RGB_LED_NUM);curbyte++)
    {
         pixels_array[curbyte] = 0;
    }
	
    rgb_show(MAX_RGB_LED_NUM);
}

void rgb_show(uint8_t led_quantity)
{
    uint8_t cur_pixel = 0;
    uint16_t b_index = 0;
    uint16_t led_pixels_len = 0;
    led_pixels_len = 3*led_quantity;
    s_led_bits_num = led_pixels_len*8;
    for(int i= 0; i < led_pixels_len; i++)
    {
        b_index=i*8;
        cur_pixel = pixels_array[i];
        for(uint8_t j=0; j<8; j++)
        {	
            if((cur_pixel&0x80)==0x80)
            {		
                pixels_bits_array[b_index+j]=  s_bit_1_duty;
            }
            else
            {
                pixels_bits_array[b_index+j]= s_bit_0_duty;		
            }
            cur_pixel=cur_pixel<<1;
        }
    }
    LED_PWM_DUTY_REG = 0x66; // > PERIDO, can not generate wave form in the first tow cycle.
    PWM_ClearPeriodIntFlag(PWM, LED_PWM_CHANNEL);
    PWM_EnablePeriodInt(PWM, LED_PWM_CHANNEL);
    NVIC_EnableIRQ(PWM_IRQn);
    PWM->CTL |= (1 << (LED_PWM_CHANNEL<<2)); // PWM output start.
    while(s_led_bit_count < s_led_bits_num)
    {
        
    }
    s_led_bit_count = 0;
    
    led_reset();
}

void led_chain_all_set(uint8_t led_quantity, uint8_t* led_array)
{
    uint8_t cur_byte, cur_led;
   
    rgb_clear();
	
    if(led_quantity > MAX_RGB_LED_NUM)
    {
        led_quantity = MAX_RGB_LED_NUM;
    }
    for(cur_byte = 0; cur_byte < (3*led_quantity); cur_byte++)
    {
        pixels_array[cur_byte] = 0;
    }
	
    for(cur_led = 0; cur_led < led_quantity; cur_led++)
    {
        cur_byte = 3*cur_led;
        pixels_array[cur_byte] = light_panel[led_array[cur_led]].g;
        pixels_array[cur_byte + 1] = light_panel[led_array[cur_led]].r;
        pixels_array[cur_byte + 2] = light_panel[led_array[cur_led]].b;
    }
    rgb_show(led_quantity);
}

void PWM_IRQHandler(void)
{
    if(s_led_bit_count < s_led_bits_num)
    {					
        *s_duty_set_address =pixels_bits_array[s_led_bit_count];	  
        s_led_bit_count++;
    }
    else
    {
        
        PWM->CTL &= LED_PWM_CNT_DISABLE;
    }
    PWM_CLEAR_INT_FLAG;
}


void Set_SevenColor_Regular(uint8_t sensor_value)
{
    RGB_STRUCT rgb_value;
    
    DEBUG_MSG(UART1,"sensor_value=%d\r\n",sensor_value);
    rgb_value=Lum_To_RGB(COLOR_WHITE_INDEX,sensor_value);
    setColor(1, rgb_value.red, rgb_value.green, rgb_value.blue);
 
    for(int i=0;i<MAX_RGB_LED_NUM-1;i=i+7)
    {
        rgb_value=Lum_To_RGB(COLOR_RED_INDEX,sensor_value);
        setColor(i+2, rgb_value.red, rgb_value.green, rgb_value.blue);
        
        rgb_value=Lum_To_RGB(COLOR_ORANGE_INDEX,sensor_value);
        setColor(i+3, rgb_value.red, rgb_value.green, rgb_value.blue);
        
        rgb_value=Lum_To_RGB(COLOR_YELL0_INDEX,sensor_value);
        setColor(i+4, rgb_value.red, rgb_value.green, rgb_value.blue);
        
        rgb_value=Lum_To_RGB(COLOR_GREEN_INDEX,sensor_value);
        setColor(i+5, rgb_value.red, rgb_value.green, rgb_value.blue);
        
        rgb_value=Lum_To_RGB(COLOR_CYAN_INDEX,sensor_value);
        setColor(i+6, rgb_value.red, rgb_value.green, rgb_value.blue);
        
        rgb_value=Lum_To_RGB(COLOR_BLUE_INDEX,sensor_value);
        setColor(i+7, rgb_value.red, rgb_value.green, rgb_value.blue);
        
        rgb_value=Lum_To_RGB(COLOR_PURPLE_INDEX,sensor_value);
        setColor(i+8, rgb_value.red, rgb_value.green, rgb_value.blue);
    }
    
    rgb_show(MAX_RGB_LED_NUM);
}

static RGB_STRUCT   Lum_To_RGB(COLORS_INDEX color_type,uint8_t sensor_value)
{
    RGB_STRUCT return_fgb_value;
  
    return_fgb_value=LED_Chain_Color_Table[color_type].lum_to_rgg_value_fun(LED_Chain_Color_Table[color_type].color_baae_value,sensor_value);

return return_fgb_value;
}

static RGB_STRUCT Color_LumToRGB_Red(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_Orange(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_Yellow(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_Green(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_Cyan(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_Blue(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_Purple(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}

static RGB_STRUCT Color_LumToRGB_White(RGB_STRUCT color_base,uint8_t sensor_value)
{
    RGB_STRUCT temp_rgb_value={0,0,0};
    uint8_t temp_scale;
   
    temp_scale=line_map(sensor_value, 0, COLOR_SCALE, 0, 100);
    if(color_base.red>0)
    {
        temp_rgb_value.red=line_map(temp_scale, 0, color_base.red, 0, COLOR_SCALE);
    }
     if(color_base.green>0)
    {
        temp_rgb_value.green=line_map(temp_scale, 0, color_base.green, 0, COLOR_SCALE);
    }
     if(color_base.blue>0)
    {
        temp_rgb_value.blue=line_map(temp_scale, 0, color_base.blue, 0, COLOR_SCALE);
    }
    return temp_rgb_value;
}
