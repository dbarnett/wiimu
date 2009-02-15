#ifndef __KBDHANDLE_H
#define __KBDHANDLE_H

#ifdef __cplusplus
   extern "C" {
#endif // __cplusplus

void add_character_to_string(char* target, int index, int is_caps, char character, char caps_character);
void add_wiimote_character_to_string(char* target, int index, char character);
int type_string_wiimote(char* target, int target_max_size);

#ifdef __cplusplus
   }
#endif // __cplusplus

#endif // __KBDHANDLE_H

