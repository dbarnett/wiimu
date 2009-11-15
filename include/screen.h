#ifndef _SCREEN_H
#define _SCREEN_H

void getScreenData(void* fb, GXRModeObj* mode);
void setScreenData(void* fb, GXRModeObj* mode);
void Finish(int retval);
void initialize_wiimu();
void ClearScreen();
s32 STM_ForceDimming(u32 amount);
void FadeOut();
void FadeIn();
void spinner();
int showmenu(char *caption, char **ent, int ecnt, int _default, char* selector);
void draw_menu(const char *caption, char *options[], int n_options);

#endif //_SCREEN_H

