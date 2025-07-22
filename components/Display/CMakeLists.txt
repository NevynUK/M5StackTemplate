idf_component_register(SRCS "Display.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES HalBase HalTab5 Exceptions lvgl esp_lvgl_port esp_lcd_touch
                    )
