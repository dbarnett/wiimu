#ifndef __MD5_H
#define __MD5_H

#include <gctypes.h>
#include <stdio.h>
#include <stdlib.h>

#define MD5_DIGEST_LENGTH	16

/* typedef a 32 bit type */
typedef unsigned long int UINT4;

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
  UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
  UINT4 buf[4];                                    /* scratch buffer */
  unsigned char in[64];                              /* input buffer */
  unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;

void		md5		(u8 *data,			u32 len,	u8 *hash);
unsigned char	*MD5_OSSL	(const unsigned char *d,	size_t n,	unsigned char *md);
void		OPENSSL_cleanse	(void *ptr,			size_t len);
void		MD5Init		();
void		MD5Update	();
void		MD5Final	();

#endif //__MD5_H

