
idf_component_register(
    SRCS "hal_esp32.cpp"
    INCLUDE_DIRS "." "include" "include/bsp"
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
