#ifndef __MELEDCHAIN_H__
#define __MELEDCHAIN_H__

#include "Mini58Series.h"
#include "mygpio.h"

#define MAX_RGB_LED_NUM   30
#define RGB_LED_SLOT1     0
#define RGB_LED_SLOT2     1
#define RGB_LED_SLOT_MAX  2
#define ALL_LED			  0


/*---------------------------------------------------------------------------------------------------------*/
/* GPIO config: should change the value in rgb_init()                                                      */
/*---------------------------------------------------------------------------------------------------------*/
//extern uint8_t rgb_led_num;
extern uint8_t rgb_led_num_slot1;
extern uint8_t rgb_led_num_slot2;
extern uint8_t pixels_slot1[MAX_RGB_LED_NUM*3];
extern uint8_t pixels_slot2[MAX_RGB_LED_NUM*3];
extern volatile uint32_t* di_gpio_addr;

struct rgb_value
{ 
  uint8_t r; 
  uint8_t g; 
  uint8_t b; 
};

void rgb_init(uint8_t led_quantity, uint8_t din_pin);
void rgb_clear(void);
void set_led_quantity(int8_t led_quantity);
void rgb_show(uint8_t led_quantity);

void led_chain_all_set(uint8_t led_quantity, uint8_t* led_array);
boolean setColor(uint8_t index, uint8_t red,uint8_t green,uint8_t blue);
boolean setColorAll(uint8_t red,uint8_t green,uint8_t blue);

#endif
