#include "MeSht2x.h"
#include "systime.h"
#include <string.h>
#include "uart_printf.h"
#include "interrupt.h"

uint8_t s_i2c_tx_array[4];
uint8_t s_i2c_rx_array[14];
volatile uint8_t i2c_tx_data_len;
volatile uint8_t i2c_rx_data_len;
volatile uint8_t g_i2c_end_flag = 0;
volatile uint8_t g_device_addr = SHT2xADDR;

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
        I2C_SET_DATA(I2C0, s_i2c_tx_array[0]);
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
            s_i2c_rx_array[rxcount] = (uint8_t) I2C_GET_DATA(I2C0);
            I2C_SET_CONTROL_REG(I2C0, I2C_SI);
            rxcount++;
        }
		else
		{
            s_i2c_rx_array[rxcount] = (uint8_t) I2C_GET_DATA(I2C0);
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
        s_i2c_rx_array[rxcount]  = (unsigned char) I2C_GET_DATA(I2C0);
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
        I2C_SET_DATA(I2C0, s_i2c_tx_array[txcount++]);
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
            I2C_SET_DATA(I2C0, s_i2c_tx_array[txcount++]);
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
    s_i2c_tx_array[0] = data_addr;
    /* I2C function to read data from slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterRx;
	I2C_SET_CONTROL_REG(I2C0, I2C_STA);
	
    uint32_t timeout= millis()+300;
    /* Wait I2C Rx Finish */
    while(g_i2c_end_flag == 0)
	{
		if(millis() > timeout)
        {
            return;
        }
	}
    memcpy(data, s_i2c_rx_array, i2c_rx_data_len);
}


void I2C_Write(uint8_t* data, uint8_t len)
{
    memcpy(s_i2c_tx_array, data, len);
    i2c_tx_data_len = len;
    g_i2c_end_flag = 0;

    /* I2C function to write data to slave */
    s_I2C0HandlerFn = (I2C_FUNC)I2C_MasterTx;
    /* I2C as master sends START signal */
    I2C_SET_CONTROL_REG(I2C0, I2C_STA);
    /* Wait I2C Tx Finish */
    uint32_t timeout= millis() + 2;
    while(g_i2c_end_flag == 0)
	{
		if(millis() > timeout)
		{
			return;
		}
	}
}

//static uint8_t readUserRegister()
//{
//    uint8_t user_register_value;
//    I2C_Read(USER_REG_R, &user_register_value, 1);
//    return user_register_value;
//}

//static void writeUserRegister(uint8_t userRegister)
//{
//    uint8_t data[2];
//    data[0] = USER_REG_R;
//    data[1] = userRegister;
//    I2C_Write(data, 2);
//}

static uint16_t readMeasurement(SHT2xMeasureType type)
{
    uint16_t value = 0;
    uint8_t command_data;
    uint8_t read_data[3];
    uint8_t low, high;
    
    //Hold Master
    switch (type) 
    {
        case HUMIDITY:
            command_data = MEASUREMENT_RH_HM;
        break;
        case TEMP:
            command_data = MEASUREMENT_T_HM;
        break;
        default:
        break;
    }
     
    I2C_Read(command_data, read_data, 3);
    
    high = read_data[0];
    low = read_data[1];
    value=(uint16_t)high << 8 | low;
    value &= ~0x0003;
    return value;
}

void softReset()
{
    uint8_t command_data;
    I2C_Write(&command_data, 1);
}

//void setHeater(uint8_t on)
//{
//    uint8_t userRegister;
//    userRegister=readUserRegister();
//    if (on)
//    {
//        userRegister=(userRegister&~SHT2x_HEATER_MASK) | SHT2x_HEATER_ON;
//    } 
//    else 
//    {
//        userRegister=(userRegister&~SHT2x_HEATER_MASK) | SHT2x_HEATER_OFF;
//    }
//}

float readRH()
{
    return -6.0+125.0/65536.00*(float)readMeasurement(HUMIDITY);
}
float readT()
{
    return -46.85+175.72/65536.00*(float)readMeasurement(TEMP);
}

void SHT2X_Init(void)
{
    I2C_Init(I2C0);
}
