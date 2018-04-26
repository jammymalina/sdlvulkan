#include "./shader_manager.h"

#include "../../string/string.h"

void init_shader(shader *s) {
    s->module = VK_NULL_HANDLE;
    s->bindings_size = 0;
    s->render_params_size = 0;
}
