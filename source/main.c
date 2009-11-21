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
#include "controls.h"

int need_sys;

static int cur_off=0;
int redraw=1;

menu_t active_menu = MAIN_MENU;

static int submenu_handler_id = -1;

void move_cursor(int old_off, int new_off) {
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
			load_menu(BACKUP_MENU);
			break;

		case 4:
			load_menu(CONFIGURATION_MENU);
			load_menu(MAIN_MENU);
			break;
		
		case 5:
            load_menu(CREDITS_SCREEN);
			break;
		
		case 6:
			load_menu(EXIT_MENU);
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
			load_menu(MAIN_MENU);
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
			load_menu(MAIN_MENU);
			break;
		default:
			break;
	}
}

#define MAIN_MENU_SIZE 7

int main_controls(button_t button, int source, int is_pressed) {
	int old_off = cur_off;

	if (!is_pressed)
		return 1;

	switch (button) {
		case A_BUTTON:
			select_main_item();
			break;
		case B_BUTTON:
			cur_off = MAIN_MENU_SIZE - 1;
			break;
		case UP_BUTTON:
			cur_off = (cur_off - 1 + MAIN_MENU_SIZE) % MAIN_MENU_SIZE;
			break;
		case DOWN_BUTTON:
			cur_off = (cur_off + 1) % MAIN_MENU_SIZE;
			break;
        case LEFT_BUTTON:
        case RIGHT_BUTTON:
            break;
		case HOME_BUTTON:
		case START_BUTTON:
			exit(0);
			break;
	}

	move_cursor(old_off, cur_off);
	return 1;
}

#define BACKUP_MENU_SIZE 3

int backup_menu_controls(button_t button, int source, int is_pressed) {
	int old_off = cur_off;

	if (!is_pressed)
		return 1;

	switch (button) {
		case A_BUTTON:
			select_backup_menu_item();
			break;
		case B_BUTTON:
			cur_off = BACKUP_MENU_SIZE - 1;
			break;
		case UP_BUTTON:
			cur_off = (cur_off - 1 + BACKUP_MENU_SIZE) % BACKUP_MENU_SIZE;
			break;
		case DOWN_BUTTON:
			cur_off = (cur_off + 1) % BACKUP_MENU_SIZE;
			break;
        case LEFT_BUTTON:
        case RIGHT_BUTTON:
            break;
		case HOME_BUTTON:
		case START_BUTTON:
			return 0;
	}

	move_cursor(old_off, cur_off);
	return 1;
}

#define EXIT_MENU_SIZE 5

int exit_menu_controls(button_t button, int source, int is_pressed) {
	int old_off = cur_off;

	if (!is_pressed)
		return 1;

	switch (button) {
		case A_BUTTON:
			select_exit_menu_item();
			break;
		case UP_BUTTON:
			cur_off = (cur_off - 1 + EXIT_MENU_SIZE) % EXIT_MENU_SIZE;
			break;
		case DOWN_BUTTON:
			cur_off = (cur_off + 1) % EXIT_MENU_SIZE;
			break;
        case LEFT_BUTTON:
        case RIGHT_BUTTON:
            break;
		case B_BUTTON:
			cur_off = EXIT_MENU_SIZE - 1;
			break;
		case HOME_BUTTON:
		case START_BUTTON:
			return 0;
	}

	move_cursor(old_off, cur_off);
	return 1;
}

void load_menu(menu_t menu) {
	if (active_menu == menu)
		return;
	if (submenu_handler_id != -1)
		remove_handler(submenu_handler_id);
	switch (menu) {
		case BACKUP_MENU:
			cur_off = 0;
			submenu_handler_id = add_handler(backup_menu_controls);
			break;
		case EXIT_MENU:
			cur_off = 1;
			submenu_handler_id = add_handler(exit_menu_controls);
			break;
		case CREDITS_SCREEN:
			submenu_handler_id = add_handler(credits_controls);
			credits();
			break;
		case MAIN_MENU:
			cur_off = 0;
			break;
		case CONFIGURATION_MENU:
			configuration();
			break;
	}
	active_menu = menu;
	redraw = 1;
    return;
}

void draw_active_menu(void) {
	if (active_menu == CREDITS_SCREEN)
		draw_credits();
	else if (redraw)
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
}

int main(int argc, char **argv)
{
	add_handler(main_controls);

	CheckIOSRetval(__IOS_LaunchNewIOS(35));
	
	initialize_wiimu();

	while(1)
	{
		check_controls();

		draw_active_menu();

		VIDEO_WaitVSync();
	}
	Finish(0);
	return 0;
}
