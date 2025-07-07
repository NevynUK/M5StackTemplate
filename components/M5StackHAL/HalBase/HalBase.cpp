/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "HalBase.h"
#include <memory>
#include <string>

#include "esp_log.h"

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

void HalBase::init()
{
    ESP_LOGI(COMPONENT_NAME, "HalBase initialized");
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