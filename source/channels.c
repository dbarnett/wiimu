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

#include "general.h"
#include "screen.h"
#include "cleanup.h"
#include "rethandle.h"
#include "auth.h"
#include "channels.h"

#include "pretty.h"
#include "sysmenu.h"
#include "su.h"
#include "editing.h"

extern int need_sys;

void launchTitle(u64 titleID, char* name, int need_sys)
{
	if(need_sys)
		SystemMenuAuth();

	VIDEO_Flush();
	VIDEO_WaitVSync();
	WIILIGHT_SetLevel(255);

	printf("Launching %s!... ", name);
	FadeOut();
	ClearScreen();

	CheckWIIRetval(WII_LaunchTitle(titleID));
}

void deleteChannel(u64 titleID, char* titleName)
{
	ClearScreen();
	printf("\x1b[1;0HYou are about to delete \"%s\"!\nAre you sure? \x1b[32;1mA\x1b[39;0m for Yes, \x1b[31;1mB\x1b[39;0m for No.\n",titleName);
	while(1) {
		WIILIGHT_SetLevel(255);
		WPAD_ScanPads();
		int buttonsDown = WPAD_ButtonsDown(0);
		if(buttonsDown & WPAD_BUTTON_A)
			break;
		if(buttonsDown & WPAD_BUTTON_B)
			return;
	}
	printf("Identifying as 00000001-00000000 (SU)!... ");
	CheckESRetval(ES_Identify(SU_IDENTIFY));
	need_sys = 1;
	printf(" OK!\n");
	u32 TitleIDH  ATTRIBUTE_ALIGN(32)=  titleID>>32;
	u32 TitleIDL  ATTRIBUTE_ALIGN(32)=  titleID;
	printf("\x1b[5;0HWould you like to also delete the ticket?\nThis will remove most traces of the title, however, you will not be able to freely redownload from the shop.\n \x1b[32;1mA\x1b[39;0m for Yes, \x1b[31;1mB\x1b[39;0m for No.\n");
	int keeptik = 1;
	while(1) {
		WIILIGHT_SetLevel(255);
		WPAD_ScanPads();
		int buttonsDown = WPAD_ButtonsDown(0);
		if(buttonsDown & WPAD_BUTTON_A) { keeptik = 0;
			break; }
		if(buttonsDown & WPAD_BUTTON_B) { keeptik = 1;
			break; }
	}
	if(!keeptik) {
		char tikfilepath[256];
		sprintf(tikfilepath, "/ticket/%08x/%08x.tik", TitleIDH, TitleIDL);
		printf("Deleting Ticket: %s...", tikfilepath);
		fflush(stdout);
		CheckISFSRetval(ISFS_Delete(tikfilepath));
		printf(" OK!\n");
	}
	char titlefilepath[256];
	sprintf(titlefilepath, "/title/%08x/%08x", TitleIDH, TitleIDL);
	printf("Deleting Title: %s...", titlefilepath);
	fflush(stdout);
	CheckISFSRetval(ISFS_Delete(titlefilepath));
	printf(" OK!\n");
}

void ChannelLauncher()
{
	if(need_sys)
		SystemMenuAuth();

	unsigned int TitleCount ATTRIBUTE_ALIGN(32);
	CheckESRetval(ES_GetNumTitles( &TitleCount ));

	u64 *titles = (u64*)memalign( 32, sizeof(u64)*TitleCount );
	u64 *TitlesIDs = (u64*)memalign( 32, sizeof(u64)*TitleCount );

	memset( titles, 0, sizeof(u64)*TitleCount );
	memset( TitlesIDs, 0, sizeof(u64)*TitleCount );
	char names[TitleCount][0x28];

	CheckESRetval(ES_GetTitles( titles, TitleCount ));

	int MAX_ENTRIES=MAX_CHAN_ENTRIES;

	int ListOff=0;
	int ListEntries=0;
	int cur_off=0;
	int i=0;
	int j=0;
	int eol=0; //End of list
	int redraw=1;
	while(1)
	{
		u32 TitleIDH  ATTRIBUTE_ALIGN(32)=  titles[j+ListOff]>>32;
		u32 TitleIDL  ATTRIBUTE_ALIGN(32)=  titles[j+ListOff];
		u64 titleID   ATTRIBUTE_ALIGN(32)=  titles[j+ListOff];

		j++;

		if( j+ListOff > TitleCount )
		{
			eol=1;
			break;
		}
				
		//Don't show boring chans, 0x48414141 is the Disc Channel which isn't a loadable channel				
		if( TitleIDH == 0x00000001 || TitleIDH == 0x00010000 || TitleIDH == 0x00010008 || TitleIDL == 0x48414141  )
		{
			continue;
		}

		if(CheckESRetval(ES_SetUID(titleID))!=0)
			continue;
		u32 tmd_size ATTRIBUTE_ALIGN(32);


		if(CheckESRetval(ES_GetStoredTMDSize(titleID, &tmd_size))!=0)
			continue;

		signed_blob *TMD = (signed_blob *)memalign( 32, tmd_size );
		memset(TMD, 0, tmd_size);

		if(CheckESRetval(ES_GetStoredTMD(titleID, TMD, tmd_size))!=0)
		{
			free(TMD);
			continue;
		}

		u32 cnt ATTRIBUTE_ALIGN(32);

		if(CheckESRetval(ES_GetNumTicketViews(titleID, &cnt))!=0)
		{
			free(TMD);
			continue;
		}
		if( cnt <= 0 )
		{
			free(TMD);
			continue;
		}
		tikview *views = (tikview *)memalign( 32, sizeof(tikview)*cnt );

		if(CheckESRetval(ES_GetTicketViews(titleID, views, cnt))!=0)
		{
			free(views);
			free(TMD);
			continue;
		}
		int z;
		for(z=0; z < 1; ++z)
		{
			tmd_content *TMDc = TMD_CONTENTS(SIGNATURE_PAYLOAD(TMD));		// OH GOD CREDIAR, WTF WAS THAT MESS!!!
			//printf("%d,",TMDc->index);
			s32 cfd = ES_OpenTitleContent( titleID, views, TMDc->index);
			CheckESRetval(cfd);
			free(views);
			u64 sz ATTRIBUTE_ALIGN(32) = 0x140;
			u8 *data = (u8*)memalign(32, sz);
			if( TMDc->size < sz )
				sz = TMDc->size;

			if( data != NULL )
			{
				if(CheckESRetval(ES_ReadContent(cfd, data, sz))!=0)
				{
					free(data);
					continue;
				}

				int y;
				int lol;
				for(lol=0;lol<ListEntries;lol++)
				{
					if((u8)(TitlesIDs[lol]>>24)==(u8)(titleID>>24) && (u8)((TitlesIDs[lol]<<8)>>24)==(u8)((titleID<<8)>>24) && (u8)((TitlesIDs[lol]<<16)>>24)==(u8)((titleID<<16)>>24))
						goto no_copy;
				}
				int chan_name_offset=0x54;					// Default to English
				if(language_setting>0)						// Don't show Japanese, because we don't have unicode.
					chan_name_offset=(language_setting*0x54);
				for(y=0;y<0x28;y++)
					names[ListEntries][y]=data[(0x9C+chan_name_offset)+(y*2)+1]; //0x54

				TitlesIDs[ListEntries++] = titleID;
				i++;	
			}
no_copy:
			CheckESRetval(ES_CloseContent(cfd));
			free(data);
		}
		free( TMD );
	}

	//printf(")\n");
	eol=0;
	while(1)
	{
		//printf("\x1b[1;30H%d:%d:%d:%d     ", cur_off, ListOff, ListEntries, eol);
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

		if ( (WPAD_Pressed & WPAD_BUTTON_B) || (PAD_Pressed & PAD_BUTTON_B) )
		{
			free(TitlesIDs);
			free(titles);
			break;
		}
		if ( (WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A) )
		{
//			char *tab[] = {"Launch", "Change Name", "Delete", "Back"};
			char *tab[] = {"Launch", "Delete", "Back"};
			char cap[100]; sprintf(cap, "\n\nWhat would you like to do with %s:", names[ListOff]);
//			s32 res = showmenu(cap, tab, 4, 0, " ->");
			s32 res = showmenu(cap, tab, 3, 0, " ->");
			switch(res)
			{
				case 0:
					launchTitle(TitlesIDs[ListOff], names[ListOff], 0);
					break;
//				case 1:
//					changeChannelName(TitlesIDs[ListOff]);
//					break;
//				case 2:
				case 1:
					deleteChannel(TitlesIDs[ListOff], names[ListOff]);
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
			if( cur_off >= MAX_ENTRIES )
			{
				cur_off = MAX_ENTRIES-1;
				if( !eol )
					redraw=1;
			}
			if( cur_off >= ListEntries )
			{
				cur_off = ListEntries-1;
				if( !eol )
					redraw=1;
			}

			printf("\x1b[%d;0H ->", HEIGHT_BEGIN+cur_off);
		} else if ( (WPAD_Pressed & WPAD_BUTTON_UP) || (PAD_Pressed & PAD_BUTTON_UP) )
		{
			printf("\x1b[%d;0H   ", HEIGHT_BEGIN+cur_off);
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
			printf("\x1b[%d;0H ->", HEIGHT_BEGIN+cur_off);
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
				printf("\x1b[%d;0H   %s (%c%c%c%c)      \n", i+HEIGHT_BEGIN, names[i], (u8)(TitlesIDs[i]>>24), (u8)((TitlesIDs[i]<<8)>>24), (u8)((TitlesIDs[i]<<16)>>24), (u8)((TitlesIDs[i]<<24)>>24) );
			}

			printf("\x1b[%d;0H ->", HEIGHT_BEGIN+cur_off);

			redraw = 0;
		}

		VIDEO_WaitVSync();
	}
	
	return;

}

