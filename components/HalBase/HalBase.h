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
 * @brief Hardware abstraction layer
 *
 */
namespace hal
{
    /**
     * @brief
     *
     */
    class HalBase
    {
    public:
        virtual ~HalBase() = default;

        /**
         * @brief
         *
         * @return std::string
         */
        virtual std::string type()
        {
            return "Base";
        }

        /**
         * @brief
         */
        virtual void init()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* -------------------------------------------------------------------------- */
        /*                                   Display                                  */
        /* -------------------------------------------------------------------------- */
        virtual int getDisplayWidth()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        virtual int getDisplayHeight()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        virtual esp_err_t InitialiseBrightnessControl()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        virtual esp_err_t SetDisplayBrightness(uint8_t brightness)
        {
            throw NotImplementedException(__FUNCTION__);
        }

        virtual uint8_t GetDisplayBrightness()
        {
            throw NotImplementedException(__FUNCTION__);
        }

        /* ---------------------------------- Lvgl ---------------------------------- */
        lv_indev_t *lvTouchpad = nullptr;

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
