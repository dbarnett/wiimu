#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <ogc/es.h>
#include <ogc/isfs.h>
#include <ogc/ipc.h>
#include <ogc/ios.h>
#include <ogc/dvd.h>
#include <ogc/wiilaunch.h>	
#include <wiiuse/wpad.h>
#include <fat.h>
#include <unistd.h>

#include "general.h"
#include "screen.h"
#include "cleanup.h"
#include "rethandle.h"
#include "auth.h"
#include "saves.h"

#include "pretty.h"
#include "sysmenu.h"
#include "su.h"
#include "editing.h"

extern int need_sys;

void saveToSD(u32 TitleIDH, u32 TitleIDL, char* titleName, char* subtitleName, char* destdir)
{
//	u64 TitleID ATTRIBUTE_ALIGN(32) = ((u64)TitleIDH<<32 | (u64)TitleIDL);
	char path[ISFS_MAXPATH + 1];
	sprintf(path, "/title/%08x/%08x/data/", TitleIDH, TitleIDL);
	if(need_sys)
		SystemMenuAuth();
	s32 res;
	u32 num = 0;
	char *nbuf, ebuf[ISFS_MAXPATH + 1], pbuf[ISFS_MAXPATH + 1];
	int i, j, k;

	//Get the entry count.
	res = ISFS_ReadDir(path, NULL, &num);
	CheckISFSRetval(res);

	//Allocate aligned buffer.
	nbuf = (char *)memalign(32, (ISFS_MAXPATH + 1) * num);
	char** filenames=calloc(num, ISFS_MAXPATH + 1);
	char** smallfilenames=calloc(num, ISFS_MAXPATH + 1);
	if(nbuf == NULL)
	{
		printf("Error: could not allocate buffer for name list!\n");
		return;
	}

	//Get the name list.
	res = ISFS_ReadDir(path, nbuf, &num);
	CheckISFSRetval(res);
	
	//Split up the name list.
	for(i = 0, k = 0; i < num; i++)
	{
		//The names are seperated by zeroes.
		for(j = 0; nbuf[k] != 0; j++, k++)
			ebuf[j] = nbuf[k];
		ebuf[j] = 0;
		k++;

		//Fill in the dir name.
		strcpy(filenames[i], ebuf);
		strcpy(smallfilenames[i], ebuf+sizeof(path));
	}

	free(nbuf);
	free(ebuf);
	free(pbuf);
	for(i=0;i<num;i++)
	{
		char* dest_file=calloc(256, sizeof(char));
		sprintf(dest_file, "%s/%s",destdir, smallfilenames[i]);
		copyfile(filenames[i], dest_file);
		free(dest_file);
	}
	free(filenames);
	free(smallfilenames);
}

void saveDeleter(u32 TitleIDH, u32 TitleIDL, u64 TitleID, char* titleName, char* subtitleName)
{
	ClearScreen();
	printf("\x1b[1;0HYou are about to delete the savegame for:\n\"%s\",\"%s\"! Are you sure? \x1b[32;1mA\x1b[39;0m for Yes, \x1b[31;1mB\x1b[39;0m for No.\n",titleName,subtitleName);
	while(1) {
		WIILIGHT_SetLevel(255);
		WPAD_ScanPads();
		int buttonsDown = WPAD_ButtonsDown(0);
		if(buttonsDown & WPAD_BUTTON_A)
			break;
		if(buttonsDown & WPAD_BUTTON_B)
			return;
	}
	printf("Are you ABSOLUTELY SURE!!! YOU WILL LOSE THE DATA PERMANENTLY!!!\n\n");
	while(1) {
		WIILIGHT_SetLevel(255);
		WPAD_ScanPads();
		int buttonsDown = WPAD_ButtonsDown(0);
		if(buttonsDown & WPAD_BUTTON_A)
			break;
		if(buttonsDown & WPAD_BUTTON_B)
			return;
	}
	if(need_sys)
		SystemMenuAuth();
	char titlefilepath[256];
	sprintf(titlefilepath, "/title/%08x/%08x/data", TitleIDH, TitleIDL);
	printf("\nDeleting Save: %s...", titlefilepath);
	fflush(stdout);
	CheckISFSRetval(ISFS_Delete(titlefilepath));
	printf(" OK!");

	ClearScreen();
}

void saveManager()
{
	printf("\n\n");
	if(need_sys)
		SystemMenuAuth();

	u32 TitleCount ATTRIBUTE_ALIGN(32);
	dbgprintf("Getting Title Count\n");
	CheckESRetval(ES_GetNumTitles( &TitleCount ));

	u64 *title_list = (u64*)memalign( 32, ALIGN_LENGTH(sizeof(u64)*TitleCount) );
	u64 *TitlesIDs = (u64*)memalign( 32, ALIGN_LENGTH(sizeof(u64)*TitleCount) );
	char names[TitleCount][0x50];

	memset( title_list, 0, sizeof(u64)*TitleCount );
	memset( TitlesIDs, 0, sizeof(u64)*TitleCount );

	dbgprintf("Getting titles\n");
	CheckESRetval(ES_GetTitles( title_list, TitleCount ));
	dbgprintf("Got titles\n");

	int MAX_ENTRIES=MAX_SAVE_ENTRIES;

	int ListOff=0;
	int ListEntries=0;
	int cur_off=0;
	int i=0;
	int j=0;
	int eol=0; //End of list
	int redraw=1;
	while(1)
	{
		u32 TitleIDH ATTRIBUTE_ALIGN(32) = title_list[j+ListOff]>>32;
		u32 TitleIDL ATTRIBUTE_ALIGN(32) = title_list[j+ListOff];
		u64 titleID  ATTRIBUTE_ALIGN(32) = title_list[j+ListOff];
		j++;

		if( j+ListOff > TitleCount )
		{
			break;
		}

		if( TitleIDH == 0x00000001 || TitleIDH == 0x00010008 || TitleIDL == 0x48414141  )
			continue;

		printf("%016llx %016llx ", titleID, title_list[j]);
		dbgprintf("Setting UID\n");
		identify_title(title_list[j]);
//		CheckESRetval(ES_SetUID(titleID));
		dbgprintf("Set UID\n");
				
		char *DataDir = (char*)memalign( 32, ALIGN_LENGTH(256) );
		memset( DataDir, 0, 256 );

		dbgprintf("Getting Data Dir\n");
		CheckESRetval(ES_GetDataDir(title_list[j], DataDir));
		dbgprintf("Got Data Dir\n");

		u64 sz ATTRIBUTE_ALIGN(32) = 0x140;

		char *data = (char*)memalign(32, ALIGN_LENGTH(sz));
		memset( data, 0, sz );

		if( data != NULL )
		{
			sprintf( DataDir, "%s/banner.bin", DataDir );

			s32 nameFD ATTRIBUTE_ALIGN(32) = ISFS_Open( DataDir, ISFS_OPEN_READ );

			CheckISFSRetval(nameFD);

			CheckISFSRetval(ISFS_Read(nameFD, data, sz));

//			char TitleName[0x50];

			int y;
			for(y=0;y<0x50;y++)
				names[ListEntries][y]=data[0x20+(y*2)+1];
					
			free( data );
			ListEntries++;

			CheckISFSRetval(ISFS_Close(nameFD));
		}

		free( DataDir );
	}
	while(1)
	{
//		printf("\x1b[1;30H%d:%d:%d:%d     ", cur_off, ListOff, ListEntries, eol);
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
			break;

		if ( (WPAD_Pressed & WPAD_BUTTON_B) || (PAD_Pressed & PAD_BUTTON_B) )
			break;
		if ( (WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A) )
		{
			char *tab[] = {"Copy to SD Card", "Delete", "Back"};
			char cap[100]; sprintf(cap, "\n\nWhat would you like to do with %s:", names[ListOff]);
			s32 res = showmenu(cap, tab, 3, 0, " ->");
			char savefilename[256]; 
			switch(res)
			{
				case 0:
					sprintf(savefilename, "/wiimu/saves/%s/", names[ListOff]);
					saveToSD(TitlesIDs[ListOff]>>32,TitlesIDs[ListOff],names[ListOff],names[ListOff]+0x20, savefilename);
					break;
				case 1:
					saveDeleter(TitlesIDs[ListOff]>>32,TitlesIDs[ListOff], TitlesIDs[ListOff], names[ListOff],names[ListOff]+0x20);
					break;
				default:
					break;
			}
		}
		if ( (WPAD_Pressed & WPAD_BUTTON_DOWN) || (PAD_Pressed & PAD_BUTTON_DOWN) )
		{
			printf("\x1b[%d;0H  ", HEIGHT_BEGIN+cur_off);
			cur_off++;
			ListOff++;
			if(ListOff>=ListEntries)
			{
				eol=1;
				ListOff=ListEntries-1;
			}
			if( cur_off >= MAX_ENTRIES || cur_off >= ListEntries)
			{
				cur_off = MAX_ENTRIES-1;
				if( !eol )
					redraw=1;
			}
			printf("\x1b[%d;0H->", HEIGHT_BEGIN+cur_off);
		} else if ( (WPAD_Pressed & WPAD_BUTTON_UP) || (PAD_Pressed & PAD_BUTTON_UP) )
		{
			printf("\x1b[%d;0H  ", HEIGHT_BEGIN+cur_off);
			cur_off--;
			ListOff--;
			if( cur_off < 0 )
			{
				cur_off=0;
				redraw=1;
			}
			if(ListOff<0)
				ListOff=0;
			if(ListOff>=ListEntries)
				eol=1;
			else
				eol=0;
			printf("\x1b[%d;0H->", HEIGHT_BEGIN+cur_off);
		}

		if( redraw )
		{
			ClearScreen();
			printf("\x1b[%d;0H\n",HEIGHT_BEGIN);

			i=0;
			j=0;
//			ListEntries=0;
			if(ListOff-(MAX_ENTRIES-1) > 0)
				i=ListOff-(MAX_ENTRIES-1);
			else 
				i=ListOff;
			int x;
			for(x=0;i<ListEntries && x<MAX_ENTRIES;i++,x++)
			{
				printf("\x1b[%d;0H   %s, %s                               \n", x+HEIGHT_BEGIN, names[i], names[i]+0x20 );
			}

			TitlesIDs[i] = title_list[(j+ListOff)-1];
			printf("\x1b[%d;0H->", HEIGHT_BEGIN+cur_off);

			redraw = 0;
		}
 
 		VIDEO_WaitVSync();
	}

}

