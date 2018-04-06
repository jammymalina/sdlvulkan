#ifndef CUSTOM_WINDOW_CONFIG_H
#define CUSTOM_WINDOW_CONFIG_H

#include <stdint.h>

typedef struct {
	uint32_t width;
	uint32_t height;
} window_configuration;

extern window_configuration window_config;

#endif
