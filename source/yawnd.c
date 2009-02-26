/*
    YaWnD : Yet Another Wii NAND Dumper 
	Dumps the contents of the Wii NAND flash to front SD or USB device
	for backup purposes.
    Copyright (C) 2008 Redbird

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Author may be contacted at redbird.fs@gmail.com.
*/

/*
	This release of YaWnD was modified to be used by other applications.
	Please do not mistake this for the original code.
*/

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <malloc.h>
#include <fat.h>
#include <string.h>

#include "yawnd.h"

// Flash Device Handle
#define WII_FLASH_DEVICE  	"/dev/flash"
// 512MB Flash Size
#define WII_FLASH_SIZE 	  	0x20000000
// 2K per block
#define FLASH_BLOCKSIZE  	2048
// 2K + ECC data
#define FLASH_BLOCKECCSIZE 	FLASH_BLOCKSIZE+64
// 4MB buffer to be safe
#define WII_MAX_BUFFER	  	4096
// Number of lines 2**n
#define WII_SCREEN_LINES  	16
// 4pass
#define WII_SCREEN_STEPS  	WII_FLASH_SIZE/(WII_MAX_BUFFER*FLASH_BLOCKSIZE)/16
// User info every xx Blocks
#define INFO_BLOCK_PACK	  	256
//
static const u32 CACHE_PAGES = 8192;

typedef enum {dev_sd, dev_usb} tDevice;
typedef enum {dmode_normal, dmode_ecc} tDumpMode;


// Prototypes
void CleanScreen();
void pressAnyButton();
void ShowOptions(tDevice dev, tDumpMode mode);
void DumpFlash(tDevice dev, tDumpMode mode);
void ReadFlash(int blocks_read, tDumpMode mode);
void AppendToFile(tDevice dev, tDumpMode mode);
char *getDumpModeName(tDumpMode mode);
int getDumpBlockSize(tDumpMode mode);
char *getDeviceFile(tDevice dev, tDumpMode mode);
char *getDeviceName(tDevice dev);
char getDeviceNum(tDevice dev);
void initialise_fat(tDevice dev);

u8 *buffer;

void YAWND_Backup()
{
	DumpFlash(dev_sd, dmode_ecc);
}

void YAWND_Backup_Zestig()
{
	DumpFlash(dev_sd, dmode_normal);
}

void DumpFlash(tDevice dev, tDumpMode mode)
{
	int totalBlocks= WII_FLASH_SIZE/FLASH_BLOCKSIZE;	// Number of pages in NAND
	int totalSteps= totalBlocks/WII_MAX_BUFFER;
	FILE *pFile;
	int blocks_read=0;
	int steps= 0;
	
	// To test
	//totalSteps= 2;
	
	buffer = (u8*) memalign(0x40, WII_MAX_BUFFER*getDumpBlockSize(mode));
	if(buffer==NULL) {
		printf("ERROR: Unable to allocate enough memory for buffer. Unable to continue. Exiting...\n");
		pressAnyButton();
		exit(0);
	}

	initialise_fat(dev);
	pFile = fopen(getDeviceFile(dev, mode), "wb+");
	if (pFile==NULL) {
		printf("ERROR: fopen(\"%s\") failed. No FAT filesystem on the %s?\n", getDeviceFile(dev, mode), getDeviceName(dev));
		pressAnyButton();
		exit(0);
	} else {
		fclose(pFile);
		
		CleanScreen();
		for(steps=0; steps<totalSteps; steps++) {
			// Block set to read in this step starts on...
			blocks_read= steps*WII_MAX_BUFFER;
			if((steps % WII_SCREEN_LINES)==0) {CleanScreen(); printf("\nDumping NAND. Pass %d of %d.", (steps/WII_SCREEN_LINES)+1, WII_SCREEN_STEPS);}
			ReadFlash(blocks_read, mode);
			AppendToFile(dev, mode);
		}
	}
	printf("\n\nEnd of Flash Dump.");
	
	pressAnyButton();
	if(buffer!=NULL) free(buffer);	
}



void ReadFlash(int blocks_read, tDumpMode mode)
{
	s32 res;	
	s32 flash_fd = -1;
	u8 *block_buffer;
	int block;
	int flash_block_size= getDumpBlockSize(mode);
	
	printf("\nReading 0x%05X : ", blocks_read);
	
	
	flash_fd = IOS_Open(WII_FLASH_DEVICE, 1 /* 2 = write, 1 = read */);

	for(block=0 ; block<WII_MAX_BUFFER; block++) {
		// Do not panic: gives something to user
		if((block%INFO_BLOCK_PACK)==0) printf(".");
		
		block_buffer= buffer + block*flash_block_size;
		
		// must seek before every block read -- offset is block number, not byte number
		IOS_Seek(flash_fd, blocks_read+block, SEEK_SET);
		res = IOS_Read(flash_fd, block_buffer, flash_block_size);
		
		// -12 is "bad block" and should be ignored (fill with zeroes)
		if(res!=flash_block_size) {
			if(res==-12) memset(block_buffer, 0, flash_block_size);
			else {
				printf("\nERROR reading flash at %d (error:%d)\n", blocks_read+block, res);
				pressAnyButton();
				exit(0);
			}
		}
	}
	
	IOS_Close(flash_fd);
}



void AppendToFile(tDevice dev, tDumpMode mode)
{
	FILE *pFile;
	u8 *block_buffer;
	int bytes= INFO_BLOCK_PACK*getDumpBlockSize(mode);
	int num_blocks= WII_MAX_BUFFER/INFO_BLOCK_PACK;
	int block;
	
	pFile = fopen(getDeviceFile(dev, mode), "a");
	if (pFile==NULL) {
		printf("\nERROR: fopen(\"%s\") failed. No FAT filesystem on the %s ?\n", getDeviceFile(dev, mode), getDeviceName(dev));
		pressAnyButton();
		exit(0);
	} else {
		printf(" -> To %s: ", getDeviceName(dev));
		
		for(block=0 ; block<num_blocks; block++) {
			block_buffer= buffer + block*bytes;
			
			if(fwrite(block_buffer, 1, bytes, pFile)!=bytes) {
				printf("\nError writting on %s. Is it Full?\n", getDeviceName(dev));
				pressAnyButton();
				exit(0);
			} else printf("* ");
		}
		fclose(pFile);
	}
}



// Wait for any button
void pressAnyButton()
{
	u32 pressed = false;
	
	printf("\nPress any button to continue...\n");
	do {
		WPAD_ScanPads();
		pressed = WPAD_ButtonsDown(0);
	} while (!pressed);
}

char *getDumpModeName(tDumpMode mode)
{
	char *names[]= {"normal", "n.+ECC"};
	
	return names[mode];
}



int getDumpBlockSize(tDumpMode mode)
{
	int sz[]= {FLASH_BLOCKSIZE, FLASH_BLOCKECCSIZE};
	
	return sz[mode];
}



char *getDeviceFile(tDevice dev, tDumpMode mode)
{
	char *names[]= {WII_FLASH_SD_FILE, WII_FLASH_USB_FILE};
	static char fn[256];
	
	snprintf(fn, sizeof(fn), "%s_%s.img", names[dev], getDumpModeName(mode));
	
	return fn;
}



char *getDeviceName(tDevice dev)
{
	char *names[]= {"SD ", "USB"};
	
	return names[dev];
}



char getDeviceNum(tDevice dev)
{
	char part[]= {"sd"};//, PI_USBSTORAGE};	// Accoding to PARTITION_INTERFACE
	
	return part[dev];
}



void CleanScreen()
{
	int i;
	int row= 2;
	
	for(i=0; i<20; i++) {
		printf("\x1b[%d;0H", row+i);
		printf("                                                                                                  \n");	
	}
	printf("\x1b[%d;0H", row);

}



void initialise_fat(tDevice dev) 
{
    if (!fatInit(CACHE_PAGES, false)) { 
        printf("Unable to initialise FAT subsystem. Are there any connected devices?\n"); 
		pressAnyButton();
		exit(0);
    } else {
    if (1) //!fatEnableReadAhead(getDeviceNum(dev), 64, 128))
        printf("Could not enable FAT read-ahead caching on %s, speed will suffer...\n", getDeviceName(dev));

    }
}
