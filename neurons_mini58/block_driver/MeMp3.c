#include "MeMp3.h"
#include "systime.h"
#include "stdio.h"
#include "MeSoftSerial.h"
#include "uart_printf.h"

#define START_CODE  					0x7E
#define END_CODE						0xEF
#define SPECIFY_NO_PLAY_CMD				0xA2
#define PLAY_OR_STOP_CMD    			0xAA
#define PLAY_NEXT_CMD					0xAC
#define PLAY_LAST_CMD					0xAD
#define VOLUME_CMD  					0xAE
#define PLAY_MODE_CMD                   0xAF
#define PAUSE_OR_CONTINUE_PLAY_CMD		0xAA
#define STOP_PLAY_CMD					0xAB
#define PLAY_MODE_SET_CMD				0xAF
#define STORE_CHOOSE_CMD				0xD2
#define RECORD_CMD  					0xD5
#define STOP_RECORD_CMD					0xD9
#define DELETE_CMD  					0xDA
#define FORMAT_CMD 						0xDE
  

static uint8_t s_frames_to_mp3[15] = {0};
static uint8_t s_frames_write_bytes = 0;
static uint8_t s_current_volume = 0;

static void init_package(void)
{
	s_frames_write_bytes = 0;
	s_frames_to_mp3[s_frames_write_bytes++] = START_CODE;
	s_frames_write_bytes++; // for length;
}

static void flush_whole_package(void)
{
	s_frames_to_mp3[1] = s_frames_write_bytes;
	s_frames_to_mp3[s_frames_write_bytes] = 0;
	for(int i= 1; i< s_frames_write_bytes; i++)
	{
		s_frames_to_mp3[s_frames_write_bytes] +=  s_frames_to_mp3[i];
	}
	s_frames_write_bytes++;
	s_frames_to_mp3[s_frames_write_bytes++] = END_CODE;
	 
	for(int i = 0; i < s_frames_write_bytes; i++)
	{
		SoftwareSerialWrite(s_frames_to_mp3[i]);
		//uart_printf(UART0, "-%x-", s_frames_to_mp3[i]);
	}
}

static void add_next_byte(uint8_t data)
{
	s_frames_to_mp3[s_frames_write_bytes++] = data;
}

static void config_store_with_SD_card(void)
{
	init_package();
	add_next_byte(STORE_CHOOSE_CMD);
	add_next_byte(1);
	flush_whole_package();
}

void mp3_send_cmd(enum CMD_TYPE cmd_type)
{
	init_package();
	
	switch(cmd_type)
	{
		case MP3_STOP_PLAY:
			add_next_byte(PLAY_OR_STOP_CMD);
		break;
		case MP3_PLAY:
			add_next_byte(PLAY_OR_STOP_CMD);
		break;
		case MP3_NEXT:
			add_next_byte(PLAY_NEXT_CMD);
		break;
		case MP3_LAST:
			add_next_byte(PLAY_LAST_CMD);
		break;
		case MP3_RECORD:
			add_next_byte(RECORD_CMD);
			add_next_byte(0x00);
			add_next_byte(100); // if the quantity of files in the sd card is less than 100, the index of the of record song will be the next index.
		break;
		case MP3_STOP_RECORD:
			add_next_byte(STOP_RECORD_CMD);
		break;
		case MP3_PAUSE_OR_CONTINUE_PLAY:
			add_next_byte(PAUSE_OR_CONTINUE_PLAY_CMD);
		break;
		case MP3_FORMAT:
			add_next_byte(FORMAT_CMD);
		break;
		default:
		break;
	}
	
	flush_whole_package();
}

void mp3_play_music(uint8_t no)
{
	if(no > 32)
	{
		return;
	}
	init_package();
	add_next_byte(SPECIFY_NO_PLAY_CMD);
	add_next_byte(0);
	add_next_byte(no);
	flush_whole_package();
}


void mp3_delete_music(uint8_t no)
{
	init_package();
	add_next_byte(DELETE_CMD);
	add_next_byte(0x00);
	add_next_byte(no);
	flush_whole_package();
}
uint8_t mp3_get_current_volume(void)
{
    return s_current_volume;
}

void mp3_set_volume(uint8_t volume)
{
    if(volume > 31) // volume range 0-31.
	{
        volume = 31 ;
	}
    s_current_volume = volume;
    
	init_package();
	add_next_byte(VOLUME_CMD);
	add_next_byte(volume);
	flush_whole_package();
}

void mp3_set_play_mode(enum PLAY_MODE mode)
{
    uint8_t play_mode;
    init_package();
    add_next_byte(PLAY_MODE_CMD);
    switch(mode)
    {
        case SINGLE_PLAY:
            play_mode = 0x00;
        break;
        case SINGLE_CYCLE_PLAY:
            play_mode = 0x01;
        break;
        case ALL_CYCLE_PLAY:
            play_mode = 0x02;
        break;
        case RAND_PLAY:
            play_mode = 0x03;
        break;
        default:
        break;
    }
    add_next_byte(play_mode);
    flush_whole_package();
}

void mp3_init(void)
{   
	pinMode(FUNCTION_KEY_1, GPIO_MODE_INPUT);
	pinMode(FUNCTION_KEY_2, GPIO_MODE_INPUT);
    pinMode(FUNCTION_KEY_3, GPIO_MODE_INPUT);
	pinMode(FUNCTION_KEY_4, GPIO_MODE_INPUT);
    pinMode(MOTOR_CTL_PIN,  GPIO_MODE_OUTPUT);
//    pinMode(KEY1_LED_PIN, GPIO_MODE_OUTPUT);
//    pinMode(KEY1_LED_PIN, GPIO_MODE_OUTPUT);
//    pinMode(KEY1_LED_PIN, GPIO_MODE_OUTPUT);
//    pinMode(KEY1_LED_PIN, GPIO_MODE_OUTPUT);
//    digitalWrite(KEY1_LED_PIN, 1);
//    digitalWrite(KEY2_LED_PIN, 1);
//    digitalWrite(KEY3_LED_PIN, 1);
//    digitalWrite(KEY4_LED_PIN, 1);
	SoftwareSerialBegin(TX_PIN, 9600);
	config_store_with_SD_card();
    delay(100);
    mp3_set_volume(20);
}
