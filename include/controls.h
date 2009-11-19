#ifndef _CONTROLS_H
#define _CONTROLS_H

#include <wiiuse/wpad.h>

#define MAX_HANDLERS 64

typedef enum {A_BUTTON, B_BUTTON, HOME_BUTTON, START_BUTTON,
    UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON,
    NUM_BUTTONS=RIGHT_BUTTON} button_t;
static const u32 wii_button_masks[] = {WPAD_BUTTON_A, WPAD_BUTTON_B,
    WPAD_BUTTON_HOME, 0, WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_LEFT,
    WPAD_BUTTON_RIGHT};
static const u32 gc_button_masks[] = {PAD_BUTTON_A, PAD_BUTTON_B,
    0, PAD_BUTTON_START, PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT,
    PAD_BUTTON_RIGHT};

typedef int (*control_handler)(button_t button, int source, int is_press);

void check_controls(void);
int add_handler(control_handler h);
void remove_handler(int h_id);

#endif //_CONTROLS_H
