#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifndef LV_ATTRIBUTE_MEM_ALIGN
    #define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMAGE_ARROW_STATE_ON
    #define LV_ATTRIBUTE_IMAGE_ARROW_STATE_ON
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMAGE_ARROW_STATE_ON uint8_t arrow_state_on_map[] = {
    0x3c, 0xef, 0x3c, 0xef, 0x3c, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x3d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x3c, 0xf7, 0x99, 0xe6, 0xda, 0xfe, 0x1b, 0xff, 0x3c, 0xf7, 0x5d,
    0xef, 0x5d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xd5, 0xe5, 0x2e, 0xd4, 0x36, 0xfe, 0x1b, 0xff, 0x5c, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef,
    0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0xfb, 0xfe, 0xb5, 0xf5, 0xaa, 0xe3, 0xaa, 0xe3, 0xd4, 0xfd, 0x1a, 0xff, 0x3c, 0xf7, 0x3c, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d,
    0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x89, 0xeb, 0x47, 0xfb, 0x89, 0xe3, 0xf0, 0xf4, 0xd9, 0xfe, 0x1b, 0xf7, 0x5d, 0xef, 0x5d, 0xe7, 0x5d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x68, 0xf3, 0x26, 0xfb,
    0x47, 0xfb, 0x89, 0xeb, 0x6e, 0xe4, 0x98, 0xfe, 0x1b, 0xff, 0x3c, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x68, 0xf3, 0xeb, 0xeb, 0x15, 0xfe, 0x1b,
    0xff, 0x3c, 0xf7, 0x5d, 0xe7, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xeb, 0x46, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x46, 0xfb, 0x48, 0xfb, 0xaa, 0xe3, 0x93, 0xf5, 0x1a, 0xff, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef,
    0x5d, 0xef, 0x5d, 0xef, 0x3d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x68, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x89, 0xeb, 0xcf, 0xf4, 0xb9, 0xfe, 0x1b, 0xf7, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x68,
    0xf3, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x89, 0xeb, 0x4d, 0xe4, 0x77, 0xfe, 0x1b, 0xff, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x26, 0xfb,
    0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x68, 0xf3, 0xcb, 0xe3, 0xf5, 0xfd, 0x1a, 0xff, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x46, 0xfb, 0x48, 0xfb, 0xaa,
    0xe3, 0x73, 0xf5, 0xfa, 0xfe, 0x3c, 0xf7, 0x3d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x27, 0xfb, 0x46, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x89, 0xe3, 0xb0, 0xe4, 0xda, 0xfe, 0x3d, 0xef, 0x1b, 0xff,
    0xb4, 0xf5, 0x69, 0xf3, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x46, 0xfb, 0x46, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x46, 0xfb, 0x47, 0xfb, 0x89, 0xe3, 0xb0, 0xe4, 0xda, 0xfe, 0x3d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xf3, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x46,
    0xfb, 0x46, 0xfb, 0x46, 0xfb, 0x46, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x68, 0xfb, 0xaa, 0xe3, 0x73, 0xed, 0xfa, 0xfe, 0x3c, 0xf7, 0x3d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xf3, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x26, 0xfb, 0x46, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x68, 0xf3,
    0xcb, 0xe3, 0xf5, 0xfd, 0x1b, 0xff, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xf3, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x46, 0xfb, 0x26, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x68, 0xeb, 0x2d, 0xe4, 0x77, 0xfe, 0x1b, 0xff, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x5d,
    0xef, 0xfb, 0xfe, 0xb4, 0xf5, 0x69, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x89, 0xeb, 0xcf, 0xf4, 0xd8, 0xfe, 0x1b, 0xff, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x68, 0xeb, 0x26, 0xfb, 0x47, 0xfb,
    0x47, 0xfb, 0x26, 0xfb, 0x47, 0xfb, 0xaa, 0xe3, 0x73, 0xfd, 0xfa, 0xfe, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x69, 0xeb, 0x26, 0xfb, 0x47, 0xfb, 0x47, 0xfb, 0x68, 0xf3, 0xeb, 0xeb, 0x15, 0xfe, 0xfb, 0xfe, 0x3c,
    0xef, 0x5d, 0xe7, 0x5d, 0xe7, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x68, 0xf3, 0x26, 0xfb, 0x47, 0xfb, 0x89, 0xeb, 0x6e, 0xe4, 0x98, 0xfe, 0x1b, 0xff, 0x3d, 0xef, 0x3d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef,
    0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xb4, 0xf5, 0x89, 0xeb, 0x47, 0xfb, 0x89, 0xe3, 0xf0, 0xf4, 0xd9, 0xfe, 0x1b, 0xff, 0x5d, 0xef, 0x5d, 0xe7, 0x3d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xd4, 0xf5, 0xaa, 0xe3, 0xaa,
    0xe3, 0xd4, 0xf5, 0x1b, 0xff, 0x3c, 0xf7, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x1b, 0xff, 0xd5, 0xe5, 0x4e, 0xd4, 0x35, 0xfe, 0x1b, 0xff, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef,
    0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x3c, 0xf7, 0x99, 0xe6, 0xda, 0xfe, 0x1b, 0xff, 0x3c, 0xef, 0x5d, 0xef, 0x3d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d,
    0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x3c, 0xf7, 0x3c, 0xef, 0x3c, 0xef, 0x5d, 0xef, 0x5d, 0xe7, 0x5d, 0xef, 0x3d, 0xef, 0x3d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x3c, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef, 0x5d, 0xef,
};

const lv_image_dsc_t arrow_state_on = {
    .header.cf = LV_COLOR_FORMAT_RGB565,
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.w = 18,
    .header.h = 26,
    .data_size = 468 * 2,
    .data = arrow_state_on_map,
};
