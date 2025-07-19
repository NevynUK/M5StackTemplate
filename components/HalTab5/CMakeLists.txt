file(GLOB_RECURSE HAL_SRCS
    # ./*.c
    ./*.cc
    ./*.cpp
)

idf_component_register(
    SRCS ${HAL_SRCS}
    INCLUDE_DIRS "."
    REQUIRES 
        driver 
        esp_lcd
        esp_codec_dev
        esp_lvgl_port
	    esp_lcd_ili9881c
        esp_driver_i2c
        esp_driver_gpio
        HalBase
    PRIV_REQUIRES usb spiffs fatfs
)
