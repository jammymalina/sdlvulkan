#include "./input.h"

input_info input;

bool init_input_info(input_info *inp_i) {
    SDL_PumpEvents();
    SDL_GetMouseState(&inp_i->mouse.x, &inp_i->mouse.y);
    inp_i->mouse.dx = 0, inp_i->mouse.dy = 0;
    inp_i->mouse.left_button_down  = SDL_BUTTON(SDL_BUTTON_LEFT);
    inp_i->mouse.right_button_down = SDL_BUTTON(SDL_BUTTON_RIGHT);
    return true;
}

static void handle_mouse_button_event(input_info *inp_i, SDL_Event *event) {
    SDL_MouseButtonEvent me = event->button;
    bool pressed = me.state == SDL_PRESSED;
    switch (me.button) {
        case SDL_BUTTON_LEFT:
            inp_i->mouse.left_button_down = pressed;
            break;
        case SDL_BUTTON_RIGHT:
            inp_i->mouse.right_button_down = pressed;
            break;
    }
}

static void handle_mouse_motion_event(input_info *inp_i, SDL_Event *event) {
    SDL_MouseMotionEvent me = event->motion;
    inp_i->mouse.x = me.x;
    inp_i->mouse.y = me.y;
    inp_i->mouse.dx = me.xrel;
    inp_i->mouse.dy = me.yrel;
}

void update_input_info(input_info *inp_i, SDL_Event *event) {
    switch (event->type) {
        case SDL_MOUSEMOTION:
            handle_mouse_motion_event(inp_i, event);
            break;
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_button_event(inp_i, event);
            break;
    }
}

bool init_input() {
    return init_input_info(&input);
}

void update_input(SDL_Event *event) {
    update_input_info(&input, event);
}
