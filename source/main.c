#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <ogc/es.h>
#include <ogc/isfs.h>
#include <ogc/ipc.h>
#include <ogc/ios.h>
#include <ogc/dvd.h>
#include <ogc/wiilaunch.h>	
#include <wiiuse/wpad.h>
#include <fat.h>
#include <gcmodplay.h>

#include "screen.h"
#include "cleanup.h"
#include "rethandle.h"
#include "auth.h"
#include "channels.h"
#include "nand.h"
#include "disc.h"
#include "saves.h"
#include "general.h"
#include "credits.h"

#include "sysmenu.h"
#include "su.h"
#include "yawnd.h"
#include "pretty.h"
#include "IOS_DVD.h"
#include "factory_mod.h"
#include "editing.h"
#include "updater.h"

extern const u8 tvaettbj0rn_mod_end[];
extern const u8 tvaettbj0rn_mod[];
extern const u32 tvaettbj0rn_mod_size;

int need_sys;

static int cur_off=0;
static int redraw=1;

volatile int A_button_held = 0;

typedef enum {MAIN_MENU, BACKUP_MENU, EXIT_MENU, CREDITS_SCREEN} menu_t;
menu_t active_menu = MAIN_MENU;

static void move_cursor(int old_off, int new_off) {
	if (old_off != -1)
		printf("\x1b[%d;0H   ", 2+old_off);
	if (new_off != -1)
		printf("\x1b[%d;0H ->", 2+new_off);
}

static void select_main_item(void) {
	switch(cur_off)
	{
		case 0:
			LoadGame();
			break;

		case 1:
			ChannelLauncher();
			break;

		case 2:
			saveManager();
			break;

		case 3:
			cur_off = 0;
			active_menu = BACKUP_MENU;
			redraw = 1;
			break;

		case 4:
			configuration();
			cur_off = 0;
			active_menu = MAIN_MENU;
			redraw = 1;
			break;
		
		case 5:
			credits();
            active_menu = CREDITS_SCREEN;
			break;
		
		case 6:
			cur_off = 0;
			active_menu = EXIT_MENU;
			redraw = 1;
			break; 

		default:
			break;
	}
}

void select_exit_menu_item(void) {
	switch (cur_off) {
		case 0: SYS_ResetSystem(SYS_RETURNTOMENU,0,0);
			break;
		case 1: Finish(0);
			break;
		case 2: SYS_ResetSystem(SYS_RESTART,0,0);
			break;
		case 3: SYS_ResetSystem(SYS_SHUTDOWN,0,0);
			break;
		case 4:
			cur_off = 0;
			active_menu = MAIN_MENU;
			redraw = 1;
			break;
		default:
			break;
	}
}

void select_backup_menu_item(void) {
	switch (cur_off) {
		case 0: backupNAND();
			break;
		case 1: backupNANDzestig();
			break;
		case 2:
			cur_off = 0;
			active_menu = MAIN_MENU;
			redraw = 1;
			break;
		default:
			break;
	}
}

int main(int argc, char **argv)
{
	int active_menu_size;

	CheckIOSRetval(__IOS_LaunchNewIOS(35));
	
	initialize_wiimu();
	
	while(1)
	{
		WPAD_ScanPads();
		PAD_ScanPads();
		u32 WPAD_Pressed = WPAD_ButtonsDown(0);
		WPAD_Pressed |= WPAD_ButtonsDown(1);
		WPAD_Pressed |= WPAD_ButtonsDown(2);
		WPAD_Pressed |= WPAD_ButtonsDown(3);

		u32 PAD_Pressed  = PAD_ButtonsDown(0);
		PAD_Pressed  |= PAD_ButtonsDown(1);
		PAD_Pressed  |= PAD_ButtonsDown(2);
		PAD_Pressed  |= PAD_ButtonsDown(3);

		u32 xPAD_Released = (WPAD_ButtonsUp(0) | WPAD_ButtonsUp(1) | WPAD_ButtonsUp(2) | WPAD_ButtonsUp(3) | 
				PAD_ButtonsUp(0) | PAD_ButtonsUp(1) | PAD_ButtonsUp(2) | PAD_ButtonsUp(3));

		if ( (WPAD_Pressed & WPAD_BUTTON_HOME) || (PAD_Pressed & PAD_BUTTON_START) )
			exit(0);
 
		if ( (WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A) )
		{
            A_button_held = 1;
			if (active_menu == MAIN_MENU)
				select_main_item();
			else if (active_menu == BACKUP_MENU)
				select_backup_menu_item();
			else if (active_menu == EXIT_MENU)
				select_exit_menu_item();
			else if (active_menu == CREDITS_SCREEN) {
				if (chk_credits() == 0) {
					cur_off = 0;
					active_menu = MAIN_MENU;
					redraw = 1;
				}
			}
		}

		if (xPAD_Released & WPAD_BUTTON_A)
			A_button_held = 0;
 
		switch (active_menu) {
			case MAIN_MENU: active_menu_size = 7;
				break;
			case BACKUP_MENU: active_menu_size = 3;
				break;
			case EXIT_MENU: active_menu_size = 5;
				break;
            case CREDITS_SCREEN: active_menu_size = 0;
                break;
		}
		if ( (WPAD_Pressed & WPAD_BUTTON_DOWN) || (PAD_Pressed & PAD_BUTTON_DOWN) )
		{
			int old_off = cur_off;
			cur_off = (cur_off+1) % active_menu_size;
			move_cursor(old_off, cur_off);
		} else if ( (WPAD_Pressed & WPAD_BUTTON_UP) || (PAD_Pressed & PAD_BUTTON_UP) )
		{
			int old_off = cur_off;
			cur_off = (cur_off-1+active_menu_size)%active_menu_size;
			move_cursor(old_off, cur_off);
		}
 
		chk_credits();
		if( redraw )
		{
			ClearScreen();
			if (active_menu == MAIN_MENU) {
				char caption[80];
				sprintf(caption, "WiiMU v%d.%d%s (Built %s %s).", VERSION_MAJOR, VERSION_MINOR, SPECIAL_BUILD, __DATE__, __TIME__);
				char *tabmain[] = { "Load Game.", "Launch Channel.", "Save Manager.", "Backup NAND.", "Configuration.", "Credits.", "Exit." };
				draw_menu(caption, tabmain, 7);
				printf("\x1b[%d;%dHCurrent language is: %s", 28, 46, lang[language_setting]);
			}
			else if (active_menu == BACKUP_MENU) {
				char *tabnand[] = { "InFeCtUs", "Zestig", "Back" };
				draw_menu("For what use would you like to back up for:", tabnand, 3);
			}
			else if (active_menu == EXIT_MENU) {
				char *tabpower[] = { "Return to Wii Menu", "Return to Loader", "Reboot", "Shutdown", "Back" };
				draw_menu("What would you like to do:", tabpower, 5);
			}
			move_cursor(-1, cur_off);
			redraw = 0;
		}
 
		VIDEO_WaitVSync();
	}
	Finish(0);
	return 0;
}
