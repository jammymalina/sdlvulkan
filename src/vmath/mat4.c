#include "./mat4.h"

void scalar_mat4(mat4 m, float x) {
    m[ 0] = x, m[ 1] = x, m[ 2] = x, m[ 3] = x,
    m[ 4] = x, m[ 5] = x, m[ 6] = x, m[ 7] = x,
    m[ 8] = x, m[ 9] = x, m[10] = x, m[11] = x,
    m[12] = x, m[13] = x, m[14] = x, m[15] = x;
}

void diagonal_mat4(mat4 m, float x) {
    m[ 0] = x, m[ 1] = 0, m[ 2] = 0, m[ 3] = 0,
    m[ 4] = 0, m[ 5] = x, m[ 6] = 0, m[ 7] = 0,
    m[ 8] = 0, m[ 9] = 0, m[10] = x, m[11] = 0,
    m[12] = 0, m[13] = 0, m[14] = 0, m[15] = x;
}

void identity_mat4(mat4 m) {
    diagonal_mat4(m, 1.0);
}

void mat3_to_mat4(mat4 dest, const mat3 m) {
    identity_mat4(dest);

    set_mat4(dest, 0, 0, get_mat3(m, 0, 0));
    set_mat4(dest, 0, 1, get_mat3(m, 0, 1));
    set_mat4(dest, 0, 2, get_mat3(m, 0, 2));

    set_mat4(dest, 1, 0, get_mat3(m, 1, 0));
    set_mat4(dest, 1, 1, get_mat3(m, 1, 1));
    set_mat4(dest, 1, 2, get_mat3(m, 1, 2));

    set_mat4(dest, 2, 0, get_mat3(m, 2, 0));
    set_mat4(dest, 2, 1, get_mat3(m, 2, 1));
    set_mat4(dest, 2, 2, get_mat3(m, 2, 2));
}

void get_row_mat4(vec4 dest, const mat4 m, size_t row) {
    dest[0] = get_mat4(m, row, 0);
    dest[1] = get_mat4(m, row, 1);
    dest[2] = get_mat4(m, row, 2);
    dest[3] = get_mat4(m, row, 3);
}

void get_column_mat4(vec4 dest, const mat4 m, size_t column) {
    dest[0] = get_mat4(m, 0, column);
    dest[1] = get_mat4(m, 1, column);
    dest[2] = get_mat4(m, 2, column);
    dest[3] = get_mat4(m, 3, column);
}

void set_row_mat4(mat4 m, size_t row, const vec4 v) {
    set_mat4(m, row, 0, v[0]);
    set_mat4(m, row, 1, v[1]);
    set_mat4(m, row, 2, v[2]);
    set_mat4(m, row, 3, v[3]);
}

void set_column_mat4(mat4 m, size_t column, const vec4 v) {
    set_mat4(m, 0, column, v[0]);
    set_mat4(m, 1, column, v[1]);
    set_mat4(m, 2, column, v[2]);
    set_mat4(m, 3, column, v[3]);
}

void mulmat4(mat4 dest, mat4 a, mat4 b) {
    float a11 = get_mat4(a, 0, 0), a12 = get_mat4(a, 0, 1), a13 = get_mat4(a, 0, 2), a14 = get_mat4(a, 0, 3);
    float a21 = get_mat4(a, 1, 0), a22 = get_mat4(a, 1, 1), a23 = get_mat4(a, 1, 2), a24 = get_mat4(a, 1, 3);
    float a31 = get_mat4(a, 2, 0), a32 = get_mat4(a, 2, 1), a33 = get_mat4(a, 2, 2), a34 = get_mat4(a, 2, 3);
    float a41 = get_mat4(a, 3, 0), a42 = get_mat4(a, 3, 1), a43 = get_mat4(a, 3, 2), a44 = get_mat4(a, 3, 3);

    float b11 = get_mat4(b, 0, 0), b12 = get_mat4(b, 0, 1), b13 = get_mat4(b, 0, 2), b14 = get_mat4(b, 0, 3);
    float b21 = get_mat4(b, 1, 0), b22 = get_mat4(b, 1, 1), b23 = get_mat4(b, 1, 2), b24 = get_mat4(b, 1, 3);
    float b31 = get_mat4(b, 2, 0), b32 = get_mat4(b, 2, 1), b33 = get_mat4(b, 2, 2), b34 = get_mat4(b, 2, 3);
    float b41 = get_mat4(b, 3, 0), b42 = get_mat4(b, 3, 1), b43 = get_mat4(b, 3, 2), b44 = get_mat4(b, 3, 3);

    set_mat4(dest, 0, 0, a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41);
	set_mat4(dest, 0, 1, a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42);
    set_mat4(dest, 0, 2, a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43);
	set_mat4(dest, 0, 3, a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44);

	set_mat4(dest, 1, 0, a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41);
	set_mat4(dest, 1, 1, a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42);
	set_mat4(dest, 1, 2, a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43);
	set_mat4(dest, 1, 3, a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44);

	set_mat4(dest, 2, 0, a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41);
	set_mat4(dest, 2, 1, a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42);
	set_mat4(dest, 2, 2, a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43);
	set_mat4(dest, 2, 3, a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44);

	set_mat4(dest, 3, 0, a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41);
	set_mat4(dest, 3, 1, a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42);
	set_mat4(dest, 3, 2, a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43);
	set_mat4(dest, 3, 3, a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44);
}

bool inverse_mat4(mat4 dest, const mat4 m) {
    float m11 = get_mat4(m, 0, 0), m12 = get_mat4(m, 0, 1), m13 = get_mat4(m, 0, 2), m14 = get_mat4(m, 0, 3);
    float m21 = get_mat4(m, 1, 0), m22 = get_mat4(m, 1, 1), m23 = get_mat4(m, 1, 2), m24 = get_mat4(m, 1, 3);
    float m31 = get_mat4(m, 2, 0), m32 = get_mat4(m, 2, 1), m33 = get_mat4(m, 2, 2), m34 = get_mat4(m, 2, 3);
    float m41 = get_mat4(m, 3, 0), m42 = get_mat4(m, 3, 1), m43 = get_mat4(m, 3, 2), m44 = get_mat4(m, 3, 3);

    float d11 = m23 * m34 * m42 - m24 * m33 * m42 +
        m24 * m32 * m43 - m22 * m34 * m43 - m23 * m32 * m44 + m22 * m33 * m44;
    float d12 = m14 * m33 * m42 - m13 * m34 * m42 -
        m14 * m32 * m43 + m12 * m34 * m43 + m13 * m32 * m44 - m12 * m33 * m44;
    float d13 = m13 * m24 * m42 - m14 * m23 * m42 +
        m14 * m22 * m43 - m12 * m24 * m43 - m13 * m22 * m44 + m12 * m23 * m44;
    float d14 = m14 * m23 * m32 - m13 * m24 * m32 -
        m14 * m22 * m33 + m12 * m24 * m33 + m13 * m22 * m34 - m12 * m23 * m34;

    float det = m11 * d11 + m21 * d12 + m31 * d13 + m41 * d14;
    if (fabs(det) < EPSILON) {
        identity_mat4(dest);
        return false;
    }

    float det_inv = 1.0 / det;

    set_mat4(dest, 0, 0, d11 * det_inv);
    set_mat4(dest, 0, 1, d12 * det_inv);
    set_mat4(dest, 0, 2, d13 * det_inv);
    set_mat4(dest, 0, 3, d14 * det_inv);

    set_mat4(dest, 1, 0, (m24 * m33 * m41 - m23 * m34 * m41 - m24 * m31 * m43 + m21 * m34 * m43 + m23 * m31 * m44 - m21 * m33 * m44) * det_inv);
    set_mat4(dest, 1, 1, (m13 * m34 * m41 - m14 * m33 * m41 + m14 * m31 * m43 - m11 * m34 * m43 - m13 * m31 * m44 + m11 * m33 * m44) * det_inv);
    set_mat4(dest, 1, 2, (m14 * m23 * m41 - m13 * m24 * m41 - m14 * m21 * m43 + m11 * m24 * m43 + m13 * m21 * m44 - m11 * m23 * m44) * det_inv);
    set_mat4(dest, 1, 3, (m13 * m24 * m31 - m14 * m23 * m31 + m14 * m21 * m33 - m11 * m24 * m33 - m13 * m21 * m34 + m11 * m23 * m34) * det_inv);

    set_mat4(dest, 2, 0, (m22 * m34 * m41 - m24 * m32 * m41 + m24 * m31 * m42 - m21 * m34 * m42 - m22 * m31 * m44 + m21 * m32 * m44) * det_inv);
    set_mat4(dest, 2, 1, (m14 * m32 * m41 - m12 * m34 * m41 - m14 * m31 * m42 + m11 * m34 * m42 + m12 * m31 * m44 - m11 * m32 * m44) * det_inv);
    set_mat4(dest, 2, 2, (m12 * m24 * m41 - m14 * m22 * m41 + m14 * m21 * m42 - m11 * m24 * m42 - m12 * m21 * m44 + m11 * m22 * m44) * det_inv);
    set_mat4(dest, 2, 3, (m14 * m22 * m31 - m12 * m24 * m31 - m14 * m21 * m32 + m11 * m24 * m32 + m12 * m21 * m34 - m11 * m22 * m34) * det_inv);

    set_mat4(dest, 3, 0, (m23 * m32 * m41 - m22 * m33 * m41 - m23 * m31 * m42 + m21 * m33 * m42 + m22 * m31 * m43 - m21 * m32 * m43) * det_inv);
    set_mat4(dest, 3, 1, (m12 * m33 * m41 - m13 * m32 * m41 + m13 * m31 * m42 - m11 * m33 * m42 - m12 * m31 * m43 + m11 * m32 * m43) * det_inv);
    set_mat4(dest, 3, 2, (m13 * m22 * m41 - m12 * m23 * m41 - m13 * m21 * m42 + m11 * m23 * m42 + m12 * m21 * m43 - m11 * m22 * m43) * det_inv);
    set_mat4(dest, 3, 3, (m12 * m23 * m31 - m13 * m22 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 + m11 * m22 * m33) * det_inv);

    return true;
}

void compose_mat4(mat4 dest, const vec3 position, const quat quaternion, const vec3 scale) {
    float x = quaternion[0], y = quaternion[1], z = quaternion[2], w = quaternion[3];
    float x2 = 2 * x, y2 = 2 * y, z2 = 2 * z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2;
    float zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    float sx = scale[0], sy = scale[1], sz = scale[2];
    float px = position[0], py = position[1], pz = position[2];

    set_mat4(dest, 0, 0, (1 - (yy + zz)) * sx);
    set_mat4(dest, 0, 1, (xy - wz) * sy);
    set_mat4(dest, 0, 2, (xz + wy) * sz);
    set_mat4(dest, 0, 3, px);

    set_mat4(dest, 1, 0, (xy + wz) * sx);
    set_mat4(dest, 1, 1, (1 - (xx + zz)) * sy);
    set_mat4(dest, 1, 2, (yz - wx) * sz);
    set_mat4(dest, 1, 3, py);

    set_mat4(dest, 2, 0, (xz - wy) * sx);
    set_mat4(dest, 2, 1, (yz + wx) * sy);
    set_mat4(dest, 2, 2, (1 - (xx + yy)) * sz);
    set_mat4(dest, 2, 3, pz);

    set_mat4(dest, 3, 0, 0.0);
    set_mat4(dest, 3, 1, 0.0);
    set_mat4(dest, 3, 2, 0.0);
    set_mat4(dest, 3, 3, 1.0);
}

float determinant_mat4(const mat4 m) {
    float m11 = get_mat4(m, 0, 0), m12 = get_mat4(m, 0, 1), m13 = get_mat4(m, 0, 2), m14 = get_mat4(m, 0, 3);
    float m21 = get_mat4(m, 1, 0), m22 = get_mat4(m, 1, 1), m23 = get_mat4(m, 1, 2), m24 = get_mat4(m, 1, 3);
    float m31 = get_mat4(m, 2, 0), m32 = get_mat4(m, 2, 1), m33 = get_mat4(m, 2, 2), m34 = get_mat4(m, 2, 3);
    float m41 = get_mat4(m, 3, 0), m42 = get_mat4(m, 3, 1), m43 = get_mat4(m, 3, 2), m44 = get_mat4(m, 3, 3);

    return m41 * (
            + m14 * m23 * m32
            - m13 * m24 * m32
            - m14 * m22 * m33
            + m12 * m24 * m33
            + m13 * m22 * m34
            - m12 * m23 * m34
        ) + m42 * (
            + m11 * m23 * m34
            - m11 * m24 * m33
            + m14 * m21 * m33
            - m13 * m21 * m34
            + m13 * m24 * m31
            - m14 * m23 * m31
        ) + m43 * (
            + m11 * m24 * m32
            - m11 * m22 * m34
            - m14 * m21 * m32
            + m12 * m21 * m34
            + m14 * m22 * m31
            - m12 * m24 * m31
        ) + m44 * (
            - m13 * m22 * m31
            - m11 * m23 * m32
            + m11 * m22 * m33
            + m13 * m21 * m32
            - m12 * m21 * m33
            + m12 * m23 * m31
        );
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
