#ifndef _SYSMENU_H
#define _SYSMENU_H

#include "sysmenu_tik.h"
#include "certs_bin.h"

#define SYSMENU_IDENTIFY \
 (const signed_blob*)sysmenu_tik, (u32)sysmenu_tik_size, &tempKeyID
#define IDENTIFY_CERTS \
 (const signed_blob*)certs_bin, (u32)certs_bin_size

#endif //_SYSMENU_H
