#include "MeTemperature.h"
#include "Mini58Series.h"
#include "mygpio.h"
#include "uart_printf.h"

void DS18B20_Reset(void)//
{
	digitalWrite(DS18B20_PIN, 0);                                // DQ set to 0
    delayMicroseconds(600);                 // >480us
    digitalWrite(DS18B20_PIN, 1);                               // DQ set to 1
    delayMicroseconds(25);                  // wait 15~60 us
    delayMicroseconds(80);                  // wait 60-240 us
}

void DS18B20_Wait()
{
    while(digitalRead(DS18B20_PIN)); 
    while(digitalRead(DS18B20_PIN) ==0);
    delayMicroseconds(80);                // wait 60-240 us
}

//write byte on 1-wire
void DS18B20_Write_byte(unsigned char val)
{
    unsigned char i;
    boolean btmp;
    for (i=8; i>0; i--)                  //
    {
        btmp = val&0x01;
        val = val >> 1;
        if(btmp) /*write "1"*/
        {
            digitalWrite(DS18B20_PIN, 0);
            delayMicroseconds(10);
            digitalWrite(DS18B20_PIN, 1);
            delayMicroseconds(60);
        }
        else /*write "0"*/
        {
            digitalWrite(DS18B20_PIN, 0);
            delayMicroseconds(60);
            digitalWrite(DS18B20_PIN, 1);
            delayMicroseconds(10);
        }
    }
}

//Read byte on 1-wire
unsigned char DS18B20_Read_byte(void)
{
    unsigned char i,val = 0;
    for (i=8; i>0; i--)                  // ???1bit
    {
        val >>= 1;
        digitalWrite(DS18B20_PIN, 1);
        delayMicroseconds(2);
        digitalWrite(DS18B20_PIN, 0);
        delayMicroseconds(6);
        digitalWrite(DS18B20_PIN, 1);
        delayMicroseconds(4);
        if(digitalRead(DS18B20_PIN))	 /*read "1"*/
        {
            val=0x80|val;
        }
        else		/*read "0"*/
        {
            val=0x00|val;
        }
        delayMicroseconds(30);
    }
    return (val);
}

void sendChangeCmd(void)
{
    DS18B20_Reset();
	delayMicroseconds(2000);
    DS18B20_Write_byte(0xCC);
    delayMicroseconds(1);
    DS18B20_Write_byte(0x44);
    delayMicroseconds(1);
}

void sendReadCmd(void)
{
    DS18B20_Reset();
    delayMicroseconds(2000);
    DS18B20_Write_byte(0xCC);
    delayMicroseconds(1);
    DS18B20_Write_byte(0xBE);
    delayMicroseconds(1);
}

float DS18B20_ReadTemp(void)
{
    float real_value;
	int16_t store_value;
    unsigned char low,high;
	
	sendChangeCmd();
    sendReadCmd();
    low = DS18B20_Read_byte();
    delayMicroseconds(1);
    high = DS18B20_Read_byte();
    store_value = high;
    store_value <<= 8;
    store_value |= low;
    real_value = store_value * 0.0625;
    return real_value;
}

void DS18B20_Init(void)
{
	pinMode(DS18B20_PIN, GPIO_MODE_QUASI);
}
