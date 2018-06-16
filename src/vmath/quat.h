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
    vec3 uv = { 0, 0, 0 }, uuv = { 0, 0, 0 };
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
        dest[3] = 1.0, dest[0] = 0, dest[1] = 0, dest[2] = 0;
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

static inline void inverseq(quat dest, const quat q) {
    conjugateq(dest, q);
    divqs(dest, dest, dotvq(q, q));
}

static inline void unit_vectors_to_quat(quat dest, const vec3 u, const vec3 v) {
    float norm_u_norm_v = sqrt(dotv3(u, u) * dotv3(v, v));
    float real_part = norm_u_norm_v + dotv3(u, v);
    vec3 t = { 0, 0, 0 };

    if (real_part < EPSILON * norm_u_norm_v) {
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

static inline void eulerq_xyz(quat dest, const vec3 euler_angle) {
    vec3 half_angle = VEC3_COPY(euler_angle);
    mulv3s(half_angle, half_angle, 0.5);
    vec3 c = { cos(half_angle[0]), cos(half_angle[1]), cos(half_angle[2]) };
    vec3 s = { sin(half_angle[0]), sin(half_angle[1]), sin(half_angle[2]) };

    dest[3] = c[0] * c[1] * c[2] + s[0] * s[1] * s[2];
    dest[0] = s[0] * c[1] * c[2] - c[0] * s[1] * s[2];
    dest[1] = c[0] * s[1] * c[2] + s[0] * c[1] * s[2];
    dest[2] = c[0] * c[1] * s[2] - s[0] * s[1] * c[2];
}

static inline void slerpq(quat dest, const quat q, const quat v, float t) {
    if (fabs(t) < EPSILON) {
        dest[3] = q[3], dest[0] = q[0], dest[1] = q[1], dest[2] = q[2];
        return;
    }
    if (fabs(t - 1.0) < EPSILON) {
        dest[3] = v[3], dest[0] = v[0], dest[1] = v[1], dest[2] = v[2];
        return;
    }

    float w1 = q[3], x1 = q[0], y1 = q[1], z1 = q[2];
    float w2 = v[3], x2 = v[0], y2 = v[1], z2 = v[2];

    float cos_half_theta = w1 * w2 + x1 * x2 + y1 * y2 + z1 * z2;

    if (cos_half_theta < 0) {
        dest[3] = -v[3], dest[0] = -v[0], dest[1] = -v[1], dest[2] = -v[2];
        cos_half_theta = fabs(cos_half_theta);
    } else {
        dest[3] = v[3], dest[0] = v[0], dest[1] = v[1], dest[2] = v[2];
    }

    if (cos_half_theta >= 1.0) {
        dest[3] = q[3], dest[0] = q[0], dest[1] = q[1], dest[2] = q[2];
        return;
    }

    float sin_half_theta = sqrt(1.0 - cos_half_theta * cos_half_theta);

    if (fabs(sin_half_theta) < EPSILON) {
        dest[3] = 0.5 * (w1 + dest[3]);
        dest[0] = 0.5 * (x1 + dest[0]);
        dest[1] = 0.5 * (y1 + dest[1]);
        dest[2] = 0.5 * (z1 + dest[2]);
        return;
    }

    float half_theta = atan2(sin_half_theta, cos_half_theta);
    float ratio_a = sin((1.0 - t) * half_theta) / sin_half_theta;
    float ratio_b = sin(t * half_theta) / sin_half_theta;

    dest[3] = w1 * ratio_a + dest[3] * ratio_b;
    dest[0] = x1 * ratio_a + dest[0] * ratio_b;
    dest[1] = y1 * ratio_a + dest[1] * ratio_b;
    dest[2] = z1 * ratio_a + dest[2] * ratio_b;
}

#endif // VMATH_QUAT_H
