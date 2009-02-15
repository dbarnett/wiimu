#ifndef __EDITING_H
#define __EDITING_H

extern int language_setting;

void configuration(void);
s32 extractChannelContents(u64 titleID, char* location);
void SystemMenuAuth();
void changeChannelName(u64 titleID);

#endif //__EDITING_H
