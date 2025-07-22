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
                    REQUIRES Utilities HalBase HalTab5 Display Exceptions
                             power_monitor_ina226 sensor_bmi270
                             esp_lcd_touch 
                             nvs_flash esp_common esp_wifi usb usb_host_hid spiffs fatfs driver spi_flash 
                             esp_cam_sensor esp_http_server esp_video esp_lvgl_port esp_common esp_driver_ppa
                             esp_codec_dev esp_lcd_ili9881c
                    )
