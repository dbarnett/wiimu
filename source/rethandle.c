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
#include <ogc/card.h>
#include <ogc/conf.h>
#include <ogc/dvd.h>
#include <ogc/ios.h>
#include <ogc/message.h>
#include <ogc/pad.h>
#include <ogc/stm.h>
#include <ogc/usbstorage.h>
#include <ogc/wiilaunch.h>
#include <wiiuse/wpad.h>
#include <wiiuse/wiiuse.h>

#include "cleanup.h"
#include "screen.h"
#include "rethandle.h"
#include "pretty.h"

void RetvalFail(int badness)
{
	WIILIGHT_SetLevel((255/4)*4);
	sleep(1);
	WIILIGHT_SetLevel((255/4)*3);
	sleep(1);
	WIILIGHT_SetLevel((255/4)*2);
	sleep(1);
	WIILIGHT_SetLevel((255/4)*1);
	sleep(1);
	WIILIGHT_SetLevel(0);
	char *tab[] = {"Attempt to Continue", "Quit to loader", "Reboot", "Shutdown"};
	char cap[100]; sprintf(cap, "\nAn error occured, what would you like to do:");
	s32 res = showmenu(cap, tab, 4, badness, " ->");
	switch(res)
	{
		case 1:
			Finish(1);
			break;
		case 3:
			SYS_ResetSystem(SYS_SHUTDOWN,0,0);
			break;
		case 2:
			SYS_ResetSystem(SYS_RESTART,0,0);
			break;
		default:
			break;
	}
}

int CheckESRetval(int retval)
{
	switch(retval)
	{
		case ES_EINVAL:
			printf("FAILED! (ES: Invalid Argument)\n");
			RetvalFail(1);
			break;

		case ES_ENOMEM:
			printf("FAILED! (ES: Out of memory)\n");
			RetvalFail(2);
			break;

		case ES_ENOTINIT:
			printf("FAILED! (ES: Not Initialized)\n");
			RetvalFail(1);
			break;

		case ES_EALIGN:
			printf("FAILED! (ES: Not Aligned)\n");
			RetvalFail(1);
			break;
		default:
			if(retval<0)
			{
				printf("FAILED! (ES: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckISFSRetval(int retval)
{
	switch(retval)
	{
		case ISFS_EINVAL:
			printf("FAILED! (ISFS: Invalid Argument)\n");
			RetvalFail(1);
			break;

		case ISFS_ENOMEM:
			printf("FAILED! (ISFS: Out of memory)\n");
			RetvalFail(2);
			break;
		default:
			if(retval<0)
			{
				CheckIPCRetval(retval);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckIPCRetval(int retval)
{
	switch(retval)
	{
		case IPC_EINVAL:
			printf("FAILED! (IPC: Invalid Argument)\n");
			RetvalFail(1);
			break;

		case IPC_ENOMEM:
			printf("FAILED! (IPC: Out of memory)\n");
			RetvalFail(2);
			break;

		case IPC_ENOHEAP:
			printf("FAILED! (IPC: Out of heap (?))\n");
			RetvalFail(2);
			break;

		case IPC_ENOENT:
			printf("FAILED! (IPC: No entity (?))\n");
			RetvalFail(1);
			break;

		case IPC_EQUEUEFULL:
			printf("FAILED! (IPC: Queue Full)\n");
			RetvalFail(0);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (IPC: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckCARDRetval(int retval)
{
	switch(retval)
	{
		case CARD_ERROR_BUSY:
			printf("FAILED! (CARD: Card is busy)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_WRONGDEVICE:
			printf("FAILED! (CARD: Wrong device connected to card slot)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_NOCARD:
			printf("FAILED! (CARD: No card connected)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_NOFILE:
			printf("FAILED! (CARD: File does not exist)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_IOERROR:
			printf("FAILED! (CARD: Internal EXI I/O Error!)\n");
			RetvalFail(1);
			break;

		case CARD_ERROR_BROKEN:
			printf("FAILED! (CARD: File/Dir Entry is broken)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_EXIST:
			printf("FAILED! (CARD: File already exists)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_NOENT:
			printf("FAILED! (CARD: No empty blocks to create file)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_INSSPACE:
			printf("FAILED! (CARD: Not enough space to write file)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_NOPERM:
			printf("FAILED! (CARD: Not enough permissions)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_LIMIT:
			printf("FAILED! (CARD: Card size limit reached)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_NAMETOOLONG:
			printf("FAILED! (CARD: Filename is too long)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_ENCODING:
			printf("FAILED! (CARD: Wrong region memory card)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_CANCELED:
			printf("FAILED! (CARD: Card operation canceled)\n");
			RetvalFail(0);
			break;

		case CARD_ERROR_FATAL_ERROR:
			printf("FAILED! (CARD: Unrecoverable error!)...\n");
			RetvalFail(1);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (CARD: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckCONFRetval(int retval)
{
	switch(retval)
	{
		case CONF_ENOMEM:
			printf("FAILED! (CONF: Out of memory)\n");
			RetvalFail(2);
			break;

		case CONF_EBADFILE:
			printf("FAILED! (CONF: File (?) is bad)\n");
			RetvalFail(3);
			break;

		case CONF_ENOENT:
			printf("FAILED! (CONF: No Entity (?))\n");
			RetvalFail(2);
			break;

		case CONF_ETOOBIG:
			printf("FAILED! (CONF: Too big)\n");
			RetvalFail(1);
			break;

		case CONF_ENOTINIT:
			printf("FAILED! (CONF: Not initialized)\n");
			RetvalFail(1);
			break;

		case CONF_ENOTIMPL:
			printf("FAILED! (CONF: Not implied)\n");
			RetvalFail(0);
			break;

		case CONF_EBADVALUE:
			printf("FAILED! (CONF: Bad value)\n");
			RetvalFail(0);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (CONF: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckDVDRetval(int retval)
{
	switch(retval)
	{
		case DVD_ERROR_FATAL:
			printf("FAILED! (DVD: Fatal Error)\n");
			RetvalFail(2);
			break;

		case DVD_ERROR_IGNORED:
			printf("FAILED! (DVD: Ignored)\n");
			RetvalFail(0);
			break;

		case DVD_ERROR_CANCELED:
			printf("FAILED! (DVD: Canceled)\n");
			RetvalFail(0);
			break;

		case DVD_ERROR_COVER_CLOSED:
			printf("FAILED! (DVD: Cover closed)\n");
			RetvalFail(0);
			break;
		default:
			if(retval<0)
			{
				printf("FAILED! (DVD: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckIOSRetval(int retval)
{
	switch(retval)
	{
		case IOS_EINVAL:
			printf("FAILED! (IOS: Invalid Argument)\n");
			RetvalFail(1);
			break;

		case IOS_EBADVERSION:
			printf("FAILED! (IOS: Bad version)\n");
			RetvalFail(0);
			break;

		case IOS_ETOOMANYVIEWS:
			printf("FAILED! (IOS: Too many ticket views)\n");
			RetvalFail(1);
			break;

		case IOS_EMISMATCH:
			printf("FAILED! (IOS: Mismatch)\n");
			RetvalFail(0);
			break;
		default:
			if(retval<0)
			{
				printf("FAILED! (IOS: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckTSS2Retval(int retval)
{
	switch(retval)
	{
		case MQ_ERROR_TOOMANY:
			printf("FAILED! (Thread Subsystem 2: Too many threads (?))\n");
			RetvalFail(2);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (Thread Subsystem 2: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckPADRetval(int retval)
{
	switch(retval)
	{
		case PAD_ERR_NO_CONTROLLER:
			printf("FAILED! (PAD: No controller)\n");
			RetvalFail(0);
			break;

		case PAD_ERR_NOT_READY:
			printf("FAILED! (PAD: Not ready)\n");
			RetvalFail(0);
			break;

		case PAD_ERR_TRANSFER:
			printf("FAILED! (PAD: Transfer)\n");
			RetvalFail(0);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (PAD: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckSTMRetval(int retval)
{
	switch(retval)
	{
		case STM_EINVAL:
			printf("FAILED! (STM: Invalid argument)\n");
			RetvalFail(0);
			break;

		case STM_ENOTINIT:
			printf("FAILED! (STM: Not initialized)\n");
			RetvalFail(1);
			break;

		case STM_ENOHANDLER:
			printf("FAILED! (STM: No handler)\n");
			RetvalFail(1);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (STM: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckUSBSTORAGERetval(int retval)
{
	switch(retval)
	{
		case USBSTORAGE_ENOINTERFACE:
			printf("FAILED! (USBSTORAGE: No interface)\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ESENSE:
			printf("FAILED! (USBSTORAGE: Sense error (?))\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ESHORTWRITE:
			printf("FAILED! (USBSTORAGE: Short write)\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ESHORTREAD:
			printf("FAILED! (USBSTORAGE: Short read)\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ESIGNATURE:
			printf("FAILED! (USBSTORAGE: Bad signature (?))\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ETAG:
			printf("FAILED! (USBSTORAGE: Bad tag (?))\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ESTATUS:
			printf("FAILED! (USBSTORAGE: Bad status (?))\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_EDATARESIDUE:
			printf("FAILED! (USBSTORAGE: Data residue (?))\n");
			RetvalFail(1);
			break;

		case USBSTORAGE_ETIMEDOUT:
			printf("FAILED! (USBSTORAGE: Timed out)\n");
			RetvalFail(0);
			break;

		case USBSTORAGE_EINIT:
			printf("FAILED! (USBSTORAGE: Not initialized (?))\n");
			RetvalFail(1);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (USBSTORAGE: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}

int CheckWIIRetval(int retval)
{
	switch(retval)
	{
		case WII_ENOTINIT:
			printf("FAILED! (WII: Not initialized)\n");
			RetvalFail(1);
			break;

		case WII_EINTERNAL:
			printf("FAILED! (WII: Internal error)\n");
			RetvalFail(2);
			break;

		case WII_ECHECKSUM:
			printf("FAILED! (WII: Checksum error)\n");
			RetvalFail(1);
			break;

		case WII_EINSTALL:
			printf("FAILED! (WII: Title not installed)\n");
			RetvalFail(1);
			break;

		case WII_E2BIG:
			printf("FAILED! (WII: Argument list too big)\n");
			RetvalFail(1);
			break;

		default:
			if(retval<0)
			{
				printf("FAILED! (WII: Unknown error %d)\n", retval);
				RetvalFail(1);
			}else
				return 0;
			break;
	}
	return 1;
}


