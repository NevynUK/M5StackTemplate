/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "HalBase.h"
#include <memory>
#include <string>

/* -------------------------------------------------------------------------- */
/*                                  Singleton                                 */
/* -------------------------------------------------------------------------- */
// Provides an injectable global singleton

static std::unique_ptr<hal::HalBase> _hal_instance;
static const std::string _tag = "HalBase";

hal::HalBase *hal::Get()
{
    if (!_hal_instance)
    {
        _hal_instance = std::make_unique<HalBase>();
    }
    return _hal_instance.get();
}

void hal::Inject(std::unique_ptr<HalBase> hal)
{
    if (!hal)
    {
        return;
    }

    // Destroy existing instance, store new instance
    Destroy();
    _hal_instance = std::move(hal);

    // Let's see what we're dealing with

    // Initialize
    _hal_instance->init();
}

void hal::Destroy()
{
    _hal_instance.reset();
}

bool hal::Check()
{
    if (_hal_instance)
    {
        return true;
    }
    return false;
}
