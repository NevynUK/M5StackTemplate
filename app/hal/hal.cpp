/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "hal.h"
#include <memory>
#include <string>
#include <mooncake_log.h>

/* -------------------------------------------------------------------------- */
/*                                  Singleton                                 */
/* -------------------------------------------------------------------------- */
// Provides an injectable global singleton

static std::unique_ptr<hal::HalBase> _hal_instance;
static const std::string _tag = "hal";

hal::HalBase *hal::Get()
{
    if (!_hal_instance)
    {
        mclog::tagWarn(_tag, "getting null hal, auto inject base");
        _hal_instance = std::make_unique<HalBase>();
    }
    return _hal_instance.get();
}

void hal::Inject(std::unique_ptr<HalBase> hal)
{
    if (!hal)
    {
        mclog::tagError(_tag, "pass null hal");
        return;
    }

    // Destroy existing instance, store new instance
    Destroy();
    _hal_instance = std::move(hal);

    // Let's see what we're dealing with
    mclog::tagInfo(_tag, "injecting hal type: {}", _hal_instance->type());

    // Initialize
    mclog::tagInfo(_tag, "invoke init");
    _hal_instance->init();
    mclog::tagInfo(_tag, "hal injected");
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
