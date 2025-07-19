/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <cstdint>
#include <memory>
#include <queue>
#include <string>
#include <mutex>
#include <vector>

#include <esp_err.h>

#include <lvgl.h>

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
        /*                                   Display                                  */
        /* -------------------------------------------------------------------------- */

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
        virtual esp_err_t InitialiseBrightnessControl()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* ---------------------------------- Lvgl ---------------------------------- */
        lv_indev_t *lvTouchpad = nullptr;

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
} // namespace hal
