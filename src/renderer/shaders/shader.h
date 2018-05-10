#ifndef SHADER_CONFIG_H
#define SHADER_CONFIG_H

#include <vulkan/vulkan.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../utils/file.h"

#define SHADERS_ROOT_FOLDER "shaders"
#define SHADER_TYPES_COUNT 6
#define MAX_SHADER_NAME_SIZE 256
#define MAX_SHADER_RENDER_PARAMS 32
#define MAX_SHADER_BINDINGS_SIZE 32

typedef enum shader_type {
    SHADER_TYPE_VERTEX    =  1,
    SHADER_TYPE_FRAGMENT  =  2,
    SHADER_TYPE_GEOMETRY  =  4,
    SHADER_TYPE_COMPUTE   =  8,
    SHADER_TYPE_TESS_CTRL = 16,
    SHADER_TYPE_TESS_EVAL = 32,
    SHADER_TYPE_UNDEFINED = 64
} shader_type;

#define SHADER_TYPE_GROUP_GRAPHICS (SHADER_TYPE_VERTEX | SHADER_TYPE_FRAGMENT)

typedef enum shader_instance_type {
    SHADER_INSTANCE_UNDEFINED = -1,
    SHADER_INSTANCE_TEST,
    SHADER_INSTANCES_TOTAL
} shader_instance_type;

typedef struct shader_config {
    shader_instance_type instance;
    char *name;
    char *directory;
    uint32_t type_bits;
} shader_config;

typedef enum render_param {
    RENDER_PARAMS_TOTAL
} render_param;

typedef enum shader_binding {
    BINDING_TYPE_UNIFORM_BUFFER,
    BINDING_TYPE_SAMPLER,
    BINDINGS_TOTAL
} shader_binding;

typedef struct shader {
    char name[MAX_SHADER_NAME_SIZE];
    shader_instance_type instance;
    shader_type type;

    VkShaderModule module;

    shader_binding bindings[MAX_SHADER_BINDINGS_SIZE];
    size_t bindings_size;

    render_param render_params[MAX_SHADER_RENDER_PARAMS];
    size_t render_params_size;
} shader;

VkDescriptorType get_descriptor_type(shader_binding type);

bool shader_resolve_path(char dest[MAX_PATH_LENGTH], const char *name, const char *folder, shader_type type);

shader_type extension_to_shader_type(const char *extension);
const char* shader_type_to_extension(shader_type type);
VkShaderStageFlagBits shader_type_to_shader_stage(shader_type type);

void init_shader(shader *s);
bool init_shader_from_file(shader *s, shader_instance_type instance, const char *name, const char *filepath);
void destroy_shader(shader *s);

#endif // SHADER_CONFIG_H
