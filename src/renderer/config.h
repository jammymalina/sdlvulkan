#ifndef RENDERER_CONFIG_H
#define RENDERER_CONFIG_H

#include <stdint.h>

typedef struct renderer_configuration {
	uint32_t width;
	uint32_t height;
} renderer_configuration;

extern renderer_configuration render_config;

#endif // RENDERER_CONFIG_H
