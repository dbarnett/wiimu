#include <wiiuse/wpad.h>

#include "controls.h"

volatile int A_button_held = 0;

static control_handler handlers[MAX_HANDLERS] = {NULL};

static int fire_handler(button_t button, int source, int is_press) {
    int h_id;
    for (h_id = MAX_HANDLERS-1; h_id >= 0; --h_id) {
        if (handlers[h_id] == NULL)
            continue;
        if (handlers[h_id](button, source, is_press) == 1)
            return 1;       // event handled
    }
    return 0;       // event not handled
}

void check_controls(void) {
    int c_num;
    button_t but_num;

    // Wiimotes
    WPAD_ScanPads();
    for (c_num = 0; c_num < 4; ++c_num) {
        u32 WPAD_Pressed = WPAD_ButtonsDown(c_num);
        u32 WPAD_Released = WPAD_ButtonsUp(c_num);
        for (but_num = 0; but_num < NUM_BUTTONS; ++but_num) {
            if (WPAD_Pressed & wii_button_masks[but_num]) {
                if (wii_button_masks[but_num] == WPAD_BUTTON_A)
                    ++A_button_held;
                fire_handler(but_num, c_num, 1);
            }
            if (WPAD_Released & wii_button_masks[but_num]) {
                if (wii_button_masks[but_num] == WPAD_BUTTON_A)
                    if (A_button_held)
                        --A_button_held;
                fire_handler(but_num, c_num, 0);
            }
        }
    }

    // GC controllers
    PAD_ScanPads();
    for (c_num = 0; c_num < 4; ++c_num) {
        u32 PAD_Pressed = PAD_ButtonsDown(c_num);
        u32 PAD_Released = PAD_ButtonsUp(c_num);
        for (but_num = 0; but_num < NUM_BUTTONS; ++but_num) {
            if (PAD_Pressed & gc_button_masks[but_num]) {
                if (gc_button_masks[but_num] == PAD_BUTTON_A)
                    ++A_button_held;
                fire_handler(but_num, c_num+4, 1);
            }
            if (PAD_Released & gc_button_masks[but_num]) {
                if (gc_button_masks[but_num] == PAD_BUTTON_A)
                    if (A_button_held)
                        --A_button_held;
                fire_handler(but_num, c_num+4, 0);
            }
        }
    }
}

int add_handler(control_handler h) {
    int h_id;
    for (h_id = 0; h_id < MAX_HANDLERS; ++h_id)
        if (handlers[h_id] == NULL)
            break;

    if (h_id >= MAX_HANDLERS)
        return -1;

    handlers[h_id] = h;

    return h_id;
}

void remove_handler(int h_id) {
    handlers[h_id] = NULL;
}
