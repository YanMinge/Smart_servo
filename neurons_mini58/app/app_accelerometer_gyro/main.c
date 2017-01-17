/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/05/19 11:45a $
 * @brief    for neuron product
 *
 * @note
 * Copyright (C) 2015 myan@makeblock.cc. All rights reserved.
 *
 ******************************************************************************/
#include "sysinit.h"
#include <stdio.h>
#include "uart_printf.h"
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"
#include "MeMPU6050.h"
#include "block_param.h"
#include "string.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Macro defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define FIRMWARE_VERSION    003 // for firmware version.

#define CTL_SUBCRIBE_DATA                 0x01
#define CTL_UNSUBCRIBE_DATA               0x02

#define REPORT_TYPE_SHAKE                 0x01
#define REPORT_TYPE_ACC_X                 0x02
#define REPORT_TYPE_ACC_Y                 0x03
#define REPORT_TYPE_ACC_Z                 0x04
#define REPORT_TYPE_GYRO_X                0x05
#define REPORT_TYPE_GYRO_Y                0x06
#define REPORT_TYPE_GYRO_Z                0x07
#define REPORT_TYPE_ANGLE_X               0x08
#define REPORT_TYPE_ANGLE_Y               0x09
#define REPORT_TYPE_ANGLE_Z               0x0a


#define SAMPLE_PERIOD					2

/*---------------------------------------------------------------------------------------------------------*/
/* Local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
union data_store{
    uint8_t data_buf[4];
    int8_t byte_val;
    int16_t int_val;
	float   float_val;
};

typedef struct {
    uint32_t report_period;
    uint32_t sample_period;
    uint32_t pre_report_time;
    uint32_t pre_sample_time;
    void* value;
    union data_store current_value;
    union data_store pre_value;
    uint8_t report_mode;
    uint8_t report_type;
    uint8_t data_type;
}MPU_DATA_PROPERTY;

static MPU_DATA_PROPERTY s_mpu_data_property[10] = {0};
static uint32_t s_offline_start_mills = 0; // used for period reporting in offline mode
static uint32_t s_sample_start_millis = 0; // used for period sampling.

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile unsigned long system_time = 0;
uint16_t g_firmware_version = FIRMWARE_VERSION;
struct mpu6050_data mpu6050_data_value;

/*---------------------------------------------------------------------------------------------------------*/
/* local Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
static void mpu_data_property_init(void);
static uint8_t len_of_data(uint8_t type);
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_online(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
	
    init_neuron_system();
	
    init_block( );

    while(1)
    {
        // protocol.
        parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();
		
        // led.
        poll_led_request();
		
        get_sensor_data();
		
        // on line.
        if(g_block_no != 0) 
        {
            send_sensor_report_online();
        }
		
        // off line.
        else if(g_block_no == 0)
        {
			uint32_t current_millis = millis();
            if((current_millis - s_offline_start_mills) > OFF_LINE_REPORT_PERIOD)
            {
                s_offline_start_mills = current_millis;
                send_sensor_report_offline();
            }
        }
    }
}


void init_block(void)
{
    g_block_type = CLASS_SENSOR;
    g_block_sub_type = BLOCK_ACCELEROMETER_GYRO;
	
    mpu6050_init();
	mpu_data_property_init();
    uart0_recv_attach(sysex_process_online, NULL);
	
    set_rgb_led(0, 0, 128);
}
	
void get_sensor_data(void)
{
    uint32_t current_millis = millis();
    if((current_millis - s_sample_start_millis) > SAMPLE_PERIOD)
    {
        s_sample_start_millis = current_millis;
        ReadMpu6050(&mpu6050_data_value);
    }
}

static void report_sensor_value_on_line(uint8_t type)
{
    uint8_t data_type, report_type, i;
    init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
    add_sysex_data(BYTE_8, &type, ON_LINE);
    
    for(i = 0; i < (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY)); i++)
    {
        if(s_mpu_data_property[i].report_type == type)
        {
            data_type = s_mpu_data_property[i].data_type;
            report_type = s_mpu_data_property[i].report_type;
            add_sysex_data(BYTE_8, &report_type, ON_LINE);
            add_sysex_data(data_type, s_mpu_data_property[i].value, ON_LINE);
            
            break;
        }
    }
    if(i == (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY))) // not find type.
    {
        return;
    }
    flush_sysex_to_send(ON_LINE);
}

void send_sensor_report_online(void)
{	
    uint8_t data_type, report_type;
    uint32_t current_millis;
    uint8_t add_head_flag  = 0;
    uint8_t i = 0;
    current_millis = millis();
    
    for(i = 0; i < (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY)); i++)
    {
        if(s_mpu_data_property[i].report_mode == REPORT_MODE_CYCLE)
        {
           if((current_millis - s_mpu_data_property[i].pre_report_time) > s_mpu_data_property[i].report_period) 
           {
               s_mpu_data_property[i].pre_report_time = current_millis;
               if(add_head_flag == 0)
               {
                   init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
                   add_head_flag = 1;
               }
               
               data_type = BYTE_8;
               report_type = s_mpu_data_property[i].report_type;
               add_sysex_data(data_type, &report_type, ON_LINE);
               data_type = s_mpu_data_property[i].data_type;
               add_sysex_data(data_type, s_mpu_data_property[i].value, ON_LINE);     
           }
        }
        else if(s_mpu_data_property[i].report_mode == REPORT_MODE_DIFF)
        {
            if((current_millis - s_mpu_data_property[i].pre_sample_time) > s_mpu_data_property[i].sample_period) 
            {
               s_mpu_data_property[i].pre_sample_time = current_millis;
                
               memset(&(s_mpu_data_property[i].current_value), 0, sizeof(s_mpu_data_property[i].current_value));
               memcpy(&(s_mpu_data_property[i].current_value), s_mpu_data_property[i].value, len_of_data(s_mpu_data_property[i].data_type));
               if(s_mpu_data_property[i].data_type == FLOAT_40)
               {
                   if(abs(s_mpu_data_property[i].current_value.float_val - s_mpu_data_property[i].pre_value.float_val) > 0.05)
                   {
                       if(add_head_flag == 0)
                       {
                           init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
                           add_head_flag = 1;
                       }
                       memcpy(&(s_mpu_data_property[i].pre_value), &(s_mpu_data_property[i].current_value), sizeof(s_mpu_data_property[i].pre_value));
                       data_type = BYTE_8;
                       report_type = s_mpu_data_property[i].report_type;
                       add_sysex_data(data_type, &report_type, ON_LINE);
                       data_type = s_mpu_data_property[i].data_type;
                       add_sysex_data(data_type, s_mpu_data_property[i].value, ON_LINE);     
                   }
               }
               else if(s_mpu_data_property[i].data_type == BYTE_8)
               {
                   if(s_mpu_data_property[i].current_value.byte_val != s_mpu_data_property[i].pre_value.byte_val)
                   {
                       if(add_head_flag == 0)
                       {
                           init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
                           add_head_flag = 1;
                       }
                       memcpy(&(s_mpu_data_property[i].pre_value), &(s_mpu_data_property[i].current_value), sizeof(s_mpu_data_property[i].pre_value));
                       data_type = BYTE_8;
                       report_type = s_mpu_data_property[i].report_type;
                       add_sysex_data(data_type, &report_type, ON_LINE);
                       data_type = s_mpu_data_property[i].data_type;
                       add_sysex_data(data_type, s_mpu_data_property[i].value, ON_LINE);  
                   }
               }
               else if(s_mpu_data_property[i].data_type == SHORT_24)
               {
                   if(s_mpu_data_property[i].current_value.int_val != s_mpu_data_property[i].pre_value.int_val)
                   {
                       if(add_head_flag == 0)
                       {
                           init_sysex_to_send(g_block_type, g_block_sub_type, ON_LINE);
                           add_head_flag = 1;
                       }
                       memcpy(&(s_mpu_data_property[i].pre_value), &(s_mpu_data_property[i].current_value), sizeof(s_mpu_data_property[i].pre_value));
                       data_type = BYTE_8;
                       report_type = s_mpu_data_property[i].report_type;
                       add_sysex_data(data_type, &report_type, ON_LINE);
                       data_type = s_mpu_data_property[i].data_type;
                       add_sysex_data(data_type, s_mpu_data_property[i].value, ON_LINE);  
                   }
               }
               else
               {
                   return;
               }
           }
            
        }
    }
  
    if(add_head_flag)
    {
        flush_sysex_to_send(ON_LINE);
    }
}

void send_sensor_report_offline(void)
{
	uint16_t uniform_value = 0;
	
	init_sysex_to_send(g_block_type, g_block_sub_type, OFF_LINE);
	
	add_sysex_data(SHORT_24, &(mpu6050_data_value.x_angle), OFF_LINE);
	add_sysex_data(SHORT_24, &(mpu6050_data_value.y_angle), OFF_LINE);
	add_sysex_data(SHORT_24, &(mpu6050_data_value.z_angle), OFF_LINE);
	add_sysex_data(UNIFORM_16, &uniform_value, OFF_LINE);
	
	flush_sysex_to_send(OFF_LINE);
}

void sysex_process_online(void)
{
    uint8_t block_type = 0;
    uint8_t sub_type = 0;
    uint8_t data_type = 0;
    uint8_t cmd_type = 0;
    uint8_t report_type;
    uint8_t report_mode = 0;
    uint8_t i = 0;
    uint32_t report_period;
    uint32_t sample_period;
    
    // read block type.
    read_sysex_type(&block_type , &sub_type, ON_LINE);
    if((block_type != g_block_type) ||(sub_type != g_block_sub_type))
    {
        send_sysex_error_code(WRONG_TYPE);
        return;
    }
	
    // read command type.
    data_type = BYTE_8;
    if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE) == false)
    {
        return;
    }
    switch(cmd_type)
    {
        case CTL_SUBCRIBE_DATA:
            data_type = BYTE_8;
            if(read_next_sysex_data(&data_type, &report_type, ON_LINE) == false)
            {
                return;
            }
            // the type is wrong.
            if(report_type > (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY)))
            {
                return;
            }
            data_type = BYTE_8;
            if(read_next_sysex_data(&data_type, &report_mode, ON_LINE) == false)
            {
                return;
            }
            if(report_mode == REPORT_MODE_REQ)
            {
                report_sensor_value_on_line(report_type);
            }
            else if(report_mode == REPORT_MODE_CYCLE)
            {
                data_type = LONG_40;
                if(read_next_sysex_data(&data_type, &report_period, ON_LINE) == false)
                {
                    return;
                }
                
                for(i = 0; i < (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY)); i++)
                {
                    if(s_mpu_data_property[i].report_type == report_type)
                    {
                        s_mpu_data_property[i].report_period = report_period;
                        s_mpu_data_property[i].report_mode = report_mode;
                    }
                }
            }
            else if(report_mode == REPORT_MODE_DIFF)
            {
                data_type = LONG_40;
                if(read_next_sysex_data(&data_type, &sample_period, ON_LINE) == false)
                {
                    return;
                }
                
                for(i = 0; i < (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY)); i++)
                {
                    if(s_mpu_data_property[i].report_type == report_type)
                    {
                        s_mpu_data_property[i].sample_period = sample_period;
                         s_mpu_data_property[i].report_mode = report_mode;
                    }
                }
            }
            else
            {
                return;
            }
        break;
        case CTL_UNSUBCRIBE_DATA:
            data_type = BYTE_8;
            if(read_next_sysex_data(&data_type, &report_type, ON_LINE) == false)
            {
                return;
            }
            for(i = 0; i < (sizeof(s_mpu_data_property)/sizeof(MPU_DATA_PROPERTY)); i++)
            {
                if(s_mpu_data_property[i].report_type == report_type)
                {
                    s_mpu_data_property[i].report_mode = REPORT_MODE_REQ;
                }
            }
        default:
        break;
    }

}

static void mpu_data_property_init(void)
{
    s_mpu_data_property[0].data_type = BYTE_8;
    s_mpu_data_property[0].pre_report_time = 0;
    s_mpu_data_property[0].report_mode = REPORT_MODE_DIFF;
    s_mpu_data_property[0].report_period = 0;
    s_mpu_data_property[0].pre_sample_time = 40;
    s_mpu_data_property[0].report_type = REPORT_TYPE_SHAKE;
    s_mpu_data_property[0].value = &(mpu6050_data_value.shake_flag);
    
    s_mpu_data_property[1].data_type = FLOAT_40;
    s_mpu_data_property[1].pre_report_time = 0;
    s_mpu_data_property[1].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[1].report_type = REPORT_TYPE_ACC_X;
    s_mpu_data_property[1].report_period = 0;
    s_mpu_data_property[1].value = &(mpu6050_data_value.x_accel_value);
    
    s_mpu_data_property[2].data_type = FLOAT_40;
    s_mpu_data_property[2].pre_report_time = 0;
    s_mpu_data_property[2].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[2].report_type = REPORT_TYPE_ACC_Y;
    s_mpu_data_property[2].report_period = 0;
    s_mpu_data_property[2].value = &(mpu6050_data_value.y_accel_value);
    
    s_mpu_data_property[3].data_type = FLOAT_40;
    s_mpu_data_property[3].pre_report_time = 0;
    s_mpu_data_property[3].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[3].report_type = REPORT_TYPE_ACC_Z;
    s_mpu_data_property[3].report_period = 0;
    s_mpu_data_property[3].value = &(mpu6050_data_value.z_accel_value);
    
    s_mpu_data_property[4].data_type = FLOAT_40;
    s_mpu_data_property[4].pre_report_time = 0;
    s_mpu_data_property[4].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[4].report_type = REPORT_TYPE_GYRO_X;
    s_mpu_data_property[4].report_period = 0;
    s_mpu_data_property[4].value = &(mpu6050_data_value.x_gyro_value);
    
    s_mpu_data_property[5].data_type = FLOAT_40;
    s_mpu_data_property[5].pre_report_time = 0;
    s_mpu_data_property[5].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[5].report_period = 0;
    s_mpu_data_property[5].report_type = REPORT_TYPE_GYRO_Y;
    s_mpu_data_property[5].value = &(mpu6050_data_value.y_gyro_value);
    
    s_mpu_data_property[6].data_type = FLOAT_40;
    s_mpu_data_property[6].pre_report_time = 0;
    s_mpu_data_property[6].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[6].report_period = 0;
    s_mpu_data_property[6].report_type = REPORT_TYPE_GYRO_Z;
    s_mpu_data_property[6].value = &(mpu6050_data_value.z_gyro_value);
    
    s_mpu_data_property[7].data_type = SHORT_24;
    s_mpu_data_property[7].pre_report_time = 0;
    s_mpu_data_property[7].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[7].report_period = 0;
    s_mpu_data_property[7].report_type = REPORT_TYPE_ANGLE_X;
    s_mpu_data_property[7].value = &(mpu6050_data_value.x_angle);
    
    s_mpu_data_property[8].data_type = SHORT_24;
    s_mpu_data_property[8].pre_report_time = 0;
    s_mpu_data_property[8].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[8].report_type = REPORT_TYPE_ANGLE_Y;
    s_mpu_data_property[8].report_period = 0;
    s_mpu_data_property[8].value = &(mpu6050_data_value.y_angle);
    
    s_mpu_data_property[9].data_type = SHORT_24;
    s_mpu_data_property[9].pre_report_time = 0;
    s_mpu_data_property[9].report_mode = REPORT_MODE_REQ;
    s_mpu_data_property[8].report_type = REPORT_TYPE_ANGLE_Z;
    s_mpu_data_property[9].report_period = 0;
    s_mpu_data_property[9].value = &(mpu6050_data_value.z_angle);
}

static uint8_t len_of_data(uint8_t type)
{
    uint8_t ret_value = 0;
    
    switch(type)
    {
        case  BYTE_8:
            ret_value = 1;
        break;
        case SHORT_24:
            ret_value = 2;
        break;
        case FLOAT_40:
            ret_value = 4;
        break;
        default:
        break;
    }
    return ret_value;
}
