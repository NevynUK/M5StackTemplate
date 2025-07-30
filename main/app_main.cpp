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

// static lv_obj_t * dialog_parent;
// static lv_obj_t * dialog_window;
// static lv_obj_t * dialog_panel;
// static lv_obj_t * dialog_title;
// static lv_obj_t * dialog_message;
// static lv_obj_t * dialog_close;
// static lv_obj_t * dialog_close_label;


// void show_dialog(const char * title, const char * message)
// {
//     lv_disp_t * display = lv_display_get_default();
//     lv_obj_t * active_screen = lv_display_get_screen_active(display);

//     /*  attach the dialog window to current active screen */
//     lv_obj_set_parent(dialog_window, active_screen);

//     lv_label_set_text(dialog_title, title);
//     lv_label_set_text(dialog_message, message);
//     lv_obj_remove_flag(dialog_window, LV_OBJ_FLAG_HIDDEN); /* show the dialog */
//     lv_obj_scroll_to_y(dialog_window, 0, LV_ANIM_ON);
// }



// static void dialog_close_event_cb(lv_event_t * e)
// {
//     lv_obj_add_flag(dialog_window, LV_OBJ_FLAG_HIDDEN);
// }

// static void create_dialog_window()
// {
//     dialog_parent = lv_obj_create(NULL);

//     dialog_window = lv_obj_create(dialog_parent);
//     lv_obj_set_width(dialog_window, lv_pct(100));
//     lv_obj_set_height(dialog_window, lv_pct(100));
//     lv_obj_set_align(dialog_window, LV_ALIGN_CENTER);
//     lv_obj_set_scrollbar_mode(dialog_window, LV_SCROLLBAR_MODE_OFF);
//     lv_obj_add_flag(dialog_window, LV_OBJ_FLAG_HIDDEN);
//     lv_obj_remove_flag(dialog_window, LV_OBJ_FLAG_GESTURE_BUBBLE); /* block gesture events when active */
//     lv_obj_set_scroll_dir(dialog_window, LV_DIR_VER);
//     lv_obj_set_style_radius(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_bg_color(dialog_window, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_bg_opa(dialog_window, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_width(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_left(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_right(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_top(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_bottom(dialog_window, 60, LV_PART_MAIN | LV_STATE_DEFAULT);

//     dialog_panel = lv_obj_create(dialog_window);
//     lv_obj_set_width(dialog_panel, 167);
//     lv_obj_set_height(dialog_panel, LV_SIZE_CONTENT);
//     lv_obj_set_x(dialog_panel, 0);
//     lv_obj_set_y(dialog_panel, 60);
//     lv_obj_set_align(dialog_panel, LV_ALIGN_TOP_MID);
//     lv_obj_set_flex_flow(dialog_panel, LV_FLEX_FLOW_COLUMN);
//     lv_obj_set_flex_align(dialog_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
//     lv_obj_remove_flag(dialog_panel, LV_OBJ_FLAG_SCROLLABLE);
//     lv_obj_set_style_bg_color(dialog_panel, lv_color_hex(0x080404), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_bg_opa(dialog_panel, 235, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_color(dialog_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_opa(dialog_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_width(dialog_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_left(dialog_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_right(dialog_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_top(dialog_panel, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_bottom(dialog_panel, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

//     dialog_title = lv_label_create(dialog_panel);
//     lv_obj_set_width(dialog_title, 140);
//     lv_obj_set_height(dialog_title, LV_SIZE_CONTENT);
//     lv_obj_set_align(dialog_title, LV_ALIGN_TOP_MID);
//     lv_label_set_long_mode(dialog_title, LV_LABEL_LONG_SCROLL_CIRCULAR);
//     lv_label_set_text(dialog_title, "Title");
//     lv_obj_set_style_text_align(dialog_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_text_font(dialog_title, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_color(dialog_title, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_opa(dialog_title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_width(dialog_title, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_border_side(dialog_title, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_left(dialog_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_right(dialog_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_top(dialog_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
//     lv_obj_set_style_pad_bottom(dialog_title, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

//     dialog_message = lv_label_create(dialog_panel);
//     lv_obj_set_width(dialog_message, 140);
//     lv_obj_set_height(dialog_message, LV_SIZE_CONTENT);
//     lv_obj_set_align(dialog_message, LV_ALIGN_CENTER);
//     lv_label_set_text(dialog_message, "Dialog message");
//     lv_obj_set_style_text_font(dialog_message, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

//     dialog_close = lv_button_create(dialog_panel);
//     lv_obj_set_width(dialog_close, 100);
//     lv_obj_set_height(dialog_close, LV_SIZE_CONTENT);
//     lv_obj_set_align(dialog_close, LV_ALIGN_BOTTOM_MID);
//     lv_obj_add_flag(dialog_close, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
//     lv_obj_remove_flag(dialog_close, LV_OBJ_FLAG_SCROLLABLE);
//     lv_obj_set_style_radius(dialog_close, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

//     dialog_close_label = lv_label_create(dialog_close);
//     lv_obj_set_width(dialog_close_label, LV_SIZE_CONTENT);
//     lv_obj_set_height(dialog_close_label, LV_SIZE_CONTENT);
//     lv_obj_set_align(dialog_close_label, LV_ALIGN_CENTER);
//     lv_label_set_text(dialog_close_label, "Close");
//     lv_obj_set_style_text_font(dialog_close_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

//     lv_obj_add_event_cb(dialog_close, dialog_close_event_cb, LV_EVENT_CLICKED, NULL);
// }



extern "C" void app_main(void)
{
    printf("Minimum free heap size: %s bytes\n", Utils::NumberWithCommas(esp_get_minimum_free_heap_size()).c_str());

    HAL::HalBase *hal = new HAL::HalTab5();
    hal->Configure();

    // ScanFileSystem("/sdcard/");

    Display *display = new Display(hal);
    display->Configure();
    // esp_lcd_touch_handle_t touchPanelHandle = static_cast<esp_lcd_touch_handle_t>(hal->GetTouchPanelHandle());
    // display->AddTouchPanel(touchPanelHandle);
    // ((HalTab5 *) hal)->SetGpioOutputCapability();
    

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

    // show_dialog("Welcome", "This is a simple LVGL application running on M5Stack Tab5.\n\nPress the button to close this dialog.");

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
