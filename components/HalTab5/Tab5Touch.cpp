/**
 * @file Tab5Touch.cpp
 * @author Mark Stevens
 * @brief Touch screen methods for Tab5
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */

#include <sdkconfig.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_lcd_touch_gt911.h"

#include "HalTab5.hpp"

using namespace HAL;

#define setbit(x, y) x |= (0x01 << y)
#define clrbit(x, y) x &= ~(0x01 << y)

/**
 * @brief Get the touch panel handle.
 *
 * @return esp_lcd_touch_handle_t Handle to the touch panel.
 */
esp_lcd_touch_handle_t HalTab5::GetTouchPanelHandle() const
{
    return _touchPanelHandle;
}

/**
 * @brief Configure the touch panel.
 */
esp_err_t HalTab5::ConfigureTouchPanel()
{
    ConfigureI2C();
    ResetTouchPanel();
    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = (uint16_t) (BSP_LCD_H_RES & 0xFFFF),
        .y_max = (uint16_t) (BSP_LCD_V_RES & 0xFFFF),
        .rst_gpio_num = BSP_LCD_TOUCH_RST,
        .int_gpio_num = BSP_LCD_TOUCH_INT,
        .levels =
            {
                .reset = 0,
                .interrupt = 0,
            },
        .flags =
            {
                .swap_xy = 0,
                .mirror_x = 0,
                .mirror_y = 0,
            },
    };
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    tp_io_config.dev_addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS_BACKUP;
    tp_io_config.scl_speed_hz = CONFIG_BSP_I2C_CLK_SPEED_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(_i2cHandle, &tp_io_config, &tp_io_handle), COMPONENT_NAME, "Creating I2C IO for touch panel failed");
    ESP_RETURN_ON_ERROR(esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, &_touchPanelHandle), COMPONENT_NAME, "Create GT911 touch panel failed");
    return (esp_lcd_touch_exit_sleep(_touchPanelHandle));
}

/**
 * @brief Reset the touch panel.
 */
void HalTab5::ResetTouchPanel()
{
    ESP_LOGI(COMPONENT_NAME, "Resetting touch panel");
    gpio_reset_pin(GPIO_NUM_23);

    uint8_t write_buf[2] = {0};
    uint8_t read_buf[1] = {0};
    write_buf[0] = PI4IO_REG_OUT_SET;
    i2c_master_transmit_receive(_pi4ioe1Handle, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS);

    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = read_buf[0];

    clrbit(write_buf[1], 4);
    clrbit(write_buf[1], 5);
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = read_buf[0];
    setbit(write_buf[1], 4);
    setbit(write_buf[1], 5);
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
