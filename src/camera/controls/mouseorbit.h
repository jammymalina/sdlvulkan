#ifndef MOUSE_ORBIT_CONTROLS_H
#define MOUSE_ORBIT_CONTROLS_H

#include <stdbool.h>
#include "../../vmath/vec3.h"

typedef struct mouse_orbit_controls {
    bool enabled;
    vec3 target;

    union {
        float min_zoom;
        float min_distance;
    };
    union {
        float max_zoom;
        float max_distance;
    };

    float min_polar_angle;
    float max_polar_angle;

    float min_azimuth_angle;
    float max_azimuth_angle;

    bool enable_damping;
    float damping_factor;

    bool enable_dolly;
    float dolly_speed;

    bool enable_pan;
    float pan_speed;
    bool sceen_space_panning;
    float key_pan_speed;

    bool auto_rotate;
    float auto_rotate_speed;

    bool enable_keys;
} mouse_orbit_controls;

#endif // MOUSE_ORBIT_CONTROLS_H
