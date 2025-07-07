/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include <sdkconfig.h>

#include "HalTab5.h"
#include <memory>
#include <string>

#include "esp_log.h"

#include "HalTab5.h"

using namespace HAL;

/* -------------------------------------------------------------------------- */
/*                            Static Data Members                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Static pointer used to ensure that only one instance of HalTab5 exists.
 */
HalTab5 *HalTab5::_instance = nullptr;

/* -------------------------------------------------------------------------- */
/*                         Constructors / Destructor                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Default constructor for this class.
 * 
 * This constructor is private to enforce the singleton pattern.
 */
HalTab5::HalTab5()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 instance created");
}

/**
 * @brief Delete the default destructor for this class as we should not need it.
 * 
 * This destructor is private to enforce the singleton pattern.
 */
HalTab5::~HalTab5()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 instance destroyed");
}

/**
 * @brief Get the singleton instance of HalTab5.
 * 
 * If the instance does not exist, it will create a new one.
 * 
 * @return HalTab5* Pointer to the singleton instance of HalTab5.
 */
HalTab5 *HalTab5::GetInstance()
{
    if (!_instance)
    {
        _instance = new HalTab5();
    }
    return _instance;
}

/* -------------------------------------------------------------------------- */
/*                                Methods                                     */
/* -------------------------------------------------------------------------- */

void HalTab5::init()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 initialized");
}
