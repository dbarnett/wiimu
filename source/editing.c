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
#include <ogc/machine/processor.h>
#include <wiiuse/wpad.h>
#include <wiiuse/wiiuse.h>
#include <fat.h>
#include <gcmodplay.h>

#include "screen.h"
#include "cleanup.h"
#include "rethandle.h"
#include "auth.h"
#include "general.h"

#include "sysmenu.h"
#include "su.h"
#include "yawnd.h"
#include "pretty.h"
#include "IOS_DVD.h"
#include "factory_mod.h"
#include "kbdhandle.h"
#include "editing.h"
#include "sysconf.h"
#include "sha1.h"
#include "md5.h"

#define CONSOLE_NICKNAME_OFFSET			0

#define LANGUAGE_OFFSET				1
#define WIIMU_LANGUAGE_OFFSET			2
#define REGION_OFFSET				3
#define AREA_OFFSET				4
#define SHOP_AREA_OFFSET			5

#define VIDEO_MODE_OFFSET			6
#define EUR_RGB_OFFSET				7
#define PROGRESSIVE_OFFSET			8
#define ASPECT_RATIO_OFFSET			9
#define SCREEN_SAVER_OFFSET			10

#define SOUND_OFFSET				11

#define IR_SENSITIVITY_OFFSET			12
#define SENSORBAR_POSITION_OFFSET		13
#define SPEAKER_VOLUME_OFFSET			14
#define MOTOR_MODE_OFFSET			15

#define PARENTAL_PASSWORD_OFFSET		16
#define PARENTAL_ANSWER_OFFSET			17

#define SAVE_OFFSET				18
#define EXIT_OFFSET				19

#define REGION_INFO_OFFSET			LANGUAGE_OFFSET
#define VIDEO_INFO_OFFSET			VIDEO_MODE_OFFSET
#define AUDIO_INFO_OFFSET			SOUND_OFFSET
#define WIIMOTE_INFO_OFFSET			IR_SENSITIVITY_OFFSET
#define PARENTAL_INFO_OFFSET			PARENTAL_PASSWORD_OFFSET
#define QUIT_OFFSET				SAVE_OFFSET

#define MENU_OPTIONS				EXIT_OFFSET
#define CONFIG_MENU_H				2


#define WIIMOTE_MII_DATA_BEGIN_1		0x0FD2
#define WIIMOTE_MII_DATA_BYTES_PER_SLOT		74
#define WIIMOTE_MII_SLOT_NUM			10

#define ERR_INVARG				-1001
#define ERR_ARRAYSIZE				-1002
#define ERR_UID					-1003
#define ERR_TMD					-1004
#define ERR_TMDSIZE				-1005
#define ERR_TIK					-1006
#define ERR_TIKCOUNT				-1007
#define ERR_CONTENT				-1008
#define ERR_NONE				0

#define SADR_LENGTH 0x1007+1


char *ON_MESSAGE[] = {
	"Shiyoukanou",				// Japanese
	"On",					// English
	"Ein",					// German
	"Sur",					// French
	"Aprendido",				// Spanish
	"Su",					// Italian
	"Aan",					// Dutch
};

char *OFF_MESSAGE[] = {
	"Shiyoukinshi",				// Japanese
	"Off",					// English
	"Aus",					// German
	"Hors Tension",				// French
	"Apagado",				// Spanish
	"Chiuso",				// Italian
	"Uit",					// Dutch
};

char *CONSOLE_NICKNAME[] = {
	"Console Nickname:            ",	// Japanese
	"Console Nickname:            ",	// English
	"Spitzname der Konsole:       ",	// German
	"Console Nickname:            ",	// French
	"Nombre de Systema:           ",	// Spanish
	"Console Nickname:            ",	// Italian
	"Console Nickname:            ",	// Dutch
};

char *REGION_INFO[] = {
	"Ittai no Ippou:",			// Japanese
	"Region Info:",				// English
	"Infos \x081 \bber die Region:",	// German
	"Info R\x082gion:",			// French
	"Informaci\x0a2n de Region:",		// Spanish
	"Info Regione:",			// Italian
	"Regio-Info:",				// Dutch
};

char *LANGUAGE[] = {
	"Onseigengo:                  ",	// Japanese
	"Language:                    ",	// English
	"Sprache:                     ",	// German
	"Langue:                      ",	// French
	"Idioma:                      ",	// Spanish
	"Lingua:                      ",	// Italian
	"Taal:                        ",	// Dutch
};

char *WIIMU_LANGUAGE[] = {
	"WiiMU no Onseigengo:         ",	// Japanese
	"WiiMU's Language:            ",	// English
	"WiiMU Sprache:               ",	// German
	"Langue WiiMU:                ",	// French
	"Idioma WiiMU:                ",	// Spanish
	"Lingua de WiiMU:             ",	// Italian
	"Taal-WiiMU:                  ",	// Dutch
};

char *REGION[] = {
	"Game Ittai:                  ",	// Japanese
	"Game Region:                 ",	// English
	"Spiel Region:                ",	// German
	"Game R\x082gion:             ",	// French
	"Region de Juego:             ",	// Spanish
	"Game Regione:                ",	// Italian
	"Game Regio:                  ",	// Dutch
};

char *AREA[] = {
	"Arika:                       ",	// Japanese
	"Area:                        ",	// English
	"Region:                      ",	// German
	"Domaine:                     ",	// French
	"Area:                        ",	// Spanish
	"Spazio:                      ",	// Italian
	"Ruimte:                      ",	// Dutch
};

char *SHOP_AREA[] = {
	"Shoppu no Gun Angoo:         ",	// Japanese
	"Shop Country Code:           ",	// English
	"Wii-Shop Landescode:         ",	// German
	"Boutique Du Code Du Pays:    ",	// French
	"Tienda Del C\x0a2 \bdigo De Pa\x0a1s:   ",	// Spanish
	"Shop Country Code:           ",	// Italian
	"Winkel Van De Landcode:      ",	// Dutch
};

char *VIDEO_INFO[] = {
	"Video no Ippou:",			// Japanese
	"Video Info:",				// English
	"Video-Info:",				// German
	"Des informations sur la vid\x082o:",	// French
	"Informaci\x0a2n de v\x0a1 \bdeo:",	// Spanish
	"Informazioni sul video:",		// Italian
	"Video-informatie:",			// Dutch
};

char *VIDEO_MODE[] = {
	"Video Modo:                  ",	// Japanese
	"Video Mode:                  ",	// English
	"Video-Modus:                 ",	// German
	"Mode vid\x082o:                  ",	// French
	"Modo de v\x0a1 \bdeo:               ",	// Spanish
	"Modalit\x085 Video:              ",	// Italian
	"Video Mode:                  ",	// Dutch
};

char *EURRGB[] = {
	"Ooshuu RGB 60Hz:             ",	// Japanese
	"Europe RGB 60Hz:             ",	// English
	"Europa RGB 60Hz:             ",	// German
	"Europe RGB 60Hz:             ",	// French
	"Europa RGB 60Hz:             ",	// Spanish
	"Europa RGB 60Hz:             ",	// Italian
	"Europa RGB 60Hz:             ",	// Dutch
};

char *PROGRESSIVE[] = {
	"Progressive Scan:            ",	// Japanese
	"Progressive Scan:            ",	// English
	"Progressiv Scan:             ",	// German
	"Progressive Scan:            ",	// French
	"Progressive Scan:            ",	// Spanish
	"Progressive Scan:            ",	// Italian
	"Progressive Scan:            ",	// Dutch
};

char *ASPECT_RATIO[] = {
	"Aspect Ratio:                ",	// Japanese
	"Aspect Ratio:                ",	// English
	"Aspektverh\x084ltnis:            ",	// German
	"Aspect Ratio:                ",	// French
	"Aspect Ratio:                ",	// Spanish
	"Aspect Ratio:                ",	// Italian
	"Aspect Ratio:                ",	// Dutch
};

char *SCREEN_SAVER[] = {
	"Setsuyaku no Furuiwakeru:    ",	// Japanese
	"Screensaver:                 ",	// English
	"Bildschirmschoner:           ",	// German
	"Economiseur d'\x082 \bcran:         ",	// French
	"Fondo de Pantalla:           ",	// Spanish
	"Screensaver:                 ",	// Italian
	"Screensaver:                 ",	// Dutch
};

char *AUDIO_INFO[] = {
	"Audio no Ippou:",			// Japanese
	"Audio Info:",				// English
	"Audio-Info:",				// German
	"Audio Info:",				// French
	"Info de Audio:",			// Spanish
	"Informazioni audio:",			// Italian
	"Audio Info:",				// Dutch
};

char *SOUND[] = {
	"Onkyoo:                      ",	// Japanese
	"Sound:                       ",	// English
	"Audio:                       ",	// German
	"Sound:                       ",	// French
	"Sonido:                      ",	// Spanish
	"Suono:                       ",	// Italian
	"Geluid:                      ",	// Dutch
};

char *POWER_INFO[] = {
	"Power no Ippou:",			// Japanese
	"Power Info:",				// English
	"Leistung:",				// German
	"Power Info:",				// French
	"Informaci\x0a2n de Energia:",		// Spanish
	"Info Potere:",				// Italian
	"Power Info:",				// Dutch
};

char *IDLE_MODE[] = {
	"Fudoo Modo:                  ",	// Japanese
	"Idle Mode:                   ",	// English
	"Idle-Modus:                  ",	// German
	"Mode Inactif:                ",	// French
	"Idle Modo:                   ",	// Spanish
	"Modalit\x085 Stand-by:           ",	// Italian
	"Stationair draaien:          ",	// Dutch
};

char *SHUTDOWN_MODE[] = {
	"Heisa Modo:                  ",	// Japanese
	"Shutdown Mode:               ",	// English
	"Shutdown-Modus:              ",	// German
	"Mode Veille:                 ",	// French
	"Modo de Apagar:              ",	// Spanish
	"Modalit\x085 Di Arresto:         ",	// Italian
	"Afsluiten Mode:              ",	// Dutch
};

char *WIIMOTE_INFO[] = {
	"Remocon no Ippou:",			// Japanese
	"Wiimote Info:",			// English
	"Wiimote Info:",			// German
	"Wiimote Info:",			// French
	"Wiimote Informaci\x0a2n:",			// Spanish
	"Info Wiimote:",			// Italian
	"Wiimote Info:",			// Dutch
};

char *IR_SENSITIVITY[] = {
	"Kando no Sekigai:            ",	// Japanese
	"Infrared Sensitivity:        ",	// English
	"Infrarot-Empfindlichkeit:    ",	// German
	"Sensibilit\x082 Infrarouge:      ",	// French
	"Sensibilidad de Infrarrojo:  ",	// Spanish
	"Sensibilit\x085 Infrarossi:      ",	// Italian
	"Infrarood Gevoeligheid:      ",	// Dutch
};

char *SENSORBAR_POSITION[] = {
	"Sensorbar Position:          ",	// Japanese
	"Sensorbar Position:          ",	// English
	"Position der Sensorleiste:   ",	// German
	"Sensorbar Position:          ",	// French
	"Sensorbar Posici\x0a2n:          ",	// Spanish
	"Sensorbar Position:          ",	// Italian
	"Sensorbar Position:          ",	// Dutch
};

char *SPEAKER_VOLUME[] = {
	"Speaker Volume:              ",	// Japanese
	"Speaker Volume:              ",	// English
	"Lautst\x084rke:                  ",	// German
	"Speaker Volume:              ",	// French
	"Volumen del Altavoz:         ",	// Spanish
	"Speaker Volume:              ",	// Italian
	"Speaker Volume:              ",	// Dutch
};

char *MOTOR_MODE[] = {
	"Motor Mode:                  ",	// Japanese
	"Motor Mode:                  ",	// English
	"R\x081ttelmodus:                 ",	// German
	"Motor Mode:                  ",	// French
	"Modo de Motor:               ",	// Spanish
	"Motor Mode:                  ",	// Italian
	"Motor Mode:                  ",	// Dutch
};

char *PARENTAL_INFO[] = {
	"Parental Control Info:",		// Japanese
	"Parental Control Info:",		// English
	"Parental Control Info:",		// German
	"Parental Control Info:",		// French
	"Informaci\x0a2n de Control de Padres:",	// Spanish
	"Parental Control Info:",		// Italian
	"Parental Control Info:",		// Dutch
};

char *PARENTAL_PASSWORD[] = {
	"Password:                    ",	// Japanese
	"Password:                    ",	// English
	"Pa\x0e1wort:                     ",	// German
	"Password:                    ",	// French
	"Contrase\x0a4 \ba:                  ",	// Spanish
	"Password:                    ",	// Italian
	"Password:                    ",	// Dutch
};

char *PARENTAL_ANSWER[] = {
	"Answer:                      ",	// Japanese
	"Answer:                      ",	// English
	"Antwort:                     ",	// German
	"Answer:                      ",	// French
	"Respuesta:                   ",	// Spanish
	"Answer:                      ",	// Italian
	"Answer:                      ",	// Dutch
};

char *SAVE[] = {
	"Save",					// Japanese
	"Save",					// English
	"Speichern",				// German
	"Save",					// French
	"Save",					// Spanish
	"Save",					// Italian
	"Save",					// Dutch
};

char *EXIT[] = {
	"Exit",					// Japanese
	"Exit",					// English
	"Beenden",				// German
	"Exit",					// French
	"Exit",					// Spanish
	"Exit",					// Italian
	"Exit",					// Dutch
};

char *video[] = {
	"NTSC",
	"PAL",
	"MPAL",
};
char *region[] = {
	"Japan",
	"United States",
	"Europe",
};
char *area[] = {
	"Japan",
	"United States",
	"Europe",
	"Australia",
	"Britain",
	"Taiwan",
	"Republic of Chine",
	"Korea",
	"Hong Kong?",
	"Ascension Island?",
	"Latin America",
	"South Africa",
};
char *shutdown[] = {
	"Standby",
	"Idle",
};
char *led[] = {
	"OFF",
	"Dim",
	"Bright",
};
char *sound[] = {
	"Mono",
	"Stereo",
	"Surround",
};
char *lang[] = {
	"Japanese",
	"English",
	"German",
	"French",
	"Spanish",
	"Italian",
	"Dutch",
};
char *sensorbar[] = {
	"Bottom",
	"Top",
};

// Speaker Volume:
// 0   = 0  Bars
// 12  = 1  Bar
// 25  = 2  Bars
// 38  = 3  Bars
// 50  = 4  Bars
// 63  = 5  Bars
// 76  = 6  Bars
// 88  = 7  Bars
// 101 = 8  Bars
// 114 = 9  Bars
// 127 = 10 Bars

char* speaker_volumes[]={
	"0 Bars",
	"1 Bar",
	"2 Bars",
	"3 Bars",
	"4 Bars",
	"5 Bars",
	"6 Bars",
	"7 Bars",
	"8 Bars",
	"9 Bars",
	"10 Bars",
};

int bar_to_raw[]={
	0,
	12,
	25,
	38,
	50,
	63,
	76,
	88,
	101,
	114,
	127,
};

int raw_to_bar[]={
0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,1,1,1,
2,2,2,2,2,2,2,2,2,2,2,2,2,
3,3,3,3,3,3,3,3,3,3,3,3,
4,4,4,4,4,4,4,4,4,4,4,4,4,
5,5,5,5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,7,7,7,7,7,
8,8,8,8,8,8,8,8,8,8,8,8,8,
9,9,9,9,9,9,9,9,9,9,9,9,9,
10,
};

u8 sadr[SADR_LENGTH];
int country;

int dont_exit_configure=1;
extern wiimote **__wpads;
int language_setting;

void save_options()
{
	printf("\nWARNING: READ THE README FIRST!\nPress \x1b[32;1mA\x1b[39;0m to save or \x1b[31;1mB\x1b[39;0m to return to the Configuration.\n");
	while(1)
	{
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

 		if((WPAD_Pressed & WPAD_BUTTON_A)	|| (PAD_Pressed & PAD_BUTTON_A))
		{
			if (country != sadr[0]){
				memset(sadr, 0, SADR_LENGTH);
				sadr[0] = country;
				int ret = SYSCONF_Set("IPL.SADR", sadr, SADR_LENGTH);
				if (ret) { printf("SYSCONF_Set IPL.SADR %d", ret); sleep(5); return;}
			}
			SYSCONF_SaveChanges();
			ClearScreen();
			return;
		}
 		if((WPAD_Pressed & WPAD_BUTTON_B)	|| (PAD_Pressed & PAD_BUTTON_B))
		{
			ClearScreen();
			return;
		}
		VIDEO_WaitVSync();
	}
}

void quit_options()
{
	printf("\nIf you quit, all unsaved changes will be lost.\nPress \x1b[32;1mA\x1b[39;0m to exit or \x1b[31;1mB\x1b[39;0m to return to the Configuration.\n");
	while(1)
	{
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

 		if((WPAD_Pressed & WPAD_BUTTON_A)	|| (PAD_Pressed & PAD_BUTTON_A))
		{
			dont_exit_configure=0;
			ClearScreen();
			return;
		}
 		if((WPAD_Pressed & WPAD_BUTTON_B)	|| (PAD_Pressed & PAD_BUTTON_B))
		{
			ClearScreen();
			return;
		}
		VIDEO_WaitVSync();
	}
}
void activate_option(int option)
{
	u8 *temp;
	int length;
	switch(option)
	{
		case CONSOLE_NICKNAME_OFFSET:
			temp=(u8*)memalign(32, 128);
			memset(temp,0,sizeof temp);//SYSCONF_GetNickName(temp);
			ClearScreen();
			printf("\n\nEnter new name for your console: %s",temp);
			length=type_string_wiimote((char*)temp, 10);
//			length=type_string_usb_kbd(temp, 10);
			if(length>0)
			{
				SYSCONF_SetNickName(temp, length);
				free(temp);
			}
			break;

		case PARENTAL_PASSWORD_OFFSET:
/*			temp=(u8*)memalign(32, 128);
			SYSCONF_GetParentalPassword(temp);
			length=type_string_usb_kbd(temp, 4);
			if(length!=4)
			{
				for(i=length;i<4;i++)
					temp[i]='\0';
			}
			SYSCONF_SetParentalPassword(temp, 4);
			free(temp);*/
			break;

		case PARENTAL_ANSWER_OFFSET:
/*			temp=(u8*)memalign(32, 128);
			SYSCONF_GetParentalAnswer(temp);
			type_string_usb_kbd(temp, 32);
			if(length!=32)
			{
				for(i=length;i<32;i++)
					temp[i]='\0';
			}
			SYSCONF_SetParentalAnswer(temp);
			free(temp);*/
			break;

		case SAVE_OFFSET:
			save_options();
			break;

		case EXIT_OFFSET:
			quit_options();
			break;

		default:
			break;
	}
}

void increase_option(int option)
{
	int tempvalue;
	switch(option)
	{
		case LANGUAGE_OFFSET:
			tempvalue=SYSCONF_GetLanguage();
			tempvalue++;
			tempvalue%=7;
			SYSCONF_SetLanguage(tempvalue);
			break;

		case WIIMU_LANGUAGE_OFFSET:
			language_setting++;
			language_setting%=7;
			ClearScreen();
			break;

		case REGION_OFFSET:
			tempvalue=SYSCONF_GetRegion();
			tempvalue++;
			tempvalue%=3;
			SYSCONF_SetRegion(tempvalue);
			break;

		case SHOP_AREA_OFFSET:
//			tempvalue=SYSCONF_GetShopCode();
			country++;
			country%=256;
//			SYSCONF_SetShopCode(tempvalue);
			break;

		case VIDEO_MODE_OFFSET:
			tempvalue=SYSCONF_GetVideo();
			tempvalue++;
			tempvalue%=3;
			SYSCONF_SetVideo(tempvalue);
			break;

		case EUR_RGB_OFFSET:
			tempvalue=SYSCONF_GetEuRGB60();
			tempvalue++;
			tempvalue%=2;
			SYSCONF_SetEuRGB60(tempvalue);
			break;

		case PROGRESSIVE_OFFSET:
			tempvalue=SYSCONF_GetProgressiveScan();
			tempvalue++;
			tempvalue%=2;
			SYSCONF_SetProgressiveScan(tempvalue);
			break;

		case ASPECT_RATIO_OFFSET:
			tempvalue=SYSCONF_GetAspectRatio();
			tempvalue++;
			tempvalue%=2;
			SYSCONF_SetAspectRatio(tempvalue);
			break;

		case SCREEN_SAVER_OFFSET:
			tempvalue=SYSCONF_GetScreenSaverMode();
			tempvalue++;
			tempvalue%=2;
			SYSCONF_SetScreenSaverMode(tempvalue);
			break;

		case SOUND_OFFSET:
			tempvalue=SYSCONF_GetSoundMode();
			tempvalue++;
			tempvalue%=3;
			SYSCONF_SetSoundMode(tempvalue);
			break;

		case IR_SENSITIVITY_OFFSET:
			tempvalue=SYSCONF_GetIRSensitivity();
			tempvalue++;
			tempvalue%=5;
			SYSCONF_SetIRSensitivity(tempvalue);
			break;

		case SENSORBAR_POSITION_OFFSET:
			tempvalue=SYSCONF_GetSensorBarPosition();
			tempvalue++;
			tempvalue%=2;
			SYSCONF_SetSensorBarPosition(tempvalue);
			break;

		case SPEAKER_VOLUME_OFFSET:
			tempvalue=raw_to_bar[SYSCONF_GetPadSpeakerVolume()];
			tempvalue++;
			tempvalue%=11;
			SYSCONF_SetPadSpeakerVolume(bar_to_raw[tempvalue]);
			break;

		case MOTOR_MODE_OFFSET:
			tempvalue=SYSCONF_GetPadMotorMode();
			tempvalue++;
			tempvalue%=2;
			SYSCONF_SetPadMotorMode(tempvalue);
			break;

	}
}

void decrease_option(int option)
{
	int tempvalue;
	switch(option)
	{
		case LANGUAGE_OFFSET:
			tempvalue=SYSCONF_GetLanguage();
			tempvalue--;
			if(tempvalue<0) tempvalue=6;
			SYSCONF_SetLanguage(tempvalue);
			break;

		case WIIMU_LANGUAGE_OFFSET:
			language_setting--;
			if(language_setting<0) language_setting=6;
			ClearScreen();
			break;

		case REGION_OFFSET:
			tempvalue=SYSCONF_GetRegion();
			tempvalue--;
			if(tempvalue<0) tempvalue=2;
			SYSCONF_SetRegion(tempvalue);
			break;

		case SHOP_AREA_OFFSET:
			tempvalue=SYSCONF_GetShopCode();
			tempvalue--;
			if(tempvalue<0) tempvalue=255;
			SYSCONF_SetShopCode(tempvalue);
			break;

		case VIDEO_MODE_OFFSET:
			tempvalue=SYSCONF_GetVideo();
			tempvalue--;
			if(tempvalue<0) tempvalue=2;
			SYSCONF_SetVideo(tempvalue);
			break;

		case EUR_RGB_OFFSET:
			tempvalue=SYSCONF_GetEuRGB60();
			tempvalue--;
			if(tempvalue<0) tempvalue=1;
			SYSCONF_SetEuRGB60(tempvalue);
			break;

		case PROGRESSIVE_OFFSET:
			tempvalue=SYSCONF_GetProgressiveScan();
			tempvalue--;
			if(tempvalue<0) tempvalue=1;
			SYSCONF_SetProgressiveScan(tempvalue);
			break;

		case ASPECT_RATIO_OFFSET:
			tempvalue=SYSCONF_GetAspectRatio();
			tempvalue--;
			if(tempvalue<0) tempvalue=1;
			SYSCONF_SetAspectRatio(tempvalue);
			break;

		case SCREEN_SAVER_OFFSET:
			tempvalue=SYSCONF_GetScreenSaverMode();
			tempvalue--;
			if(tempvalue<0) tempvalue=1;
			SYSCONF_SetScreenSaverMode(tempvalue);
			break;

		case SOUND_OFFSET:
			tempvalue=SYSCONF_GetSoundMode();
			tempvalue--;
			if(tempvalue<0) tempvalue=2;
			SYSCONF_SetSoundMode(tempvalue);
			break;

		case IR_SENSITIVITY_OFFSET:
			tempvalue=SYSCONF_GetIRSensitivity();
			tempvalue--;
			if(tempvalue<0) tempvalue=4;
			SYSCONF_SetIRSensitivity(tempvalue);
			break;

		case SENSORBAR_POSITION_OFFSET:
			tempvalue=SYSCONF_GetSensorBarPosition();
			tempvalue--;
			if(tempvalue<0) tempvalue=1;
			SYSCONF_SetSensorBarPosition(tempvalue);
			break;

		case SPEAKER_VOLUME_OFFSET:
			tempvalue=raw_to_bar[SYSCONF_GetPadSpeakerVolume()];
			tempvalue--;
			if(tempvalue<0) tempvalue=10;
			SYSCONF_SetPadSpeakerVolume(bar_to_raw[tempvalue]);
			break;

		case MOTOR_MODE_OFFSET:
			tempvalue=SYSCONF_GetPadMotorMode();
			tempvalue--;
			if(tempvalue<0) tempvalue=1;
			SYSCONF_SetPadMotorMode(tempvalue);
			break;

	}
}

void configuration( void )
{
	dont_exit_configure=1;
	int cur_off=0,disp_off=0;
	int redraw=1;
	SYSCONF_Init();
	if (SYSCONF_GetLength("IPL.SADR") != SADR_LENGTH) { printf("IPL.SADR Length Incorrect. %d", SYSCONF_GetLength("IPL.SADR")); sleep(5); return;}
	int ret = SYSCONF_Get("IPL.SADR", sadr, SADR_LENGTH);
	if (ret < 0 )  { printf("SYSCONF_Get IPL.SADR %d", ret); sleep(5); return;}
	country = sadr[0];
	
	while(dont_exit_configure)
	{
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

 		if((WPAD_Pressed & WPAD_BUTTON_A)	|| (PAD_Pressed & PAD_BUTTON_A))
			activate_option(cur_off);

 		if((WPAD_Pressed & WPAD_BUTTON_B) 	|| (PAD_Pressed & PAD_BUTTON_B))
			cur_off=EXIT_OFFSET;

 		if((WPAD_Pressed & WPAD_BUTTON_DOWN)	|| (PAD_Pressed & PAD_BUTTON_DOWN))
			cur_off++;

 		if((WPAD_Pressed & WPAD_BUTTON_UP)	|| (PAD_Pressed & PAD_BUTTON_UP))
			cur_off--;

 		if((WPAD_Pressed & WPAD_BUTTON_LEFT)	|| (PAD_Pressed & PAD_BUTTON_LEFT))
			decrease_option(cur_off);

 		if((WPAD_Pressed & WPAD_BUTTON_RIGHT)	|| (PAD_Pressed & PAD_BUTTON_RIGHT))
			increase_option(cur_off);

		if((WPAD_ANY_BUTTON(WPAD_Pressed)) || PAD_ANY_BUTTON(PAD_Pressed))
			redraw=1;

		if(cur_off>MENU_OPTIONS)	cur_off=0;
		if(cur_off<0)			cur_off=MENU_OPTIONS;

		if( redraw )
		{
			u8 *nickname=(u8*)memalign(32, 128);
			SYSCONF_GetNickName(nickname); 
			printf("\x1b[%d;0H  %s%s\n\n", CONFIG_MENU_H,	CONSOLE_NICKNAME[language_setting], nickname);
			free(nickname);

//			printf("%s:\n",					REGION_INFO[language_setting]);
			printf("  %s< %s >\n",				LANGUAGE[language_setting],		lang[SYSCONF_GetLanguage()]);
			printf("  %s< %s >\n",				WIIMU_LANGUAGE[language_setting],	lang[language_setting]);
			printf("  %s< %s >\n",				REGION[language_setting], 		region[SYSCONF_GetRegion()]);
			printf("  %s[ %s ]\n",				AREA[language_setting],			area[SYSCONF_GetArea()]);
			printf("  %s< %u >\n\n",			SHOP_AREA[language_setting],		country);

//			printf("%s:\n",					VIDEO_INFO[language_setting]);
			printf("  %s< %s >\n",				VIDEO_MODE[language_setting],		video[SYSCONF_GetVideo()]);
			printf("  %s< %s >\n",				EURRGB[language_setting],		SYSCONF_GetEuRGB60()==0?		ON_MESSAGE[language_setting] : OFF_MESSAGE[language_setting] );
			printf("  %s< %s >\n",				PROGRESSIVE[language_setting],		SYSCONF_GetProgressiveScan()==0?	ON_MESSAGE[language_setting] : OFF_MESSAGE[language_setting] );
			printf("  %s< %s >\n",				ASPECT_RATIO[language_setting],		SYSCONF_GetAspectRatio()==0?		"4:3"			       : "16:9" );
			printf("  %s< %s >\n\n",			SCREEN_SAVER[language_setting],		SYSCONF_GetScreenSaverMode()==0?	ON_MESSAGE[language_setting] : OFF_MESSAGE[language_setting] );
			
//			printf("%s:\n",					AUDIO_INFO[language_setting]);
			printf("  %s< %s >\n\n",			SOUND[language_setting],		sound[SYSCONF_GetSoundMode()]);

//			printf("%s:\n",					WIIMOTE_INFO[language_setting]);
			printf("  %s< %d >\n",				IR_SENSITIVITY[language_setting],	SYSCONF_GetIRSensitivity());
			printf("  %s< %s >\n",				SENSORBAR_POSITION[language_setting],	sensorbar[SYSCONF_GetSensorBarPosition()]);
			printf("  %s< %s >\n",				SPEAKER_VOLUME[language_setting],	speaker_volumes[raw_to_bar[SYSCONF_GetPadSpeakerVolume()]]);
			printf("  %s< %s >\n\n",			MOTOR_MODE[language_setting],		SYSCONF_GetPadMotorMode()==0?		ON_MESSAGE[language_setting] : OFF_MESSAGE[language_setting] );;
			
			printf("%s\n",					PARENTAL_INFO[language_setting]);
			s8 *pw=(s8*)memalign(32, 128);
			s8 *answer=(s8*)memalign(32, 128);
			if(SYSCONF_GetParentalPassword(pw)>=0) 
				printf("  %s%s\n",				PARENTAL_PASSWORD[language_setting], 	pw);
			else
				printf("  %s<Not Set>\n",			PARENTAL_PASSWORD[language_setting]);
			if(SYSCONF_GetParentalAnswer(answer)>=0)
				printf("  %s%s\n\n",				PARENTAL_ANSWER[language_setting], 	answer);
			else
				printf("  %s<Not Set>\n\n",			PARENTAL_ANSWER[language_setting]);
			free(pw);
			free(answer);
			printf("  %s\n",				SAVE[language_setting]);
			printf("  %s\n",				EXIT[language_setting]);
			
			disp_off=cur_off;
			if(cur_off>=REGION_INFO_OFFSET)		disp_off+=1;
			if(cur_off>=VIDEO_INFO_OFFSET)		disp_off+=1;
			if(cur_off>=AUDIO_INFO_OFFSET)		disp_off+=1;
			if(cur_off>=WIIMOTE_INFO_OFFSET)	disp_off+=1;
			if(cur_off>=PARENTAL_INFO_OFFSET)	disp_off+=2;
			if(cur_off>=QUIT_OFFSET)		disp_off+=1;
			printf("\x1b[%d;0H*", 			CONFIG_MENU_H+disp_off);
			redraw=0;
		}
		VIDEO_WaitVSync();
	}
	VIDEO_WaitVSync();
	return;
}

s32 PullMiiFromWiimote(int slot, int wiimote, u8* MiiData)
{
	if(slot>=WIIMOTE_MII_SLOT_NUM)
		return ERR_INVARG;
	if(sizeof MiiData < WIIMOTE_MII_DATA_BYTES_PER_SLOT)
		return ERR_ARRAYSIZE;
	short miiOffset = WIIMOTE_MII_DATA_BEGIN_1 + (slot * WIIMOTE_MII_DATA_BYTES_PER_SLOT);
	wiiuse_read_data(__wpads[wiimote], MiiData, miiOffset, WIIMOTE_MII_DATA_BYTES_PER_SLOT, NULL);
	return ERR_NONE;
}

s32 extractChannelContents(u64 titleID, char* location)
{
	u32 TitleIDH=TITLE_UPPER(titleID);
	u32 TitleIDL=TITLE_LOWER(titleID);
	if(ES_SetUID(titleID)<0)
		return ERR_UID;
	u32 tmd_size ATTRIBUTE_ALIGN(32);


	if(ES_GetStoredTMDSize(titleID, &tmd_size)<0)
		return ERR_TMDSIZE;

	signed_blob *TMD = (signed_blob *)memalign( 32, tmd_size );
	memset(TMD, 0, tmd_size);

	if(ES_GetStoredTMD(titleID, TMD, tmd_size)<0)
	{
		free(TMD);
		return ERR_TMD;
	}

	u32 cnt ATTRIBUTE_ALIGN(32);

	if(ES_GetNumTicketViews(titleID, &cnt)<0)
	{
		free(TMD);
		return ERR_TIKCOUNT;
	}
	if( cnt <= 0 )
	{
		free(TMD);
		return ERR_TIKCOUNT;
	}

	tikview *views = (tikview *)memalign( 32, sizeof(tikview)*cnt );
	if(ES_GetTicketViews(titleID, views, cnt)<0)
	{
		free(views);
		free(TMD);
		return ERR_TIK;
	}
	printf("Allocated and filled views.\n");
	sleep(3);

	Identify_SU();
	int z;
	tmd_content *TMDc = TMD_CONTENTS(((tmd*)(SIGNATURE_PAYLOAD(TMD))));		// OH GOD CREDIAR, WTF WAS THAT MESS!
	// List format is "XXXXXXXX = YYYYYYYY" where X is index, and Y is cid.
	char *lookup_list=calloc(21,((tmd*)(SIGNATURE_PAYLOAD(TMD)))->num_contents);
	ClearScreen();
	printf("\nNumber of contents: %d\n",((tmd*)(SIGNATURE_PAYLOAD(TMD)))->num_contents);
	sleep(1);
	for(z=0; z < ((tmd*)(SIGNATURE_PAYLOAD(TMD)))->num_contents; z++)
	{
		/* Get Content */
		char nameDirectory[80];
		sprintf(nameDirectory,"/title/%08x/%08x/content/%08x.app",TitleIDH,TitleIDL,TMDc[z].cid);
		s32 contentFd=ISFS_Open(nameDirectory,ISFS_OPEN_READ);
		u8 *data=calloc(TMDc[z].size,1);
		if(contentFd<0)
		{
			switch(contentFd)
			{
				case ISFS_EINVAL:
					printf("FAILED! (Invalid Argument %s)\n\tQuitting...\n",nameDirectory);
					sleep(5);
					Finish(1);
					break;
			
				case ISFS_ENOMEM:
					printf("FAILED! (Out of memory %s)\n\tQuitting...\n",nameDirectory);
					sleep(5);
					Finish(1);
					break;
			
				default:
					goto skip;
//					Finish(1);
					break;
			}
		}
		int isUnaligned = ((int)data)%32 | TMDc[z].size%32;
		int alignedLen = (TMDc[z].size+31)&0xffffffe0;
		unsigned char* alignedBuf;
		if(isUnaligned) alignedBuf = memalign(32, alignedLen);
		else alignedBuf = data;
		ISFS_Seek(contentFd,0,0);
		ISFS_Read(contentFd, alignedBuf, alignedLen);
		// If it was unaligned, you have to copy it and clean up
		if(isUnaligned){
			memcpy(data, alignedBuf, TMDc[z].size);
			free(alignedBuf);
		}
		ISFS_Close(contentFd);

		// Do copying here.
		// data is the actual content data (use fwrite with it :P).
		// Copy the file with it's index as it's filename
		char* destination=calloc(sizeof(location)+14, 1);
		char lookup_entry[21];
		sprintf(destination, "%s/%08x.app",location,TMDc[z].index);
		sprintf(lookup_entry, "%08x = %08x\n",TMDc[z].index,TMDc[z].cid);
		strcat(lookup_list, lookup_entry);
		printf("Got destination as: %s\n", destination);
		sleep(3);
		FILE *dfd=fopen(destination,"wb+");
		printf("Opened %s\n", destination);
		sleep(3);
//		free(destination);
		fwrite(data, TMDc[z].size, 1, dfd);
		printf("Wrote data to %s\n", destination);
		sleep(3);
		fclose(dfd);
		printf("Closed %s\n", destination);
		sleep(2);
skip:
		_nop();
	}
	// Make a file containing the lookups called files.txt
	char* lookup_file=calloc(sizeof(location)+14, 1);
	sprintf(lookup_file, "%s/files.txt",location);
	printf("Got destination as: %s\n", lookup_file);
	sleep(3);
	FILE* lfd=fopen(lookup_file,"wb+");
	printf("Opened %s\n", lookup_file);
	sleep(3);
//	free(lookup_file);
	fwrite(lookup_list, 21, ((tmd*)SIGNATURE_PAYLOAD(TMD))->num_contents, lfd);
	printf("Wrote lookups to %s\n", lookup_file);
	sleep(3);
	fclose(lfd);
	printf("Closed %s\n", lookup_file);
	sleep(2);
	printf("Freed TMD and views");
	sleep(1);
	SystemMenuAuth();
	return ERR_NONE;
}

void changeChannelName(u64 titleID)
{
	printf("Identifying as 00000001-00000000 (SU)!... ");
	CheckESRetval(ES_Identify(SU_IDENTIFY));
//	if(ES_SetUID(titleID)<0)
//		return;
	u32 tmd_size ATTRIBUTE_ALIGN(32);


	if(CheckESRetval(ES_GetStoredTMDSize(titleID, &tmd_size))!=0)
		return;

	signed_blob *TMD = (signed_blob *)memalign( 32, tmd_size );
	memset(TMD, 0, tmd_size);

	if(CheckESRetval(ES_GetStoredTMD(titleID, TMD, tmd_size))!=0)
	{
		free(TMD);
		return;
	}

	u32 cnt ATTRIBUTE_ALIGN(32);

	if(CheckESRetval(ES_GetNumTicketViews(titleID, &cnt))!=0)
	{
		free(TMD);
		return;
	}

	if( cnt <= 0 )
	{
		free(TMD);
		return;
	}
	tikview *views = (tikview *)memalign( 32, sizeof(tikview)*cnt );

	if(CheckESRetval(ES_GetTicketViews(titleID, views, cnt))!=0)
	{
		free(views);
		free(TMD);
		return;
	}
	char *name=calloc((0x54/2),1);	int z;
	for(z=0; z < 1; ++z)
	{
		tmd_content *TMDc = TMD_CONTENTS(((tmd*)(SIGNATURE_PAYLOAD(TMD))));		// OH GOD CREDIAR, WTF WAS THAT MESS!!!
		//printf("%d,",TMDc->index);
		s32 cfd = ES_OpenTitleContent( titleID, TMDc->index);
		free(views);
		if(CheckESRetval(cfd)!=0)
		{	
			;
			//printf("ES_OpenContent(%d) failed\n", cfd);
			//sleep(10);
			//exit(0);
		} else {
			u64 sz ATTRIBUTE_ALIGN(32) = 0x140;
			u8 *data = (u8*)memalign(32, sz);
			if( TMDc->size < sz )
				sz = TMDc->size;

			if( data != NULL )
			{
				if(ES_ReadContent(cfd, data, sz)<0)
				{
					free(data);
					return;
				}

				int y;
				int chan_name_offset=(language_setting*0x54);				// Set to WiiMU's language
				for(y=0;y<(0x54/2);y++)
					name[y]=data[(0x9C+chan_name_offset)+(y*2)+1];

			}
			if(CheckESRetval(ES_CloseContent(cfd))!=0)
			{
				;
				//printf("ES_CloseContent failed\n");
				//sleep(10);
				//exit(0);
			}
			free(data);
		}
	}
	int wiistring_size;
reenter:
	wiistring_size=type_string_wiimote(name, 0x54/2);
	if(wiistring_size<=0)
	{
		printf("\n\nPlease enter a name!\n");
		sleep(3);
		goto reenter;
	}
	name[wiistring_size+1]=0;
	/* Get Content */
	char nameDirectory[80];
	tmd_content *TMDc = TMD_CONTENTS(((tmd*)(SIGNATURE_PAYLOAD(TMD))));
	sprintf(nameDirectory,"/title/%08x/%08x/content/%08x.app",TITLE_UPPER(titleID),TITLE_LOWER(titleID),TMDc->cid);
	s32 contentFd=ISFS_Open(nameDirectory,ISFS_OPEN_RW);
	CheckISFSRetval(contentFd);
	ClearScreen();
	printf("\n\nOpened content!\n");
	u8 *data=calloc(TMDc->size,1);
	int isUnaligned = ((int)data)%32 | TMDc->size%32;
	int alignedLen = (TMDc->size+31)&0xffffffe0;
	u8* alignedBuf;
	if(isUnaligned) alignedBuf = memalign(32, alignedLen);
	else alignedBuf = data;
	CheckISFSRetval(ISFS_Seek(contentFd,0,0));
	CheckISFSRetval(ISFS_Read(contentFd, alignedBuf, alignedLen));
	printf("Read content!\n");
	int y;
	int chan_name_offset=(SYSCONF_GetArea()*0x54);		// Edits the one for the Wii's system Menu
	char* nameOut=calloc(96,1);
	for(y=0;y<(0x54/2);y++)
		nameOut[(y*2)+1]=name[y];
	printf("Wrote new name! %s\n", name);
	CheckISFSRetval(ISFS_Seek(contentFd,(0x9C+chan_name_offset),0));
	printf("Seeked to location!\n");
	if(nameOut==NULL)
	{
		printf("FAILED! (Name Out is NULL!)\n");
		sleep(5);
		Finish(1);
	}
	if(((u32)nameOut%32)!=0)
	{
		isUnaligned = ((int)nameOut)%32 | 0x54%32;
		alignedLen = (0x54+31)&0xffffffe0;
		if(isUnaligned){ alignedBuf = memalign(32, alignedLen); memcpy(alignedBuf, nameOut, 0x54); }
		else alignedBuf = (u8*)nameOut;
	}
	CheckISFSRetval(ISFS_Write(contentFd, alignedBuf, 0x54));
	printf("Wrote content name!\nReading new Header Chunk!\n");
	CheckISFSRetval(ISFS_Seek(contentFd,0,0));
	CheckISFSRetval(ISFS_Read(contentFd, alignedBuf, alignedLen));
	printf("Read content!\n");
	u8* header_chunk=calloc(0x640, 1);
	int i;
	for(i=0;i<0x630;i++)
		header_chunk[i]=alignedBuf[i];
	for(i=0x630;i<0x640;i++)
		header_chunk[i]=0;
	u8* hash=calloc(0x10,1);
	md5(header_chunk, 0x640, hash);
	CheckISFSRetval(ISFS_Seek(contentFd,0x630,0));
	printf("Seeked to location!\n");
	if(hash==NULL)
	{
		printf("FAILED! (Hash is NULL!)\n");
		sleep(5);
		Finish(1);
	}
	if(((u32)hash%32)!=0)
	{
		isUnaligned = ((int)hash)%32 | 0x10%32;
		alignedLen = (0x10+31)&0xffffffe0;
		if(isUnaligned){ alignedBuf = memalign(32, alignedLen); memcpy(alignedBuf, hash, 0x10); }
		else alignedBuf = hash;
	}
	CheckISFSRetval(ISFS_Write(contentFd, alignedBuf, 0x10));
	printf("Wrote Header Chunk!\n");

	CheckISFSRetval(ISFS_Close(contentFd));
	printf("Closed content!\n");
	free( TMD );
}

