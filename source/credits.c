#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#include "credits.h"
#include "screen.h"

long scrollSpeed;
long originalScrollSpeed;
int isFinished;
static void * credits_speed_loop(void *arg);
lwp_t credits_speed_thread;

static void * credits_speed_loop(void *arg)
{
	while(!isFinished)
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

		if((WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A))
		{
			scrollSpeed=originalScrollSpeed/2;
		}else
			scrollSpeed=originalScrollSpeed;
	}
	return NULL;	
}

void WaitForAButton()
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

	while( !((WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A)) )
	{
		WPAD_ScanPads();
		PAD_ScanPads();
		WPAD_Pressed = WPAD_ButtonsDown(0);
		WPAD_Pressed |= WPAD_ButtonsDown(1);
		WPAD_Pressed |= WPAD_ButtonsDown(2);
		WPAD_Pressed |= WPAD_ButtonsDown(3);
		PAD_Pressed  = PAD_ButtonsDown(0);
		PAD_Pressed  |= PAD_ButtonsDown(1);
		PAD_Pressed  |= PAD_ButtonsDown(2);
		PAD_Pressed  |= PAD_ButtonsDown(3);
	}
	while( (WPAD_Pressed & WPAD_BUTTON_A) || (PAD_Pressed & PAD_BUTTON_A) )
	{
		WPAD_ScanPads();
		PAD_ScanPads();
		WPAD_Pressed = WPAD_ButtonsDown(0);
		WPAD_Pressed |= WPAD_ButtonsDown(1);
		WPAD_Pressed |= WPAD_ButtonsDown(2);
		WPAD_Pressed |= WPAD_ButtonsDown(3);
		PAD_Pressed  = PAD_ButtonsDown(0);
		PAD_Pressed  |= PAD_ButtonsDown(1);
		PAD_Pressed  |= PAD_ButtonsDown(2);
		PAD_Pressed  |= PAD_ButtonsDown(3);
	}
}

void credits_style1()
{
	originalScrollSpeed=(long)((1)*1000000000);
	scrollSpeed=originalScrollSpeed;
	isFinished=0;
	ClearScreen();
	printf("\n\n");
	struct timespec credits_waitTime;
	struct timespec timeWait;
	credits_waitTime.tv_sec=(time_t)0;
	credits_waitTime.tv_nsec=scrollSpeed;
	LWP_CreateThread(&credits_speed_thread, credits_speed_loop,NULL,NULL,0,80);
/*	printf("+--------------------------------------------+\n");
	printf("|      ___ ___ ____    . _ _____ _____ _     |\n");
	printf("|     /  _| _ \  __)__/||_|_   _|  __/| |    |\n");
	printf("|     | (_|   /  __) _ || | | | \__  \|_|    |\n");
	printf("|     \___/_|_\____)___/|_| |_| /____/|_|    |\n");
	printf("|  Because everybody needs help eventually!  |\n");
	printf("+--------------------------------------------+\n");*/
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|           ___ ___ ____    . _ _____ _____ _          |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|          /  _| _ \\  __)__/||_|_   _|  __/| |         |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|          | (_|   /  __) _ || | | | \\__  \\|_|         |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|          \\___/_|_\\____)___/|_| |_| /____/|_|         |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|       Because everybody needs help eventually!       |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Main Coding: SquidMan                   (T_T) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Assistant Coder: crediar                (O.O) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Libraries: sysconf.c (tona),            (^_^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             libupdate (AlexLH),                (X_X) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             wiiuse (para),                     (=.=) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             libfat (svpe),                     (@_@) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             libogc (shagkur, WinterMute)       (0_0) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Other Code: Mupen64GC Team (ISFS Help), (*_*) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             Bool (Wiilight Demo),              (1_0) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             Redbird (YaWnD NAND Dumper),       (OwO) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             emu_kidid (GCBooter),              (OvO) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             Waninkoko (Wii or GC Game Check)   (X.X) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             tona (A lot of stuff =P)           (^_^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             raven (Multichoice code/Listings)  (>.<) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Assistance: bushing,                    (<.<) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             marcan,                            (^.^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             crediar,                           (O.O) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             tona                               (^_^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Testers: Muzer,                         (T.T) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             AlexLH                             (X_X) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|        Special Thanks: Team Twiizers,          (ToT) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             Team Twizzlers,                    (^o^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             WiiBrew.org,                       (-.-) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             #wiidev @ EFnet,                   (].[) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             #wiidevot @ EFnet,                 (^w^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             #wiihelp @ EFnet,                  (}/{) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             #hackmii @ EFnet,                  (HAX) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             My fangirl Dianne,                 (>w<) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             erisu-chan,                        (0o0) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             Kisho-kun,                         (*.*) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             Garrett Ranck-Buhr,                (o_o) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             My many supporters,                (*v*) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|             and You!                           (^v^) |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                  Press A to leave!                   |\n"); credits_waitTime.tv_nsec=scrollSpeed; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("|                                                      |\n"); credits_waitTime.tv_nsec=scrollSpeed/4; timeWait=credits_waitTime; nanosleep(&timeWait);
	printf("+------------------------------------------------------+\n");
	isFinished=1;
	WaitForAButton();
}

void credits()
{
	credits_style1();
}

