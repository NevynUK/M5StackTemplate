/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "hal/hal_esp32.h"
#include <app.h>
#include <hal/hal.h>
#include <memory>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"


extern "C" void app_main(void)
{
    std::unique_ptr<HalEsp32> hal = std::make_unique<HalEsp32>();
    hal->init();

    // Application layer initialization callback
    // app::InitCallback_t callback;

    // callback.onHalInjection = []()
    // {
    //     // Inject hardware abstraction for ESP32 platform
    //     hal::Inject(std::make_unique<HalEsp32>());
    // };

    // // Application layer startup
    // app::Init(callback);
    // while (!app::IsDone())
    // {
    //     app::Update();
    //     vTaskDelay(1);
    // }
    // app::Destroy();


        esp_chip_info_t chip_info;
    uint32_t flash_size;

    esp_chip_info(&chip_info);
    printf(
        "This is %s chip with %d CPU core(s), %s%s%s%s, ", CONFIG_IDF_TARGET, chip_info.cores, (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "", (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "", (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
        (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("Silicon revision v%d.%d, ", major_rev, minor_rev);
    if (esp_flash_get_size(NULL, &flash_size) != ESP_OK)
    {
        printf("Get flash size failed");
        return;
    }

    printf("%s bytes %s flash\n", Utils::NumberWithCommas(flash_size).c_str(), (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %s bytes\n", Utils::NumberWithCommas(esp_get_minimum_free_heap_size()).c_str());

    // SDCard *sd_card = SDCard::GetInstance();
    // sd_card->Setup();
    // ListFiles(sd_card->GetMountPoint());

    // Display *display = Display::GetInstance();
    // display->Setup();
    // uint32_t brightness = 0;
    // int direction = 10;
    // while (true)
    // {
    //     if (brightness >= 100)
    //     {
    //         direction = -10;
    //     }
    //     else 
    //     {
    //         if (brightness == 0)
    //         {
    //             direction = 10;
    //         }
    //     }
    //     brightness += direction;
    //     display->SetBrightness(brightness);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

    vTaskDelay(portMAX_DELAY / portTICK_PERIOD_MS);
}
