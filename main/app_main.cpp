/**
 * @file app_main.cpp
 * @author Mark Stevens
 * @brief
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */
#include <memory>
#include <cstring>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <dirent.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include <inttypes.h>

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "HalBase.hpp"
#include "HalTab5.hpp"
#include "Utils.hpp"
#include "Display.hpp"

// Touch panel callback.

void ScanFileSystem(const std::string &directory)
{
    std::string target_path = directory;

    DIR* dir = opendir(target_path.c_str());

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

extern "C" void app_main(void)
{
    printf("Minimum free heap size: %s bytes\n", Utils::NumberWithCommas(esp_get_minimum_free_heap_size()).c_str());

    HAL::HalBase *hal = new HAL::HalTab5();
    hal->Configure();

    ScanFileSystem("/sdcard/");

    Display *display = new Display(hal);
    display->Configure();

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

    display->DrawFilledRectangle(100, 100, 300, 200, lv_color_hex(0xFFFFFF));

    while (true)
    {
        hal->SetDisplayBrightness(100);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        hal->SetDisplayBrightness(0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelay(portMAX_DELAY / portTICK_PERIOD_MS);
}
