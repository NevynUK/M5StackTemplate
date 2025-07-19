/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c_master.h"

#include <HalBase.hpp>
#include <lvgl.h>
#include "esp_lvgl_port.h"

#include "esp_lcd_types.h"
#include "esp_lcd_mipi_dsi.h"
#include "sdkconfig.h"

#include "esp_lcd_touch.h"


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

/* Display */
/* LCD color formats */
#define ESP_LCD_COLOR_FORMAT_RGB565 (1)
#define ESP_LCD_COLOR_FORMAT_RGB888 (2)

/* LCD display color format */
// #if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
// #define BSP_LCD_COLOR_FORMAT        (ESP_LCD_COLOR_FORMAT_RGB888)
// #else
#define BSP_LCD_COLOR_FORMAT (ESP_LCD_COLOR_FORMAT_RGB565)
// #endif
/* LCD display color bytes endianess */
#define BSP_LCD_BIGENDIAN (0)
/* LCD display color bits */
#define BSP_LCD_BITS_PER_PIXEL (16)
/* LCD display color space */
#define BSP_LCD_COLOR_SPACE (ESP_LCD_COLOR_SPACE_RGB)

/* LCD display definition 720x1280 */
#define BSP_LCD_H_RES (720)
#define BSP_LCD_V_RES (1280)

#define BSP_LCD_MIPI_DSI_LCD_HSYNC (10)
#define BSP_LCD_MIPI_DSI_LCD_HBP (40)
#define BSP_LCD_MIPI_DSI_LCD_HFP (40)
#define BSP_LCD_MIPI_DSI_LCD_VSYNC (4)
#define BSP_LCD_MIPI_DSI_LCD_VBP (16)
#define BSP_LCD_MIPI_DSI_LCD_VFP (16)

#define BSP_LCD_MIPI_DSI_LANE_NUM (2)            // 2 data lanes
#define BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS (730) // 720*1280 RGB24 60Hz //(900) // 900Mbps

#define BSP_MIPI_DSI_PHY_PWR_LDO_CHAN (3)        // LDO_VO3 is connected to VDD_MIPI_DPHY
#define BSP_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV (2500)


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief BSP display configuration structure
     *
     */
    typedef struct
    {
        int dummy;
    } bsp_display_config_t;

    /**
     * @brief BSP display return handles
     *
     */
    typedef struct
    {
        esp_lcd_dsi_bus_handle_t mipi_dsi_bus; /*!< MIPI DSI bus handle */
        esp_lcd_panel_io_handle_t io;          /*!< ESP LCD IO handle */
        esp_lcd_panel_handle_t panel;          /*!< ESP LCD panel (color) handle */
        esp_lcd_panel_handle_t control;        /*!< ESP LCD panel (control) handle */
    } bsp_lcd_handles_t;

    /**
     * @brief Create new display panel
     *
     * For maximum flexibility, this function performs only reset and initialization of the display.
     * You must turn on the display explicitly by calling esp_lcd_panel_disp_on_off().
     * The display's backlight is not turned on either. You can use bsp_display_backlight_on/off(),
     * bsp_display_brightness_set() (on supported boards) or implement your own backlight control.
     *
     * If you want to free resources allocated by this function, you can use esp_lcd API, ie.:
     *
     * \code{.c}
     * esp_lcd_panel_del(panel);
     * esp_lcd_panel_del(control);
     * esp_lcd_panel_io_del(io);
     * esp_lcd_del_dsi_bus(mipi_dsi_bus);
     * \endcode
     *
     * @param[in]  config    display configuration
     * @param[out] ret_handles all esp_lcd handles in one structure
     * @return
     *      - ESP_OK         On success
     *      - Else           esp_lcd failure
     */
    esp_err_t bsp_display_new_with_handles(const bsp_display_config_t *config, bsp_lcd_handles_t *ret_handles);

    /**
     * @brief BSP touch configuration structure
     */
    typedef struct
    {
        void *dummy; /*!< Prepared for future use. */
    } bsp_touch_config_t;

    /**
     * @brief Create new touchscreen
     *
     * If you want to free resources allocated by this function, you can use esp_lcd_touch API, ie.:
     *
     * \code{.c}
     * esp_lcd_touch_del(tp);
     * \endcode
     *
     * @param[in]  config    touch configuration
     * @param[out] ret_touch esp_lcd_touch touchscreen handle
     * @return
     *      - ESP_OK         On success
     *      - Else           esp_lcd_touch failure
     */
    esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch);

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

    void bsp_reset_tp();

#ifdef __cplusplus
}
#endif

/**
 * @brief Namespace for the Hardware Abstraction Layer (HAL).
 */
namespace HAL
{
    /**
     * @brief Implementation of the Hardware Abstraction Layer for the M5Stack Tab5 device.
     */
    class HalTab5 : public HalBase
    {
    public:
        /**
         * @brief Name of this class for logging purposes.
         */
        const char *COMPONENT_NAME = "HalTab5";

        /**
         * @brief Configure and initialise the hardware.
         */
        void Configure() override;

        /* -------------------------------------------------------------------------- */
        /*                                    I2C                                     */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the I2C bus.
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        esp_err_t ConfigureI2C() override;

        /* -------------------------------------------------------------------------- */
        /*                               IO Expanders                                 */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the IO Expanders.
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        esp_err_t ConfigureIoExpanders();

        /* -------------------------------------------------------------------------- */
        /*                            Display Methods                                 */
        /* -------------------------------------------------------------------------- */

        lv_disp_t *lvDisp = nullptr;
        lv_indev_t *lvKeyboard = nullptr;

        /**
         * @brief Initialise the brightness control.
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        esp_err_t InitialiseBrightnessControl(void) override;
                /**
         * @brief Set the display brightness.
         *
         * @param brightness Brightness level from 0 to 100.
         */
        esp_err_t SetDisplayBrightness(uint8_t brightness) override;

        /**
         * @brief Get the display height.
         *
         * @return int Display height in pixels.
         */
        uint8_t GetDisplayBrightness() override;

        /* -------------------------------------------------------------------------- */
        /*                                   GPIO                                     */
        /* -------------------------------------------------------------------------- */
        void gpioInitOutput(uint8_t pin) override;
        void gpioSetLevel(uint8_t pin, bool level) override;
        void gpioReset(uint8_t pin) override;

    private:
        /* -------------------------------------------------------------------------- */
        /*                        Private I2C Methods and Data                        */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Port number for the I2C master bus.
         */
        const i2c_port_num_t MASTER_I2C_PORT_NUMBER = 0;

        /**
         * @brief GPIO number for the I2C master SDA line.
         */
        const gpio_num_t MASTER_I2C_SDA_GPIO = GPIO_NUM_31;

        /**
         * @brief GPIO number for the I2C master SCL line.
         */
        const gpio_num_t MASTER_I2C_SCL_GPIO = GPIO_NUM_32;

        /**
         * @brief I2C master bus handle.
         */
        i2c_master_bus_handle_t _i2cHandle = nullptr;

        /* -------------------------------------------------------------------------- */
        /*                    Private IO Expanders Data and Methods                   */
        /*                                                                            */
        /* Chip: PI4IOE5V6416                                                         */
        /* Datasheet: https://www.diodes.com/datasheet/download/PI4IOE5V6416.pdf      */
        /*                                                                            */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief I2C device addresses for the first PI4IOE expander.
         */
        const uint8_t I2C_DEV_ADDR_PI4IOE1 = 0x43;

        /**
         * @brief I2C device address for the second PI4IOE expander.
         */
        const uint8_t I2C_DEV_ADDR_PI4IOE2 = 0x44;

        /**
         * @brief I2C master timeout in milliseconds.
         */
        const uint8_t I2C_MASTER_TIMEOUT_MS = 50;

        /**
         * @brief Reset register address
         */
        const uint8_t PI4IO_REG_CHIP_RESET = 0x01;

        /**
         * @brief IO register address.
         */
        const uint8_t PI4IO_REG_IO_DIR = 0x03;


        const uint8_t PI4IO_REG_OUT_SET = 0x05;

        /**
         * @brief Output high impedance register address.
         */
        const uint8_t PI4IO_REG_OUT_H_IM = 0x07;

        /**
         * @brief Default state register address.
         */
        const uint8_t PI4IO_REG_IN_DEF_STA = 0x09;

        /**
         * @brief Pull up/down enable register address.
         */
        const uint8_t PI4IO_REG_PULL_EN = 0x0B;

        /**
         * @brief Pull up/down selection.
         */
        const uint8_t PI4IO_REG_PULL_SEL = 0x0D;

        /**
         * @brief Set the state for a pin.
         */
        const uint8_t PI4IO_REG_IN_STA = 0x0F;

        /**
         * @brief Interrupt mask register address.
         */
        const uint8_t PI4IO_REG_INT_MASK = 0x11;

        /**
         * @brief Interrupt status register address.
         */
        const uint8_t PI4IO_REG_IRQ_STA = 0x13;

        /**
         * @brief Handle for the first PI4IOE expander.
         */
        i2c_master_dev_handle_t _pi4ioe1Handle = nullptr;

        /**
         * @brief Handle for the second PI4IOE expander.
         */
        i2c_master_dev_handle_t _pi4ioe2Handle = nullptr;

        /**
         * @brief Current LCD brightness level.
         *
         * This is a value from 0 to 100, where 0 is off and 100 is maximum brightness.
         */
        uint8_t _current_lcd_brightness = 100;

        void set_gpio_output_capability();
    };
}
