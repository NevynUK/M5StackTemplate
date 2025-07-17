/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_spiffs.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "esp_vfs_fat.h"
#include "usb/usb_host.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "freertos/task.h"
#include "sdmmc_cmd.h"
#include "esp_lcd_st7703.h"
#include "esp_lcd_ili9881c.h"
#include "bsp/m5stack_tab5.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "esp_lcd_touch_gt911.h"
#include "bsp_err_check.h"
#include "esp_codec_dev_defaults.h"

static const char *TAG = "M5STACK_TAB5";

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
static lv_indev_t *disp_indev = NULL;
#endif // (BSP_CONFIG_NO_GRAPHIC_LIB == 0)

// sys i2c
static bool i2c_initialized = false;
static i2c_master_bus_handle_t i2c_handle = NULL;

void bsp_fake_i2c_init(i2c_master_bus_handle_t bus_handle)
{
    i2c_handle = bus_handle;
    i2c_initialized = true;
}

//==================================================================================
// i2c
//==================================================================================
esp_err_t bsp_i2c_init(void)
{
    /* I2C was initialized before */
    if (i2c_initialized)
    {
        return ESP_OK;
    }

    i2c_master_bus_config_t i2c_bus_conf =
    {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .i2c_port = BSP_I2C_NUM,
        .flags.enable_internal_pullup = true,
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&i2c_bus_conf, &i2c_handle));

    i2c_initialized = true;

    return ESP_OK;
}

esp_err_t bsp_i2c_deinit(void)
{
    BSP_ERROR_CHECK_RETURN_ERR(i2c_del_master_bus(i2c_handle));
    i2c_initialized = false;
    return ESP_OK;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    return i2c_handle;
}


void bsp_reset_tp()
{
    // ESP_LOGI(TAG, "reset tp");

    // ESP_LOGI(TAG, "reset gpio %d", GPIO_NUM_23);
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

//==================================================================================
// lcd st7703 1280x720  gt911
//==================================================================================
// Bit number used to represent command and parameter
#define LCD_LEDC_CH LEDC_CHANNEL_1 // CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH

esp_err_t bsp_display_brightness_init(void)
{
    // gpio_config_t io_conf = {};

    // io_conf.intr_type = GPIO_INTR_DISABLE;   //disable interrupt
    // io_conf.mode = GPIO_MODE_OUTPUT;         //set as output mode
    // io_conf.pin_bit_mask = 1 << BSP_LCD_BACKLIGHT; //select pin
    // io_conf.pull_down_en = 0;                //disable pull-down mode
    // io_conf.pull_up_en = 0;                  //disable pull-up mode
    // gpio_config(&io_conf);                   //configure GPIO with the given settings

    // gpio_set_level(BSP_LCD_BACKLIGHT, 1);

    // Setup LEDC peripheral for PWM backlight control
    const ledc_timer_config_t lcd_backlight_timer =
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        //  .duty_resolution = LEDC_TIMER_10_BIT,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        // .freq_hz = 20000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&lcd_backlight_timer));

    const ledc_channel_config_t lcd_backlight_channel =
    {
        .gpio_num = BSP_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LCD_LEDC_CH,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&lcd_backlight_channel));

    return ESP_OK;
}

esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100)
    {
        brightness_percent = 100;
    }
    if (brightness_percent < 0)
    {
        brightness_percent = 0;
    }

    ESP_LOGI(TAG, "Setting LCD backlight: %d%%", brightness_percent);
    // uint32_t duty_cycle = (1023 * brightness_percent) / 100; // LEDC resolution set to 10bits, thus: 100% = 1023
    uint32_t duty_cycle = (4095 * brightness_percent) / 100; // LEDC resolution set to 12bits, thus: 100% = 4095
    BSP_ERROR_CHECK_RETURN_ERR(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH, duty_cycle));
    BSP_ERROR_CHECK_RETURN_ERR(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_LEDC_CH));
    return ESP_OK;
}

esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(100);
}

static esp_err_t bsp_enable_dsi_phy_power(void)
{
#if BSP_MIPI_DSI_PHY_PWR_LDO_CHAN > 0
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    static esp_ldo_channel_handle_t phy_pwr_chan = NULL;
    esp_ldo_channel_config_t ldo_cfg =
    {
        .chan_id = BSP_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = BSP_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_cfg, &phy_pwr_chan), TAG, "Acquire LDO channel for DPHY failed");
    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");
#endif // BSP_MIPI_DSI_PHY_PWR_LDO_CHAN > 0

    return ESP_OK;
}

esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
    esp_err_t ret = ESP_OK;
    bsp_lcd_handles_t handles;
    ret = bsp_display_new_with_handles(config, &handles);

    *ret_panel = handles.panel;
    *ret_io = handles.io;

    return ret;
}

#include "ili9881_init_data.c"

esp_err_t bsp_display_new_with_handles(const bsp_display_config_t *config, bsp_lcd_handles_t *ret_handles)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t disp_panel = NULL;

    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Brightness init failed");
    ESP_RETURN_ON_ERROR(bsp_enable_dsi_phy_power(), TAG, "DSI PHY power failed");

    /* create MIPI DSI bus first, it will initialize the DSI PHY as well */
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_dsi_bus_config_t bus_config =
    {
        .bus_id = 0,
        .num_data_lanes = BSP_LCD_MIPI_DSI_LANE_NUM,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus), TAG, "New DSI bus init failed");

    ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
    // we use DBI interface to send LCD commands and parameters
    esp_lcd_dbi_io_config_t dbi_config =
    {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,   // according to the LCD spec
        .lcd_param_bits = 8, // according to the LCD spec
    };
    ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io), err, TAG, "New panel IO failed");

    ESP_LOGI(TAG, "Install LCD driver of ili9881c");
    esp_lcd_dpi_panel_config_t dpi_config =
    {
        .virtual_channel = 0,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = 60, // 720*1280 RGB24 60Hz RGB24 // 80,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .num_fbs = 1,
        .video_timing =
        {
            .h_size = BSP_LCD_H_RES,
            .v_size = BSP_LCD_V_RES,
            .hsync_back_porch = 140,
            .hsync_pulse_width = 40,
            .hsync_front_porch = 40,
            .vsync_back_porch = 20,
            .vsync_pulse_width = 4,
            .vsync_front_porch = 20,
        },
        .flags.use_dma2d = true,
    };

    ili9881c_vendor_config_t vendor_config =
    {
        .init_cmds = tab5_lcd_ili9881c_specific_init_code_default,
        .init_cmds_size = sizeof(tab5_lcd_ili9881c_specific_init_code_default) / sizeof(tab5_lcd_ili9881c_specific_init_code_default[0]),
        .mipi_config =
        {
            .dsi_bus = mipi_dsi_bus,
            .dpi_config = &dpi_config,
            .lane_num = 2,
        },
    };

    const esp_lcd_panel_dev_config_t lcd_dev_config =
    {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = -1,
        .vendor_config = &vendor_config,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(io, &lcd_dev_config, &disp_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(disp_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(disp_panel));
    //  ESP_ERROR_CHECK(esp_lcd_panel_mirror(disp_panel, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(disp_panel, true));

    /* Return all handles */
    ret_handles->io = io;
    ret_handles->mipi_dsi_bus = mipi_dsi_bus;
    ret_handles->panel = disp_panel;
    ret_handles->control = NULL;

    ESP_LOGI(TAG, "Display initialized with resolution %dx%d", BSP_LCD_H_RES, BSP_LCD_V_RES);

    return ret;

err:
    if (disp_panel)
    {
        esp_lcd_panel_del(disp_panel);
    }
    if (io)
    {
        esp_lcd_panel_io_del(io);
    }
    if (mipi_dsi_bus)
    {
        esp_lcd_del_dsi_bus(mipi_dsi_bus);
    }
    return ret;
}

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
    /* Initilize I2C */
    BSP_ERROR_CHECK_RETURN_ERR(bsp_i2c_init());

    /* Initialize touch */
    const esp_lcd_touch_config_t tp_cfg =
    {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = -1, // BSP_LCD_TOUCH_RST, // NC
        .int_gpio_num = 23, // BSP_LCD_TOUCH_INT,
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
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), TAG, "");
    return esp_lcd_touch_new_i2c_gt911(tp_io_handle, &tp_cfg, ret_touch);
}

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
static lv_display_t *bsp_display_lcd_init(const bsp_display_cfg_t *cfg)
{
    assert(cfg != NULL);
    bsp_lcd_handles_t lcd_panels;
    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_new_with_handles(NULL, &lcd_panels));

    /* Add LCD screen */
    ESP_LOGD(TAG, "Add LCD screen");
    const lvgl_port_display_cfg_t disp_cfg =
    {
        .io_handle = lcd_panels.io,
        .panel_handle = lcd_panels.panel,
        .control_handle = lcd_panels.control,
        .buffer_size = cfg->buffer_size,
        .double_buffer = cfg->double_buffer,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
        .rotation =
        {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
    #if LVGL_VERSION_MAJOR >= 9
        #if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
        .color_format = LV_COLOR_FORMAT_RGB888,
        #else
        .color_format = LV_COLOR_FORMAT_RGB565,
        #endif
    #endif
        .flags =
        {
            .buff_dma = cfg->flags.buff_dma,
            .buff_spiram = cfg->flags.buff_spiram,
    #if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = (BSP_LCD_BIGENDIAN ? true : false),
    #endif
    #if CONFIG_BSP_DISPLAY_LVGL_AVOID_TEAR
            .sw_rotate = false, /* Avoid tearing is not supported for SW rotation */
    #else
            .sw_rotate = cfg->flags.sw_rotate, /* Only SW rotation is supported for 90° and 270° */
    #endif
    #if CONFIG_BSP_DISPLAY_LVGL_FULL_REFRESH
            .full_refresh = true,
    #elif CONFIG_BSP_DISPLAY_LVGL_DIRECT_MODE
            .direct_mode = true,
    #endif
        }
    };

    const lvgl_port_display_dsi_cfg_t dpi_cfg =
    {
        .flags =
        {
    #if CONFIG_BSP_DISPLAY_LVGL_AVOID_TEAR
            .avoid_tearing = true,
    #else
            .avoid_tearing = false,
    #endif
        }
    };

    return lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);
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

lv_display_t *bsp_display_start(void)
{
    bsp_display_cfg_t cfg =
    {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_DRAW_BUFF_SIZE,
        .double_buffer = BSP_LCD_DRAW_BUFF_DOUBLE,
        .flags = 
        {
    #if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
            .buff_dma = false,
    #else
            .buff_dma = true,
    #endif
            .buff_spiram = false,
            .sw_rotate = true,
        }
    };
    return bsp_display_start_with_config(&cfg);
}

lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg)
{
    lv_display_t *disp;

    assert(cfg != NULL);
    BSP_ERROR_CHECK_RETURN_NULL(lvgl_port_init(&cfg->lvgl_port_cfg));

    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_brightness_init());

    BSP_NULL_CHECK(disp = bsp_display_lcd_init(cfg), NULL);
    BSP_NULL_CHECK(disp_indev = bsp_display_indev_init(disp), NULL);
    return disp;
}

lv_indev_t *bsp_display_get_input_dev(void)
{
    return disp_indev;
}

void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation)
{
    lv_disp_set_rotation(disp, rotation);
}

bool bsp_display_lock(uint32_t timeout_ms)
{
    return lvgl_port_lock(timeout_ms);
}

void bsp_display_unlock(void)
{
    lvgl_port_unlock();
}
#endif // (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
