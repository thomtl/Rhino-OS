#pragma once

#include <rhino/common.h>

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t bpp;
    bool allowRounding;
    bool clearFramebuffer;
    bool success;
} udi_video_mode_set_t;