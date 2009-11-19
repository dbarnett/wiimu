#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <ogcsys.h>
#include <gccore.h>
#include <ogc/system.h>
#include <wiiuse/wpad.h>

#include "credits.h"
#include "screen.h"
#include "controls.h"

extern volatile int A_button_held;

typedef struct _credits_line {
	char line[60];
	int scroll_delay;
} credits_line;

const credits_line credits_data[] = {
{"+------------------------------------------------------+", 1},
{"|           ___ ___ ____    . _ _____ _____ _          |", 1},
{"|          /  _| _ \\  __)__/||_|_   _|  __/| |         |", 1},
{"|          | (_|   /  __) _ || | | | \\__  \\|_|         |", 1},
{"|          \\___/_|_\\____)___/|_| |_| /____/|_|         |", 1},
{"|       Because everybody needs help eventually!       |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|        Main Coding: SquidMan                   (T_T) |", 4},
{"|        Assistant Coder: crediar                (O.O) |", 4},
{"|        Assistant Coder: mu_mind                (b.d) |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|        Libraries: sysconf.c (tona),            (^_^) |", 4},
{"|             libupdate (AlexLH),                (X_X) |", 4},
{"|             wiiuse (para),                     (=.=) |", 4},
{"|             libfat (svpe),                     (@_@) |", 4},
{"|             libogc (shagkur, WinterMute)       (0_0) |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|        Other Code: Mupen64GC Team (ISFS Help), (*_*) |", 4},
{"|             Bool (Wiilight Demo),              (1_0) |", 4},
{"|             Redbird (YaWnD NAND Dumper),       (OwO) |", 4},
{"|             emu_kidid (GCBooter),              (OvO) |", 4},
{"|             Waninkoko (Wii or GC Game Check)   (X.X) |", 4},
{"|             tona (A lot of stuff =P)           (^_^) |", 4},
{"|             raven (Multichoice code/Listings)  (>.<) |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|        Assistance: bushing,                    (<.<) |", 4},
{"|             marcan,                            (^.^) |", 4},
{"|             crediar,                           (O.O) |", 4},
{"|             tona                               (^_^) |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|        Testers: Muzer,                         (T.T) |", 4},
{"|             AlexLH                             (X_X) |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|        Special Thanks: Team Twiizers,          (ToT) |", 4},
{"|             Team Twizzlers,                    (^o^) |", 4},
{"|             WiiBrew.org,                       (-.-) |", 4},
{"|             #wiidev @ EFnet,                   (].[) |", 4},
{"|             #wiidevot @ EFnet,                 (^w^) |", 4},
{"|             #wiihelp @ EFnet,                  (}/{) |", 4},
{"|             #hackmii @ EFnet,                  (HAX) |", 4},
{"|             My fangirl Dianne,                 (>w<) |", 4},
{"|             erisu-chan,                        (0o0) |", 4},
{"|             Kisho-kun,                         (*.*) |", 4},
{"|             Garrett Ranck-Buhr,                (o_o) |", 4},
{"|             My many supporters,                (*v*) |", 4},
{"|             and You!                           (^v^) |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"|                                                      |", 1},
{"|                  Press A to leave!                   |", 4},
{"|                                                      |", 1},
{"+------------------------------------------------------+", 1},
{"", -1} };

static const credits_line *credits_cursor = NULL;

static syswd_t credits_alarm;
static volatile int credits_time = 0;
static int last_credits_time = 0;

static void credits_alarm_event(syswd_t alarm, void *arg)
{
	credits_time += (A_button_held ? 2 : 1);
}

int chk_credits(void) {
	if ((credits_cursor == NULL) || ((*credits_cursor).scroll_delay == -1)) {
		SYS_RemoveAlarm(credits_alarm);
		return 0;
	}
	if (credits_time - last_credits_time >= (*credits_cursor).scroll_delay) {
		last_credits_time += (*credits_cursor).scroll_delay;
		if (credits_cursor == credits_data) {		// drawing first line
			ClearScreen();
            printf("\n\n");
		}
		printf("%s\n", (*credits_cursor).line);
		++credits_cursor;
	}
	return 1;
}

static void finish_credits(void) {
	load_menu(MAIN_MENU);
}

static void cancel_credits(void) {
	credits_cursor = NULL;
	chk_credits();
	finish_credits();
}

int credits_controls(button_t button, int source, int is_pressed) {
	if (!is_pressed)
		return 1;

	switch (button) {
		case A_BUTTON:
			if (chk_credits() == 0) {
				finish_credits();
			}
			break;
		case B_BUTTON:
			cancel_credits();
			break;
        case UP_BUTTON:
        case DOWN_BUTTON:
        case LEFT_BUTTON:
        case RIGHT_BUTTON:
            break;
        case HOME_BUTTON:
        case START_BUTTON:
            return 0;
	}

    return 1;
}

static struct timespec credits_start = {0, 0};
static struct timespec credits_tick = {0, 75000000L};

void credits_style1()
{
	credits_cursor = credits_data;
	last_credits_time = 0;
	credits_time = 0;
    SYS_CreateAlarm(&credits_alarm);
	SYS_SetPeriodicAlarm(credits_alarm, &credits_start, &credits_tick, &credits_alarm_event, NULL);
}

void credits()
{
	credits_style1();
}

