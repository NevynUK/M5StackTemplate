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
#include <lvgl.h>
#include <mutex>
#include <vector>

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

        /* --------------------------------- Display -------------------------------- */
        virtual int getDisplayWidth()
        {
            return 1280;
        }

        virtual int getDisplayHeight()
        {
            return 720;
        }

        virtual void setDisplayBrightness(uint8_t brightness)
        {
        }

        virtual uint8_t getDisplayBrightness()
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

        /* -------------------------------- Interface ------------------------------- */
        virtual bool usbCDetect()
        {
            return false;
        }

        virtual bool usbADetect()
        {
            return false;
        }

        virtual void initPortAI2c()
        {
        }

        virtual void deinitPortAI2c()
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

        /* ------------------------------ UART monitor ------------------------------ */
        struct UartMonitorData_t
        {
            std::mutex mutex;
            std::queue<uint8_t> rxQueue;
            std::queue<uint8_t> txQueue;
        };

        UartMonitorData_t uartMonitorData;

        virtual void uartMonitorSend(std::string msg, bool newLine = true)
        {
            std::lock_guard<std::mutex> lock(uartMonitorData.mutex);
            for (auto c: msg)
            {
                uartMonitorData.txQueue.push(c);
            }
            if (newLine)
            {
                uartMonitorData.txQueue.push('\n');
            }
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

/**
 * @brief Get the HAL instance
 *
 * @return hal::HalBase&
 */
inline hal::HalBase *GetHAL()
{
    return hal::Get();
}

/**
 * @brief
 *
 */
class LvglLockGuard
{
public:
    LvglLockGuard()
    {
        GetHAL()->lvglLock();
    }

    ~LvglLockGuard()
    {
        GetHAL()->lvglUnlock();
    }
};
