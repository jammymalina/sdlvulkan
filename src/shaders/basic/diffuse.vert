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

mat4 perspective(float fov, float aspect, float z_near, float z_far) {
    mat4 Result = mat4(1.0);
    float tan_half_fov = tan(0.5 * fov);
    Result[0][0] = 1.0 / (aspect * tan_half_fov);
    Result[1][1] = 1.0 / (tan_half_fov);
    Result[2][2] = z_far / (z_near - z_far);
    Result[2][3] = -1;
    Result[3][2] = -(z_far * z_near) / (z_far - z_near);
    return Result;
}

mat4 translate(vec3 d) {
    mat4 Result = mat4(1.0);
    Result[3] = vec4(d, 1.0);
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

    // mat4 transform_matrix = ortho(-2.0, 2.0, -1.5, 1.5);
    mat4 transform_matrix = perspective(90, 4.0 / 3.0, 0.0, 200.0) * translate(vec3(2.0, -3.0, -3.0));
    gl_Position = transform_matrix * vec4(position, 1.0);
}
