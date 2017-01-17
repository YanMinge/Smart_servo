#include "MeTemperature.h"
#include "Mini58Series.h"
#include "mygpio.h"

static uint8_t s_read_count = 0;

static uint8_t calcrc_1byte(uint8_t one_byte)      
{      
   uint8_t i,crc_one_byte;       
   crc_one_byte=0;   
   for(i = 0; i < 8; i++)      
   {      
        if(((crc_one_byte^one_byte)&0x01))      
        {      
           crc_one_byte^=0x18;       
           crc_one_byte>>=1;      
           crc_one_byte|=0x80;      
        }            
        else
        {
            crc_one_byte>>=1;
        }
        one_byte>>=1;            
    }     
    return crc_one_byte;     
}     

static uint8_t calcrc_bytes(uint8_t *p, uint8_t len)    
{    
    uint8_t crc=0;    
    while(len--)  
    {    
        crc=calcrc_1byte(crc^(*p++));    
    }    
    return crc;    
}

void DS18B20_Reset(void)
{
	digitalWrite(DS18B20_PIN, 0);   // DQ set to 0
    delayMicroseconds(600);         // >480us
    digitalWrite(DS18B20_PIN, 1);   // DQ set to 1
    delayMicroseconds(25);          // wait 15~60 us
    delayMicroseconds(80);          // wait 60-240 us
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
            digitalWrite(DS18B20_PIN, 1);
            delayMicroseconds(2);
            digitalWrite(DS18B20_PIN, 0);
            delayMicroseconds(2);
            digitalWrite(DS18B20_PIN, 1);
            delayMicroseconds(60);
        }
        else /*write "0"*/
        {
            digitalWrite(DS18B20_PIN, 1);
            delayMicroseconds(2);
            digitalWrite(DS18B20_PIN, 0);
            delayMicroseconds(60);
            digitalWrite(DS18B20_PIN, 1); // release the one-wire bus.
            delayMicroseconds(10);
        }
    }
}

//Read byte on 1-wire
unsigned char DS18B20_Read_byte(void)
{
    unsigned char i,val = 0;
    for (i=8; i>0; i--)                  // 8bit
    {
        val >>= 1;
        digitalWrite(DS18B20_PIN, 1);
        delayMicroseconds(2);
        digitalWrite(DS18B20_PIN, 0);
        delayMicroseconds(3);
        digitalWrite(DS18B20_PIN, 1); // for release the bus.
        delayMicroseconds(2);
        if(digitalRead(DS18B20_PIN))	 /*read "1"*/
        {
            val=0x80|val;
        }
        else		/*read "0"*/
        {
            val=0x00|val;
        }
        delayMicroseconds(60);
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

boolean  DS18B20_ReadTemp(float * temperature)
{ 
	int16_t store_value;
	
	sendChangeCmd();
    sendReadCmd();
    
    uint8_t data[9];
    for(int i = 0; i < 9; i++)
    {
        data[i] = DS18B20_Read_byte();
    }
    uint8_t crc = 0;
    crc = calcrc_bytes(data, 8);
    if(crc == data[8])
    {
        s_read_count++;
        if(s_read_count < 3)
        {
            return false;
        }
        store_value = data[1];
        store_value <<= 8;
        store_value |= data[0];
        *temperature = store_value * 0.0625;
        return true;
    }
    else
    {
        // crc check error.
        return false;
    }
   
}

void DS18B20_Init(void)
{
	pinMode(DS18B20_PIN, GPIO_MODE_QUASI); // bidirectoin.
}
