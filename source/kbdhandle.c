#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include "wiimote_map.h"

#include "general.h"
#include "screen.h"

#define MINIMUM_CHARACTER	0
#define MAXIMUM_CHARACTER	99	

void add_character_to_string(char* target, int index, int is_caps, char character, char caps_character)
{
	if(is_caps)
		target[index]=caps_character;
	else
		target[index]=character;
	printf("\b%c ", target[index]);
}

void add_wiimote_character_to_string(char* target, int index, char character)
{
	target[index]=character;
	printf("\b%c ", target[index]);
}

int type_string_wiimote(char* target, int target_max_size)
{
	int poll_for_keys=1;
	int target_current_size=0;
	s32 current_key_index=0;
	int is_caps=0, is_caps_lock=0;
	int dont_add=0;
	char characters_to_do=wiimote_key_map[current_key_index];
	int enter_show=0;
	while(poll_for_keys && target_current_size < target_max_size)
	{
		is_caps=0;
		dont_add=0;
		WPAD_ScanPads();
		PAD_ScanPads();
		u32 WPAD_Pressed;
		u32 WPAD1_Pressed = WPAD_ButtonsDown(0);
		u32 WPAD2_Pressed = WPAD_ButtonsDown(1);
		u32 WPAD3_Pressed = WPAD_ButtonsDown(2);
		u32 WPAD4_Pressed = WPAD_ButtonsDown(3);

		u32 PAD_Pressed;
		u32 PAD1_Pressed  = PAD_ButtonsDown(0);
		u32 PAD2_Pressed  = PAD_ButtonsDown(1);
		u32 PAD3_Pressed  = PAD_ButtonsDown(2);
		u32 PAD4_Pressed  = PAD_ButtonsDown(3);

		WPAD_Pressed	  = WPAD1_Pressed | WPAD2_Pressed | WPAD3_Pressed | WPAD4_Pressed;
		PAD_Pressed	  = PAD1_Pressed  | PAD2_Pressed  | PAD3_Pressed  | PAD4_Pressed ;

		if(is_caps_lock && current_key_index < 26)
		{
			is_caps=current_key_index;
			current_key_index+=40;
		}
		if(enter_show<0)
			printf("\b\x07F");
		else
			printf("\b%c",wiimote_key_map[current_key_index]);
		if(is_caps_lock && is_caps < 26)
			current_key_index=is_caps;
		is_caps=0;
		enter_show++;
		enter_show%=50;

 		if((WPAD_Pressed & WPAD_BUTTON_UP)						|| (PAD_Pressed & PAD_BUTTON_UP))
		{
			if(current_key_index<=MINIMUM_CHARACTER)
				current_key_index=MAXIMUM_CHARACTER;
			else
				current_key_index--;
			characters_to_do=wiimote_key_map[current_key_index];
		}

		if((WPAD_Pressed & WPAD_BUTTON_DOWN)						|| (PAD_Pressed & PAD_BUTTON_DOWN))
		{
			if(current_key_index>=MAXIMUM_CHARACTER)
				current_key_index=MINIMUM_CHARACTER;
			else
				current_key_index++;
			characters_to_do=wiimote_key_map[current_key_index];
		}

		if((WPAD_Pressed & WPAD_BUTTON_1)						|| (PAD_Pressed & PAD_BUTTON_X))
		{
			is_caps_lock++;
			is_caps_lock%=2;
		}

		if((WPAD_Pressed & WPAD_BUTTON_RIGHT)	|| (WPAD_Pressed & WPAD_BUTTON_A)	|| (PAD_Pressed & PAD_BUTTON_RIGHT)	|| (PAD_Pressed & PAD_BUTTON_A))
		{
			if(is_caps_lock && current_key_index < 26)
			{
				is_caps=current_key_index;
				current_key_index+=40;
			}
			characters_to_do=wiimote_key_map[current_key_index];
			if(!dont_add)
				add_wiimote_character_to_string(target, target_current_size, characters_to_do);
			if(is_caps_lock && is_caps < 26)
				current_key_index=is_caps;
			characters_to_do=wiimote_key_map[current_key_index];
			target_current_size++;
		}

		if((WPAD_Pressed & WPAD_BUTTON_LEFT)	|| (WPAD_Pressed & WPAD_BUTTON_B)	|| (PAD_Pressed & PAD_BUTTON_LEFT)	|| (PAD_Pressed & PAD_BUTTON_B))
		{
			if(target_current_size>0)
			{
				dont_add=1;
				target[--target_current_size]=0;
				printf("\b");
			}
		}

		if((WPAD_Pressed & WPAD_BUTTON_PLUS)						|| (PAD_Pressed & PAD_BUTTON_START))
		{
			dont_add=1;
			poll_for_keys=0;
		}
		
		if((WPAD_Pressed & WPAD_BUTTON_MINUS)						|| (PAD_Pressed & PAD_BUTTON_Y))
		{
			return 0;
		}
		
		VIDEO_WaitVSync();
	}
	ClearScreen();
	return target_current_size;
}
