#ifndef APP_INPUT_H
#define APP_INPUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct input_info {
    struct {
        int x, y;
        int dx, dy;
        bool left_button_down, right_button_down;
    } mouse;
} input_info;

bool init_input_info(input_info *inp_i);
void update_input_info(input_info *inp_i, SDL_Event *event);

extern input_info input;

bool init_input();
void update_input(SDL_Event *event);

#endif // APP_INPUT_H
