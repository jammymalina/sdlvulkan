#ifndef APP_INPUT_H
#define APP_INPUT_H

#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define INPUT_MAX_CALLBACK_FUNCTIONS 32

#define MOUSE_LEFT_BUTTON   SDL_BUTTON_LEFT
#define MOUSE_RIGHT_BUTTON  SDL_BUTTON_RIGHT
#define MOUSE_MIDDLE_BUTTON SDL_BUTTON_MIDDLE

typedef enum input_event_type {
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE,
    EVENT_TYPES_TOTAL
} input_event_type;

typedef struct input_event {
    union {
        struct {
            int x, y;
            int dx, dy;
            bool left_button_down, middle_button_down, right_button_down;
        } mousemotion;
        struct {
            int x, y;
            uint8_t button;
            uint8_t clicks;
        } mouseclick;
        struct {
            int x, y;
            uint8_t button;
        } mouserelease;
    };
} input_event;

typedef void input_event_callback(const input_event*);

typedef struct input_handler {
    struct {
        int x, y;
        bool left_button_down, middle_button_down, right_button_down;
    } mouse;
    input_event_callback *callbacks[EVENT_TYPES_TOTAL][INPUT_MAX_CALLBACK_FUNCTIONS];
    size_t callbacks_count[EVENT_TYPES_TOTAL];
} input_handler;

bool init_input_handler(input_handler *inp);
void update_input_handler(input_handler *inp, SDL_Event *event);
bool add_event_listener_input_handler(input_handler *inp, input_event_type event_type, input_event_callback *callback);
bool remove_event_listener_input_handler(input_handler *inp, input_event_type event_type,
    input_event_callback *callback);

extern input_handler input;

bool init_input();
void update_input(SDL_Event *event);
bool add_event_listener(input_event_type event_type, input_event_callback *callback);
bool remove_event_listener(input_event_type event_type, input_event_callback *callback);

#endif // APP_INPUT_H
