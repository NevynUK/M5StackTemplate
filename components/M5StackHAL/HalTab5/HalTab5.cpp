/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include <sdkconfig.h>

#include "HalTab5.h"
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

#include "HalTab5.h"

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

#define CONFIG_BSP_ERROR_CHECK 1
#define CONFIG_BSP_I2C_NUM 1
#define CONFIG_BSP_I2C_CLK_SPEED_HZ 100000
#define CONFIG_BSP_I2S_NUM 1
#define CONFIG_BSP_SPIFFS_MOUNT_POINT "/spiffs"
#define CONFIG_BSP_SPIFFS_PARTITION_LABEL "storage"
#define CONFIG_BSP_SPIFFS_MAX_FILES 5
#define CONFIG_BSP_LCD_DPI_BUFFER_NUMS 1
#define CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH 1
#define CONFIG_BSP_LCD_COLOR_FORMAT_RGB565 1

#define BSP_LDO_PROBE_SD_CHAN 4
#define BSP_LDO_PROBE_SD_VOLCOMPONENT_NAMEE_MV 3300

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

/**
 * @brief Initialize the HalTab5 instance.
 */
void HalTab5::init()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 initialized");
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
    sd_pwr_ctrl_ldo_config_t ldo_config =
    {
        .ldo_chan_id = BSP_LDO_PROBE_SD_CHAN,   // `LDO_VO4` is used as the SDMMC IO power
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
    esp_vfs_fat_sdmmc_mount_config_t mount_config =
    {
        .format_if_mount_failed = false,
        .max_files = (int) maximumFiles,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = true,
        .use_one_fat = false,
    };
    result = esp_vfs_fat_sdmmc_mount(mountPoint.c_str(), &host, &slot_config, &mount_config, &_sdCard);

    if (result != ESP_OK)
    {
        ESP_LOGE(COMPONENT_NAME, "Failed to initialize the card (%s).  Make sure SD card lines have pull-up resistors in place.",  esp_err_to_name(result));
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
    ESP_LOGI(COMPONENT_NAME, "Configuring display");
    ConfigureDisplayBrightnessControl();
    SetDisplayBrightness(100);

    return ESP_OK;
}

/**
 * @brief Configure the display brightness control interface.
 */
esp_err_t HalTab5::ConfigureDisplayBrightnessControl()
{
    ledc_timer_config_t lcd_backlight_timer = {};
    lcd_backlight_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    lcd_backlight_timer.duty_resolution = LEDC_TIMER_12_BIT;
    lcd_backlight_timer.timer_num = LEDC_TIMER_0;
    lcd_backlight_timer.freq_hz = 5000;
    lcd_backlight_timer.clk_cfg = LEDC_AUTO_CLK;
    ESP_ERROR_CHECK(ledc_timer_config(&lcd_backlight_timer));

    ledc_channel_config_t lcd_backlight_channel = {};
    lcd_backlight_channel.gpio_num   = GPIO_LCD_BACKLIGHT;
    lcd_backlight_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    lcd_backlight_channel.channel    = LEDC_CHANNEL_1;
    lcd_backlight_channel.intr_type  = LEDC_INTR_DISABLE;
    lcd_backlight_channel.timer_sel  = LEDC_TIMER_0;
    lcd_backlight_channel.duty       = 0;
    lcd_backlight_channel.hpoint     = 0;
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
    return 0;
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

//     BSP_ERROR_CHECK_RETURN_NULL(bsp_display_brightness_init());

//     BSP_NULL_CHECK(disp = bsp_display_lcd_init(cfg), NULL);
//     BSP_NULL_CHECK(disp_indev = bsp_display_indev_init(disp), NULL);
//     return disp;
// }


// esp_err_t bsp_display_backlight_off(void)
// {
//     return bsp_display_brightness_set(0);
// }

// esp_err_t bsp_display_backlight_on(void)
// {
//     return bsp_display_brightness_set(100);
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

// esp_err_t bsp_display_new_with_handles(const bsp_display_config_t* config, bsp_lcd_handles_t* ret_handles)
// {
//     esp_err_t ret                     = ESP_OK;
//     esp_lcd_panel_io_handle_t io      = NULL;
//     esp_lcd_panel_handle_t disp_panel = NULL;

//     ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Brightness init failed");
//     ESP_RETURN_ON_ERROR(bsp_enable_dsi_phy_power(), TAG, "DSI PHY power failed");

//     /* create MIPI DSI bus first, it will initialize the DSI PHY as well */
//     esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
//     esp_lcd_dsi_bus_config_t bus_config   = {
//         .bus_id             = 0,
//         .num_data_lanes     = BSP_LCD_MIPI_DSI_LANE_NUM,
//         .phy_clk_src        = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
//         .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
//     };
//     ESP_RETURN_ON_ERROR(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus), TAG, "New DSI bus init failed");

//     ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
//     // we use DBI interface to send LCD commands and parameters
//     esp_lcd_dbi_io_config_t dbi_config = {
//         .virtual_channel = 0,
//         .lcd_cmd_bits    = 8,  // according to the LCD spec
//         .lcd_param_bits  = 8,  // according to the LCD spec
//     };
//     ESP_GOTO_ON_ERROR(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &io), err, TAG, "New panel IO failed");

// #if defined(LCD_MIPI_DSI_USE_ILI9881C) && !defined(LCD_MIPI_DSI_USE_ST7703)
//     ESP_LOGI(TAG, "Install LCD driver of ili9881c");
//     esp_lcd_dpi_panel_config_t dpi_config = {
//         .virtual_channel    = 0,
//         .dpi_clk_src        = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
//         .dpi_clock_freq_mhz = 60,  // 720*1280 RGB24 60Hz RGB24 // 80,
//         .pixel_format       = LCD_COLOR_PIXEL_FORMAT_RGB565,
//         .num_fbs            = 1,
//         .video_timing =
//             {
//                 .h_size            = BSP_LCD_H_RES,
//                 .v_size            = BSP_LCD_V_RES,
//                 .hsync_back_porch  = 140,
//                 .hsync_pulse_width = 40,
//                 .hsync_front_porch = 40,
//                 .vsync_back_porch  = 20,
//                 .vsync_pulse_width = 4,
//                 .vsync_front_porch = 20,
//             },
//         .flags.use_dma2d = true,
//     };

//     ili9881c_vendor_config_t vendor_config = {
//         .init_cmds      = tab5_lcd_ili9881c_specific_init_code_default,
//         .init_cmds_size = sizeof(tab5_lcd_ili9881c_specific_init_code_default) /
//                           sizeof(tab5_lcd_ili9881c_specific_init_code_default[0]),
//         .mipi_config =
//             {
//                 .dsi_bus    = mipi_dsi_bus,
//                 .dpi_config = &dpi_config,
//                 .lane_num   = 2,
//             },
//     };

//     const esp_lcd_panel_dev_config_t lcd_dev_config = {
//         .bits_per_pixel = 16,
//         .rgb_ele_order  = LCD_RGB_ELEMENT_ORDER_RGB,
//         .reset_gpio_num = -1,
//         .vendor_config  = &vendor_config,
//     };
//     ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(io, &lcd_dev_config, &disp_panel));
//     ESP_ERROR_CHECK(esp_lcd_panel_reset(disp_panel));
//     ESP_ERROR_CHECK(esp_lcd_panel_init(disp_panel));
//     //  ESP_ERROR_CHECK(esp_lcd_panel_mirror(disp_panel, false, true));
//     ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(disp_panel, true));

// #endif

//     /* Return all handles */
//     ret_handles->io           = io;
//     ret_handles->mipi_dsi_bus = mipi_dsi_bus;
//     ret_handles->panel        = disp_panel;
//     ret_handles->control      = NULL;

//     ESP_LOGI(TAG, "Display initialized with resolution %dx%d", BSP_LCD_H_RES, BSP_LCD_V_RES);

//     return ret;

// err:
//     if (disp_panel) {
//         esp_lcd_panel_del(disp_panel);
//     }
//     if (io) {
//         esp_lcd_panel_io_del(io);
//     }
//     if (mipi_dsi_bus) {
//         esp_lcd_del_dsi_bus(mipi_dsi_bus);
//     }
//     return ret;
// }
