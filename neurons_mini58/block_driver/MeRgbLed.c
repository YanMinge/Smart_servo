#include "MeRgbLed.h"
#include "MePwm.h"

void rgb_led_set(uint8_t r_value, uint8_t g_value, uint8_t b_value)
{
	pwm_write(R_PIN, r_value, 0, 255);
	pwm_write(G_PIN, g_value, 0, 255);
	pwm_write(B_PIN, b_value, 0, 255);
}
