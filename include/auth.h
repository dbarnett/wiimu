#ifndef _AUTH_H
#define _AUTH_H

void TitleAuth(u64 titleID);
void SystemMenuAuth();
s32 identify_title(u64 titleID);
void zero_sig(signed_blob *sig);
void brute_tmd(tmd *p_tmd);
void brute_tik(tik *p_tik);
void forge_tmd(signed_blob *s_tmd);
void forge_tik(signed_blob *s_tik);
void Make_SUID(void);
s32 Identify(const u8 *certs, u32 certs_size, const u8 *idtmd, u32 idtmd_size, const u8 *idticket, u32 idticket_size);
s32 Identify_SU(void);

#endif //_AUTH_H

