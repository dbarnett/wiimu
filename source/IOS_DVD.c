#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include "IOS_DVD.h"

#define ALIGNED(n) __attribute__((aligned(n)))
#define PHYSADDR(x) ((unsigned long *)(0x7FFFFFFF & ((unsigned long)(x))))

int di_fd=0;

u8 inbuf[0x20] ALIGNED(0x20);
u8 outbuf[0x20] ALIGNED(0x20);
ioctlv *buffer=(void*)0x93000000;

int DVDInit( void )
{
	di_fd = IOS_Open("/dev/di",0);
	return di_fd;
}
int DVDGetHandle( void )
{
	return di_fd;
}
int DVDLowGetCoverStatus( void )
{
	memset(inbuf, 0, 0x20 );
	memset(outbuf, 0, 0x20 );

	((u32*)inbuf)[0x00] = 0x88000000;

	DCFlushRange(inbuf, 0x20);
	int ret = IOS_Ioctl( di_fd, 0x88, inbuf, 0x20, outbuf, 0x20);
	if( ret < 0 )
		return ret;

	return outbuf[3];
}
int DVDLowRequestError( void )
{
	memset(inbuf, 0, 0x20 );
	memset(outbuf, 0, 0x20 );

	((u32*)inbuf)[0x00] = 0xE0000000;

	DCFlushRange(inbuf, 0x20);
	int ret = IOS_Ioctl( di_fd, 0xE0, inbuf, 0x20, outbuf, 0x20);
	if( ret < 0 )
		return ret;
	return outbuf[0];
}
int DVDLowRead( void *dst, u32 size, u32 offset)
{
	memset(inbuf, 0, 0x20 );

	((u32*)inbuf)[0x00] = 0x71000000;
	((u32*)inbuf)[0x01] = size;
	((u32*)inbuf)[0x02] = offset;

	DCFlushRange(inbuf, 0x20);
	return IOS_Ioctl( di_fd, 0x71, inbuf, 0x20, (u8*)dst, size);
}
int DVDLowUnencryptedRead( void *dst, u32 size, u32 offset)
{
	memset(inbuf, 0, 0x20 );

	((u32*)inbuf)[0x00] = 0x8D000000;
	((u32*)inbuf)[0x01] = size;
	((u32*)inbuf)[0x02] = offset;

	DCFlushRange(inbuf, 0x20);
	return IOS_Ioctl( di_fd, 0x8D, inbuf, 0x20, (u8*)dst, size);
}
int DVDLowReset( void )
{
	memset(inbuf, 0, 0x20 );

	((u32*)inbuf)[0x00] = 0x8A000000;
	((u32*)inbuf)[0x01] = 1;

	DCFlushRange(inbuf, 0x20);
	return IOS_Ioctl( di_fd, 0x8A, inbuf, 0x20, 0, 0);
}
int DVDLowStopMotor( void )
{
	memset(inbuf, 0, 0x20 );
	((u32*)inbuf)[0x00] = 0xE3000000;
	((u32*)inbuf)[0x01] = 0;	// optional, unknown
	((u32*)inbuf)[0x02] = 0;	// optional, unknown

	DCFlushRange(inbuf, 0x20);
	return IOS_IoctlAsync( di_fd, 0xE3, inbuf, 0x20, outbuf, 0x20, NULL, NULL);	//does not really return something (outbuf)
}
int DVDLowClosePartition( void )
{
	memset(inbuf, 0, 0x20 );
	((u32*)inbuf)[0x00] = 0x8C000000;
	return IOS_Ioctl( di_fd, 0x8C, inbuf, 0x20, 0, 0);
}
int DVDLowReadDiskID( void *dst )
{
	memset(inbuf, 0, 0x20 );
	memset((u8*)dst, 0, 0x20 );

	((u32*)inbuf)[0x00] = 0x70000000;

	DCFlushRange(inbuf, 0x20);
	return IOS_Ioctl( di_fd, 0x70, inbuf, 0x20, (u8*)dst, 0x20);
}
int DVDLowOpenPartition( unsigned int offset )
{
	//let's open the partition

	((u32*)buffer)[(0x40>>2)]	=  0x8B000000;
	((u32*)buffer)[(0x40>>2)+1]	=  offset>>2;

	//in
	((u32*)buffer)[0x00] = *((unsigned long *)(0x7FFFFFFF & ((unsigned long)(buffer+0x40))));
	((u32*)buffer)[0x01] = 0x20;					//0x04
	((u32*)buffer)[0x02] = 0;						//0x08
	((u32*)buffer)[0x03] = 0x2A4;					//0x0C 
	((u32*)buffer)[0x04] = 0;						//0x10
	((u32*)buffer)[0x05] = 0;						//0x14

	//out
	((u32*)buffer)[0x06] = *((unsigned long *)(0x7FFFFFFF & ((unsigned long)(buffer+0x380))));	//0x18
	((u32*)buffer)[0x07] = 0x49E4;					//0x1C
	((u32*)buffer)[0x08] = *((unsigned long *)(0x7FFFFFFF & ((unsigned long)(buffer+0x360))));	//0x20
	((u32*)buffer)[0x09] = 0x20;					//0x24

	DCFlushRange(buffer, 0x100);

	return IOS_Ioctlv( DVDGetHandle(), 0x8B, 3, 2, buffer);
}
