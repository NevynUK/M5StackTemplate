/**
 * @file HalTab5.hpp
 * @author Mark Stevens
 * @brief Hardware Abstraction Layer for the M5Stack Tab5.
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include <sdkconfig.h>

#include <string>
#include <mutex>

#include "esp_err.h"
#include "driver/i2c_master.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_touch_gt911.h"
#include "driver/gpio.h"
#include "sdmmc_cmd.h"

#include <HalBase.hpp>

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
        /*                                   SD Card                                  */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the SD card.
         *
         * @param mount_point The mount point for the filesystem (default = DEFAULT_MOUNT_POINT).
         * @param max_files The maximum number of files that can be opened simultaneously (default = 10).
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        esp_err_t ConfigureSdCard(const std::string &mount_point = DEFAULT_MOUNT_POINT, const size_t max_files = 10) override;

        /* -------------------------------------------------------------------------- */
        /*                            Display Methods                                 */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief GPIO controlling the LCD backlight.
         */
        const gpio_num_t BSP_LCD_BACKLIGHT = GPIO_NUM_22;

        /**
         * @brief GPIO connected to the LCD reset.
         *
         */
        const gpio_num_t BSP_LCD_RST = GPIO_NUM_NC;

        /**
         * @brief Indicates whether the LCD display uses big-endian color format.
         */
        const bool BSP_LCD_BIGENDIAN = false;
        /**
         * @brief Number of bits per pixel for the LCD display.
         */
        const uint32_t BSP_LCD_BITS_PER_PIXEL = 16;

        /* LCD color formats */
        // #define ESP_LCD_COLOR_FORMAT_RGB565 (1)
        // #define ESP_LCD_COLOR_FORMAT_RGB888 (2)

        /* LCD display color format */
        // #if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
        // #define BSP_LCD_COLOR_FORMAT        (ESP_LCD_COLOR_FORMAT_RGB888)
        // #else
        // #define BSP_LCD_COLOR_FORMAT (ESP_LCD_COLOR_FORMAT_RGB565)
        // #endif
        /* LCD display color space */
        // #define BSP_LCD_COLOR_SPACE (ESP_LCD_COLOR_SPACE_RGB)

        /**
         * @brief Horizontal resolution of the display.
         */
        const uint32_t BSP_LCD_H_RES = 720;

        /**
         * @brief Vertical resolution of the display.
         */
        const uint32_t BSP_LCD_V_RES = 1280;

        /**
         * @brief Number of data lanes on the MIPI DSI.
         */
        const uint8_t BSP_LCD_MIPI_DSI_LANE_NUM = 2;

        /**
         * @brief Data rate (in MHz) of the MIPI DSI lanes (720*1280 RGB24 60Hz //(900) // 900Mbps).
         */
        const uint32_t BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS = 730;

        /**
         * @brief LDO channel ID (LDO_VO3 is connected to VDD_MIPI_DPHY)
         */
        const int BSP_MIPI_DSI_PHY_PWR_LDO_CHAN = 3;

        /**
         * @brief LDO voltage in millivolts.
         */
        const int BSP_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV = 2500;

        /**
         * @brief Configure the display.
         *
         * @param cfg Display configuration.
         * @return lv_display_t* Pointer to the LVGL display object.
         */
        esp_err_t ConfigureDisplay();

        /**
         * @brief Get the Display Width object.
         *
         * @return uint32_t Width of the display.
         */
        uint32_t GetDisplayWidth() override;

        /**
         * @brief Get the display height.
         *
         * @return uint32_t Display height in pixels.
         */
        uint32_t GetDisplayHeight() override;

        /**
         * @brief Check if the display uses big-endian color format.
         *
         * @return true if the display uses big-endian format, false otherwise.
         */
        bool IsDisplayBigEndian() const override;

        /**
         * @brief Initialise the brightness control.
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        esp_err_t ConfigureDisplayBrightnessControl(void) override;
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

        /**
         * @brief Get the IO handle for the display.
         *
         * @return esp_lcd_panel_io_handle_t
         */
        esp_lcd_panel_io_handle_t GetIoHandle() const;

        /**
         * @brief Get the MIPI DSI bus handle for the display.
         *
         * @return esp_lcd_dsi_bus_handle_t
         */
        esp_lcd_dsi_bus_handle_t GetMipiDsiBusHandle() const;

        /**
         * @brief Get the panel handle for the display.
         *
         * @return esp_lcd_panel_handle_t
         */
        esp_lcd_panel_handle_t GetPanelHandle() const;

        /**
         * @brief Get the control handle for the display.
         *
         * @return esp_lcd_panel_handle_t
         */
        esp_lcd_panel_handle_t GetControlHandle() const;

        /* -------------------------------------------------------------------------- */
        /*                                 Touch Panel                                */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief GPIO connected to the LCD touch reset.
         */
        const gpio_num_t BSP_LCD_TOUCH_RST = GPIO_NUM_NC;

        /**
         * @brief GPIO connected to the LCD touch interrupt.
         */
        const gpio_num_t BSP_LCD_TOUCH_INT = GPIO_NUM_23;

        /**
         * @brief Configure the touch panel.
         */
        esp_err_t ConfigureTouchPanel() override;

        /**
         * @brief Reset the touch panel.
         */
        void ResetTouchPanel();

        /**
         * @brief Get the touch panel handle.
         *
         * @return esp_lcd_touch_handle_t Handle to the touch panel.
         */
        void *GetTouchPanelHandle() const override;

        /* -------------------------------------------------------------------------- */
        /*                                   GPIO                                     */
        /* -------------------------------------------------------------------------- */
        void gpioInitOutput(uint8_t pin) override;
        void gpioSetLevel(uint8_t pin, bool level) override;
        void gpioReset(uint8_t pin) override;

        void SetGpioOutputCapability();

/* -------------------------------------------------------------------------- */
/*                                   Audio                                    */
/* -------------------------------------------------------------------------- */
/* Audio */
#define BSP_I2S_SCLK (GPIO_NUM_27)     // Bit clock      BSP_I2S_BCLK  <--> ES7210/ESP311 I2S_BCLK
#define BSP_I2S_MCLK (GPIO_NUM_30)     // Master clock   BSP_I2S_MCLK  <--> ES7210/ESP311 I2S_MCLK
#define BSP_I2S_LCLK (GPIO_NUM_29)     // Word select    BSP_I2S_WR    <--> ES7210/ESP311 I2S_WR
#define BSP_I2S_DOUT (GPIO_NUM_26)     // Data output    BSP_I2S_DOUT  ---> ES8388        I2S_DSIN
#define BSP_I2S_DSIN (GPIO_NUM_28)     // Data input     BSP_I2S_DIN   <--- ES7210        I2S_DOUT
#define BSP_POWER_AMP_IO (GPIO_NUM_NC) // (GPIO_NUM_53)

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

        /* -------------------------------------------------------------------------- */
        /*                       Private SD Card Data Methods                         */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Default mount point for the SD card.
         */
        static const std::string DEFAULT_MOUNT_POINT;

        /**
         * @brief Power control channel for the SD card.
         */
        const int LDO_PROBE_SD_CHAN = 4;

        /**
         * @brief Power control voltage for the SD card.
         */
        const int LDO_PROBE_SD_VOLTAGE_MV = 3300;

        /**
         * @brief SD Card bus width.
         */
        const int SDMMC_BUS_WIDTH = 4;

        /**
         * @brief SD Card detection pin.
         */
        const gpio_num_t GPIO_SDMMC_DET = GPIO_NUM_NC;

        /**
         * @brief SD Card Clock GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_CLK = GPIO_NUM_43;

        /**
         * @brief SD Card Command GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_CMD = GPIO_NUM_44;

        /**
         * @brief SD Card Data 0 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D0 = GPIO_NUM_39;

        /**
         * @brief SD Card Data 1 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D1 = GPIO_NUM_40;

        /**
         * @brief SD Card Data 2 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D2 = GPIO_NUM_41;

        /**
         * @brief SD Card Data 3 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D3 = GPIO_NUM_42;

        /**
         * @brief Mutex for thread safety.
         */
        static std::mutex _mutex;

        /**
         * @brief Pointer to the SD card structure.
         * 
         * This structure holds information about the SD card, such as its type, size, and other properties.
         */
        sdmmc_card_t *_card = nullptr;

        /**
         * @brief Power control handle for the SD card.
         */
        sd_pwr_ctrl_handle_t pwr_ctrl_handle = nullptr;

        /**
         * @brief Mount point for the filesystem.
         */
        std::string _mountPoint = "";

        /**
         * @brief Maximum number of files that can be opened simultaneously.
         */
        size_t _maxFiles = 0;

        /* -------------------------------------------------------------------------- */
        /*                       Private Display Data Methods                         */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Handle for the MIPI DSI bus.
         */
        esp_lcd_dsi_bus_handle_t _mipiDsiBusHandle;

        /**
         * @brief Handle for the IO panel.
         */
        esp_lcd_panel_io_handle_t _ioHandle;

        /**
         * @brief Handle for the colour panel.
         */
        esp_lcd_panel_handle_t _panelHandle;

        /**
         * @brief Handle for the control panel.
         */
        esp_lcd_panel_handle_t _controlHandle;

        /**
         * @brief Enable MIPI DSI PHY power
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        esp_err_t EnableDsiPhyPower(void);

        /* -------------------------------------------------------------------------- */
        /*                    Private Touch Panel Data and Methods                    */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Handle for the touch panel.
         */
        esp_lcd_touch_handle_t _touchPanelHandle = nullptr;

        // void set_gpio_output_capability();
    };
} // namespace HAL
