/**
 * @file Tab5Display.cpp
 * @author Mark Stevens
 * @brief Display methods for the Tab5 device.
 * @version 0.1
 * @date 2025-07-19
 * 
 * @copyright Copyright (c) 2025
 */
#include <sdkconfig.h>

#include <algorithm>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "driver/ledc.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "esp_lcd_ili9881c.h"

#include "HalTab5.hpp"

#include "ili9881_init_data.c"

using namespace HAL;

/**
 * @brief Get the Display Width object.
 * 
 * @return uint32_t Width of the display.
 */
uint32_t HalTab5::GetDisplayWidth()
{
    return BSP_LCD_H_RES;
}

/**
 * @brief Get the display height.
 *
 * @return uint32_t Display height in pixels.
 */
uint32_t HalTab5::GetDisplayHeight()
{
    return BSP_LCD_V_RES;
}

/**
 * @brief Initialise the brightness control.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::InitialiseBrightnessControl(void)
{
    const ledc_timer_config_t lcd_backlight_timer =
    {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&lcd_backlight_timer));

    const ledc_channel_config_t lcd_backlight_channel =
    {
        .gpio_num = BSP_LCD_BACKLIGHT,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_1,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&lcd_backlight_channel));

    return ESP_OK;
}

/**
 * @brief Set the display brightness
 *
 * @param brightness Brightness level from 0 to 100.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::SetDisplayBrightness(uint8_t brightness)
{
    _current_lcd_brightness = std::clamp(brightness, (uint8_t) 0, (uint8_t) 100);
    ESP_LOGI(COMPONENT_NAME, "Setting LCD backlight: %d%%", _current_lcd_brightness);
    // uint32_t duty_cycle = (1023 * _current_lcd_brightness) / 100; // LEDC resolution set to 10bits, thus: 100% = 1023
    uint32_t duty_cycle = (4095 * _current_lcd_brightness) / 100; // LEDC resolution set to 12bits, thus: 100% = 4095
    ESP_RETURN_ON_ERROR(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty_cycle), COMPONENT_NAME, "Failed to set LEDC duty");
    ESP_RETURN_ON_ERROR(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1), COMPONENT_NAME, "Failed to update LEDC duty");
    return ESP_OK;
}

/**
 * @brief Get the display brightness.
 *
 * @return uint8_t Brightness level from 0 to 100.
 */
uint8_t HalTab5::GetDisplayBrightness()
{
    return _current_lcd_brightness;
}

/**
 * @brief Get the IO handle for the display.
 * 
 * @return esp_lcd_panel_io_handle_t 
 */
esp_lcd_panel_io_handle_t HalTab5::GetIoHandle() const
{
    return _ioHandle;
}

/**
 * @brief Get the MIPI DSI bus handle for the display.
 * 
 * @return esp_lcd_dsi_bus_handle_t 
 */
esp_lcd_dsi_bus_handle_t HalTab5::GetMipiDsiBusHandle() const
{
    return _mipiDsiBusHandle;
}

/**
 * @brief Get the panel handle for the display.
 * 
 * @return esp_lcd_panel_handle_t 
 */
esp_lcd_panel_handle_t HalTab5::GetPanelHandle() const
{
    return _panelHandle;
}

/**
 * @brief Get the control handle for the display.
 * 
 * @return esp_lcd_panel_handle_t 
 */
esp_lcd_panel_handle_t HalTab5::GetControlHandle() const
{
    return _controlHandle;
}

/**
 * @brief Enable MIPI DSI PHY power
 * 
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::EnableDsiPhyPower(void)
{
    static esp_ldo_channel_handle_t phy_pwr_chan = NULL;
    esp_ldo_channel_config_t ldo_cfg =
    {
        .chan_id = BSP_MIPI_DSI_PHY_PWR_LDO_CHAN,
        .voltage_mv = BSP_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
    };
    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_cfg, &phy_pwr_chan), COMPONENT_NAME, "Acquire LDO channel for DPHY failed");
    ESP_LOGI(COMPONENT_NAME, "MIPI DSI PHY Powered on");

    return ESP_OK;
}

/**
 * @brief Configure the display.
 * 
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::ConfigureDisplay()
{
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t disp_panel = NULL;

    // ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), COMPONENT_NAME, "Brightness init failed");
    ESP_RETURN_ON_ERROR(EnableDsiPhyPower(), COMPONENT_NAME, "DSI PHY power failed");

    /* create MIPI DSI bus first, it will initialize the DSI PHY as well */
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_dsi_bus_config_t bus_config =
    {
        .bus_id = 0,
        .num_data_lanes = BSP_LCD_MIPI_DSI_LANE_NUM,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus), COMPONENT_NAME, "New DSI bus init failed");

    ESP_LOGI(COMPONENT_NAME, "Install MIPI DSI LCD control panel");
    // we use DBI interface to send LCD commands and parameters
    esp_lcd_dbi_io_config_t dbi_config =
    {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,   // according to the LCD spec
        .lcd_param_bits = 8, // according to the LCD spec
    };
    //
    //  TODO: Fix this.
    //
    // ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io), err, COMPONENT_NAME, "New panel IO failed");
    esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io);

    ESP_LOGI(COMPONENT_NAME, "Install LCD driver of ili9881c");
    esp_lcd_dpi_panel_config_t dpi_config = {};
    dpi_config.virtual_channel = 0;
    dpi_config.dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT;
    dpi_config.dpi_clock_freq_mhz = 60; // 720*1280 RGB24 60Hz RGB24 // 80
    dpi_config.pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565;
    dpi_config.num_fbs = 1;
    dpi_config.video_timing = {};
    dpi_config.video_timing.h_size = BSP_LCD_H_RES;
    dpi_config.video_timing.v_size = BSP_LCD_V_RES;
    dpi_config.video_timing.hsync_back_porch = 140;
    dpi_config.video_timing.hsync_pulse_width = 40;
    dpi_config.video_timing.hsync_front_porch = 40;
    dpi_config.video_timing.vsync_back_porch = 20;
    dpi_config.video_timing.vsync_pulse_width = 4;
    dpi_config.video_timing.vsync_front_porch = 20;
    dpi_config.flags = {};
    dpi_config.flags.use_dma2d = true;

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

    esp_lcd_panel_dev_config_t lcd_dev_config = {};
    lcd_dev_config.bits_per_pixel = 16;
    lcd_dev_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
    lcd_dev_config.reset_gpio_num = -1;
    lcd_dev_config.vendor_config = &vendor_config;

    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(io, &lcd_dev_config, &disp_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(disp_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(disp_panel));
    //
    //  TODO: Fix this.
    //
    //  ESP_ERROR_CHECK(esp_lcd_panel_mirror(disp_panel, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(disp_panel, true));

    _ioHandle = io;
    _mipiDsiBusHandle = mipi_dsi_bus;
    _panelHandle = disp_panel;
    _controlHandle = NULL;

    ESP_LOGI(COMPONENT_NAME, "Display initialized with resolution %dx%d", BSP_LCD_H_RES, BSP_LCD_V_RES);

    return ESP_OK;
}
