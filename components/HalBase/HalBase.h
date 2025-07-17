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
         *
         */
        virtual void init()
        {
        }

        /* -------------------------------------------------------------------------- */
        /*                                   Display                                  */
        /* -------------------------------------------------------------------------- */
        virtual int getDisplayWidth()
        {
            return 1280;
        }

        virtual int getDisplayHeight()
        {
            return 720;
        }

        virtual esp_err_t InitialiseBrightnessControl()
        {
            return ESP_FAIL;
        }

        virtual esp_err_t SetDisplayBrightness(uint8_t brightness)
        {
            return ESP_FAIL;
        }

        virtual uint8_t GetDisplayBrightness()
        {
            return 0;
        }

        /* ---------------------------------- Lvgl ---------------------------------- */
        lv_indev_t *lvTouchpad = nullptr;

        virtual void lvglLock()
        {
        }

        virtual void lvglUnlock()
        {
        }

        virtual void gpioInitOutput(uint8_t pin)
        {
        }

        virtual void gpioSetLevel(uint8_t pin, bool level)
        {
        }

        virtual void gpioReset(uint8_t pin)
        {
        }
    };

    /**
     * @brief Get the HAL instance
     *
     * @return HalBase&
     */
    HalBase *Get();

    /**
     * @brief Inject the HAL, which will call init() to initialize the HAL
     *
     * @param hal
     */
    void Inject(std::unique_ptr<HalBase> hal);

    /**
     * @brief Destroy the HAL instance
     *
     */
    void Destroy();

    /**
     * @brief Check if the HAL instance exists
     *
     * @return true
     * @return false
     */
    bool Check();

} // namespace hal
