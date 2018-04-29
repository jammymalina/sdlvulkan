#ifndef SHADER_LIST
#define SHADER_LIST {                            \
    {                                            \
        .instance = SHADER_INSTANCE_TEST,        \
        .name = "test", .directory = "basic",    \
        .type_bits = SHADER_TYPE_GROUP_GRAPHICS, \
        .vertex_layout = VERTEX_LAYOUT_DRAW_VERT \
    }                                            \
}
#endif // SHADER_LIST

#ifndef RENDER_PROGRAM_LIST
#define RENDER_PROGRAM_LIST {                    \
    {                                            \
        .instance = RENDER_PROGRAM_TEST,         \
        .shader_instances = {                    \
            vert: SHADER_INSTANCE_TEST,          \
            frag: SHADER_INSTANCE_TEST,          \
            geom: SHADER_INSTANCE_UNDEFINED,     \
            tesc: SHADER_INSTANCE_UNDEFINED,     \
            tese: SHADER_INSTANCE_UNDEFINED,     \
            comp: SHADER_INSTANCE_UNDEFINED      \
        }                                        \
    }                                            \
}

#endif // RENDER_PROGRAM_LIST
