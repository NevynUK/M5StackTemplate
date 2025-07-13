#include <stdio.h>
#include <math.h>
#include <tuple>
#include <memory>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

#include <esp_system.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_check.h>
#include <esp_timer.h>

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "Display.hpp"

/**
 * @brief Lock LVGL for exclusive access.
 */
void Display::Lock(uint32_t timeout)
{
    lvgl_port_lock(timeout);
}

/**
 * @brief Unlock LVGL.
 */
void Display::Unlock()
{
    lvgl_port_unlock();
}

/**
 * @brief Setup the display and initialise all components.
 */
void Display::Configure(std::unique_ptr<HAL::HalBase> &hal)
{
    ESP_LOGI(COMPONENT_NAME, "Add LCD screen");

    lvgl_port_cfg_t lvglConfig = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvglConfig);

    Lock();

    lvgl_port_display_cfg_t displayConfig = {};
    displayConfig.io_handle = hal->GetPanelIoHandle();
    displayConfig.panel_handle = hal->GetDisplayPanelHandle();
    displayConfig.control_handle = nullptr;
    displayConfig.buffer_size = 30000;
    // displayConfig.buffer_size = hal->GetDisplayHeight() * hal->GetDisplayWidth();
    displayConfig.double_buffer = true;
    displayConfig.hres = hal->GetDisplayWidth();
    displayConfig.vres = hal->GetDisplayHeight();
    displayConfig.monochrome = false;
    /* Rotation values must be same as used in esp_lcd for initial settings of the screen */
    displayConfig.rotation = {};
    displayConfig.rotation.swap_xy = false;
    displayConfig.rotation.mirror_x = false;
    displayConfig.rotation.mirror_y = false;
    displayConfig.color_format = LV_COLOR_FORMAT_RGB565;
    displayConfig.flags = {};
    displayConfig.flags.buff_dma = true;
    displayConfig.flags.buff_spiram = false;
    displayConfig.flags.swap_bytes = false;
    displayConfig.flags.sw_rotate = true;

    lvgl_port_display_dsi_cfg_t dpiConfig = {};
    dpiConfig.flags = {};
    dpiConfig.flags.avoid_tearing = false;

    _lvglDisplayHandle = lvgl_port_add_disp_dsi(&displayConfig, &dpiConfig);
    if (_lvglDisplayHandle == nullptr)
    {
        ESP_LOGE(COMPONENT_NAME, "Failed to add DSI display");
        return;
    }

    ESP_LOGI(COMPONENT_NAME, "Display configured successfully");

    Unlock();

    // TouchPanel::Setup(&_touchPanelHandle);
    // _touchPanelConfiguration.disp = _lvglDisplayHandle;
    // _touchPanelConfiguration.handle = _touchPanelHandle;
    // lvgl_port_add_touch(&_touchPanelConfiguration);

    // SetBrightness(75);
    // Rotate(_lvglDisplayHandle, LV_DISPLAY_ROTATION_90);

    // _screen = lv_scr_act();

    // lv_obj_set_style_bg_color(_screen, lv_color_black(), LV_PART_MAIN);
}

/**
 * @brief Rotate the display.
 *
 * @param display Pointer to the LVGL display.
 * @param direction Rotation direction.
 */
void Display::Rotate(lv_display_rotation_t direction)
{
    if (_lvglDisplayHandle)
    {
        lv_display_set_rotation(_lvglDisplayHandle, direction);
    }
}


// /**
//  * @brief Teardown the display and clean up resources.
//  */
// void Teardown()
// {
// }

// //     // /**
// //     //  * @brief Handle for the touch panel.
// //     //  */
// //     // esp_lcd_touch_handle_t _touchPanelHandle;

// //     // /**
// //     //  * @brief Touch panel configuration.
// //     //  */
// //     // lvgl_port_touch_cfg_t _touchPanelConfiguration;

// //     // /**
// //     //  * @brief LVGL handle for the display.
// //     //  */
// //     // lv_display_t *_lvglDisplayHandle = NULL;

// //     // /**
// //     //  * @brief Pointer to the screen object to be used in drawing operations.
// //     //  */
// //     // lv_obj_t *_screen = nullptr;

// // /**
// //  * @brief Set the brightness of the LCD backlight.
// //  *
// //  * @param brightness_percent Brightness level as a percentage (0-100).
// //  */
// // void SetBrightness(int brightness_percent)
// // {
// //     if (brightness_percent > 100)
// //     {
// //         brightness_percent = 100;
// //     }
// //     if (brightness_percent < 0)
// //     {
// //         brightness_percent = 0;
// //     }

// //     ESP_LOGI(COMPONENT_NAME, "Setting LCD backlight: %d%%", brightness_percent);

// //     uint32_t duty_cycle = (1023 * brightness_percent) / 100;

// //     ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LCD_BACKLIGHT_LEDC_CH, duty_cycle));
// //     ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LCD_BACKLIGHT_LEDC_CH));
// // }

// // /**
// //  * @brief Turn off the LCD backlight.
// //  */
// // void BacklightOff()
// // {
// //     SetBrightness(0);
// // }

// // /**
// //  * @brief Turn on the LCD backlight to full brightness.
// //  */
// // void BacklightOn()
// // {
// //     SetBrightness(100);
// // }

// // /**
// //  * @brief Draw a rectangle filled with the colour white.
// //  *
// //  * @param x X coordinate of the rectangle.
// //  * @param y Y coordinate of the rectangle.
// //  * @param width Width of the rectangle.
// //  * @param height Height of the rectangle.
// //  * @param colour Colour of the rectangle (default to white).
// //  */
// // void DrawFilledRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour)
// // {
// //     if (_screen)
// //     {
// //         lvgl_port_lock(0);

// //         lv_obj_t *rectangle = lv_obj_create(_screen);
// //         lv_obj_set_size(rectangle, width, height);
// //         lv_obj_set_pos(rectangle, x, y);
// //         lv_obj_set_style_radius(rectangle, 0, 0);
// //         lv_obj_set_style_bg_color(rectangle, colour, LV_STATE_DEFAULT);

// //         lvgl_port_unlock();
// //     }
// // }

// // /**
// //  * @brief Draw a box outline on the screen.
// //  *
// //  * @param x X coordinate of the box.
// //  * @param y Y coordinate of the box.
// //  * @param width Width of the box.
// //  * @param height Height of the box.
// //  * @param colour Colour of the box outline.
// //  */
// // void DrawBoxOutline(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour)
// // {
// //     if (_screen)
// //     {
// //         lvgl_port_lock(0);

// //         lv_obj_t *box = lv_obj_create(_screen);
// //         lv_obj_set_size(box, width, height);
// //         lv_obj_set_pos(box, x, y);
// //         lv_obj_set_style_radius(box, 0, 0);
// //         lv_obj_set_style_border_color(box, colour, LV_STATE_DEFAULT);
// //         lv_obj_set_style_border_width(box, 2, LV_STATE_DEFAULT);       // Set the border width as needed
// //         lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, LV_STATE_DEFAULT); // Make the background transparent

// //         lvgl_port_unlock();
// //     }
// // }

// /**
//  * @brief Draw a new label on the screen.
//  *
//  * @param x X offset of the label.
//  * @param y Y offset of the label.
//  * @param text Label text.
//  * @return lv_obj_t* Pointer to the label object.
//  */
// lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text)
// {
//     return DrawLabel(x, y, text, LV_FONT_DEFAULT, lv_color_white());
// }

// /**
//  * @brief Draw a new label on the screen.
//  *
//  * @param x X offset of the label.
//  * @param y Y offset of the label.
//  * @param text Label text.
//  * @param colour Colour of the label (default to white).
//  * @return lv_obj_t* Pointer to the label object.
//  */
// lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, lv_color_t colour)
// {
//     return DrawLabel(x, y, text, LV_FONT_DEFAULT, colour);
// }

// /**
//  * @brief Draw a new label on the screen.
//  *
//  * @param x X offset of the label.
//  * @param y Y offset of the label.
//  * @param text Label text.
//  * @param font Font to use for the label (default to default font specified in config).
//  * @param colour Colour of the label (default to white).
//  * @return lv_obj_t* Pointer to the label object.
//  */
// lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *font, lv_color_t colour)
// {
//     lv_obj_t *label = nullptr;

//     if (_screen)
//     {
//         lvgl_port_lock(0);

//         label = lv_label_create(_screen);
//         lv_label_set_text(label, text);
//         lv_obj_set_style_text_color(label, colour, LV_STATE_DEFAULT);
//         lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);
//         lv_obj_set_pos(label, x, y);

//         lvgl_port_unlock();
//     }
//     return (label);
// }

// /**
//  * @brief Draw a filled circle on the screen.
//  *
//  * @param x X coordinate of the circle center.
//  * @param y Y coordinate of the circle center.
//  * @param radius Radius of the circle.
//  * @param colour Colour of the circle.
//  * @return lv_obj_t* Pointer to the circle object.
//  */
// lv_obj_t *DrawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, lv_color_t colour)
// {
//     lv_obj_t *circle = nullptr;

//     if (_screen)
//     {
//         lvgl_port_lock(0);

//         circle = lv_obj_create(_screen);
//         lv_obj_set_size(circle, 2 * radius, 2 * radius);
//         lv_obj_set_pos(circle, x - radius, y - radius);
//         lv_obj_set_style_radius(circle, radius, 0);
//         lv_obj_set_style_bg_color(circle, colour, LV_STATE_DEFAULT);
//         lv_obj_set_style_border_width(circle, 0, LV_STATE_DEFAULT);

//         lvgl_port_unlock();
//     }

//     return(circle);
// }

// /**
//  * @brief Draw a new button on the screen.
//  *
//  * @param x X offset of the button.
//  * @param y Y offset of the button.
//  * @param width Width of the button.
//  * @param height Height of the button.
//  * @param text Button text.
//  * @param callback Event callback function for the button.
//  * @return tuple<lv_obj_t *, lv_obj_t *> Pointer to the button object and a pointer to the label on the button.
//  */
// std::tuple<lv_obj_t *, lv_obj_t *> DrawButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *text, lv_event_cb_t callback)
// {
//     lv_obj_t *button = nullptr;
//     lv_obj_t *label = nullptr;

//     if (_screen)
//     {
//         lvgl_port_lock(0);

//         button = lv_btn_create(_screen);
//         lv_obj_set_size(button, width, height);
//         lv_obj_set_pos(button, x, y);
//         lv_obj_add_event_cb(button, callback, LV_EVENT_ALL, button);

//         label = lv_label_create(button);
//         lv_label_set_text(label, text);
//         lv_obj_set_style_text_color(label, lv_color_make(255, 255, 0), LV_STATE_DEFAULT);
//         lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

//         lvgl_port_unlock();
//     }
//     return {button, label};
// }

// /**
//  * @brief Set the color of an LVGL object.
//  *
//  * @param object Pointer to the LVGL object.
//  * @param colour Colour to set.
//  */
// void SetColour(lv_obj_t *object, lv_color_t colour)
// {
//     if (object)
//     {
//         lvgl_port_lock(0);
//         lv_obj_set_style_bg_color(object, colour, LV_STATE_DEFAULT);
//         lvgl_port_unlock();
//     }
// }

// /**
//  * @brief Enable an LVGL object.
//  *
//  * @param obj Pointer to the LVGL object.
//  */
// void Enable(lv_obj_t *object)
// {
//     if (object)
//     {
//         lvgl_port_lock(0);
//         lv_obj_clear_state(object, LV_STATE_DISABLED);
//         lvgl_port_unlock();
//     }
// }

// /**
//  * @brief Disable an LVGL object.
//  *
//  * @param obj Pointer to the LVGL object.
//  */
// void Disable(lv_obj_t *object)
// {
//     if (object)
//     {
//         lvgl_port_lock(0);
//         lv_obj_add_state(object, LV_STATE_DISABLED);
//         lvgl_port_unlock();
//     }
// }

// /**
//  * @brief Get the current screen object.
//  *
//  * @return lv_obj_t* Pointer to the current screen object.
//  */
// lv_obj_t *GetCurrentScreen()
// {
//     return _screen;
// }

// /**
//  * @brief Create a new screen object.
//  *
//  * @return lv_obj_t* Pointer to the new screen object.
//  */
// lv_obj_t *CreateNewScreen()
// {
//     ESP_LOGI(COMPONENT_NAME, "CreateNewScreen - Enter");

//     lv_obj_t *new_screen = lv_obj_create(NULL);
//     lv_obj_set_style_bg_color(new_screen, lv_color_black(), LV_PART_MAIN);
//     _screen = new_screen;
//     lv_screen_load(new_screen);

//     ESP_LOGI(COMPONENT_NAME, "CreateNewScreen - Exit");
//     return new_screen;
// }

// /**
//  * @brief Delete the specified screen object.
//  *
//  * @param screen Pointer to the screen object to delete.
//  */
// void DeleteScreen(lv_obj_t *screen)
// {
//     ESP_LOGI(COMPONENT_NAME, "DeleteScreen - Enter");

//     if (screen)
//     {
//         lvgl_port_lock(0);
//         lv_obj_del(screen);
//         lvgl_port_unlock();
//     }

//     ESP_LOGI(COMPONENT_NAME, "DeleteScreen - Exit");
// }
