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
#include <wiiuse/wpad.h>
#include <fat.h>

#include "screen.h"
#include "general.h"
#include "pretty.h"
#include "rethandle.h"
#include "sysconf.h"

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void Finish(int retval)
{
	VIDEO_Flush();
	VIDEO_WaitVSync();
	printf("\n\nEnding ES!... ");
	WIILIGHT_SetLevel(255/4);
	int retval2=__ES_Close();
	WIILIGHT_SetLevel(255);
	if(retval2!=0)
	{
		printf("FAILED!\n\tQuitting...\n");
		goto _es_phailed;
	}
	printf("SUCCESS!\nEnding ISFS!... ");
	WIILIGHT_SetLevel(255/4);
_es_phailed:
	retval2=ISFS_Deinitialize();
	WIILIGHT_SetLevel(255);
	if(retval2!=0)
	{
		printf("FAILED!\n\tQuitting...\n");
		goto _isfs_phailed;
	}
	printf("SUCCESS!\n");
	
_isfs_phailed:
	exit(retval);
}

void initialize_wiimu()
{
	VIDEO_Init();
	WPAD_Init();
	AUDIO_Init(NULL);
	PAD_Init();
	rmode=VIDEO_GetPreferredMode(NULL);
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	VIDEO_WaitVSync();
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	if(__ES_Init()!=0)
	{
		printf("FAILED!\n\tQuitting... \n");
		RetvalFail(1);
	}
	if(ISFS_Initialize()!=0)
	{
		printf("FAILED!\n\tQuitting... \n");
		RetvalFail(1);
	}
	if(WII_Initialize()!=0)
	{
		printf("FAILED!\n\tQuitting... \n");
		RetvalFail(1);
	}
	fatInitDefault();
	
	Light_Start();
	language_setting=SYSCONF_GetLanguage();
	
}

void getScreenData(void* fb, GXRModeObj* mode)
{
	fb=xfb;
	mode=rmode;
}

void setScreenData(void* fb, GXRModeObj* mode)
{
	xfb=fb;
	rmode=mode;
}

void ClearScreen()
{
	VIDEO_WaitVSync();
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
}

s32 STM_ForceDimming(u32 amount)
{
	int res;
	static u32 bufin[0x08] ATTRIBUTE_ALIGN(32);
	//_viReg[1] = 0;
	bufin[0] = amount;
	s32 stm_fd=IOS_Open("/dev/stm/immediate",0);
	res= IOS_Ioctl(stm_fd,0x5001,bufin,0x20,NULL,0x20);
	IOS_Close(stm_fd);
	return res;
}

void FadeOut()
{
	int i;
	for(i=0;i<100;i++)
	{
		WIILIGHT_SetLevel((255*i)/100);
//		STM_ForceDimming(i);
	}
}

void FadeIn()
{
	int i;
	for(i=100;i<0;i--)
	{
		WIILIGHT_SetLevel((255*i)/100);
//		STM_ForceDimming(i);
	}
}

int showmenu(char *caption, char *ent[], int ecnt, int _default, char* selector)
{
	int i, sel, run, cnt;
	
	cnt = ecnt - 1;
	
	//Sanity check.
	if(ent == NULL)
		return -1;
	if(_default > cnt)
		return -1;
	
	sel = _default;
	run = 1;
	while(run)
	{
		ClearScreen();
		printf("%s\n", caption);
		
		for(i = 0; i < ecnt; i++) {
			if(i == sel){
				printf("%s", selector);
			}else{
				printf("   ");
			}printf("%s\n", ent[i]);
		}
		
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

		if(WPAD_Pressed & WPAD_BUTTON_DOWN || PAD_Pressed & PAD_BUTTON_DOWN)
		{
			if(sel < cnt)
				sel++;
			else
				sel = 0;
		}
		
		if(WPAD_Pressed & WPAD_BUTTON_UP || PAD_Pressed & PAD_BUTTON_UP)
		{
			if(sel > 0)
				sel--;
			else
				sel = cnt;
		}
		
		if(WPAD_Pressed & WPAD_BUTTON_A || PAD_Pressed & PAD_BUTTON_A)
			run = 0;
	}
	
	return sel;
}

//Borrowed from bushing's patchmii_core.
char spinner_chars[] = "/-\\|";
int spin = 0;
void spinner()
{
	printf("\b%c", spinner_chars[spin++]);
	if(!spinner_chars[spin])
		spin=0;
}
////////////////////////////////////////

