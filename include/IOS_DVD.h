#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>


#define DVD_COVER_OPEN	1
#define DVD_COVER_CLOSE	2

int DVDInit(void);
int DVDGetHandle( void );
int DVDLowGetCoverStatus( void );
int DVDLowRequestError( void );
int DVDLowRead( void *dst, u32 size, u32 offset);
int DVDLowUnencryptedRead( void *dst, u32 size, u32 offset);
int DVDLowReset( void );
int DVDLowStopMotor( void );
int DVDLowClosePartition( void );
int DVDLowReadDiskID( void *dst );
int DVDLowOpenPartition( unsigned int offset );
