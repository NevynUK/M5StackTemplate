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

idf_component_register(SRCS "app_main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES Utilities M5StackHAL power_monitor_ina226 rx8130 esp_lcd_touch sensor_bmi270 nvs_flash esp_common esp_wifi usb usb_host_hid
                             esp_cam_sensor esp_http_server esp_video esp_lvgl_port esp_common spi_flash esp_driver_ppa imlib
                             Display
                    EMBED_TXTFILES "../audio/canon_in_d.mp3" "../audio/startup_sfx.mp3" "../audio/shutdown_sfx.mp3")
