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
/*                                  Singleton                                 */
/* -------------------------------------------------------------------------- */
// Provides an injectable global singleton

// static std::unique_ptr<hal::HalBase> _hal_instance;
// static const std::string _tag = "HalBase";

// hal::HalBase *hal::Get()
// {
//     if (!_hal_instance)
//     {
//         ESP_LOGW(_tag.c_str(), "Getting null hal, auto inject base\n");
//         _hal_instance = std::make_unique<HalBase>();
//     }
//     return _hal_instance.get();
// }

// void hal::Inject(std::unique_ptr<HalBase> hal)
// {
//     if (!hal)
//     {
//         ESP_LOGE(_tag.c_str(), "Pass null hal");
//         return;
//     }

//     // Destroy existing instance, store new instance
//     Destroy();
//     _hal_instance = std::move(hal);

//     // Let's see what we're dealing with
//     ESP_LOGI(_tag.c_str(), "Injecting hal type: %s\n", _hal_instance->type().c_str());

//     // Initialize
//     ESP_LOGI(_tag.c_str(), "Invoking init\n");
//     _hal_instance->init();
//     ESP_LOGI(_tag.c_str(), "HAL injected\n");
// }

// void hal::Destroy()
// {
//     _hal_instance.reset();
// }

// bool hal::Check()
// {
//     if (_hal_instance)
//     {
//         return true;
//     }
//     return false;
// }

/**
 * @brief Static pointer to the singleton instance of HalBase.
 */
HalBase *HalBase::_instance = nullptr;

/**
 * @brief Default constructor for this class.
 * 
 * This constructor is private to enforce the singleton pattern.
 */
HalBase::HalBase()
{
    ESP_LOGI("HalBase", "HalBase instance created");
}

/**
 * @brief Delete the default destructor for this class as we should not need it.
 * 
 * This destructor is private to enforce the singleton pattern.
 */
HalBase::~HalBase()
{
    ESP_LOGI("HalBase", "HalBase instance destroyed");
}

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
