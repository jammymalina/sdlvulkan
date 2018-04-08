#ifndef CUSTOM_WINDOW_CONFIG_H
#define CUSTOM_WINDOW_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct window_configuration {
	uint32_t width;
	uint32_t height;
	bool fullscreen;
} window_configuration;

extern window_configuration window_config;

#endif
