/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP BSP: ESP32-P4 Function EV Board
 */

#pragma once

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/sdmmc_host.h"
#include "driver/i2s_std.h"
#include "driver/i2s_tdm.h"
#include "bsp/config.h"
#include "bsp/display.h"
#include "esp_codec_dev.h"
#include "sdkconfig.h"

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
    #include "lvgl.h"
    #include "esp_lvgl_port.h"
#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0

/**************************************************************************************************
 *  BSP Capabilities
 **************************************************************************************************/
#define BSP_CAPS_DISPLAY 1
#define BSP_CAPS_TOUCH 1
#define BSP_CAPS_BUTTONS 0
#define BSP_CAPS_AUDIO 1
#define BSP_CAPS_AUDIO_SPEAKER 1
#define BSP_CAPS_AUDIO_MIC 1
#define BSP_CAPS_SDCARD 1
#define BSP_CAPS_IMU 0

/**************************************************************************************************
 *  ESP-BOX pinout
 **************************************************************************************************/
/* SYS I2C */
#define BSP_I2C_NUM 0
#define BSP_I2C_SCL (GPIO_NUM_32)
#define BSP_I2C_SDA (GPIO_NUM_31)

/* EXT I2C */
#define BSP_EXT_I2C_NUM 1
#define BSP_EXT_I2C_SCL (GPIO_NUM_54)
#define BSP_EXT_I2C_SDA (GPIO_NUM_53)

// /* Ext Keyboard */
// #define TAB5_TCA8418_INT_PIN 50 // Interrupt input

/* Audio */
#define BSP_I2S_SCLK (GPIO_NUM_27)     // Bit clock      BSP_I2S_BCLK  <--> ES7210/ESP311 I2S_BCLK
#define BSP_I2S_MCLK (GPIO_NUM_30)     // Master clock   BSP_I2S_MCLK  <--> ES7210/ESP311 I2S_MCLK
#define BSP_I2S_LCLK (GPIO_NUM_29)     // Word select    BSP_I2S_WR    <--> ES7210/ESP311 I2S_WR
#define BSP_I2S_DOUT (GPIO_NUM_26)     // Data output    BSP_I2S_DOUT  ---> ES8388        I2S_DSIN
#define BSP_I2S_DSIN (GPIO_NUM_28)     // Data input     BSP_I2S_DIN   <--- ES7210        I2S_DOUT
#define BSP_POWER_AMP_IO (GPIO_NUM_NC) // (GPIO_NUM_53)

/* Display */
#define BSP_LCD_BACKLIGHT (GPIO_NUM_22)
#define BSP_LCD_RST (GPIO_NUM_NC)       //
#define BSP_LCD_TOUCH_RST (GPIO_NUM_NC) // IO Expander control
#define BSP_LCD_TOUCH_INT (GPIO_NUM_NC) // 23

/* uSD card */
#define BSP_SD_D0 (GPIO_NUM_39)
#define BSP_SD_D1 (GPIO_NUM_40)
#define BSP_SD_D2 (GPIO_NUM_41)
#define BSP_SD_D3 (GPIO_NUM_42)
#define BSP_SD_CMD (GPIO_NUM_44)
#define BSP_SD_CLK (GPIO_NUM_43)

#ifdef __cplusplus
extern "C"
{
#endif

    /**************************************************************************************************
     *
     * I2C interface
     *
     * There are multiple devices connected to I2C peripheral:
     *  - Codec ES8311 (configuration only)
     *  - LCD Touch controller
     **************************************************************************************************/

    /**
     * @brief Init I2C driver
     *
     * @return
     *      - ESP_OK                On success
     *      - ESP_ERR_INVALID_ARG   I2C parameter error
     *      - ESP_FAIL              I2C driver installation error
     *
     */
    esp_err_t bsp_i2c_init(void);

    /**
     * @brief Deinit I2C driver and free its resources
     *
     * @return
     *      - ESP_OK                On success
     *      - ESP_ERR_INVALID_ARG   I2C parameter error
     *
     */
    esp_err_t bsp_i2c_deinit(void);

    /**
     * @brief Get I2C driver handle
     *
     * @return
     *      - I2C handle
     *
     */
    i2c_master_bus_handle_t bsp_i2c_get_handle(void);

    esp_err_t bsp_i2c_scan();

    esp_err_t bsp_ext_i2c_init(void);
    esp_err_t bsp_ext_i2c_deinit(void);
    i2c_master_bus_handle_t bsp_ext_i2c_get_handle(void);

    esp_err_t bsp_grove_i2c_init(void);
    esp_err_t bsp_grove_i2c_deinit(void);
    i2c_master_bus_handle_t bsp_grove_i2c_get_handle(void);

    /**************************************************************************************************
     *
     * I2S audio interface
     *
     * There are two devices connected to the I2S peripheral:
     *  - Codec ES8311 for output(playback) and input(recording) path
     *
     * For speaker initialization use bsp_audio_codec_speaker_init() which is inside initialize I2S with bsp_audio_init().
     * For microphone initialization use bsp_audio_codec_microphone_init() which is inside initialize I2S with
     *bsp_audio_init(). After speaker or microphone initialization, use functions from esp_codec_dev for play/record audio.
     * Example audio play:
     * \code{.c}
     * esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
     * esp_codec_dev_open(spk_codec_dev, &fs);
     * esp_codec_dev_write(spk_codec_dev, wav_bytes, bytes_read_from_spiffs);
     * esp_codec_dev_close(spk_codec_dev);
     * \endcode
     **************************************************************************************************/

    
/**************************************************************************************************
 *
 * LCD interface
 *
 * ESP-BOX is shipped with 2.4inch ST7789 display controller.
 * It features 16-bit colors, 320x240 resolution and capacitive touch controller.
 *
 * LVGL is used as graphics library. LVGL is NOT thread safe, therefore the user must take LVGL mutex
 * by calling bsp_display_lock() before calling and LVGL API (lv_...) and then give the mutex with
 * bsp_display_unlock().
 *
 * Display's backlight must be enabled explicitly by calling bsp_display_backlight_on()
 **************************************************************************************************/
#define BSP_LCD_PIXEL_CLOCK_MHZ (80)

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)

    #define BSP_LCD_DRAW_BUFF_SIZE (BSP_LCD_H_RES * 50) // Frame buffer size in pixels
    #define BSP_LCD_DRAW_BUFF_DOUBLE (0)

    /**
     * @brief BSP display configuration structure
     *
     */
    typedef struct
    {
        lvgl_port_cfg_t lvgl_port_cfg; /*!< LVGL port configuration */
        uint32_t buffer_size;          /*!< Size of the buffer for the screen in pixels */
        bool double_buffer;            /*!< True, if should be allocated two buffers */

        struct
        {
            unsigned int buff_dma:1;    /*!< Allocated LVGL buffer will be DMA capable */
            unsigned int buff_spiram:1; /*!< Allocated LVGL buffer will be in PSRAM */
            unsigned int sw_rotate:1;   /*!< Use software rotation (slower), The feature is unavailable under avoid-tear mode */
        } flags;
    } bsp_display_cfg_t;

    /**
     * @brief Initialize display
     *
     * This function initializes SPI, display controller and starts LVGL handling task.
     * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
     *
     * @return Pointer to LVGL display or NULL when error occured
     */
    lv_display_t *bsp_display_start(void);

    /**
     * @brief Initialize display
     *
     * This function initializes SPI, display controller and starts LVGL handling task.
     * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
     *
     * @param cfg display configuration
     *
     * @return Pointer to LVGL display or NULL when error occured
     */
    lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg);

    /**
     * @brief Get pointer to input device (touch, buttons, ...)
     *
     * @note The LVGL input device is initialized in bsp_display_start() function.
     *
     * @return Pointer to LVGL input device or NULL when not initialized
     */
    lv_indev_t *bsp_display_get_input_dev(void);

    /**
     * @brief Take LVGL mutex
     *
     * @param timeout_ms Timeout in [ms]. 0 will block indefinitely.
     * @return true  Mutex was taken
     * @return false Mutex was NOT taken
     */
    bool bsp_display_lock(uint32_t timeout_ms);

    /**
     * @brief Give LVGL mutex
     *
     */
    void bsp_display_unlock(void);

    /**
     * @brief Rotate screen
     *
     * Display must be already initialized by calling bsp_display_start()
     *
     * @param[in] disp Pointer to LVGL display
     * @param[in] rotation Angle of the display rotation
     */
    void bsp_display_rotate(lv_display_t *disp, lv_disp_rotation_t rotation);
#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0

    void bsp_io_expander_pi4ioe_init(i2c_master_bus_handle_t bus_handle);

    void bsp_set_charge_qc_en(bool en);

    void bsp_set_charge_en(bool en);

    void bsp_set_usb_5v_en(bool en);

    void bsp_set_ext_5v_en(bool en);

    void bsp_generate_poweroff_signal();

    bool bsp_headphone_detect();

    void bsp_set_ext_antenna_enable(bool en);

    void bsp_set_wifi_power_enable(bool en);

    void bsp_reset_tp();

    bool bsp_usb_c_detect();

    bool bsp_usb_a_detect();

    /**************************************************************************************************
     *
     * USB
     *
     **************************************************************************************************/

    /**
     * @brief Power modes of USB Host connector
     */
    typedef enum bsp_usb_host_power_mode_t {
        BSP_USB_HOST_POWER_MODE_USB_DEV, //!< Power from USB DEV port
    } bsp_usb_host_power_mode_t;

    /**
     * @brief Start USB host
     *
     * This is a one-stop-shop function that will configure the board for USB Host mode
     * and start USB Host library
     *
     * @param[in] mode        USB Host connector power mode (Not used on this board)
     * @param[in] limit_500mA Limit output current to 500mA (Not used on this board)
     * @return
     *     - ESP_OK                 On success
     *     - ESP_ERR_INVALID_ARG    Parameter error
     *     - ESP_ERR_NO_MEM         Memory cannot be allocated
     */
    esp_err_t bsp_usb_host_start(bsp_usb_host_power_mode_t mode, bool limit_500mA);

    /**
     * @brief Stop USB host
     *
     * USB Host lib will be uninstalled and power from connector removed.
     *
     * @return
     *     - ESP_OK              On success
     *     - ESP_ERR_INVALID_ARG Parameter error
     */
    esp_err_t bsp_usb_host_stop(void);

#ifdef __cplusplus
}
#endif
