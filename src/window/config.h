#ifndef CUSTOM_WINDOW_CONFIG_H
#define CUSTOM_WINDOW_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct window_configuration {
    int width;
    int height;
    bool fullscreen;
} window_configuration;

extern window_configuration window_config;

#endif
