#ifndef _GENERAL_H
#define _GENERAL_H

#include <ogcsys.h>
#include <gccore.h>
#include <gcmodplay.h>

#define VERSION_MAJOR		0
#define	VERSION_MINOR		1
#define	SPECIAL_BUILD		"alpha4"

#define DUMP_BUF_SIZE		256

#define PHYSADDR(x) 		((unsigned long *)(0x7FFFFFFF & ((unsigned long)(x))))

#define HEIGHT_BEGIN 		2
#define MAX_SAVE_ENTRIES	24
#define MAX_CHAN_ENTRIES	24
#define MAX_IOS_ENTRIES		24
#define MAIN_MENU_LIST_SIZE	7

#define VIDEO_MODE_DEFAULT	0
#define VIDEO_MODE_PAL		1
#define VIDEO_MODE_NTSC		2

#define ISALIGNED(x)		((((u32)x)&0x1F)==0)
// Turn upper and lower into a full title ID
#define TITLE_ID(x,y)		(((u64)(x) << 32) | (y))
// Get upper half of a title ID
#define TITLE_UPPER(x)		((u32)((x) >> 32))
// Get lower half of a title ID
#define TITLE_LOWER(x)		((u32)(x))

#define ALIGN_LENGTH(x)		(((x)+31)&(~31))

#define WPAD_ANY_PAD(x)		(WPAD_ANY_BUTTON(x)  || (x & WPAD_BUTTON_UP) || (x & WPAD_BUTTON_DOWN) || (x & WPAD_BUTTON_LEFT) || (x & WPAD_BUTTON_RIGHT) || (x & WPAD_BUTTON_PLUS)  || (x & WPAD_BUTTON_MINUS) || (x & WPAD_BUTTON_HOME)) 
#define PAD_ANY_PAD(x)		( PAD_ANY_BUTTON(x)  || (x & PAD_BUTTON_B)   || (x & PAD_BUTTON_X)     || (x & PAD_BUTTON_Y)     || (x & PAD_BUTTON_UP)     || (x & PAD_BUTTON_DOWN)   || (x & PAD_BUTTON_LEFT)   || (x & PAD_BUTTON_RIGHT)  || (x & PAD_BUTTON_START)) 
#define WPAD_ANY_BUTTON(x)	((x & WPAD_BUTTON_A) || (x & WPAD_BUTTON_B)  || (x & WPAD_BUTTON_1)    || (x & WPAD_BUTTON_2)    || (x & WPAD_BUTTON_PLUS)  || (x & WPAD_BUTTON_MINUS) || (x & WPAD_BUTTON_HOME)) 
#define PAD_ANY_BUTTON(x)	((x & PAD_BUTTON_A)  || (x & PAD_BUTTON_B)   || (x & PAD_BUTTON_X)     || (x & PAD_BUTTON_Y)     || (x & PAD_BUTTON_START)) 

#define USE_WIILIGHT		1

#define dbgprintf(x)		printf((x));sleep(1)

#define DEBUG_IOS		1

extern u32 tempKeyID;

extern int language_setting;

void copyfile(char *path, char *dest);
s32 readfile(char *filepath, u8 *filearray, u32 max_size, u32 *file_size);
void wait_press_A();

#endif //_GENERAL_H

