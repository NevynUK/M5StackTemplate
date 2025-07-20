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

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "HalBase.hpp"
#include "HalTab5.hpp"
#include "Utils.hpp"

extern "C" void app_main(void)
{
    printf("Minimum free heap size: %s bytes\n", Utils::NumberWithCommas(esp_get_minimum_free_heap_size()).c_str());

    std::unique_ptr<HAL::HalBase> hal = std::make_unique<HAL::HalTab5>();
    lvgl_port_cfg_t lvglConfig = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvglConfig);
    hal->Configure();

    /* Add LCD screen */
    lvgl_port_display_cfg_t disp_cfg = {};
    disp_cfg.io_handle = static_cast<HAL::HalTab5 *>(hal.get())->GetIoHandle();
    disp_cfg.panel_handle = static_cast<HAL::HalTab5 *>(hal.get())->GetPanelHandle();
    disp_cfg.control_handle = static_cast<HAL::HalTab5 *>(hal.get())->GetControlHandle();
    disp_cfg.buffer_size = hal->GetDisplayHeight() * hal->GetDisplayWidth();
    disp_cfg.double_buffer = true;
    disp_cfg.hres = hal->GetDisplayWidth();
    disp_cfg.vres = hal->GetDisplayHeight();
    disp_cfg.monochrome = false;
    disp_cfg.rotation = {};
    disp_cfg.rotation.swap_xy = false;
    disp_cfg.rotation.mirror_x = false;
    disp_cfg.rotation.mirror_y = false;
    disp_cfg.color_format = LV_COLOR_FORMAT_RGB565;
    disp_cfg.flags = {};
    disp_cfg.flags.buff_dma = true;
    disp_cfg.flags.buff_spiram = true;
    disp_cfg.flags.swap_bytes = hal->IsDisplayBigEndian();
    disp_cfg.flags.sw_rotate = true;

    lvgl_port_display_dsi_cfg_t dpi_cfg = {};
    dpi_cfg.flags.avoid_tearing = false;

    lv_disp_t *lvDisp = lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);
    lv_display_set_rotation(lvDisp, LV_DISPLAY_ROTATION_90);
    hal->SetDisplayBrightness(100);

    // Touchpad lvgl indev
    // lvTouchpad = lv_indev_create();
    // lv_indev_set_type(lvTouchpad, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(lvTouchpad, lvgl_read_cb);
    // lv_indev_set_display(lvTouchpad, lvDisp);

    lvgl_port_unlock();


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

    lvgl_port_lock(0);
    /**
     * @brief Pointer to the screen object to be used in drawing operations.
     */
    lv_obj_t *_screen = nullptr;

    _screen = lv_scr_act();

    lv_obj_set_style_bg_color(_screen, lv_color_black(), LV_PART_MAIN);

    lv_obj_t *rectangle = lv_obj_create(_screen);
    lv_obj_set_size(rectangle, 200, 200);
    lv_obj_set_pos(rectangle, 100, 100);
    lv_obj_set_style_radius(rectangle, 0, 0);
    lv_obj_set_style_bg_color(rectangle, lv_color_hex(0xFFFFFF), LV_STATE_DEFAULT);

    lvgl_port_unlock();

    while (true)
    {
        hal->SetDisplayBrightness(100);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        hal->SetDisplayBrightness(0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelay(portMAX_DELAY / portTICK_PERIOD_MS);
}
