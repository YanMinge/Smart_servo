#ifndef __MEMP3_H__
#define __MEMP3_H__

#include "mygpio.h"

#define FUNCTION_KEY_1      P1_4 	// preess to record, and release to stop record.   
#define FUNCTION_KEY_2      P1_5  	// press to pause, press again to continue.
#define FUNCTION_KEY_3		P0_5	// short press to play next song, and long press to increase the volume. 
#define FUNCTION_KEY_4		P0_4	// short press to play last song, and long press to decrease the volume. 

enum CMD_TYPE
{
	MP3_PLAY = 0x01,
	MP3_DELETE,
	MP3_LAST,
	MP3_NEXT,
	MP3_PAUSE_OR_CONTINUE_PLAY,
	MP3_STOP_PLAY,
	MP3_FORMAT,
	MP3_RECORD,
	MP3_STOP_RECORD
};

enum MP3_KEY_CONTROL_STATE
{
	STATE_NO_PRESS,
	STATE_RECORD_PRESS,
	STATE_PAUSE_OR_CONTINUE_PRESS,
	STATE_STOP_PRESS,
	STATE_VOLUME_INCREASE_PRESS,
	STATE_VOLUME_DECREASE_PRESS,
	STATE_NEXT_SONG_PRESS,
	STATE_LAST_SONG_PRESS,
	STATE_KEY3_LONG_OR_SHORT_PRESS,
	STATE_KEY4_LONG_OR_SHORT_PRESS,
};

void mp3_send_cmd(enum CMD_TYPE cmd_type);

void mp3_play_music(uint8_t no);

void mp3_delete_music(uint8_t no);

void mp3_play_mode_set(uint8_t mode);
	
void mp3_set_volume(uint8_t volume);

void mp3_init(void);

#endif
