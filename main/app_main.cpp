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
#include "Display.hpp"

// Touch panel callback.
//  static void lvgl_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
//  {
//      if (_lcd_touch_handle == NULL)
//      {
//          data->state = LV_INDEV_STATE_REL;
//          return;
//      }

//     uint16_t touch_x[1];
//     uint16_t touch_y[1];
//     uint16_t touch_strength[1];
//     uint8_t touch_cnt = 0;

//     esp_lcd_touch_read_data(_lcd_touch_handle);
//     bool touchpad_pressed = esp_lcd_touch_get_coordinates(_lcd_touch_handle, touch_x, touch_y, touch_strength, &touch_cnt, 1);

//     if (!touchpad_pressed)
//     {
//         data->state = LV_INDEV_STATE_REL;
//     }
//     else
//     {
//         data->state = LV_INDEV_STATE_PR;
//         data->point.x = touch_x[0];
//         data->point.y = touch_y[0];
//     }
// }

extern "C" void app_main(void)
{
    printf("Minimum free heap size: %s bytes\n", Utils::NumberWithCommas(esp_get_minimum_free_heap_size()).c_str());

    HAL::HalBase *hal = new HAL::HalTab5();
    hal->Configure();

    Display *display = new Display(hal);
    display->Configure();

    /* Add touch input (for selected screen) */
    // const lvgl_port_touch_cfg_t touch_cfg =
    // {
    //     .disp = disp,
    //     .handle = tp,
    // };

    // return lvgl_port_add_touch(&touch_cfg);
    // Touchpad lvgl indev
    // lvTouchpad = lv_indev_create();
    // lv_indev_set_type(lvTouchpad, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(lvTouchpad, lvgl_read_cb);
    // lv_indev_set_display(lvTouchpad, lvDisp);

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
    // sd_card->Configure();
    // ListFiles(sd_card->GetMountPoint());

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
