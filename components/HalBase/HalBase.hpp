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

#include <cstdint>
// #include <memory>
// #include <queue>
// #include <string>
// #include <mutex>
// #include <vector>

#include <esp_err.h>

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "NotImplementedException.hpp"

#define BSP_ERROR_CHECK_RETURN_ERR(x) ESP_ERROR_CHECK(x)
#define BSP_ERROR_CHECK_RETURN_NULL(x) ESP_ERROR_CHECK(x)
#define BSP_ERROR_CHECK(x, ret) ESP_ERROR_CHECK(x)
#define BSP_NULL_CHECK(x, ret) assert(x)
#define BSP_NULL_CHECK_GOTO(x, goto_tag) assert(x)

/**
 * @brief BSP display configuration structure
 *
 */
// typedef struct
// {
//     uint32_t buffer_size;          /*!< Size of the buffer for the screen in pixels */
//     bool double_buffer;            /*!< True, if should be allocated two buffers */

//     struct
//     {
//         unsigned int buff_dma:1;    /*!< Allocated LVGL buffer will be DMA capable */
//         unsigned int buff_spiram:1; /*!< Allocated LVGL buffer will be in PSRAM */
//         unsigned int sw_rotate:1;   /*!< Use software rotation (slower), The feature is unavailable under avoid-tear mode */
//     } flags;
// } bsp_display_cfg_t;


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
        virtual esp_err_t InitialiseBrightnessControl()
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
