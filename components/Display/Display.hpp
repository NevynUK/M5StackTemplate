/**
 * @file Display.hpp
 * @author Mark Stevens
 * @brief This provides the primitive methods for setting up and controlling the display.
 * 
 * This class provides a high-level interface for display operations on the M5Stack Tab5
 * using the LVGL graphics library. It handles display initialization, drawing primitives,
 * UI elements creation, and display management functions.
 * 
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

/**
 * @brief Display controller class for M5Stack Tab5.
 * 
 * This class provides a comprehensive interface for controlling the display hardware
 * and rendering graphics using the LVGL library. It supports various drawing operations,
 * UI element creation, and display management functions.
 * 
 * Features:
 * - Display hardware initialization and configuration
 * - Backlight control and brightness adjustment
 * - Drawing primitives (rectangles, circles, labels)
 * - UI elements (buttons with callbacks)
 * - Screen management (create, delete, switch)
 * - Thread-safe display locking mechanism
 * - Display rotation support
 * 
 * The class uses the hardware abstraction layer (HalBase) to access display-specific
 * hardware functionality while providing a consistent interface for application code.
 */
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
    explicit Display(HalBase *halBase);

    /**
     * @brief Default destructor.
     */
    ~Display() = default;

    /**
     * @brief Initialize and configure the display hardware and LVGL.
     * 
     * Sets up the display controller, initializes LVGL graphics library,
     * and prepares the display for rendering operations.
     */
    void Configure();

    /**
     * @brief Lock the display for exclusive access during drawing operations.
     * 
     * @param timeout Maximum time to wait for the lock in milliseconds (0 = wait forever)
     */
    void Lock(uint32_t timeout = 0);

    /**
     * @brief Unlock the display to allow other tasks to access it.
     */
    void Unlock();

    /**
     * @brief Set the display backlight brightness.
     * 
     * @param brightnessPercent Brightness level as a percentage (0-100)
     */
    void SetBrightness(uint8_t brightnessPercent);

    /**
     * @brief Turn off the display backlight.
     */
    void BacklightOff();

    /**
     * @brief Turn on the display backlight.
     */
    void BacklightOn();

    /**
     * @brief Rotate the display orientation.
     * 
     * @param lvgl_disp Pointer to the LVGL display object
     * @param dir Rotation direction (0°, 90°, 180°, 270°)
     */
    void Rotate(lv_display_t *lvgl_disp, lv_display_rotation_t dir);

    /**
     * @brief Draw a filled rectangle on the display.
     * 
     * @param x X coordinate of the top-left corner
     * @param y Y coordinate of the top-left corner
     * @param width Width of the rectangle in pixels
     * @param height Height of the rectangle in pixels
     * @param colour Fill color (default: white)
     */
    void DrawFilledRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour = lv_color_white());

    /**
     * @brief Draw a rectangle outline (border only) on the display.
     * 
     * @param x X coordinate of the top-left corner
     * @param y Y coordinate of the top-left corner
     * @param width Width of the rectangle in pixels
     * @param height Height of the rectangle in pixels
     * @param colour Border color (default: white)
     */
    void DrawBoxOutline(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour = lv_color_white());

    /**
     * @brief Draw a filled circle on the display.
     * 
     * @param x X coordinate of the circle center
     * @param y Y coordinate of the circle center
     * @param radius Radius of the circle in pixels
     * @param colour Fill color (default: white)
     * @return lv_obj_t* Pointer to the created LVGL object
     */
    lv_obj_t *DrawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, lv_color_t colour = lv_color_white());

    /**
     * @brief Draw a text label on the display using default font and color.
     * 
     * @param x X coordinate of the label position
     * @param y Y coordinate of the label position
     * @param text Text string to display
     * @return lv_obj_t* Pointer to the created LVGL label object
     */
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text);

    /**
     * @brief Draw a text label on the display with specified color.
     * 
     * @param x X coordinate of the label position
     * @param y Y coordinate of the label position
     * @param text Text string to display
     * @param colour Text color
     * @return lv_obj_t* Pointer to the created LVGL label object
     */
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, lv_color_t colour);

    /**
     * @brief Draw a text label on the display with specified font.
     * 
     * @param x X coordinate of the label position
     * @param y Y coordinate of the label position
     * @param text Text string to display
     * @param font Pointer to the LVGL font to use
     * @return lv_obj_t* Pointer to the created LVGL label object
     */
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *font);

    /**
     * @brief Draw a text label on the display with specified font and color.
     * 
     * @param x X coordinate of the label position
     * @param y Y coordinate of the label position
     * @param text Text string to display
     * @param font Pointer to the LVGL font to use
     * @param colour Text color
     * @return lv_obj_t* Pointer to the created LVGL label object
     */
    lv_obj_t *DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *font, lv_color_t colour);

    /**
     * @brief Create a button with text and event callback.
     * 
     * @param x X coordinate of the button position
     * @param y Y coordinate of the button position
     * @param width Width of the button in pixels
     * @param height Height of the button in pixels
     * @param text Text to display on the button
     * @param callback Event callback function for button press events
     * @return std::tuple<lv_obj_t *, lv_obj_t *> Tuple containing button object and label object
     */
    std::tuple<lv_obj_t *, lv_obj_t *> DrawButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *text, lv_event_cb_t callback);

    /**
     * @brief Set the color of an LVGL object.
     * 
     * @param object Pointer to the LVGL object
     * @param colour Color to apply to the object
     */
    void SetColour(lv_obj_t *object, lv_color_t colour);

    /**
     * @brief Enable an LVGL object (make it interactive).
     * 
     * @param object Pointer to the LVGL object to enable
     */
    void Enable(lv_obj_t *object);

    /**
     * @brief Disable an LVGL object (make it non-interactive).
     * 
     * @param object Pointer to the LVGL object to disable
     */
    void Disable(lv_obj_t *object);

    /**
     * @brief Get the currently active screen object.
     * 
     * @return lv_obj_t* Pointer to the current screen object
     */
    lv_obj_t *GetCurrentScreen();

    /**
     * @brief Create a new screen object.
     * 
     * @return lv_obj_t* Pointer to the newly created screen object
     */
    lv_obj_t *CreateNewScreen();

    /**
     * @brief Delete a screen object and free its memory.
     * 
     * @param screen Pointer to the screen object to delete
     */
    void DeleteScreen(lv_obj_t *screen);

private:
    /**
     * @brief Pointer to the hardware abstraction layer for accessing display hardware.
     */
    HAL::HalBase *_halBase;

    /**
     * @brief Handle to the LVGL display driver instance.
     */
    lv_disp_t *_displayHandle = nullptr;

    /**
     * @brief Pointer to the screen object to be used in drawing operations.
     */
    lv_obj_t *_screen = nullptr;

    lv_indev_t* _touchpad = nullptr;
    lv_indev_t *_inputDevice = nullptr;

    /**
     * @brief Default constructor is deleted to enforce dependency injection.
     */
    Display() = delete;

    /**
     * @brief Copy constructor is deleted to prevent copying.
     */
    Display(const Display &) = delete;

    /**
     * @brief Move constructor is deleted to prevent moving.
     */
    Display(Display &&) = delete;

    /**
     * @brief Copy assignment operator is deleted to prevent assignment.
     */
    Display &operator=(const Display &) = delete;
   
    static void lvgl_read_cb(lv_indev_t *indev, lv_indev_data_t *data);
}; // class Display