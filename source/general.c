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
#include <wiiuse/wpad.h>
#include <fat.h>
#include <gcmodplay.h>
#include <unistd.h>

#include "screen.h"
#include "cleanup.h"
#include "rethandle.h"
#include "auth.h"
#include "channels.h"
#include "nand.h"

#include "sysmenu.h"
#include "su.h"
#include "yawnd.h"
#include "pretty.h"
#include "IOS_DVD.h"
#include "factory_mod.h"
#include "editing.h"
#include "updater.h"
#include "general.h"

u32 tempKeyID;

void wait_press_A()
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

	while( !(WPAD_Pressed & WPAD_BUTTON_A) && !(PAD_Pressed & PAD_BUTTON_A) )
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

//Borrowed from tona's AnyRegion_Changer.
//Original name was ISFS_ReadFileToArray.
s32 readfile(char *filepath, u8 *filearray, u32 max_size, u32 *file_size)
{
	s32 ret, fd;
	static fstats filestats ATTRIBUTE_ALIGN(32);
	
	*file_size = 0;
	ret = ISFS_Open(filepath, ISFS_OPEN_READ);
	CheckISFSRetval(ret);
	fd = ret;
	
	CheckISFSRetval(ISFS_GetFileStats(fd, &filestats));
	
	*file_size = filestats.file_length;
	
	if (*file_size > max_size)
	{
		printf("File is too large! Size: %u Max: %u\n", *file_size, max_size);
		return -1;
	}
	
	ret = ISFS_Read(fd, filearray, *file_size);
	CheckISFSRetval(ret);
	*file_size = ret;
	if (ret != filestats.file_length)
	{
		printf("Error! ISFS_Read Only read: %d\n", ret);
		return -1;
	}
	
	ret = ISFS_Close(fd);
	CheckISFSRetval(ret);
	
	return 0;
}

/*void copyfile(char *path, char *dest)
{
	FILE *fp;
	s32 fd, i = 0, l = 0;
	u32 bytes = 0;
	char *buf;

	printf("\nCopying \"%s\"... ", path);

	//Open for read.
	fd = ISFS_Open(path, ISFS_OPEN_READ);
	CheckISFSRetval(fd);

	//Create on sd.
	fp = fopen(dest, "wb");
	if(fp == NULL)
	{
		printf("Error creating file!\n");
		CheckISFSRetval(ISFS_Close(fd));
		return;
	}

	//Allocate buffer.
	buf = (char *)memalign(32, DUMP_BUF_SIZE + 1);

	printf(" ");

	//Copy to sd.
	while((i = ISFS_Read(fd, buf, DUMP_BUF_SIZE)) > 0)
	{
		fwrite(buf, 1, i, fp);
		bytes += i;
		l++;
		if(l % 50)
			spinner();
	}

	//Read error?
	CheckISFSRetval(i);
	printf("\bDone! (%u bytes)\n", bytes);

	free(buf);
	fclose(fp);
	CheckISFSRetval(ISFS_Close(fd));
}
*/

// These are my versions that I wrote from scratch.
void copy_from_nand(s32 file, char* dstpath)
{
	FILE *fp;
	fstats *filestats = memalign(32, sizeof(filestats) * 20);
	dbgprintf("Getting File Stats\n");
	CheckISFSRetval(ISFS_GetFileStats(file, filestats));
	//Open on sd.
	fp = fopen(dstpath, "wb+");
	if(fp == NULL)
	{
		printf("Error opening file!\n");
		free(filestats);
		return;
	}

	//Allocate buffer.
	u8 *buf = memalign(32, ALIGN_LENGTH(filestats->file_length));  //filestats->file_length);

	dbgprintf("Reading File\n");
	if(filestats->file_length<=0) dbgprintf("Not long enough\n");
	if(buf==NULL) dbgprintf("No buffer\n");
	if(((u32)buf%32)!=0) dbgprintf("Not aligned\n");
	CheckISFSRetval(ISFS_Read(file, buf, filestats->file_length));

	dbgprintf("Writing File\n");
	fwrite(buf, filestats->file_length, 1, fp);

	char string[50];
	sprintf(string, "Done! (%u bytes)\n", filestats->file_length);
	dbgprintf(string);

	free(buf);
	free(filestats);
	fclose(fp);
}

void copyfile(char* srcpath, char* dstpath)
{
	char string[50];
	sprintf(string, "\nCopying \"%s\"... ", srcpath);
	printf(string);
	printf("Opening File\n");
	sleep(5);
	s32 file = ISFS_Open(srcpath, ISFS_OPEN_READ);
	CheckISFSRetval(file);
	copy_from_nand(file, dstpath);
	CheckISFSRetval(ISFS_Close(file));
}

/*void copy_to_nand(s32 file, char* srcpath)
{
	FILE *fp;
	fstats *filestats = memalign(32, sizeof(filestats) * 20);
	dbgprintf("Getting File Stats\n");
	CheckISFSRetval(ISFS_GetFileStats(file, filestats));
	//Open on sd.
	fp = fopen(dstpath, "rb");
	if(fp == NULL)
	{
		printf("Error opening file!\n");
		free(filestats);
		return;
	}

	//Allocate buffer.
	u8 *buf = memalign(32, ALIGN_LENGTH(filestats->file_length));  //filestats->file_length);

	dbgprintf("Reading File\n");
	if(filestats->file_length<=0) dbgprintf("Not long enough\n");
	if(buf==NULL) dbgprintf("No buffer\n");
	if(((u32)buf%32)!=0) dbgprintf("Not aligned\n");
	CheckISFSRetval(ISFS_Read(file, buf, filestats->file_length));

	dbgprintf("Writing File\n");
	fwrite(buf, filestats->file_length, 1, fp);

	char string[50];
	sprintf(string, "Done! (%u bytes)\n", filestats->file_length);
	dbgprintf(string);

	free(buf);
	free(filestats);
	fclose(fp);
}

void copyfile_to_nand(char* srcpath, char* dstpath)
{
	char string[50];
	sprintf(string, "\nCopying \"%s\"... ", srcpath);
	printf(string);
	printf("Opening File\n");
	sleep(5);
	s32 file = ISFS_Open(srcpath, ISFS_OPEN_READ);
	CheckISFSRetval(file);
	copy_from_nand(file, dstpath);
	CheckISFSRetval(ISFS_Close(file));
}
*/

