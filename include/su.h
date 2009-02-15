#ifndef _SU_H
#define _SU_H

#include "su_tik.h"
#include "su_tmd.h"
#include "certs_bin.h"

#define SU_IDENTIFY \
 (const signed_blob*)certs_bin, (u32)certs_bin_size, (const signed_blob*)su_tmd, (u32)su_tmd_size, (const signed_blob*)su_tik, (u32)su_tik_size, NULL

#endif //_SU_H
