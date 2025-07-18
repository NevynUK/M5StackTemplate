/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

 #include <sdkconfig.h>

#include <memory>
#include <cstring>

#include <dirent.h>
#include <sys/types.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "Utils.hpp"
#include <HalBase.h>
#include <HalTab5.h>

using namespace HAL;

extern "C" void app_main(void)
{
    std::unique_ptr<HalBase> hal = std::make_unique<HalTab5>();
    hal->init();

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

    printf("SD card interface configured successfully\n");
    std::string mountPoint = "/sdcard";
    if (hal->Mount() != ESP_OK)
    {
        printf("Failed to mount SD card\n");
    }
    else
    {
        printf("SD card mounted successfully\n");
        DIR* dir = opendir(mountPoint.c_str());
        if (dir == nullptr)
        {
            printf("Failed to open directory: %s\n", mountPoint.c_str());
        }
        else
        {
            printf("Directory opened successfully: %s\n", mountPoint.c_str());
            struct dirent* entry;
            while ((entry = readdir(dir)) != nullptr)
            {
                if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..")
                {
                    continue;
                }
                printf("Found file: %s\n", entry->d_name);
            }

            closedir(dir);
        }
    }

    // Display *display = Display::GetInstance();
    // display->Setup();

    // hal->lvglLock();
    /**
     * @brief Pointer to the screen object to be used in drawing operations.
     */
    // lv_obj_t *_screen = nullptr;

    // _screen = lv_scr_act();

    // lv_obj_set_style_bg_color(_screen, lv_color_black(), LV_PART_MAIN);

    // lv_obj_t *rectangle = lv_obj_create(_screen);
    // lv_obj_set_size(rectangle, 200, 200);
    // lv_obj_set_pos(rectangle, 100, 100);
    // lv_obj_set_style_radius(rectangle, 0, 0);
    // lv_obj_set_style_bg_color(rectangle, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);

    // hal->lvglUnlock();

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
    //     hal->setDisplayBrightness(brightness);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

    vTaskDelay(portMAX_DELAY / portTICK_PERIOD_MS);
}
