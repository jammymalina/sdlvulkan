#version 450
#extension GL_ARB_separate_shader_objects: enable

mat4 ortho(float left, float right, float bottom, float top) {
    mat4 Result = mat4(1.0);
    Result[0][0] = 2.0 / (right - left);
    Result[1][1] = 2.0 / (top - bottom);
    Result[3][0] = - (right + left) / (right - left);
    Result[3][1] = - (top + bottom) / (top - bottom);
    return Result;
}

out gl_PerVertex {
    vec4 gl_Position;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 interpolated_position;
layout (location = 1) out vec3 interpolated_normal;
layout (location = 2) out vec2 interpolated_uv;

void main() {
    interpolated_position = vec4(position, 1.0);
    interpolated_normal = vec3(normal);
    interpolated_uv = uv;

    mat4 transform_matrix = ortho(-2.0, 2.0, -1.5, 1.5);
    gl_Position = transform_matrix * vec4(position, 1.0);
}
