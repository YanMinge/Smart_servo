#include "MeDht11.h"
#include "Mini58Series.h"
#include "mygpio.h"
#include "systime.h"
#include "uart_printf.h"
#include "timer.h"

void DHT11_Reset(void)
{
    digitalWrite(DHT11_PIN, 0);                    // DQ set to 0
    delay(22);                         // >18ms
    digitalWrite(DHT11_PIN, 1);                   // DQ set to 1
    delayMicroseconds(30);             // wait 20~40 us
}

static boolean DHT11_Wait()
{
	while(digitalRead(DHT11_PIN))		//wait dht11 to low
	{
		if(whileCoutOver_us(100))
		{
			return false;
		}
	}
	clearWhileCount();
	
	while(digitalRead(DHT11_PIN)== 0)     // wait dht11 to high 80us
	{
		if(whileCoutOver_us(160))
		{
			return false;
		}
	}
	clearWhileCount();
	
	while(digitalRead(DHT11_PIN))     // wait data to start.
	{
		if(whileCoutOver_us(160))
		{
			return false;
		}
	}
	clearWhileCount();
	
	return true;
}


//Read byte on 1-wire
boolean DHT11_Read_Byte(unsigned char *byte)
{
    unsigned char i;
	
	*byte = 0;
    for (i=8; i>0; i--)
    {
        while(digitalRead(DHT11_PIN)== 0)
		{
			
			if(whileCoutOver_us(100))
			{
				return false;
			}
		}
		clearWhileCount();
		
		delayMicroseconds(40); 
		if(digitalRead(DHT11_PIN) == 1)
		{
			(*byte) |= 1<<(i-1);
		}
		while(digitalRead(DHT11_PIN))
		{
			if(whileCoutOver_us(120))
			{
				return false;
			}
		}
		clearWhileCount();
    }
	
	return true;
}



static int s_temperature, s_humidity;
static unsigned long lastReadTime = 0;
static unsigned char data[5];

static void readSensor(void)
{
	int i;
	static boolean s_flag = 0;
	unsigned long startTime = millis();
	
	// - Max sample rate DHT11 is 1 Hz   (duty circle 1000 ms)
	if(((startTime - lastReadTime) < 999) && s_flag ==1)
	{
		return;
	}
	s_flag = 1;
	
	lastReadTime = startTime;
	
	DHT11_Reset();
	
	TIMER_DisableInt(TIMER1);
	
	if(DHT11_Wait() == false) //DHT11 response error.
	{
		TIMER_EnableInt(TIMER1);
		return;
	}
	for(i = 0; i < 5; i++)
	{
		if(DHT11_Read_Byte(data + i) == false)
		{
			TIMER_EnableInt(TIMER1);
			return;
		}
	}
	
	if(data[4] == (data[0] + data[1] + data[2] + data[3]))
	{
		s_temperature = data[2];
		s_humidity = data[0];
	}
	
	TIMER_EnableInt(TIMER1);
}

int DHT11_ReadTemp(void)
{
	readSensor();
	return s_temperature;
}

int DHT11_ReadHumidity(void)
{
	readSensor();
	return s_humidity;
}

void DHT11_Init()
{
	pinMode(DHT11_PIN,GPIO_MODE_QUASI);
}
