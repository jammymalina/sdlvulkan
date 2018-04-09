#ifndef RENDERER_CONFIG_H
#define RENDERER_CONFIG_H

#include <stdint.h>

typedef struct renderer_configuration {
	int width;
	int height;
} renderer_configuration;

extern renderer_configuration render_config;

#endif // RENDERER_CONFIG_H
