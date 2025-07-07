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
/*                                  Singleton                                 */
/* -------------------------------------------------------------------------- */
// Provides an injectable global singleton

// static std::unique_ptr<hal::HalTab5> _hal_instance;
// static const std::string _tag = "HalTab6";

// hal::HalTab5 *hal::Get()
// {
//     if (!_hal_instance)
//     {
//         ESP_LOGW(_tag.c_str(), "Getting null hal, auto inject base\n");
//         _hal_instance = std::make_unique<HalTab5>();
//     }
//     return _hal_instance.get();
// }

// void hal::Inject(std::unique_ptr<HalTab5> hal)
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
 * @brief Static pointer used to ensure that only one instance of HalTab5 exists.
 */
HalTab5 *HalTab5::_instance = nullptr;

HalTab5::HalTab5()
{
    ESP_LOGI("HalTab5", "HalTab5 instance created");
}

HalTab5::~HalTab5()
{
    ESP_LOGI("HalTab5", "HalTab5 instance destroyed");
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

void HalTab5::init()
{
    ESP_LOGI("HalTab5", "HalTab5 initialized");
}
