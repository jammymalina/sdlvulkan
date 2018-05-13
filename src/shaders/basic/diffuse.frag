#version 450
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec4 vertex_position;
layout (location = 1) in vec4 n;
layout (location = 2) in vec2 uv;

layout (location = 0) out vec4 out_color;

void main() {
    vec4 light = normalize(vec4(0, 0 , 0, 1) - vertex_position);
    // reflectance based on lamberts law of cosine
    float theta = clamp(dot(n, light), 0.0, 1.0);
    out_color = vec4(theta, theta, theta, 1.0);
}
