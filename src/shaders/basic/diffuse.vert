#version 450
#extension GL_ARB_separate_shader_objects: enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 interpolated_position;
layout (location = 1) out vec4 interpolated_normal;
layout (location = 2) out vec2 interpolated_uv;

void main() {
    interpolated_position = vec4(position, 1.0);
    interpolated_normal = vec4(normal, 0.0);
    interpolated_uv = uv;

    mat4 transform_matrix = mat4(1.0);
    // transform_matrix[3] = vec4(0, 0, 2.0, 1.0);
    gl_Position = transform_matrix * vec4(position, 1.0);
}
