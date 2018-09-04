#ifndef VMATH_QUAT_H
#define VMATH_QUAT_H

#include "./vmath.h"
#include "./vec3.h"

typedef float quat[4];

#define QUAT_COPY(q) { q[0], q[1], q[2], q[3] }

void addq(quat dest, const quat q, const quat v);
void subq(quat dest, const quat q, const quat v);
void mulq(quat dest, const quat q, const quat v);
void addqs(quat dest, const quat q, float x);
void subqs(quat dest, const quat q, float x);
void mulqs(quat dest, const quat q, float x);
void divqs(quat dest, const quat q, const float x);

float dotvq(const quat q, const quat v);
float lengthq(const quat q);

void normalizeq(quat dest, const quat q);
void conjugateq(quat dest, const quat q);
void inverseq(quat dest, const quat q);

void unit_vectors_to_quat(quat dest, const vec3 u, const vec3 v);
void eulerq_xyz_to_quat(quat dest, const vec3 euler_angle);

void slerpq(quat dest, const quat q, const quat v, float t);

#endif // VMATH_QUAT_H
