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
