#ifndef _NAND_H
#define _NAND_H

#define NAND_SIZE		536870912
#define NAND_READ_SIZE		2112
#define NAND_ACCESSES		(NAND_SIZE/NAND_READ_SIZE)
#define NAND_FLASH_DIR		"/dev/flash"

int backupNAND();
int backupNANDzestig();
//int restoreNAND();								// MEGA DANGEROUS!!! NOT GOING TO ADD TO _ANY_ MAIN BUILD!!! ADD IN AT YOUR OWN RISK!!!

#endif //_NAND_H

