#ifndef __MEDHT11_H__
#define __MEDHT11_H__

#define DHT11_PIN  P0_4

extern void DHT11_Init(void);
extern int DHT11_ReadTemp(void);
extern int DHT11_ReadHumidity(void);

#endif
