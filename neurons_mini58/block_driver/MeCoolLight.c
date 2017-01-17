#include "MeCoolLight.h"

static uint8_t s_cool_light_value = 0;

void cool_light_set(uint8_t number, uint8_t on_off)
{
    switch(number)
    {
        case 1:
            s_cool_light_value = (s_cool_light_value&0xfe) | on_off;
            digitalWrite(LIGHT1_EN_PIN, on_off);
        break;
        case 2:
            s_cool_light_value = (s_cool_light_value&0xfd) | (on_off << 1);
            digitalWrite(LIGHT2_EN_PIN, on_off);
        break;
        case 3:
            s_cool_light_value = (s_cool_light_value&0xfb) | (on_off << 2);
            digitalWrite(LIGHT3_EN_PIN, on_off);
        break;
        case 4:
            s_cool_light_value = (s_cool_light_value&0xf7) | (on_off << 3);
            digitalWrite(LIGHT4_EN_PIN, on_off);
        break;
        default:
        break;
    }
    if(s_cool_light_value == 0)
    {
        digitalWrite(COMMON_EN_PIN, 0);
    }
    else
    {
        digitalWrite(COMMON_EN_PIN, 1);
    }
}

void cool_light_init(void)
{
    pinMode(COMMON_EN_PIN, GPIO_MODE_OUTPUT);
    pinMode(LIGHT1_EN_PIN, GPIO_MODE_OUTPUT);
    pinMode(LIGHT2_EN_PIN, GPIO_MODE_OUTPUT);                          
    pinMode(LIGHT3_EN_PIN, GPIO_MODE_OUTPUT);
    pinMode(LIGHT4_EN_PIN, GPIO_MODE_OUTPUT);
    digitalWrite(COMMON_EN_PIN, 0);
    digitalWrite(LIGHT1_EN_PIN, 0);
    digitalWrite(LIGHT2_EN_PIN, 0);
    digitalWrite(LIGHT3_EN_PIN, 0);
    digitalWrite(LIGHT4_EN_PIN, 0);
}
