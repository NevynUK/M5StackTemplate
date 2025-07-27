/**
 * @file HalBase.hpp
 * @author Mark Stevens
 * @brief
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include <sdkconfig.h>

#include <cstdint>

#include <esp_err.h>

#include "NotImplementedException.hpp"

/**
 * @brief Namespace for the Hardware Abstraction Layer (HAL).
 */
namespace HAL
{
    /**
     * @brief Base class for hardware abstraction layer.
     */
    class HalBase
    {
    public:
        /**
         * @brief Destroy the HalBase object.
         */
        virtual ~HalBase() = default;

        /**
         * @brief Configure and initialise the hardware.
         */
        virtual void Configure()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* -------------------------------------------------------------------------- */
        /*                                     I2C                                    */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the I2C bus.
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        virtual esp_err_t ConfigureI2C()
        {
            throw NotImplementedException(__FUNCTION__);
        }

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
        virtual esp_err_t ConfigureSdCard(const std::string &mount_point = "/sdcard", const size_t max_files = 10)
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* -------------------------------------------------------------------------- */
        /*                                   Display                                  */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the display.
         *
         * @param cfg Display configuration.
         * @return lv_display_t* Pointer to the LVGL display object.
         */
        virtual esp_err_t ConfigureDisplay()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Get the Display Width object.
         *
         * @return uint32_t Width of the display.
         */
        virtual uint32_t GetDisplayWidth()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Get the display height.
         *
         * @return uint32_t Display height in pixels.
         */
        virtual uint32_t GetDisplayHeight()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Check if the display uses big-endian color format.
         *
         * @return true if the display uses big-endian format, false otherwise.
         */
        virtual bool IsDisplayBigEndian() const
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Get the display brightness.
         *
         * @return uint8_t Brightness level from 0 to 100.
         */
        virtual uint8_t GetDisplayBrightness()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Set the display brightness.
         *
         * @param brightness Brightness level from 0 to 100.
         */
        virtual esp_err_t SetDisplayBrightness(uint8_t brightness)
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Initialise the brightness control.
         *
         * @return esp_err_t ESP_OK on success, or an error code on failure.
         */
        virtual esp_err_t ConfigureDisplayBrightnessControl()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* -------------------------------------------------------------------------- */
        /*                                 Touch Panel                                */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the touch panel.
         */
        virtual esp_err_t ConfigureTouchPanel()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /**
         * @brief Get the handle to the touch panel.
         */
        virtual void *GetTouchPanelHandle() const
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* -------------------------------------------------------------------------- */
        /*                                   GPIO                                     */
        /* -------------------------------------------------------------------------- */
        virtual void gpioInitOutput(uint8_t pin)
        {
            throw NotImplementedException(__FUNCTION__);
        }

        virtual void gpioSetLevel(uint8_t pin, bool level)
        {
            throw NotImplementedException(__FUNCTION__);
        }

        virtual void gpioReset(uint8_t pin)
        {
            throw NotImplementedException(__FUNCTION__);
        }
    };
} // namespace HAL
