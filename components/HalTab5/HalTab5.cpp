/**
 * @file HalTab5.cpp
 * @author Mark Stevens
 * @brief 
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */
#include "HalTab5.hpp"

#include <esp_timer.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "driver/ledc.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

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
#include "esp_lcd_ili9881c.h"
#include "esp_lcd_touch_gt911.h"
#include "esp_codec_dev_defaults.h"


using namespace HAL;

static const char *COMPONENT_NAME = "HalTab5";

static lv_indev_t *disp_indev = NULL;

#define BSP_ERROR_CHECK_RETURN_ERR(x) ESP_ERROR_CHECK(x)
#define BSP_ERROR_CHECK_RETURN_NULL(x) ESP_ERROR_CHECK(x)
#define BSP_ERROR_CHECK(x, ret) ESP_ERROR_CHECK(x)
#define BSP_NULL_CHECK(x, ret) assert(x)
#define BSP_NULL_CHECK_GOTO(x, goto_tag) assert(x)

// sys i2c
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

//==================================================================================
// lcd st7703 1280x720  gt911
//==================================================================================
// Bit number used to represent command and parameter

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
    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_cfg, &phy_pwr_chan), COMPONENT_NAME, "Acquire LDO channel for DPHY failed");
    ESP_LOGI(COMPONENT_NAME, "MIPI DSI PHY Powered on");
#endif // BSP_MIPI_DSI_PHY_PWR_LDO_CHAN > 0

    return ESP_OK;
}

#include "ili9881_init_data.c"

esp_err_t HalTab5::NewDisplay(const bsp_display_config_t *config, bsp_lcd_handles_t *ret_handles)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_panel_io_handle_t io = NULL;
    esp_lcd_panel_handle_t disp_panel = NULL;

    // ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), COMPONENT_NAME, "Brightness init failed");
    ESP_RETURN_ON_ERROR(bsp_enable_dsi_phy_power(), COMPONENT_NAME, "DSI PHY power failed");

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
    _controlHandle = NULL; // No control panel for this display
    /* Return all handles */
    // ret_handles->io = io;
    // ret_handles->mipi_dsi_bus = mipi_dsi_bus;
    // ret_handles->panel = disp_panel;
    // ret_handles->control = NULL;

    ESP_LOGI(COMPONENT_NAME, "Display initialized with resolution %dx%d", BSP_LCD_H_RES, BSP_LCD_V_RES);

    return ret;

    //
    //  TODO: Cleanup code.
    //
    // if (disp_panel)
    // {
    //     esp_lcd_panel_del(disp_panel);
    // }
    // if (io)
    // {
    //     esp_lcd_panel_io_del(io);
    // }
    // if (mipi_dsi_bus)
    // {
    //     esp_lcd_del_dsi_bus(mipi_dsi_bus);
    // }
    // return ret;
}

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
    /* Initilize I2C */
    // BSP_ERROR_CHECK_RETURN_ERR(bsp_i2c_init());

    /* Initialize touch */
    const esp_lcd_touch_config_t tp_cfg =
    {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
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

lv_indev_t *bsp_display_get_input_dev(void)
{
    return disp_indev;
}

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




/* -------------------------------------------------------------------------- */
/*                                    I2C                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the I2C bus.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::ConfigureI2C()
{
    if (_i2cHandle)
    {
        return ESP_OK;
    }

    i2c_master_bus_config_t i2c_bus_conf = {};
    i2c_bus_conf.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_conf.sda_io_num = MASTER_I2C_SDA_GPIO;
    i2c_bus_conf.scl_io_num = MASTER_I2C_SCL_GPIO;
    i2c_bus_conf.i2c_port = MASTER_I2C_PORT_NUMBER;
    i2c_bus_conf.flags.enable_internal_pullup = true;
    esp_err_t result = i2c_new_master_bus(&i2c_bus_conf, &_i2cHandle);
    if (result != ESP_OK)
    {
        ESP_LOGE(COMPONENT_NAME, "Failed to create I2C master bus");
        _i2cHandle = nullptr;
    }

    bsp_fake_i2c_init(_i2cHandle);
    return result;
}

/* -------------------------------------------------------------------------- */
/*                          IO Expander Methods                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the IO Expanders.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::ConfigureIoExpanders()
{
    ConfigureI2C();

    uint8_t write_buf[2] = {};
    uint8_t read_buf[1] = {};

    i2c_device_config_t deviceConfig = {};
    deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    deviceConfig.device_address = I2C_DEV_ADDR_PI4IOE1;
    deviceConfig.scl_speed_hz = 400000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(_i2cHandle, &deviceConfig, &_pi4ioe1Handle));

    write_buf[0] = PI4IO_REG_CHIP_RESET;
    write_buf[1] = 0xff;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    write_buf[0] = PI4IO_REG_CHIP_RESET;
    i2c_master_transmit_receive(_pi4ioe1Handle, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the port direction (0 = input, 1 = output).
    //
    write_buf[0] = PI4IO_REG_IO_DIR;
    write_buf[1] = 0b01111111;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Disable High-Impedance for used pins.
    //
    write_buf[0] = PI4IO_REG_OUT_H_IM;
    write_buf[1] = 0b00000000;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the pull-up/pull-down resistors for the pins (0 = pull down, 1 = pull up).
    //
    write_buf[0] = PI4IO_REG_PULL_SEL;
    write_buf[1] = 0b01111111;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set pull up/down enable ability (0 = enable, 1 = disable).
    //  Port 7 enabled, all others disabled.
    //
    write_buf[0] = PI4IO_REG_PULL_EN;
    write_buf[1] = 0b01111111;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the output ports high.
    //
    //  P1(SPK_EN), P2(EXT5V_EN), P4(LCD_RST), P5(TP_RST), P6(CAM)RST
    //
    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = 0b01110110;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Now for the second IO expander.
    //
    deviceConfig.device_address = I2C_DEV_ADDR_PI4IOE2;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(_i2cHandle, &deviceConfig, &_pi4ioe2Handle));

    write_buf[0] = PI4IO_REG_CHIP_RESET;
    write_buf[1] = 0xFF;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    write_buf[0] = PI4IO_REG_CHIP_RESET;
    i2c_master_transmit_receive(_pi4ioe2Handle, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the port direction (0 = input, 1 = output).
    //
    write_buf[0] = PI4IO_REG_IO_DIR;
    write_buf[1] = 0b10111001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Disable High-Impedance for used pins.
    //
    write_buf[0] = PI4IO_REG_OUT_H_IM;
    write_buf[1] = 0b00000110;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the pull-up/pull-down resistors for the pins (0 = pull down, 1 = pull up).
    //
    write_buf[0] = PI4IO_REG_PULL_SEL;
    write_buf[1] = 0b10111001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set pull up/down enable ability (0 = enable, 1 = disable).
    //
    write_buf[0] = PI4IO_REG_PULL_EN;
    write_buf[1] = 0b11111001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the input default state (0 = low, 1 = high).
    //
    write_buf[0] = PI4IO_REG_IN_DEF_STA;
    write_buf[1] = 0b01000000;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS); // P6 default high level
    //
    //  Set interrupt ability (0 = enable, 1 = disable).
    //  Port 6 enabled, all others disabled.
    //
    write_buf[0] = PI4IO_REG_INT_MASK;
    write_buf[1] = 0b10111111;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the output ports high.
    //
    //  P0(WLAN_PWR_EN), P3(USB5V_EN)
    //
    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = 0b00001001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);

    return ESP_OK;
}

/**
 * @brief Configure and initialise the hardware.
 */
void HalTab5::Configure()
{
    ConfigureI2C();

    ConfigureIoExpanders();

    // bsp_reset_tp();
    bsp_display_cfg_t cfg =
    {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * BSP_LCD_V_RES,
        .double_buffer = true,
        .flags = 
        {
            .buff_dma = true,
            .buff_spiram = true,
            .sw_rotate = true,
        }
    };
    InitialiseBrightnessControl();
    lvDisp = ConfigureDisplay(&cfg);
    lv_display_set_rotation(lvDisp, LV_DISPLAY_ROTATION_90);
    SetDisplayBrightness(100);

    // Touchpad lvgl indev
    // lvTouchpad = lv_indev_create();
    // lv_indev_set_type(lvTouchpad, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(lvTouchpad, lvgl_read_cb);
    // lv_indev_set_display(lvTouchpad, lvDisp);

    lvgl_port_unlock();
}

static const gpio_num_t _driver_gpios[] = {
    // EXT I2C
    GPIO_NUM_0,
    GPIO_NUM_1,
    // esp-hosted esp32c6
    GPIO_NUM_8,
    GPIO_NUM_9,
    GPIO_NUM_10,
    GPIO_NUM_11,
    GPIO_NUM_12,
    GPIO_NUM_13,
    GPIO_NUM_15,
    // Display
    GPIO_NUM_22,
    GPIO_NUM_23,
    // Audio
    GPIO_NUM_26,
    GPIO_NUM_27,
    GPIO_NUM_28,
    GPIO_NUM_29,
    GPIO_NUM_30,
    // SYS I2C
    GPIO_NUM_31,
    GPIO_NUM_32,
    // uSD card
    GPIO_NUM_39,
    GPIO_NUM_40,
    GPIO_NUM_41,
    GPIO_NUM_42,
    GPIO_NUM_43,
    GPIO_NUM_44,
};

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

lv_display_t *HalTab5::ConfigureDisplay(const bsp_display_cfg_t *cfg)
{
    lv_display_t *disp;

    assert(cfg != NULL);
    // BSP_ERROR_CHECK_RETURN_NULL(lvgl_port_init(&cfg->lvgl_port_cfg));

    bsp_lcd_handles_t lcd_panels;
    BSP_ERROR_CHECK_RETURN_NULL(NewDisplay(NULL, &lcd_panels));

    /* Add LCD screen */
    ESP_LOGD(COMPONENT_NAME, "Add LCD screen");
    lvgl_port_display_cfg_t disp_cfg = {};
    disp_cfg.io_handle = _ioHandle;
    disp_cfg.panel_handle = _panelHandle;
    disp_cfg.control_handle = _controlHandle;
    disp_cfg.buffer_size = cfg->buffer_size;
    disp_cfg.double_buffer = cfg->double_buffer;
    disp_cfg.hres = BSP_LCD_H_RES;
    disp_cfg.vres = BSP_LCD_V_RES;
    disp_cfg.monochrome = false;
    disp_cfg.rotation = {};
    disp_cfg.rotation.swap_xy = false;
    disp_cfg.rotation.mirror_x = false;
    disp_cfg.rotation.mirror_y = false;
    disp_cfg.color_format = LV_COLOR_FORMAT_RGB565;
    disp_cfg.flags = {};
    disp_cfg.flags.buff_dma = cfg->flags.buff_dma;
    disp_cfg.flags.buff_spiram = cfg->flags.buff_spiram;
    disp_cfg.flags.swap_bytes = (BSP_LCD_BIGENDIAN ? true : false);
    disp_cfg.flags.sw_rotate = cfg->flags.sw_rotate; /* Only SW rotation is supported for 90° and 270° */

    lvgl_port_display_dsi_cfg_t dpi_cfg = {};
    dpi_cfg.flags.avoid_tearing = false;

    disp = lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);

    BSP_NULL_CHECK(disp_indev = bsp_display_indev_init(disp), NULL);
    return disp;
}

void HalTab5::gpioInitOutput(uint8_t pin)
{
    gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t) pin, GPIO_MODE_OUTPUT);
}

void HalTab5::gpioSetLevel(uint8_t pin, bool level)
{
    gpio_set_level((gpio_num_t) pin, level);
}

void HalTab5::gpioReset(uint8_t pin)
{
    gpio_set_level((gpio_num_t) pin, false);
}

