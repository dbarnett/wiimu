#ifndef _CHANNELS_H
#define _CHANNELS_H

#include <ogcsys.h>
#include <gccore.h>

void launchTitle(u64 titleID, char* name, int need_sys);
void deleteChannel(u64 titleID, char* titleName);
void channelToSD(u32 TitleIDH, u32 TitleIDL, char* titleName, char* destdir);
void ChannelLauncher();

#endif //_CHANNELS_H

