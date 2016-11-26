#include "MeColourSensor.h"
#include "uart_printf.h"
#include <string.h>
#include <math.h>
#include "Interrupt.h"
#include "systime.h"
#include "i2c.h"
#include "mygpio.h"

#define TCS34725_ADDRESS          (0x29)

#define TCS34725_COMMAND_BIT      (0x80)

#define TCS34725_ENABLE           (0x00)
#define TCS34725_ENABLE_AIEN      (0x10)    /* RGBC Interrupt Enable */
#define TCS34725_ENABLE_WEN       (0x08)    /* Wait enable - Writing 1 activates the wait timer */
#define TCS34725_ENABLE_AEN       (0x02)    /* RGBC Enable - Writing 1 actives the ADC, 0 disables it */
#define TCS34725_ENABLE_PON       (0x01)    /* Power on - Writing 1 activates the internal oscillator, 0 disables it */
#define TCS34725_ATIME            (0x01)    /* Integration time */
#define TCS34725_WTIME            (0x03)    /* Wait time (if TCS34725_ENABLE_WEN is asserted) */
#define TCS34725_WTIME_2_4MS      (0xFF)    /* WLONG0 = 2.4ms   WLONG1 = 0.029s */
#define TCS34725_WTIME_204MS      (0xAB)    /* WLONG0 = 204ms   WLONG1 = 2.45s  */
#define TCS34725_WTIME_614MS      (0x00)    /* WLONG0 = 614ms   WLONG1 = 7.4s   */
#define TCS34725_AILTL            (0x04)    /* Clear channel lower interrupt threshold */
#define TCS34725_AILTH            (0x05)
#define TCS34725_AIHTL            (0x06)    /* Clear channel upper interrupt threshold */
#define TCS34725_AIHTH            (0x07)
#define TCS34725_PERS             (0x0C)    /* Persistence register - basic SW filtering mechanism for interrupts */
#define TCS34725_PERS_NONE        (0b0000)  /* Every RGBC cycle generates an interrupt                                */
#define TCS34725_PERS_1_CYCLE     (0b0001)  /* 1 clean channel value outside threshold range generates an interrupt   */
#define TCS34725_PERS_2_CYCLE     (0b0010)  /* 2 clean channel values outside threshold range generates an interrupt  */
#define TCS34725_PERS_3_CYCLE     (0b0011)  /* 3 clean channel values outside threshold range generates an interrupt  */
#define TCS34725_PERS_5_CYCLE     (0b0100)  /* 5 clean channel values outside threshold range generates an interrupt  */
#define TCS34725_PERS_10_CYCLE    (0b0101)  /* 10 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_15_CYCLE    (0b0110)  /* 15 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_20_CYCLE    (0b0111)  /* 20 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_25_CYCLE    (0b1000)  /* 25 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_30_CYCLE    (0b1001)  /* 30 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_35_CYCLE    (0b1010)  /* 35 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_40_CYCLE    (0b1011)  /* 40 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_45_CYCLE    (0b1100)  /* 45 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_50_CYCLE    (0b1101)  /* 50 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_55_CYCLE    (0b1110)  /* 55 clean channel values outside threshold range generates an interrupt */
#define TCS34725_PERS_60_CYCLE    (0b1111)  /* 60 clean channel values outside threshold range generates an interrupt */
#define TCS34725_CONFIG           (0x0D)
#define TCS34725_CONFIG_WLONG     (0x02)    /* Choose between short and long (12x) wait times via TCS34725_WTIME */
#define TCS34725_CONTROL          (0x0F)    /* Set the gain level for the sensor */
#define TCS34725_ID               (0x12)    /* 0x44 = TCS34721/TCS34725, 0x4D = TCS34723/TCS34727 */
#define TCS34725_STATUS           (0x13)
#define TCS34725_STATUS_AINT      (0x10)    /* RGBC Clean channel interrupt */
#define TCS34725_STATUS_AVALID    (0x01)    /* Indicates that the RGBC channels have completed an integration cycle */
#define TCS34725_CDATAL           (0x14)    /* Clear channel data */
#define TCS34725_CDATAH           (0x15)
#define TCS34725_RDATAL           (0x16)    /* Red channel data */
#define TCS34725_RDATAH           (0x17)
#define TCS34725_GDATAL           (0x18)    /* Green channel data */
#define TCS34725_GDATAH           (0x19)
#define TCS34725_BDATAL           (0x1A)    /* Blue channel data */
#define TCS34725_BDATAH           (0x1B)

typedef enum
{
  TCS34725_INTEGRATIONTIME_2_4MS  = 0xFF,   /**<  2.4ms - 1 cycle    - Max Count: 1024  */
  TCS34725_INTEGRATIONTIME_24MS   = 0xF6,   /**<  24ms  - 10 cycles  - Max Count: 10240 */
  TCS34725_INTEGRATIONTIME_50MS   = 0xEB,   /**<  50ms  - 20 cycles  - Max Count: 20480 */
  TCS34725_INTEGRATIONTIME_101MS  = 0xD5,   /**<  101ms - 42 cycles  - Max Count: 43008 */
  TCS34725_INTEGRATIONTIME_154MS  = 0xC0,   /**<  154ms - 64 cycles  - Max Count: 65535 */
  TCS34725_INTEGRATIONTIME_700MS  = 0x00    /**<  700ms - 256 cycles - Max Count: 65535 */
}
tcs34725IntegrationTime_t;

typedef enum
{
  TCS34725_GAIN_1X                = 0x00,   /**<  No gain  */
  TCS34725_GAIN_4X                = 0x01,   /**<  4x gain  */
  TCS34725_GAIN_16X               = 0x02,   /**<  16x gain */
  TCS34725_GAIN_60X               = 0x03    /**<  60x gain */
}
tcs34725Gain_t;

uint8_t i2c_tx_array[3];
uint8_t i2c_rx_array[14];
volatile uint8_t i2c_tx_data_len;
volatile uint8_t i2c_rx_data_len;
volatile uint8_t g_i2c_end_flag = 0;
volatile uint8_t g_device_addr = 0;

static uint8_t gammatable[255] = {0};

void I2C_Init(I2C_T *i2c)
{
	if(i2c == I2C0)
	{
		SYS->P3_MFP &= ~(SYS_MFP_P34_Msk | SYS_MFP_P35_Msk); 	
		SYS->P3_MFP |= (SYS_MFP_P34_I2C0_SDA | SYS_MFP_P35_I2C0_SCL);    
	
		CLK_EnableModuleClock(I2C0_MODULE);
		I2C_Open(I2C0, 200000);
		
		/* Enable I2C interrupt */
		I2C_EnableInt(I2C0);
		NVIC_EnableIRQ(I2C0_IRQn);
	}
	else if(i2c == I2C1)
	{
		SYS->P2_MFP &= ~(SYS_MFP_P22_Msk | SYS_MFP_P23_Msk); 	
		SYS->P2_MFP |= (SYS_MFP_P22_I2C1_SCL | SYS_MFP_P23_I2C1_SDA);    
	
		CLK_EnableModuleClock(I2C1_MODULE);
		I2C_Open(I2C1, 100000);

		/* Enable I2C interrupt */
		I2C_EnableInt(I2C1);
		NVIC_EnableIRQ(I2C1_IRQn);
		
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    static uint8_t rxcount = 0;
    if(u32Status == 0x08)                       /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C0,(g_device_addr<<1) + 0);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C0,i2c_tx_array[0]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STA | I2C_SI); 
    }
    else if(u32Status == 0x10)                  /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C0, (g_device_addr<<1) + 1); /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x40)                  /* SLA+R has been transmitted and ACK has been received */
    {
		if(i2c_rx_data_len == 1)
		{
			I2C_SET_CONTROL_REG(I2C0, I2C_SI);
		}
		else if(i2c_rx_data_len >1)
		{
			I2C_SET_CONTROL_REG(I2C0, I2C_SI|I2C_AA);
		}
    }
	else if(u32Status == 0x50)
	{
		if(rxcount == i2c_rx_data_len - 1)
        {
            i2c_rx_array[rxcount] = (uint8_t) I2C_GET_DATA(I2C0);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
            rxcount++;
        }
		else
		{
            i2c_rx_array[rxcount] = (uint8_t) I2C_GET_DATA(I2C0);
			rxcount++;
			if(rxcount == i2c_rx_data_len - 1)
			{
				I2C_SET_CONTROL_REG(I2C0, I2C_SI);
			}
			else
			{
				I2C_SET_CONTROL_REG(I2C0, I2C_SI|I2C_AA);
			}
            
		}
	}
    else if(u32Status == 0x58)          
        /* DATA has been received and NACK has been returned */
    {
        i2c_rx_array[rxcount]  = (unsigned char) I2C_GET_DATA(I2C0);
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
        g_i2c_end_flag = 1;
        rxcount = 0;
    }
    else
    {
        //TO DO.
		I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
		DEBUG_MSG(UART0, "unexpected status:%d\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    static uint8_t txcount = 0;
    if(u32Status == 0x08)                       /* START has been transmitted */
    {
        I2C_SET_DATA(I2C0, g_device_addr<<1);      /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    { 
        I2C_SET_DATA(I2C0, i2c_tx_array[txcount++]);
        I2C_SET_CONTROL_REG(I2C0, I2C_SI);

    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(txcount != i2c_tx_data_len)
        {
            I2C_SET_DATA(I2C0, i2c_tx_array[txcount++]);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
            txcount = 0;
            g_i2c_end_flag = 1;
        }
    }
    else
    {
        // TODO.
		I2C_SET_CONTROL_REG(I2C0, I2C_STO | I2C_SI);
		DEBUG_MSG(UART0, "unexpected status:%d, ctl:%d\n", u32Status, I2C0->CTL);
    }
}

void I2C_Read(uint8_t data_addr,uint8_t* data, uint8_t length)
{
    i2c_rx_data_len = length;
    
    g_i2c_end_flag = 0;
    i2c_tx_array[0] = TCS34725_COMMAND_BIT | data_addr;
    /* I2C function to read data from slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterRx;
	I2C_SET_CONTROL_REG(I2C0, I2C_STA);
	
    /* Wait I2C Rx Finish */
    while(g_i2c_end_flag == 0)
	{
		if(whileCoutOver_us(2000))
		{
			return;
		}
	}
	clearWhileCount();
    memcpy(data, i2c_rx_array, i2c_rx_data_len);
}


void I2C_Write(uint8_t address,uint8_t data)
{
    g_device_addr = TCS34725_ADDRESS;
    i2c_tx_array[0] = TCS34725_COMMAND_BIT | address;
    i2c_tx_array[1] = data;
    i2c_tx_data_len = 2;
    g_i2c_end_flag = 0;

    /* I2C function to write data to slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;
    /* I2C as master sends START signal */
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);
    /* Wait I2C Tx Finish */
    while(g_i2c_end_flag == 0)
	{
		if(whileCoutOver_us(2000))
		{
			return;
		}
	}
	clearWhileCount();
}


/**************************************************************************/
float powf(const float x, const float y)
{
  return (float)(pow((double)x, (double)y));
}


/**************************************************************************/
/*!
    Enables the device
*/
/**************************************************************************/
void TCS34725_enable(void)
{
  I2C_Write(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  delay(3);
  I2C_Write(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}

/**************************************************************************/
/*!
    Disables the device (putting it in lower power sleep mode)
*/
/**************************************************************************/
void TCS3472_disable(void)
{
  /* Turn the device off to save power */
  uint8_t reg = 0;
  I2C_Read(TCS34725_ENABLE, &reg, 1);
  I2C_Write(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

/**************************************************************************/
/*!
    Sets the integration time for the TC34725
*/
/**************************************************************************/
void TCS34725_setIntegrationTime(tcs34725IntegrationTime_t it)
{
  /* Update the timing register */
  I2C_Write(TCS34725_ATIME, it);
}

/**************************************************************************/
/*!
    Adjusts the gain on the TCS34725 (adjusts the sensitivity to light)
*/
/**************************************************************************/
void TCS34725_setGain(tcs34725Gain_t gain)
{
	/* Update the timing register */
	I2C_Write(TCS34725_CONTROL, gain);
}

/**************************************************************************/
/*!
    @brief  Reads the raw red, green, blue and clear channel values
*/
/**************************************************************************/
void TCS34725_getRawData (uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  I2C_Read(TCS34725_CDATAL, (uint8_t  *)c, 2);
  I2C_Read(TCS34725_RDATAL, (uint8_t  *)r, 2);
  I2C_Read(TCS34725_GDATAL, (uint8_t *)g, 2);
  I2C_Read(TCS34725_BDATAL, (uint8_t *)b, 2);
}

struct colour_sensor_data colour_sensor_read(void)
{
	struct colour_sensor_data ret_data;
	uint16_t r_value, g_value, b_value, c_value;
	uint8_t temp_r, temp_g, temp_b;
	uint8_t id;
	I2C_Read(TCS34725_ID, &id, 1);
	TCS34725_getRawData(&r_value, &g_value, &b_value, &c_value);
	temp_r = r_value*255/c_value;
	temp_g = g_value*255/c_value;
	temp_b = b_value*255/c_value;
	
	ret_data.r_value = gammatable[temp_r];
	ret_data.g_value = gammatable[temp_g] - 6;
	ret_data.b_value = gammatable[temp_b];
	//DEBUG_MSG(UART0, "r_value:%d, g_value:%d, b_value:%d, c_value:%d\n", ret_data.r_value, ret_data.g_value, ret_data.b_value, c_value);
	return ret_data;
}

void colour_sensor_init(void)
{
	for (int i=0; i<256; i++) 
	{
		float x = i;
		x /= 255;
		x = pow(x, 2.5);
		x *= 255;
		gammatable[i] = x;
    }
    I2C_Init(I2C0);
	TCS34725_setIntegrationTime(TCS34725_INTEGRATIONTIME_24MS);
	TCS34725_setGain(TCS34725_GAIN_4X);

	/* Note: by default, the device is in power down mode on bootup */
	TCS34725_enable();
	
	pwm_write(P0_4, 1000, 2);
}
