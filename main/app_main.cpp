/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "hal/hal_esp32.h"
#include <app.h>
#include <hal/hal.h>
#include <memory>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main(void)
{
    // Application layer initialization callback
    app::InitCallback_t callback;

    callback.onHalInjection = []()
    {
        // Inject hardware abstraction for ESP32 platform
        hal::Inject(std::make_unique<HalEsp32>());
    };

    // Application layer startup
    app::Init(callback);
    while (!app::IsDone())
    {
        app::Update();
        vTaskDelay(1);
    }
    app::Destroy();
}
