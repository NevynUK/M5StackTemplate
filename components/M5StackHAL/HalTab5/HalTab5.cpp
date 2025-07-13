/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include <sdkconfig.h>

#include <memory>
#include <string>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#include "driver/ledc.h"
#include "esp_ldo_regulator.h"
#include "esp_lcd_ili9881c.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"

#include "driver/i2c_master.h"

#include "HalTab5.h"
#include "ILI9881InitialisationData.h"

using namespace HAL;

/* -------------------------------------------------------------------------- */
/*                            Static Data Members                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Static pointer used to ensure that only one instance of HalTab5 exists.
 */
HalTab5 *HalTab5::_instance = nullptr;

/**
 * @brief Mount point for the SD card.
 */
// const std::string HalTab5::MOUNT_POINT = "/sdcard";

// #define CONFIG_BSP_ERROR_CHECK 1
// #define CONFIG_BSP_I2C_NUM 1
// #define CONFIG_BSP_I2C_CLK_SPEED_HZ 100000
// #define CONFIG_BSP_I2S_NUM 1
// #define CONFIG_BSP_SPIFFS_MOUNT_POINT "/spiffs"
// #define CONFIG_BSP_SPIFFS_PARTITION_LABEL "storage"
// #define CONFIG_BSP_SPIFFS_MAX_FILES 5
// #define CONFIG_BSP_LCD_DPI_BUFFER_NUMS 1
// #define CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH 1
// #define CONFIG_BSP_LCD_COLOR_FORMAT_RGB565 1

#define BSP_LDO_PROBE_SD_CHAN 4
// #define BSP_LDO_PROBE_SD_VOLCOMPONENT_NAMEE_MV 3300

// extern const ili9881c_lcd_init_cmd_t tab5_lcd_ili9881c_specific_init_code_default[];

/* -------------------------------------------------------------------------- */
/*                         Constructors / Destructor                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Default constructor for this class.
 *
 * This constructor is private to enforce the singleton pattern.
 */
HalTab5::HalTab5()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 instance created");
}

/**
 * @brief Delete the default destructor for this class as we should not need it.
 *
 * This destructor is private to enforce the singleton pattern.
 */
HalTab5::~HalTab5()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 instance destroyed");
}

/**
 * @brief Get the singleton instance of HalTab5.
 *
 * If the instance does not exist, it will create a new one.
 *
 * @return HalTab5* Pointer to the singleton instance of HalTab5.
 */
HalTab5 *HalTab5::GetInstance()
{
    if (!_instance)
    {
        _instance = new HalTab5();
    }
    return _instance;
}

/* -------------------------------------------------------------------------- */
/*                                Methods                                     */
/* -------------------------------------------------------------------------- */

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

/* -------------------------------------------------------------------------- */
/*                            SD Card Methods                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the SD card interface.
 *
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::Mount(std::string mountPoint, size_t maximumFiles)
{
    esp_err_t result = ESP_OK;

    if (_sdCard)
    {
        return ESP_ERR_INVALID_STATE;
    }

    /**
     * @brief Use settings defined above to initialize SD card and mount FAT filesystem.
     *   Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
     *   Please check its source code and implement error recovery when developing
     *   production applications.
     */
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = BSP_LDO_PROBE_SD_CHAN, // `LDO_VO4` is used as the SDMMC IO power
    };

    if (!_sdCardPowerControlHandle)
    {
        result = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &_sdCardPowerControlHandle);
        if (result != ESP_OK)
        {
            ESP_LOGE(COMPONENT_NAME, "Failed to new an on-chip ldo power control driver");
            return result;
        }
    }
    host.pwr_ctrl_handle = _sdCardPowerControlHandle;

    /**
     * @brief This initializes the slot without card detect (CD) and write protect (WP) signals.
     *   Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
     */
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = SDMMC_BUS_WIDTH;
    slot_config.clk = GPIO_SDMMC_CLK;
    slot_config.cmd = GPIO_SDMMC_CMD;
    slot_config.d0 = GPIO_SDMMC_D0;
    slot_config.d1 = GPIO_SDMMC_D1;
    slot_config.d2 = GPIO_SDMMC_D2;
    slot_config.d3 = GPIO_SDMMC_D3;

    /**
     * @brief Options for mounting the filesystem.
     *   If format_if_mount_failed is set to true, SD card will be partitioned and
     *   formatted in case when mounting fails.
     */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = (int) maximumFiles,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = true,
        .use_one_fat = false,
    };
    result = esp_vfs_fat_sdmmc_mount(mountPoint.c_str(), &host, &slot_config, &mount_config, &_sdCard);

    if (result != ESP_OK)
    {
        ESP_LOGE(COMPONENT_NAME, "Failed to initialize the card (%s).  Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(result));
        return result;
    }

    return result;
}

/**
 * @brief Unmount the SD card.
 *
 * @param mountPoint The mount point of the SD card.
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::Unmount(std::string mountPoint)
{
    ESP_LOGI(COMPONENT_NAME, "Unmounting SD card");
    esp_err_t result = esp_vfs_fat_sdcard_unmount(mountPoint.c_str(), _sdCard);
    _sdCard = nullptr;

    return result;
}

/**
 * @brief Check if the SD card is mounted.
 *
 * @return true If the SD card is mounted.
 * @return false If the SD card is not mounted.
 */
bool HalTab5::IsSdCardMounted()
{
    ESP_LOGI(COMPONENT_NAME, "Checking if SD card is mounted");
    return _sdCard != nullptr;
}

/* -------------------------------------------------------------------------- */
/*                             Display Methods                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the display.
 *
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::ConfigureDisplay()
{
    ESP_LOGI(COMPONENT_NAME, "Configuring LCD Panel");
    ConfigureDisplayBrightnessControl();
    SetDisplayBrightness(100);
    ConfigureDisplayPower();
    ConfigureLCDPanel();

    return ESP_OK;
}

/**
 * @brief Get the display width in pixels.
 * 
 * @return int Display width in pixels.
 */
inline int HalTab5::GetDisplayWidth()
{
    return 1280;
}

/**
 * @brief Get the display height in pixels.
 * 
 * @return int Display height in pixels.
 */
inline int HalTab5::GetDisplayHeight()
{
    return 720;
}

/**
 * @brief Get the panel IO handle.
 */
esp_lcd_panel_io_handle_t HalTab5::GetPanelIoHandle() const
{
    return _panelIOHandle;
}

/**
 * @brief Get the display panel handle.
 */
esp_lcd_panel_handle_t HalTab5::GetDisplayPanelHandle() const
{
    return _displayPanelHandle;
}

/**
 * @brief Get the MIPI DSI bus handle.
 */
esp_lcd_dsi_bus_handle_t HalTab5::GetMipiDsiBusHandle() const
{
    return _mipiDSIBusHandle;
}

/**
 * @brief Configure the display power.
 *
 * This function configures the power for the MIPI DSI PHY, allowing it to transition from "No Power" state to "Shutdown" state.
 *
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::ConfigureDisplayPower()
{
    ESP_LOGI(COMPONENT_NAME, "Configuring display power");
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    static esp_ldo_channel_handle_t phy_pwr_chan = NULL;
    esp_ldo_channel_config_t ldo_cfg = {};
    ldo_cfg.chan_id = 3;
    ldo_cfg.voltage_mv = 2500;
    ESP_RETURN_ON_ERROR(esp_ldo_acquire_channel(&ldo_cfg, &phy_pwr_chan), COMPONENT_NAME, "Acquire LDO channel for DPHY failed");
    ESP_LOGI(COMPONENT_NAME, "MIPI DSI PHY Powered on");

    return ESP_OK;
}

/**
 * @brief Configure the LCD panel.
 *
 * This function sets up the LCD panel for the M5Stack Tab5.
 *
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::ConfigureLCDPanel()
{
    ESP_LOGI(COMPONENT_NAME, "Configuring LCD panel");

    ConfigureI2C();
    ConfigureIoExpanders();
    vTaskDelay(pdMS_TO_TICKS(100)); // Wait for the panel to stabilize
    //
    //  Create MIPI DSI bus first, it will initialize the DSI PHY as well.
    //
    esp_lcd_dsi_bus_config_t mipiDsiBusConfig = {};
    mipiDsiBusConfig.bus_id = 0;
    mipiDsiBusConfig.num_data_lanes = BSP_LCD_MIPI_DSI_LANE_NUM;
    mipiDsiBusConfig.phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT;
    mipiDsiBusConfig.lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS;
    ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&mipiDsiBusConfig, &_mipiDSIBusHandle), COMPONENT_NAME, "New DSI bus init failed");
    //
    //  DBI interface is used to send LCD commands and data.
    //
    esp_lcd_dbi_io_config_t dbiConfig = {};
    dbiConfig.virtual_channel = 0;
    dbiConfig.lcd_cmd_bits = 8;        // According to the LCD spec
    dbiConfig.lcd_param_bits = 8;      // According to the LCD spec
    if (esp_lcd_new_panel_io_dbi(_mipiDSIBusHandle, &dbiConfig, &_panelIOHandle) != ESP_OK)
    {
        ESP_LOGE(COMPONENT_NAME, "New panel IO failed");
        if (_mipiDSIBusHandle)
        {
            esp_lcd_del_dsi_bus(_mipiDSIBusHandle);
        }
    }
    //
    // Install ili9881c LCD driver.
    //
    esp_lcd_dpi_panel_config_t dpiConfig = {};
    dpiConfig.virtual_channel = 0;
    dpiConfig.dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT;
    dpiConfig.dpi_clock_freq_mhz = 60;
    dpiConfig.pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565;
    dpiConfig.num_fbs = 1;
    dpiConfig.video_timing = {};
    dpiConfig.video_timing.h_size = GetDisplayWidth();
    dpiConfig.video_timing.v_size = GetDisplayHeight();
    dpiConfig.video_timing.hsync_back_porch = 140;
    dpiConfig.video_timing.hsync_pulse_width = 40;
    dpiConfig.video_timing.hsync_front_porch = 40;
    dpiConfig.video_timing.vsync_back_porch = 20;
    dpiConfig.video_timing.vsync_pulse_width = 4;
    dpiConfig.video_timing.vsync_front_porch = 20;
    dpiConfig.flags.use_dma2d = true;

    ili9881c_vendor_config_t vendorConfig = {};
    vendorConfig.init_cmds = tab5_lcd_ili9881c_specific_init_code_default;
    vendorConfig.init_cmds_size = sizeof(tab5_lcd_ili9881c_specific_init_code_default) / sizeof(ili9881c_lcd_init_cmd_t);
    vendorConfig.mipi_config = {};
    vendorConfig.mipi_config.dsi_bus = _mipiDSIBusHandle;
    vendorConfig.mipi_config.dpi_config = &dpiConfig;
    vendorConfig.mipi_config.lane_num = 2;

    esp_lcd_panel_dev_config_t lcdDeviceConfig = {};
    lcdDeviceConfig.bits_per_pixel = 16;
    lcdDeviceConfig.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
    lcdDeviceConfig.reset_gpio_num = -1;
    lcdDeviceConfig.vendor_config = &vendorConfig;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_ili9881c(_panelIOHandle, &lcdDeviceConfig, &_displayPanelHandle), COMPONENT_NAME, "New panel init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_reset(_displayPanelHandle), COMPONENT_NAME, "Panel reset failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_init(_displayPanelHandle), COMPONENT_NAME, "Panel init failed");
    ESP_RETURN_ON_ERROR(esp_lcd_panel_disp_on_off(_displayPanelHandle, true), COMPONENT_NAME, "Panel display on failed");

    ESP_LOGI(COMPONENT_NAME, "LCD panel configured successfully");

    return ESP_OK;
}

/**
 * @brief Configure the display brightness control interface.
 */
esp_err_t HalTab5::ConfigureDisplayBrightnessControl()
{
    ESP_LOGI(COMPONENT_NAME, "Configuring display brightness control");
    ledc_timer_config_t lcd_backlight_timer = {};
    lcd_backlight_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    lcd_backlight_timer.duty_resolution = LEDC_TIMER_12_BIT;
    lcd_backlight_timer.timer_num = LEDC_TIMER_0;
    lcd_backlight_timer.freq_hz = 5000;
    lcd_backlight_timer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&lcd_backlight_timer));

    ledc_channel_config_t lcd_backlight_channel = {};
    lcd_backlight_channel.gpio_num = GPIO_LCD_BACKLIGHT;
    lcd_backlight_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    lcd_backlight_channel.channel = LEDC_CHANNEL_1;
    lcd_backlight_channel.intr_type = LEDC_INTR_DISABLE;
    lcd_backlight_channel.timer_sel = LEDC_TIMER_0;
    lcd_backlight_channel.duty = 0;
    lcd_backlight_channel.hpoint = 0;
    ESP_ERROR_CHECK(ledc_channel_config(&lcd_backlight_channel));

    return ESP_OK;
}

/**
 * @brief Set the display brightness.
 *
 * @param brightness Brightness level (0-100).
 */
esp_err_t HalTab5::SetDisplayBrightness(uint8_t brightnessPercent)
{
    ESP_LOGI(COMPONENT_NAME, "Setting display brightness to %d%%", brightnessPercent);
    if (brightnessPercent > 100)
    {
        brightnessPercent = 100;
    }

    ESP_LOGI(COMPONENT_NAME, "Setting LCD backlight: %d%%", brightnessPercent);
    uint32_t duty_cycle = (4095 * brightnessPercent) / 100;
    esp_err_t result = ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, duty_cycle);
    ESP_RETURN_ON_ERROR(result, COMPONENT_NAME, "Failed to set LEDC duty cycle");
    result = ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ESP_RETURN_ON_ERROR(result, COMPONENT_NAME, "Failed to update LEDC duty cycle");
    _displayBrightness = brightnessPercent;

    return result;
}

/**
 * @brief Get the display brightness.
 *
 * @return uint8_t Display brightness level (0-100).
 */
uint8_t HalTab5::GetDisplayBrightness()
{
    return _displayBrightness;
}

//     bsp_reset_tp();
//     bsp_display_cfg_t cfg = {.lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
//                              .buffer_size   = BSP_LCD_H_RES * BSP_LCD_V_RES,
//                              .double_buffer = true,
//                              .flags         = {
// #if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
//                                  .buff_dma = false,
// #else
//                                  .buff_dma = true,
// #endif
//                                  .buff_spiram = true,
//                                  .sw_rotate   = true,
//                              }};
//     lvDisp = bsp_display_start_with_config(&cfg);
//     lv_display_set_rotation(lvDisp, LV_DISPLAY_ROTATION_90);
//     bsp_display_backlight_on();

//     // Touchpad lvgl indev
//     mclog::tagInfo(_tag, "create lvgl touchpad indev");
//     lvTouchpad = lv_indev_create();
//     lv_indev_set_type(lvTouchpad, LV_INDEV_TYPE_POINTER);
//     lv_indev_set_read_cb(lvTouchpad, lvgl_read_cb);
//     lv_indev_set_display(lvTouchpad, lvDisp);

// lv_display_t* bsp_display_start_with_config(const bsp_display_cfg_t* cfg)
// {
//     lv_display_t* disp;

//     assert(cfg != NULL);
//     BSP_ERROR_CHECK_RETURN_NULL(lvgl_port_init(&cfg->lvgl_port_cfg));

//     BSP_NULL_CHECK(disp = bsp_display_lcd_init(cfg), NULL);
//     BSP_NULL_CHECK(disp_indev = bsp_display_indev_init(disp), NULL);
//     return disp;
// }

// #if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
// static lv_display_t* bsp_display_lcd_init(const bsp_display_cfg_t* cfg)
// {
//     assert(cfg != NULL);
//     bsp_lcd_handles_t lcd_panels;
//     BSP_ERROR_CHECK_RETURN_NULL(bsp_display_new_with_handles(NULL, &lcd_panels));

//     /* Add LCD screen */
//     ESP_LOGD(TAG, "Add LCD screen");
//     const lvgl_port_display_cfg_t disp_cfg =
//     {.io_handle      = lcd_panels.io,
//      .panel_handle   = lcd_panels.panel,
//      .control_handle = lcd_panels.control,
//      .buffer_size    = cfg->buffer_size,
//      .double_buffer  = cfg->double_buffer,
//      .hres           = BSP_LCD_H_RES,
//      .vres           = BSP_LCD_V_RES,
//      .monochrome     = false,
//      /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
//      .rotation =
//          {
//              .swap_xy  = false,
//              .mirror_x = false,
//              .mirror_y = false,
//          },
// #if LVGL_VERSION_MAJOR >= 9
// #if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
//      .color_format = LV_COLOR_FORMAT_RGB888,
// #else
//      .color_format = LV_COLOR_FORMAT_RGB565,
// #endif
// #endif
//      .flags = {
//          .buff_dma    = cfg->flags.buff_dma,
//          .buff_spiram = cfg->flags.buff_spiram,
// #if LVGL_VERSION_MAJOR >= 9
//          .swap_bytes = (BSP_LCD_BIGENDIAN ? true : false),
// #endif
// #if CONFIG_BSP_DISPLAY_LVGL_AVOID_TEAR
//          .sw_rotate = false, /* Avoid tearing is not supported for SW rotation */
// #else
//          .sw_rotate   = cfg->flags.sw_rotate, /* Only SW rotation is supported for 90° and 270° */
// #endif
// #if CONFIG_BSP_DISPLAY_LVGL_FULL_REFRESH
//          .full_refresh = true,
// #elif CONFIG_BSP_DISPLAY_LVGL_DIRECT_MODE
//          .direct_mode = true,
// #endif
//      } };

//     const lvgl_port_display_dsi_cfg_t dpi_cfg = {.flags = {
// #if CONFIG_BSP_DISPLAY_LVGL_AVOID_TEAR
//                                                      .avoid_tearing = true,
// #else
//                                                      .avoid_tearing = false,
// #endif
//                                                  }};

//     return lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);
// }


