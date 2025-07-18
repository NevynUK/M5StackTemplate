/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c_master.h"

#include <HalBase.h>
// #include <ina226.hpp>
#include <lvgl.h>
#include "esp_lvgl_port.h"


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


class HalEsp32 : public hal::HalBase
{
public:
    const char *COMPONENT_NAME = "HalEsp32";

    std::string type() override
    {
        return "Tab5";
    }

    /* -------------------------------------------------------------------------- */
    /*                                    I2C                                     */
    /* -------------------------------------------------------------------------- */

    /**
     * @brief Configure the I2C bus.
     *
     * @return esp_err_t ESP_OK on success, or an error code on failure.
     */
    esp_err_t ConfigureI2C();

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

    void init() override;

    // INA226 ina226;
    lv_disp_t *lvDisp = nullptr;
    lv_indev_t *lvKeyboard = nullptr;

    esp_err_t InitialiseBrightnessControl(void) override;
    esp_err_t SetDisplayBrightness(uint8_t brightness) override;
    uint8_t GetDisplayBrightness() override;

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

    void set_gpio_output_capability();
    uint8_t _current_lcd_brightness = 100;
};
