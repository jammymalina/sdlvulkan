#include "./matrix_transform.h"

void translate_mat4(mat4 dest, const vec3 v) {
}

void rotate_mat4(mat4 dest, float angle, const vec3 v) {
}

void scale_mat4(mat4 dest, const vec3 v) {
}

void ortho(mat4 dest, float left, float right, float bottom, float top) {
    identity_mat4(dest);
    set_mat4(dest, 0, 0, 2.0 / (right - left));
    set_mat4(dest, 1, 1, 2.0 / (top - bottom));
    set_mat4(dest, 0, 3, -(right + left) / (right - left));
    set_mat4(dest, 1, 3, -(top + bottom) / (top - bottom));
}

void perspective_RH_ZO(mat4 dest, float fov, float aspect, float z_near, float z_far) {
    diagonal_mat4(dest, 0.0);
    float tan_half_fov = tan(0.5 * fov);
    set_mat4(dest, 0, 0, 1.0 / (aspect * tan_half_fov));
    set_mat4(dest, 1, 1, 1.0 / (tan_half_fov));
    set_mat4(dest, 2, 2, z_far / (z_near - z_far));
    set_mat4(dest, 3, 2, -1);
    set_mat4(dest, 2, 3, -(z_far * z_near) / (z_far - z_near));
}

void look_at_RH(mat4 dest, const vec3 eye, const vec3 center, const vec3 up) {
    identity_mat4(dest);

    vec3 f = { 0, 0, 0 };
    vec3 s = { 0, 0, 0 };
    vec3 u = { 0, 0, 0 };

    subv3(f, center, eye);
    normalizev3(f, f);

    crossv3(s, f, up);
    normalizev3(s, s);

    crossv3(u, s, f);

    set_mat4(dest, 0, 0, s[0]);
    set_mat4(dest, 0, 1, s[1]);
    set_mat4(dest, 0, 2, s[2]);

    set_mat4(dest, 1, 0, u[0]);
    set_mat4(dest, 1, 1, u[1]);
    set_mat4(dest, 1, 2, u[2]);

    set_mat4(dest, 2, 0, -f[0]);
    set_mat4(dest, 2, 1, -f[1]);
    set_mat4(dest, 2, 2, -f[2]);

    set_mat4(dest, 0, 3, -dotv3(s, eye));
    set_mat4(dest, 1, 3, -dotv3(u, eye));
    set_mat4(dest, 2, 3,  dotv3(f, eye));
}
