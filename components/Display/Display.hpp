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

#include <tuple>

#include <lvgl.h>
#include <esp_lvgl_port.h>

#include "HalBase.hpp"

using namespace HAL;

class Display
{
public:
    /**
     * @brief Name of this class for logging purposes.
     */
    const char *COMPONENT_NAME = "Display";

    /**
     * @brief Constructor for the Display class.
     * 
     * @param halBase Pointer to the HalBase instance for hardware access.
     */
    explicit Display(HalBase *halBase) : _halBase(halBase) {}
    ~Display() = default;

    void Configure();
    void SetBrightness(uint8_t brightnessPercent);
    void BacklightOff();
    void BacklightOn();
    void Rotate(lv_display_t *lvgl_disp, lv_display_rotation_t dir);
    void DrawFilledRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour = lv_color_white());
    void DrawBoxOutline(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour = lv_color_white());
    lv_obj_t *DrawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, lv_color_t colour = lv_color_white());
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text);
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, lv_color_t colour);
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *);
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *, lv_color_t colour);
    std::tuple<lv_obj_t *, lv_obj_t *> DrawButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *text, lv_event_cb_t callback);
    void SetColour(lv_obj_t *object, lv_color_t colour);
    void Enable(lv_obj_t *object);
    void Disable(lv_obj_t *object);
    lv_obj_t *GetCurrentScreen();
    lv_obj_t *CreateNewScreen();
    void DeleteScreen(lv_obj_t *screen);

 
private:
    HAL::HalBase *_halBase;

    lv_disp_t *_displayHandle = nullptr;

    /**
     * @brief Pointer to the screen object to be used in drawing operations.
     */
    lv_obj_t *_screen = nullptr;

    //
    //  Disable the default constructor, copy constructor, move and assignment operator.
    //
    Display() = delete;
    Display(const Display &) = delete;
    Display(Display &&) = delete;
    Display &operator=(const Display &) = delete;
}; // class Display