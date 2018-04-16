#include "./material.h"

size_t bit_count_image_format(texture_format format) {
    switch (format) {
        case FMT_NONE:		
            return 0;
        case FMT_RGBA8:		
            return 32;
        case FMT_XRGB8:		
            return 32;
        case FMT_RGB565:	
            return 16;
        case FMT_L8A8:		
            return 16;
        case FMT_ALPHA:		
            return 8;
        case FMT_LUM8:		
            return 8;
        case FMT_INT8:		
            return 8;
        case FMT_DXT1:		
            return 4;
        case FMT_DXT5:		
            return 8;
        case FMT_DEPTH:		
            return 32;
        case FMT_X16:		
            return 16;
        case FMT_Y16_X16:	
            return 32;
        default:
            return 0;
    }
}
