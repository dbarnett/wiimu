#ifndef _RETHANDLE_H
#define _RETHANDLE_H

void RetvalFail(int badness);
int CheckESRetval(int retval);
int CheckISFSRetval(int retval);
int CheckIPCRetval(int retval);
int CheckCARDRetval(int retval);
int CheckCONFRetval(int retval);
int CheckDVDRetval(int retval);
int CheckIOSRetval(int retval);
int CheckTSS2Retval(int retval);
int CheckPADRetval(int retval);
int CheckSTMRetval(int retval);
int CheckUSBSTORAGERetval(int retval);
int CheckWIIRetval(int retval);

#endif //_RETHANDLE_H

