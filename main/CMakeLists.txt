# file(GLOB_RECURSE APP_LAYER_SRCS
#     assets/*.c
#     assets/*.cc
#     assets/*.cpp
# )

# file(GLOB_RECURSE MY_HAL_SRCS
#     ./hal/*.c
#     ./hal/*.cc
#     ./hal/*.cpp
# )

idf_component_register(SRCS "app_main.cpp" ${APP_LAYER_SRCS} ${MY_HAL_SRCS}
                    INCLUDE_DIRS "."
                    REQUIRES Utilities HalBase power_monitor_ina226 HalTab5 esp_lcd_touch sensor_bmi270 nvs_flash esp_common esp_wifi usb usb_host_hid
                             esp_cam_sensor esp_http_server esp_video esp_lvgl_port esp_common spi_flash esp_driver_ppa
                             spiffs fatfs driver esp_codec_dev esp_lcd_ili9881c
                    )
