idf_component_register(SRCS "HalBase/HalBase.cpp" "HalTab5/HalTab5.cpp"
                    INCLUDE_DIRS "." "HalBase" "HalTab5"
                    REQUIRES esp_common esp_lvgl_port spiffs fatfs esp_driver_sdmmc
                    )

