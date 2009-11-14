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
#include <ogc/dvd.h>
#include <ogc/wiilaunch.h>	
#include <wiiuse/wpad.h>
#include <fat.h>
#include <gcmodplay.h>
#include <unistd.h>

#include "auth.h"
#include "rethandle.h"
#include "cleanup.h"
#include "general.h"

#include "sysmenu.h"
#include "su.h"
#include "md5.h"
#include "sha1.h"

static u8 su_tmd_x[0x208] ATTRIBUTE_ALIGN(32);
static u8 su_tik_x[STD_SIGNED_TIK_SIZE] ATTRIBUTE_ALIGN(32);
int su_id_filled = 0;

extern int need_sys;

void TitleAuth(u64 titleID)
{
	u32 TMD_size ATTRIBUTE_ALIGN(32);
	CheckESRetval(ES_GetStoredTMDSize(titleID, &TMD_size));

	signed_blob *TMD = (signed_blob *)memalign( 32, ALIGN_LENGTH(TMD_size) );
	memset(TMD, 0, TMD_size);

	CheckESRetval(ES_GetStoredTMD(titleID, TMD, TMD_size));
	CheckESRetval(ES_Identify(SU_IDENTIFY));
	
	char ticket_path[ISFS_MAXPATH];
	sprintf(ticket_path,"/ticket/%08x/%08x.tik",(u32)(titleID>>32),(u32)titleID);
	s32 ticket_fd=ISFS_Open(ticket_path,ISFS_OPEN_READ);
	CheckISFSRetval(ticket_fd);
	signed_blob *ticket=(signed_blob *)memalign( 32, ALIGN_LENGTH(STD_SIGNED_TIK_SIZE) );
	memset(ticket, 0, STD_SIGNED_TIK_SIZE);
	CheckISFSRetval(ISFS_Read(ticket_fd,ticket,STD_SIGNED_TIK_SIZE));
	CheckISFSRetval(ISFS_Close(ticket_fd));
	printf("\nIdentifying as %08x-%08x!... \n",(u32)(titleID>>32),(u32)titleID);
	CheckESRetval(ES_Identify(certs_bin, certs_bin_size, TMD, TMD_size, ticket, STD_SIGNED_TIK_SIZE, &tempKeyID));
	printf("SUCCESS!\n");
	if(titleID != 0x0000000100000002LL) need_sys = 1; else need_sys = 0;
	free(TMD);
	free(ticket);
}

void SystemMenuAuth()
{
	dbgprintf("\nIdentifying as 00000001-00000002 (System Menu)!... \n");
/*	u32 TMD_size ATTRIBUTE_ALIGN(32);
	CheckESRetval(ES_GetStoredTMDSize(0x0000000100000002LL, &TMD_size));

	signed_blob *TMD = (signed_blob *)memalign( 32, TMD_size );
	memset(TMD, 0, TMD_size);

	CheckESRetval(ES_GetStoredTMD(0x0000000100000002LL, TMD, TMD_size));

	CheckESRetval(ES_Identify(certs_bin, certs_bin_size, TMD, TMD_size, sysmenu_tik, sysmenu_tik_size, &tempKeyID));
	
	//printf("SUCCESS!\n");

	free(TMD);*/
	need_sys = 0;
	TitleAuth(0x0000000100000002LL);
}

s32 identify_title(u64 titleID)
{
	u32 tmdSize;
	s32 res;
	signed_blob *ptmd;
	u8 *tik;
	u32 tik_size;
	char buf[ISFS_MAXPATH + 1];
	
	printf("Identifying as 00000001-00000000 (SU)!... ");
	s32 retval=ES_Identify(SU_IDENTIFY);
	CheckESRetval(retval);
	
	//Get tmd size.
	CheckESRetval(ES_GetStoredTMDSize(titleID, &tmdSize));
	
	//Get tmd.
	ptmd = memalign(32, tmdSize);
	CheckESRetval(ES_GetStoredTMD(titleID, ptmd, tmdSize));

	u32 TitleIDH ATTRIBUTE_ALIGN(32) = titleID>>32;
	u32 TitleIDL ATTRIBUTE_ALIGN(32) = titleID;
	//Get ticket.
	tik = memalign(32, ALIGN_LENGTH(STD_SIGNED_TIK_SIZE * 4));
	sprintf(buf, "/ticket/%08x/%08x.tik", TitleIDH, TitleIDL);
	res = readfile(buf, tik, STD_SIGNED_TIK_SIZE * 4, &tik_size);
	if(res < 0)
	{
		printf("Error reading ticket!\n");
		return -1;
	}
	
	//Identify.
	printf("Identifying as %016llx... ", titleID);
	CheckESRetval(ES_Identify((signed_blob*)certs_bin, certs_bin_size, ptmd, tmdSize, (signed_blob*)tik, tik_size, NULL));
	CheckESRetval(ES_SetUID(titleID));
	printf("Done!\n");
	
	free(ptmd);
	free(tik);
	if(titleID != 0x0000000100000002LL) need_sys = 1; else need_sys = 0;
	
	return 0;
}

void zero_sig(signed_blob *sig) {
  u8 *sig_ptr = (u8 *)sig;
  memset(sig_ptr + 4, 0, SIGNATURE_SIZE(sig)-4);
}

void brute_tmd(tmd *p_tmd) {
  u16 fill;
  for(fill=0; fill<65535; fill++) {
    p_tmd->fill3=fill;
    sha1 hash;
    //    debug_printf("SHA1(%p, %x, %p)\n", p_tmd, TMD_SIZE(p_tmd), hash);
    make_sha1((u8 *)p_tmd, TMD_SIZE(p_tmd), hash);;
  
    if (hash[0]==0) {
      //      debug_printf("setting fill3 to %04hx\n", fill);
      return;
    }
  }
  printf("Unable to fix tmd :(\n");
  exit(4);
}

void brute_tik(tik *p_tik) {
  u16 fill;
  for(fill=0; fill<65535; fill++) {
    p_tik->padding=fill;
    sha1 hash;
    //    debug_printf("SHA1(%p, %x, %p)\n", p_tmd, TMD_SIZE(p_tmd), hash);
    make_sha1((u8 *)p_tik, sizeof(tik), hash);
  
    if (hash[0]==0) return;
  }
  printf("Unable to fix tik :(\n");
  exit(5);
}
    
void forge_tmd(signed_blob *s_tmd) {
//  debug_printf("forging tmd sig");
  zero_sig(s_tmd);
  brute_tmd(SIGNATURE_PAYLOAD(s_tmd));
}

void forge_tik(signed_blob *s_tik) {
//  debug_printf("forging tik sig");
  zero_sig(s_tik);
  brute_tik(SIGNATURE_PAYLOAD(s_tik));
}

#ifdef DEBUG_IDENT
s32 __sanity_check_certlist(const signed_blob *certs, u32 certsize)
{
	int count = 0;
	signed_blob *end;
	
	if(!certs || !certsize) return 0;
	
	end = (signed_blob*)(((u8*)certs) + certsize);
	while(certs != end) {
#ifdef DEBUG_ES
		printf("Checking certificate at %p\n",certs);
#endif
		certs = ES_NextCert(certs);
		if(!certs) return 0;
		count++;
	}
#ifdef DEBUG_ES
	printf("Num of certificates: %d\n",count);
#endif
	return count;
}
#endif

void Make_SUID(void){
	signed_blob *s_tmd, *s_tik;
	tmd *p_tmd;
	tik *p_tik;
	
	memset(su_tmd_x, 0, sizeof su_tmd_x);
	memset(su_tik_x, 0, sizeof su_tik_x);
	s_tmd = (signed_blob*)&su_tmd_x[0];
	s_tik = (signed_blob*)&su_tik_x[0];
	*s_tmd = *s_tik = 0x10001;
	p_tmd = (tmd*)SIGNATURE_PAYLOAD(s_tmd);
	p_tik = (tik*)SIGNATURE_PAYLOAD(s_tik);
	
	
	strcpy(p_tmd->issuer, "Root-CA00000001-CP00000004");
	p_tmd->title_id = TITLE_ID(1,2);
	
	p_tmd->num_contents = 1;
	
	forge_tmd(s_tmd);
	
	strcpy(p_tik->issuer, "Root-CA00000001-XS00000003");
	p_tik->ticketid = 0x000038A45236EE5FLL;
	p_tik->titleid = TITLE_ID(1,2);
	
	memset(p_tik->cidx_mask, 0xFF, 0x20);
	forge_tik(s_tik);
	
	su_id_filled = 1;
	
}

s32 Identify(const u8 *certs, u32 certs_size, const u8 *idtmd, u32 idtmd_size, const u8 *idticket, u32 idticket_size) {
	s32 ret;
	u32 keyid = 0;
	ret = ES_Identify((signed_blob*)certs, certs_size, (signed_blob*)idtmd, idtmd_size, (signed_blob*)idticket, idticket_size, &keyid);
	if (ret < 0){
		switch(ret){
			case ES_EINVAL:
				printf("Error! ES_Identify (ret = %d;) Data invalid!\n", ret);
				break;
			case ES_EALIGN:
				printf("Error! ES_Identify (ret = %d;) Data not aligned!\n", ret);
				break;
			case ES_ENOTINIT:
				printf("Error! ES_Identify (ret = %d;) ES not initialized!\n", ret);
				break;
			case ES_ENOMEM:
				printf("Error! ES_Identify (ret = %d;) No memory!\n", ret);
				break;
			default:
				printf("Error! ES_Identify (ret = %d)\n", ret);
				break;
		}
#ifdef DEBUG_IDENT
		printf("\tTicket: %u Std: %u Max: %u\n", idticket_size, STD_SIGNED_TIK_SIZE, MAX_SIGNED_TMD_SIZE);
		printf("\tTMD invalid? %d %d %d Tik invalid? %d %d\n", !(signed_blob*)idtmd, !idtmd_size, !IS_VALID_SIGNATURE((signed_blob*)idtmd), !(signed_blob*)idticket, !IS_VALID_SIGNATURE((signed_blob*)idticket));
		printf("\tCerts: Sane? %d\n", __sanity_check_certlist((signed_blob*)certs, certs_size));
		if (!ISALIGNED(certs)) printf("\tCertificate data is not aligned!\n");
		if (!ISALIGNED(idtmd)) printf("\tTMD data is not aligned!\n");
		if (!ISALIGNED(idticket)) printf("\tTicket data is not aligned!\n");
#endif
	}
	else
		printf("OK!\n");
	return ret;
}


s32 Identify_SU(void) {
	if (!su_id_filled)
		Make_SUID();
	
	printf("\nIdentifying as SU...");
	fflush(stdout);
	return Identify((const u8*)certs_bin, (u32)certs_bin_size, su_tmd_x, sizeof su_tmd_x, su_tik_x, sizeof su_tik_x);
}


