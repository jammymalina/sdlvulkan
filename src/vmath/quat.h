#ifndef VMATH_QUAT_H
#define VMATH_QUAT_H

#include "./vmath.h"
#include "./vec3.h"

typedef float quat[4];

#define QUAT_COPY(q) { q[0], q[1], q[2], q[3] }

static inline void addq(quat dest, const quat q, const quat v) {
    dest[0] = q[0] + v[0];
    dest[1] = q[1] + v[1];
    dest[2] = q[2] + v[2];
    dest[3] = q[3] + v[3];
}

static inline void subq(quat dest, const quat q, const quat v) {
    dest[0] = q[0] - v[0];
    dest[1] = q[1] - v[1];
    dest[2] = q[2] - v[2];
    dest[3] = q[3] - v[3];
}

static inline void mulq(quat dest, const quat q, const quat v) {
    vec3 uv = { 0, 0, 0}, uuv = { 0, 0, 0 };
    vec3 quat_vec = { q[0], q[1], q[2] };
    crossv3(uv, quat_vec, v);
    crossv3(uuv, quat_vec, uv);

    vec3 t = { 0, 0, 0 };
    mulv3s(t, uv, q[3]);
    addv3(t, t, uuv);
    mulv3s(t, t, 2.0);

    addq(dest, v, t);
}

static inline void addqs(quat dest, const quat q, float x) {
    dest[0] = q[0] + x;
    dest[1] = q[1] + x;
    dest[2] = q[2] + x;
    dest[3] = q[3] + x;
}

static inline void subqs(quat dest, const quat q, float x) {
    dest[0] = q[0] - x;
    dest[1] = q[1] - x;
    dest[2] = q[2] - x;
    dest[3] = q[3] - x;
}

static inline void mulqs(quat dest, const quat q, float x) {
    dest[0] = q[0] * x;
    dest[1] = q[1] * x;
    dest[2] = q[2] * x;
    dest[3] = q[3] * x;
}

static inline void divqs(quat dest, const quat q, const float x) {
    dest[0] = q[0] / x;
    dest[1] = q[1] / x;
    dest[2] = q[2] / x;
    dest[3] = q[3] / x;
}

static inline float dotvq(const quat q, const quat v) {
    return q[0] * v[0] + q[1] * v[1] + q[2] * v[2] + q[3] * v[3];
}

static inline float lengthq(const quat q) {
    return sqrt(dotvq(q, q));
}

static inline void normalizeq(quat dest, const quat q) {
    const float len = lengthq(q);
    if (len <= EPSILON) {
        dest[3] = 0, dest[0] = 0, dest[1] = 0, dest[2] = 0;
        return;
    }
    divqs(dest, q, len);
}

static inline void conjugateq(quat dest, const quat q) {
    float x = q[0], y = q[1], z = q[2], w = q[3];
    dest[0] =  w;
    dest[1] = -x;
    dest[2] = -y;
    dest[3] = -z;
}

static inline void inverseq(quat dest) {
    quat q = { dest[0], dest[1], dest[2], dest[3] };
    conjugateq(dest, dest);
    divqs(dest, dest, dotvq(q, q));
}

static inline void normalized_axis_to_quat(quat dest, const vec3 u, const vec3 v) {
    float norm_u_norm_v = sqrt(dotv3(u, u) * dotv3(v, v));
    float real_part = norm_u_norm_v + dotv3(u, v);
    vec3 t = { 0, 0, 0 };

    if (real_part < 1.e-6f * norm_u_norm_v) {
        real_part = 0;
        t[0] = 0, t[1] = -u[2], t[2] = u[1];
        if (fabs(u[0]) > fabs(u[2])) {
            t[0] = -u[1], t[1] = u[0], t[2] = 0;
        }
    } else {
        crossv3(t, u, v);
    }
    dest[3] = real_part;
    dest[0] = t[0];
    dest[1] = t[1];
    dest[2] = t[2];
}

static inline void eulerq(quat dest, const vec3 euler_angle) {
    vec3 half_angle = VEC3_COPY(euler_angle);
    mulv3s(half_angle, half_angle, 0.5);
    vec3 c = { cos(half_angle[0]), cos(half_angle[1]), cos(half_angle[2]) };
    vec3 s = { sin(half_angle[0]), sin(half_angle[1]), sin(half_angle[2]) };

    dest[3] = c[0] * c[1] * c[3] + s[0] * s[1] * s[3];
    dest[0] = s[0] * c[1] * c[3] - c[0] * s[1] * s[3];
    dest[1] = c[0] * s[1] * c[3] + s[0] * c[1] * s[3];
    dest[2] = c[0] * c[1] * s[3] - s[0] * s[1] * c[3];
}

#endif // VMATH_QUAT_H
