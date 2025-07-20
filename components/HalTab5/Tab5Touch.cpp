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

static const char *COMPONENT_NAME = "HalTab5";

static bool i2c_initialized = false;
static i2c_master_bus_handle_t i2c_handle = NULL;

void bsp_fake_i2c_init(i2c_master_bus_handle_t bus_handle)
{
    i2c_handle = bus_handle;
    i2c_initialized = true;
}

void bsp_reset_tp()
{
    // ESP_LOGI(COMPONENT_NAME, "reset tp");

    // ESP_LOGI(COMPONENT_NAME, "reset gpio %d", GPIO_NUM_23);
    // gpio_reset_pin(GPIO_NUM_23);

    // uint8_t write_buf[2] = {0};
    // uint8_t read_buf[1] = {0};

    // write_buf[0] = PI4IO_REG_OUT_SET;
    // i2c_master_transmit_receive(i2c_dev_handle_pi4ioe1, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS);

    // write_buf[0] = PI4IO_REG_OUT_SET;
    // write_buf[1] = read_buf[0];
    // clrbit(write_buf[1], 4);
    // clrbit(write_buf[1], 5);
    // i2c_master_transmit(i2c_dev_handle_pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    // vTaskDelay(100 / portTICK_PERIOD_MS);

    // write_buf[0] = PI4IO_REG_OUT_SET;
    // write_buf[1] = read_buf[0];
    // setbit(write_buf[1], 4);
    // setbit(write_buf[1], 5);
    // i2c_master_transmit(i2c_dev_handle_pi4ioe1, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    // vTaskDelay(100 / portTICK_PERIOD_MS);
}

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
    /* Initilize I2C */
    // BSP_ERROR_CHECK_RETURN_ERR(bsp_i2c_init());

    /* Initialize touch */
    const esp_lcd_touch_config_t tp_cfg =
    {
        // .x_max = BSP_LCD_H_RES,
        // .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC, // -1, // BSP_LCD_TOUCH_RST, // NC
        .int_gpio_num = GPIO_NUM_23, // 23, // BSP_LCD_TOUCH_INT,
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
    tp_io_config.dev_addr = ESP_LCD_TOUCH_IO_I2C_GT911_ADDRESS_BACKUP; // Change GT911 address
    tp_io_config.scl_speed_hz = CONFIG_BSP_I2C_CLK_SPEED_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), COMPONENT_NAME, "");
    return esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, ret_touch);
}

static esp_lcd_touch_handle_t _touch_handle;

esp_lcd_touch_handle_t bsp_display_get_touch_handle(void)
{
    return _touch_handle;
}

esp_lcd_touch_handle_t _lcd_touch_handle;

static lv_indev_t *bsp_display_indev_init(lv_display_t *disp)
{
    esp_lcd_touch_handle_t tp;
    BSP_ERROR_CHECK_RETURN_NULL(bsp_touch_new(NULL, &tp));
    esp_lcd_touch_exit_sleep(tp); // !!!
    assert(tp);
    _lcd_touch_handle = tp;

    /* Add touch input (for selected screen) */
    const lvgl_port_touch_cfg_t touch_cfg =
    {
        .disp = disp,
        .handle = tp,
    };

    return lvgl_port_add_touch(&touch_cfg);
}

// lv_indev_t *bsp_display_get_input_dev(void)
// {
//     return disp_indev;
// }

// extern esp_lcd_touch_handle_t _lcd_touch_handle;

// static void lvgl_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
// {
//     if (_lcd_touch_handle == NULL)
//     {
//         data->state = LV_INDEV_STATE_REL;
//         return;
//     }

//     uint16_t touch_x[1];
//     uint16_t touch_y[1];
//     uint16_t touch_strength[1];
//     uint8_t touch_cnt = 0;

//     esp_lcd_touch_read_data(_lcd_touch_handle);
//     bool touchpad_pressed = esp_lcd_touch_get_coordinates(_lcd_touch_handle, touch_x, touch_y, touch_strength, &touch_cnt, 1);

//     if (!touchpad_pressed)
//     {
//         data->state = LV_INDEV_STATE_REL;
//     }
//     else
//     {
//         data->state = LV_INDEV_STATE_PR;
//         data->point.x = touch_x[0];
//         data->point.y = touch_y[0];
//     }
// }
