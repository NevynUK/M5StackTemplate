/**
 * @file Display.hpp
 * @author Mark Stevens
 * @brief This provides the primitive methods for setting up and controlling the display.
 * @version 0.1
 * @date 2025-03-02
 *
 * @copyright Copyright (c) 2025
 */
#pragma once

#include <sdkconfig.h>

#include <tuple>
#include <memory>

#include <lvgl.h>
#include <esp_lvgl_port.h>
#include "esp_lcd_mipi_dsi.h"

#include "HalBase.h"

class Display
{
public:
    Display() = default;
    ~Display() = default;

    /**
     * @brief Lock LVGL for exclusive access.
     */
    void Lock(uint32_t timeout = 0);

    /**
     * @brief Unlock LVGL.
     */
    void Unlock();

    /**
     * @brief Configure the display using the parameters from the hardware abstraction layer.
     *
     * @param hal A unique pointer to the hardware abstraction layer instance.
     */
    void Configure(std::unique_ptr<HAL::HalBase> &hal);

    /**
     * @brief Rotate the display in the specified direction.
     *
     * @param display Pointer to the LVGL display.
     * @param direction Rotation direction.
     */
    void Rotate(lv_display_rotation_t direction);

    // void Teardown();
    // void SetBrightness(int brightness_percent);
    // void BacklightOff();
    // void BacklightOn();
    // void Rotate(lv_display_t *lvgl_disp, lv_display_rotation_t dir);
    // void DrawFilledRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour = lv_color_white());
    // void DrawBoxOutline(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour = lv_color_white());
    // lv_obj_t *DrawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, lv_color_t colour = lv_color_white());
    // lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text);
    // lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, lv_color_t colour);
    // lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *);
    // lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *, lv_color_t colour);
    // std::tuple<lv_obj_t *, lv_obj_t *> DrawButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *text, lv_event_cb_t callback);
    // void SetColour(lv_obj_t *object, lv_color_t colour);
    // void Enable(lv_obj_t *obectj);
    // void Disable(lv_obj_t *object);
    // lv_obj_t *GetCurrentScreen();
    // lv_obj_t *CreateNewScreen();
    // void DeleteScreen(lv_obj_t *screen);

private:
    /**
     * @brief Name of this class.
     *
     * This is used for logging and debugging purposes.
     */
    static constexpr const char *COMPONENT_NAME = "Display";

    lv_display_t *_lvglDisplayHandle = nullptr; // Handle to the LVGL display
}; // class Display
