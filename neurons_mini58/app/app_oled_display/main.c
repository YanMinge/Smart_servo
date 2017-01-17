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
#include <string.h>
#include "uart_printf.h"
#include "mygpio.h"
#include "protocol.h"
#include "systime.h"
#include "Interrupt.h"
#include "MeOledDisplay.h"
/*---------------------------------------------------------------------------------------------------------*/
/* Micro defines                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/ 
#define FIRMWARE_VERSION           003
#define CMD_SET_DISPLAY_STRING     0x01
#define CMD_SET_DISPLAY_FACE       0x02

#define LINE_TOP        0x01
#define LINE_MIDDLE     0x02
#define LINE_BOTTOM     0x03

#define ASCII_STRING_TYPE   0x01

#define DISPLAY_STRING  0x01   
#define DISPLAY_FACE    0x02
/*---------------------------------------------------------------------------------------------------------*/
/* local variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t device_type=0;
uint8_t device_sub_type=0;

char data_number_offline_pkg=0;
char offline_update_flag=0;    

uint16_t g_firmware_version = FIRMWARE_VERSION;

// time variables.
uint8_t  flash_blink_count;
uint8_t  flash_change_count;
uint8_t  face_type;
uint8_t  pre_face_type;
uint8_t  eye_blink_flag;              //0x00:no eye blink       0x01:eye blink
uint8_t  eye_blink_refresh_flag;   
uint8_t  blink_one_times_end_flag;
uint8_t  g_display_type;              //0x01:display string     0x02:display face
float    g_display_value=0;

volatile unsigned long system_time = 0;

union offline_data{
    uint8_t data_buf[8];
    uint8_t byte8_val;
    int8_t  byte16_val;
    int16_t short16_val;
    int32_t long32_val;
    float   float32_val;
};

char  display_buf[3][30]={0};
char  string_add_unit_buffer[2][30];
/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void init_block(void);
void get_sensor_data(void);
void send_sensor_report_offline(void);
void sysex_process_online(void);
void sysex_process_offline(void);
void OLED_offline_updata_display(void);
void OLED_online_updata_display(void);
void Clear_Face_Display_Flag(void);
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
    
    boot_animation(7,3);
    
    while(1)
    {
        parse_uart0_recv_buffer();
        flush_uart1_to_uart0();
        flush_uart0_local_buffer();

        poll_led_request();                  
        
        OLED_offline_updata_display();
        
        OLED_online_updata_display();          
    }        
}

void init_block(void)
{        
    g_block_type = CLASS_DISPLAY;
    g_block_sub_type = BLOCK_OLED_DISPLAY;
        
    OLED_Init();  

    uart0_recv_attach(sysex_process_online, sysex_process_offline);  
    set_rgb_led(0, 0, 128);
}

void sysex_process_online(void)
{
    uint8_t  block_type,sub_type, data_type, cmd_type, string_type, location_type;
    uint8_t  face_no=0;
    uint16_t string_len;
    uint8_t  string_buf[128];
    
    // read block type.
    read_sysex_type(&block_type, &sub_type, ON_LINE);
    if((block_type != g_block_type) || (sub_type != g_block_sub_type))
    {
        send_sysex_error_code(WRONG_TYPE);
        return;
    }
        
    // read command type.
    data_type = BYTE_8;
    if(read_next_sysex_data(&data_type, &cmd_type, ON_LINE)== false)
    {
        return;
    }
        
    if(cmd_type == CMD_SET_DISPLAY_STRING)
    {       
        g_display_type=DISPLAY_STRING;
        // read display location.
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type, &location_type, ON_LINE)== false)
        {
            return;
        }
          
        // read string type.
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type, &string_type, ON_LINE)== false)
        {
            return;
        }
        
        if(string_type == ASCII_STRING_TYPE)
        {  
            // read string length.
            data_type = SHORT_16;
            if(read_next_sysex_data(&data_type, &string_len, ON_LINE) == false)
            {
                return;
            }        
           
            if(string_len > 128)
            {
                string_len = 128;
            }
        
            data_type = BYTE_8;
            for(int i = 0; i < string_len; i++)
            {
                if(read_next_sysex_data(&data_type, &(string_buf[i]), ON_LINE) == false)
                {
                    return;
                }
            }     
        
            // excute
            string_buf[string_len]='\0';
                            
            if(location_type==LINE_TOP)
            {
                Clear_Face_Display_Flag();
                OLED_Clear_Display_GRAM();
                OLED_ShowString_Emoji(0,2,string_buf);        
            }
            else if(location_type==LINE_MIDDLE)
            {
                Clear_Face_Display_Flag();
                OLED_Clear_Display_GRAM();   
                OLED_ShowString_Emoji(0,3,string_buf);
            }
            else if(location_type==LINE_BOTTOM)
            {
                Clear_Face_Display_Flag();
                OLED_Clear_Display_GRAM();  
                OLED_ShowString_Emoji(0,5,string_buf);
            }       
            OLED_Refresh_Gram();
        }                        
    }
    else if(cmd_type == CMD_SET_DISPLAY_FACE)
    {      
        g_display_type=DISPLAY_FACE;
        // read display face type.
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type, &face_no, ON_LINE)== false)
        {
            return;
        }   
        
        if((1<=face_no)&&(face_no<=8))
        {
            face_type=face_no;
        }
           
        Clear_Face_Display_Flag();
   
        // read eye blink flag.
        data_type = BYTE_8;
        if(read_next_sysex_data(&data_type, &eye_blink_flag, ON_LINE) == false)
        {
            return;
        }  
        OLED_Clear();
    }
}

void sysex_process_offline(void)
{    
    union   offline_data first_data ={0};

    uint8_t block_type, sub_type, data_type;
    uint16_t decimalt_value=0; 
    char typedata_to_string_buffer[30]={0};
    char display_count=0;//计算离线模式数据个数

    // read block type.
    read_sysex_type(&block_type, &sub_type, OFF_LINE); 
    device_type=block_type;
    device_sub_type=sub_type; 
        
    // read data.
    while(read_next_sysex_data(&data_type, &first_data, OFF_LINE))
    {    
        memset(typedata_to_string_buffer,0,sizeof(typedata_to_string_buffer));   
        
        switch(data_type)
        {         
            case BYTE_8:
            {
                g_display_value = (float)(first_data.byte8_val);
                sprintf(typedata_to_string_buffer,"%-d",(uint8_t)first_data.byte8_val);   
            }
            break;
            case BYTE_16:
            {
                g_display_value = (float)(first_data.byte16_val);
                sprintf(typedata_to_string_buffer,"%-d",(int8_t)first_data.byte16_val); 
            }
            break;
            case SHORT_16:
            {
                g_display_value = (float)(first_data.short16_val);
                sprintf(typedata_to_string_buffer,"%-d",(int16_t)first_data.short16_val); 
            }
            break;
            case SHORT_24:  
            {
                g_display_value = (float)(first_data.short16_val);
                sprintf(typedata_to_string_buffer,"%-d",(int16_t)first_data.short16_val); 
            }
            break;
            case LONG_40:
            {
                g_display_value = (float)(first_data.long32_val);
                sprintf(typedata_to_string_buffer,"%-d",(int32_t)first_data.long32_val); 
            }
            break;
            case FLOAT_40:
            {
                g_display_value = first_data.float32_val;           
                
                if((first_data.float32_val)>=0)
                {
                    decimalt_value=((int)(g_display_value*100))-((int)g_display_value*100);
                    sprintf(typedata_to_string_buffer,"%-d.%02d",(int)g_display_value,decimalt_value);
                }
                else
                {
                    g_display_value=-g_display_value;
                    decimalt_value=((int)(g_display_value*100))-((int)g_display_value*100);
                    sprintf(typedata_to_string_buffer,"-%-d.%02d",(int)g_display_value,decimalt_value);
                }               
            }
            break;
            default:  
            break;         
        }                         
        memcpy(display_buf[display_count],typedata_to_string_buffer,sizeof(typedata_to_string_buffer));
        display_count++;  
        data_number_offline_pkg = display_count;
        offline_update_flag=1;     
    }    
}

void OLED_offline_updata_display(void)
{
    uint32_t currentMillis = 0;               // store the current value from millis()
    static uint32_t previousMillis = 0;       // for comparison with currentMillis    
       
    currentMillis = millis();
    if(currentMillis - previousMillis > 100)//OLED display value
    {
        previousMillis = currentMillis;                        
                       
        if(offline_update_flag==1)
        {
            offline_update_flag=0;
             
            OLED_Clear_Display_GRAM();
            
            if((device_type==CLASS_CONTROL)&&(device_sub_type==BLOCK_BUTTON))
            {                      
                OLED_ShowString(8,2,(uint8_t *)"Button");
                    
                if((uint8_t)g_display_value==0x01)
                {
                    OLED_ShowString(8,5,(uint8_t *)"On ");
                }
                else
                {
                    OLED_ShowString(8,5,(uint8_t *)"Off ");
                }
            }
            else if((device_type==CLASS_CONTROL)&&(device_sub_type==BLOCK_SELF_LOCKING_SWITCH))
            {                                             
                OLED_ShowString(8,2,(uint8_t *)"Switch");
                    
                if((uint8_t)g_display_value==0x01)
                {
                    OLED_ShowString(8,5,(uint8_t *)"On ");
                }
                else
                {
                    OLED_ShowString(8,5,(uint8_t *)"Off ");
                }
            }
            else if((device_type==CLASS_CONTROL)&&(device_sub_type==BLOCK_TOUCH_KEY))
            {                                          
                OLED_ShowString(8,2,(uint8_t *)"Touch Sensor");
                    
                if((uint8_t)g_display_value==0x01)
                {
                    OLED_ShowString(8,5,(uint8_t *)"On ");
                }
                else
                {
                    OLED_ShowString(8,5,(uint8_t *)"Off ");
                }
            }          
            else if((device_type==CLASS_CONTROL)&&(device_sub_type==BLOCK_MAKEY_MAKEY))
            {     
                switch((uint8_t)g_display_value)
                {
                    case 0x00:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 0 0 0");
                        break;
                    }
                    case 0x01:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 0 0 0");
                        break;
                    }
                    case 0x02:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 1 0 0");
                        break;
                    }
                    case 0x03:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 1 0 0");
                        break;
                    }
                    case 0x04:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 0 1 0");
                        break;
                    }
                    case 0x05:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 0 1 0");
                        break;
                    }
                    case 0x06:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 1 1 0");
                        break;
                    }
                    case 0x07:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 1 1 0");
                        break;
                    }
                    case 0x08:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 0 0 1");
                        break;
                    }
                    case 0x09:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 0 0 1");
                        break;
                    }
                    case 0x0A:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 1 0 1");
                        break;
                    }
                    case 0x0B:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 1 0 1");
                        break;
                    }
                    case 0x0C:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 0 1 1");
                        break;
                    }
                    case 0x0D:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 0 1 1");
                        break;
                    }
                    case 0x0E:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    0 1 1 1");
                        break;
                    }
                    case 0x0F:
                    {
                        OLED_ShowString(8,4,(uint8_t *)"    1 1 1 1");
                        break;
                    }
                    default:break;           
                }              
            }     
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_RAINING_SENSOR))
            {                                 
                OLED_ShowString(8,2,(uint8_t *)"Raining");
                    
                if((uint8_t)g_display_value==0x01)
                {
                    OLED_ShowString(8,5,(uint8_t *)"On ");
                }
                else
                {
                    OLED_ShowString(8,5,(uint8_t *)"Off ");
                }
            } 
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_PIR_SENSOR))
            {                                   
                if((uint8_t)g_display_value==0x01)
                {
                    Offline_Display_Emoji_String(9,"On "); 
                }
                else
                {
                    Offline_Display_Emoji_String(9,"Off"); 
                }
            }                                                   
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_LINE_FOLLOWER))
            {                                                                
                if((uint8_t)g_display_value==0x00)
                {
                    OLED_ShowString(8,2,(uint8_t *)"Left: On ");
                    OLED_ShowString(8,5,(uint8_t *)"Right:On ");
                }
                else if((uint8_t)g_display_value==0x01)
                {
                    OLED_ShowString(8,2,(uint8_t *)"Left: On ");
                    OLED_ShowString(8,5,(uint8_t *)"Right:Off");
                }
                else if((uint8_t)g_display_value==0x02)
                {
                    OLED_ShowString(8,2,(uint8_t *)"Left: Off");
                    OLED_ShowString(8,5,(uint8_t *)"Right:On ");
                }
                else if((uint8_t)g_display_value==0x03)
                {
                    OLED_ShowString(8,2,(uint8_t *)"Left: Off");
                    OLED_ShowString(8,5,(uint8_t *)"Right:Off");
                }           
            } 
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_LIGHT_SENSOR))
            {                                                                                      
                Offline_Display_Emoji_String(12,display_buf[0]);                             
            }     
            else if((device_type==CLASS_CONTROL)&&(device_sub_type==BLOCK_POTENTIOMETER))
            {                                                                                      
                Offline_Display_Emoji_String(7,display_buf[0]);                             
            }  
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_TEMPERATURE_HUMIDITY))
            {                
                sprintf(string_add_unit_buffer[0],"%sdegs",display_buf[0]); 
                sprintf(string_add_unit_buffer[1],"%s%%",display_buf[1]);
                Offline_Display_Two_Emoji_String(13,string_add_unit_buffer[0],14,string_add_unit_buffer[1]);   
            }  
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_TEMPERATURE))
            {                                                                                      
                Offline_Display_Emoji_String(13,display_buf[0]);                             
            }   
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_ULTRASONIC_SENSOR))
            {                              
                sprintf(string_add_unit_buffer[0],"%scm",display_buf[0]);    
                Offline_Display_Emoji_String(8,string_add_unit_buffer[0]); 
            }   
            else if((device_type==CLASS_CONTROL)&&(device_sub_type==BLOCK_JOYSTICK))
            {                                                                                      
                OLED_ShowString(8,2,(uint8_t *)"X:");
                OLED_ShowString(24,2,(uint8_t *)display_buf[0]);
                OLED_ShowString(8,5,(uint8_t *)"Y:");
                OLED_ShowString(24,5,(uint8_t *)display_buf[1]); 
            } 
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_PM_SENSOR))
            {                                                                                      
                OLED_ShowString(8,2,(uint8_t *)"PM2.5:");
                OLED_ShowString(24,2,(uint8_t *)display_buf[0]);
                OLED_ShowString(8,5,(uint8_t *)"PM10:");
                OLED_ShowString(24,5,(uint8_t *)display_buf[1]); 
            } 
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_TENSIOMETER))
            {                                 
                sprintf(string_add_unit_buffer[0],"%s%%",display_buf[0]);    
                Offline_Display_Emoji_String(14,string_add_unit_buffer[0]);  
            }  
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_WIND_SPEED_SENSOR))
            {                                 
                sprintf(string_add_unit_buffer[0],"%sm/s",display_buf[0]);    
                Offline_Display_Emoji_String(1,string_add_unit_buffer[0]);  
            }  
            else if((device_type==CLASS_SENSOR)&&(device_sub_type==BLOCK_ACCELEROMETER_GYRO))
            {                                                 
                OLED_Other_type_ShowString(8,2,(uint8_t *)"X:");
                OLED_Other_type_ShowString(8,4,(uint8_t *)"Y:");
                OLED_Other_type_ShowString(8,6,(uint8_t *)"Z:");
                OLED_Other_type_ShowString(24,2,(uint8_t *)display_buf[0]);
                OLED_Other_type_ShowString(24,4,(uint8_t *)display_buf[1]);
                OLED_Other_type_ShowString(24,6,(uint8_t *)display_buf[2]);
            }  
            else
            {
                if(data_number_offline_pkg==1)
                {                  
                    OLED_Other_type_ShowString(8,2,(uint8_t *)display_buf[0]);
                }
                if(data_number_offline_pkg==2)//Drawing Temperature and humidity meter icon
                {                  
                    OLED_Other_type_ShowString(8,2,(uint8_t *)display_buf[0]);
                    OLED_Other_type_ShowString(8,4,(uint8_t *)display_buf[1]);
                }
                if(data_number_offline_pkg==3)//Drawing Temperature and humidity meter icon
                {                                      
                    OLED_Other_type_ShowString(8,2,(uint8_t *)display_buf[0]);
                    OLED_Other_type_ShowString(8,4,(uint8_t *)display_buf[1]);
                    OLED_Other_type_ShowString(8,6,(uint8_t *)display_buf[2]);              
                }
            }
            OLED_Refresh_Gram();            
        }
    }     
}

void OLED_online_updata_display(void)
{
    uint32_t currentMillis = 0;               // store the current value from millis()
    static uint32_t previousMillis = 0;       // for comparison with currentMillis    
    static uint32_t previousMillis_flash = 0;        
    
    if(g_display_type!=DISPLAY_FACE)
    {  
       return;
    }
    
    if(face_type!=pre_face_type||eye_blink_flag==1)
    {
        eye_blink_refresh_flag=0;
    }
    if(face_type!=pre_face_type||eye_blink_flag==0)
    {
        blink_one_times_end_flag=0;
    }  
    
    if(face_type!=pre_face_type)//表情类型变化
    {              
        currentMillis = millis();
        if(currentMillis - previousMillis > 67)//OLED display value
        {
            previousMillis = currentMillis; 
                        
            flash_change_count++; 
            if(flash_change_count>4)
            {
                flash_change_count=0;
            }
                        
            if(flash_change_count==1)
            {
                Display_Face_Blink_Flash_Clear_20PX(face_type);    
            }
            else if(flash_change_count==2)
            {
                OLED_Clear_Display_GRAM();    
                LCD_DrawLine(0,36,127,36,1);
                OLED_Refresh_Gram(); 
            }
            else if(flash_change_count==3)
            {
                LCD_DrawLine(0,36,127,36,0);
                OLED_Refresh_Gram();  
                Display_Face(face_type); 
                Display_Face_Blink_Flash_Clear_20PX(face_type);
                OLED_Refresh_Gram();  
            }
            else if(flash_change_count==4)
            {               
                Display_Face(face_type);
                OLED_Refresh_Gram(); 
                pre_face_type=face_type;                
            }
        } 
    }
    else//表情类型不变化
    {   
        if(eye_blink_flag==1)//表情类型眨眼
        {     
            if(blink_one_times_end_flag == 1)
            {
                currentMillis = millis();
                if(currentMillis - previousMillis_flash > 4000)//OLED display value
                {
                    previousMillis_flash = currentMillis; 
                    blink_one_times_end_flag = 0;
                }    
            }
            
            if(blink_one_times_end_flag == 0)
            {
                currentMillis = millis();
                if(currentMillis - previousMillis_flash > 67)//OLED display value
                {
                    previousMillis_flash = currentMillis; 
                    flash_blink_count++; 
                    if(flash_blink_count>4)
                    {
                        flash_blink_count = 0;
                    }                   
                    
                    if(flash_blink_count==1)
                    {               
                        Display_Face(face_type);
                        OLED_Refresh_Gram();
                        blink_one_times_end_flag = 1;
                    }
                    else if(flash_blink_count==2)
                    {
                        Display_Face_Blink_Flash_Clear_20PX(face_type);
                    }
                    else if(flash_blink_count==3)
                    {
                        OLED_Fill(0,0,127,63,0); 
                    }
                    else if(flash_blink_count==4)
                    {
                        Display_Face(face_type);
                        Display_Face_Blink_Flash_Clear_20PX(face_type);
                        OLED_Refresh_Gram();                       
                    }
                }
            } 
        }
        else//表情类型不变化不眨眼
        {
            if(eye_blink_refresh_flag==0)
            {
                eye_blink_refresh_flag=1;
                
                OLED_Fill(0,0,127,63,0);                
                Display_Face(face_type);                
                OLED_Refresh_Gram();                 
            }
        }    
    }
}

void Clear_Face_Display_Flag(void)
{
    eye_blink_flag=0;
    eye_blink_refresh_flag=0;
    flash_blink_count=0;
    flash_change_count=0;
    blink_one_times_end_flag=0;
}
