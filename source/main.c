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

int main(int argc, char **argv)
{
	CheckIOSRetval(__IOS_LaunchNewIOS(35));
	
	initialize_wiimu();
	
	int cur_off=0;
	int redraw=1;
	char tab[5][64];
	memset(tab, 0, 5*64);

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

		if ( (WPAD_Pressed & WPAD_BUTTON_HOME) || (PAD_Pressed & PAD_BUTTON_START) )
			exit(0);
 
		if ( (WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A) )
		{
			redraw=1;
			ClearScreen();
			s32 res;
			if(cur_off == 3) {
				char *tabnand[] = { "InFeCtUs", "Zestig", "Back" };
				res = showmenu("\n\nFor what use would you like to back up for:", tabnand, 3, 0, " ->");
				if(res==0)
					backupNAND();
				else if(res==1)
					backupNANDzestig();
				//strcpy(tab[0], "Infectus"); strcpy(tab[1], "Zestig"); strcpy(tab[2], "Back");
			}else if(cur_off == 6){
				char *tabpower[] = { "Return to Wii Menu", "Return to Loader", "Reboot", "Shutdown", "Back" };
				res = showmenu("\n\nWhat would you like to do:", tabpower, 5, 0, " ->");
				if(res==0)
					SYS_ResetSystem(SYS_RETURNTOMENU,0,0);
				else if(res==1)
					Finish(0);
				else if(res==2)
					SYS_ResetSystem(SYS_RESTART,0,0);
				else if(res==3)
					SYS_ResetSystem(SYS_SHUTDOWN,0,0);
			}

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
					break;
				case 4:
					configuration();
					break;
				
				case 5:
					credits();
					break;
				
/*				case 6:
					update_WiiMU();
					break;
				
				case 7:*/
				case 6:
					break; 

				default:
					break;
			}
			VIDEO_WaitVSync();
			ClearScreen();
			redraw=1;
			cur_off=0;
		}
 
		if ( (WPAD_Pressed & WPAD_BUTTON_DOWN) || (PAD_Pressed & PAD_BUTTON_DOWN) )
		{
			printf("\x1b[%d;0H   ", 2+cur_off);
			cur_off++;
			if( cur_off >= MAIN_MENU_LIST_SIZE)
			{
				cur_off = 0;
			}
			redraw=1;
		} else if ( (WPAD_Pressed & WPAD_BUTTON_UP) || (PAD_Pressed & PAD_BUTTON_UP) )
		{
			printf("\x1b[%d;0H   ", 2+cur_off);
			cur_off--;
			if( cur_off < 0 )
			{
				cur_off=MAIN_MENU_LIST_SIZE-1;
			}
			redraw=1;
		}
 
		if( redraw )
		{
			printf("\x1b[1;0HWiiMU v%d.%d%s (Built %s %s). Current language is: %s\n", VERSION_MAJOR, VERSION_MINOR, SPECIAL_BUILD, __DATE__, __TIME__, lang[language_setting]);
			printf("    Load Game.\n");
			printf("    Launch Channel.\n");
			printf("    Save Manager.\n");
			printf("    Backup NAND.\n");
			printf("    Configuration.\n");
			printf("    Credits.\n");
//			printf("    Update!\n");
			printf("    Exit.\n");

			printf("\x1b[%d;0H ->", 2+cur_off);
 
			redraw = 0;
		}
 
		VIDEO_WaitVSync();
	}
	Finish(0);
	return 0;
}
