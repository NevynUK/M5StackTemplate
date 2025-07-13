/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <sdkconfig.h>

#include <memory>
#include <string>

#include "esp_log.h"
#include "esp_lcd_ili9881c.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"

#include "HalBase.h"

using namespace HAL;

/* -------------------------------------------------------------------------- */
/*                            Static Data Members                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Static pointer to the singleton instance of HalBase.
 */
HalBase *HalBase::_instance = nullptr;

/**
 * @brief Mount point for the SD card.
 */
const std::string HalBase::MOUNT_POINT = "/sdcard";

/* -------------------------------------------------------------------------- */
/*                         Constructors / Destructor                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Default constructor for this class.
 * 
 * This constructor is private to enforce the singleton pattern.
 */
HalBase::HalBase()
{
    ESP_LOGI(COMPONENT_NAME, "HalBase instance created");
}

/**
 * @brief Delete the default destructor for this class as we should not need it.
 * 
 * This destructor is private to enforce the singleton pattern.
 */
HalBase::~HalBase()
{
    ESP_LOGI(COMPONENT_NAME, "HalBase instance destroyed");
}

/* -------------------------------------------------------------------------- */
/*                                Methods                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Get the singleton instance of the HalBase class.
 * 
 * If the instance does not exist, it will create a new one.
 *
 * @return const HalBase* Pointer to the singleton instance of HalBase.
 */
HalBase *HalBase::GetInstance()
{
    if (!_instance)
    {
        _instance = new HalBase();
    }
    return _instance;
}

/* -------------------------------------------------------------------------- */
/*                                    I2C                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the I2C bus.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalBase::ConfigureI2C()
{
    return ESP_FAIL;
}

/* -------------------------------------------------------------------------- */
/*                               IO Expanders                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the IO Expanders.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalBase::ConfigureIoExpanders()
{
    return ESP_FAIL;
}

/* -------------------------------------------------------------------------- */
/*                            SD Card Methods                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the SD card interface.
 * 
 * @param mountPoint The mount point of the SD card.
 * @param maximumFiles The maximum number of files that can be opened on the SD card.
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalBase::Mount(std::string mountPoint, size_t maximumFiles)
{
    return ESP_FAIL;
}

/**
 * @brief Unmount the SD card.
 * 
 * @param mountPoint The mount point of the SD card.
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalBase::Unmount(std::string mountPoint)
{
    return ESP_FAIL;
}

/**
 * @brief Check if the SD card is mounted.
 * 
 * @return true If the SD card is mounted.
 * @return false If the SD card is not mounted.
 */
bool HalBase::IsSdCardMounted()
{
    return false;
}

/* -------------------------------------------------------------------------- */
/*                            Display Methods                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the display.
 * 
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalBase::ConfigureDisplay()
{
    return ESP_OK;
}

/**
 * @brief Panel IO handle.
 */
esp_lcd_panel_io_handle_t HalBase::GetPanelIoHandle() const
{
    return nullptr;
}

/**
 * @brief Display panel handle.
 */
esp_lcd_panel_handle_t HalBase::GetDisplayPanelHandle() const
{
    return nullptr;
}

/**
 * @brief MIPI DSI bus handle.
 */
esp_lcd_dsi_bus_handle_t HalBase::GetMipiDsiBusHandle() const
{
    return nullptr;
}

        /** 
 * @brief Configure the display brightness control interface.
 */
esp_err_t HalBase::ConfigureDisplayBrightnessControl()
{
    return ESP_OK;
}

/**
 * @brief Set the display brightness.
 * 
 * @param brightness Brightness level (0-100).
 */
esp_err_t HalBase::SetDisplayBrightness(uint8_t brightnessPercent)
{
    return ESP_OK;
}

/**
 * @brief Get the display brightness.
 * 
 * @return uint8_t Display brightness level (0-100).
 */
uint8_t HalBase::GetDisplayBrightness()
{
    return 0;
}
