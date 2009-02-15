#ifndef __SHA1_H
#define __SHA1_H

#include <gctypes.h>

typedef struct {
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void make_sha1(u8 *data, u32 len, u8 *hash);
void SHA1Transform(unsigned long state[5], unsigned char buffer[64]);
void SHA1Init(SHA1_CTX* context);
void SHA1Update(SHA1_CTX* context, unsigned char* data, unsigned int len);
void SHA1Final(unsigned char digest[20], SHA1_CTX* context);

void SHA1_STEVE(unsigned char *ptr, unsigned int size, unsigned char *outbuf);

#endif //__SHA1_H

