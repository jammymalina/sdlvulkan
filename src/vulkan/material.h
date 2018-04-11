#ifndef MATERIAL_H
#define MATERIAL_H

#include "../utils/lib.h"

typedef enum texture_type {
    TT_DISABLED,
    TT_2D,
    TT_CUBIC
} texture_type;

typedef enum texture_format {
    FMT_NONE,
    FMT_RGBA8,
    FMT_XRGB8,
    FMT_ALPHA,
    FMT_L8A8,
    FMT_LUM8,
    FMT_INT8,
    FMT_DXT1,
    FMT_DXT5,
    FMT_X16,
    FMT_Y16_X16,
    FMT_RGB565,
    FMT_DEPTH
} texture_format;

typedef enum texture_samples {
    SAMPLE_1  = BIT(0),
    SAMPLE_2  = BIT(1),
    SAMPLE_4  = BIT(2),
    SAMPLE_8  = BIT(3),
    SAMPLE_16 = BIT(4)
} texture_samples;

typedef enum texture_filter {
    TF_LINEAR,
    TF_NEAREST,
    TF_DEFAULT
} texture_filter;

typedef enum texture_repeat {
    TR_REPEAT,
    TR_CLAMP,
    TR_CLAMP_TO_ZERO,
    TR_CLAMP_TO_ZERO_ALPHA
} texture_repeat;

#endif // MATERIAL_H
