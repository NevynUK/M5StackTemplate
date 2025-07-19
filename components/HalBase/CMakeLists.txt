file(GLOB_RECURSE HAL_SRCS
    ./*.c
    ./*.cc
    ./*.cpp
)

idf_component_register(SRCS ${HAL_SRCS}
                    INCLUDE_DIRS "."
                    REQUIRES esp_common esp_lvgl_port Exceptions)

