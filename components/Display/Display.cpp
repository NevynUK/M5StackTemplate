#include <stdio.h>
#include <tuple>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

// #include <esp_system.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_check.h>
// #include <esp_lcd_panel_io.h>
// #include <esp_lcd_panel_vendor.h>
// #include <esp_lcd_panel_ops.h>
// #include <esp_timer.h>
// #include <driver/gpio.h>
// #include <driver/ledc.h>
// #include <driver/spi_master.h>
// #include <esp_lcd_ili9341.h>

#include <lvgl.h>
#include <esp_lvgl_port.h>

// #include <esp_lcd_touch.h>
// #include <esp_lcd_touch_xpt2046.h>

// #include "TouchPanel.hpp"
#include "Display.hpp"
#include "HalBase.hpp"
#include "HalTab5.hpp"

/**
 * @brief Set the brightness of the LCD backlight.
 *
 * @param brightnessPercent Brightness level as a percentage (0-100).
 */
void Display::SetBrightness(uint8_t brightnessPercent)
{
    if (_halBase)
    {
        _halBase->SetDisplayBrightness(brightnessPercent);
    }
}

/**
 * @brief Turn off the LCD backlight.
 */
void Display::BacklightOff()
{
    SetBrightness(0);
}

/**
 * @brief Turn on the LCD backlight to full brightness.
 */
void Display::BacklightOn()
{
    SetBrightness(100);
}

/**
 * @brief Rotate the display.
 *
 * @param lvgl_disp Pointer to the LVGL display.
 * @param dir Rotation direction.
 */
void Display::Rotate(lv_display_t *lvgl_disp, lv_display_rotation_t dir)
{
    if (lvgl_disp)
    {
        lv_display_set_rotation(lvgl_disp, dir);
    }
}

/**
 * @brief Draw a rectangle filled with the colour white.
 *
 * @param x X coordinate of the rectangle.
 * @param y Y coordinate of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 * @param colour Colour of the rectangle (default to white).
 */
void Display::DrawFilledRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour)
{
    if (_screen)
    {
        lvgl_port_lock(0);

        lv_obj_t *rectangle = lv_obj_create(_screen);
        lv_obj_set_size(rectangle, width, height);
        lv_obj_set_pos(rectangle, x, y);
        lv_obj_set_style_radius(rectangle, 0, 0);
        lv_obj_set_style_bg_color(rectangle, colour, LV_STATE_DEFAULT);

        lvgl_port_unlock();
    }
}

/**
 * @brief Draw a box outline on the screen.
 *
 * @param x X coordinate of the box.
 * @param y Y coordinate of the box.
 * @param width Width of the box.
 * @param height Height of the box.
 * @param colour Colour of the box outline.
 */
void Display::DrawBoxOutline(uint32_t x, uint32_t y, uint32_t width, uint32_t height, lv_color_t colour)
{
    if (_screen)
    {
        lvgl_port_lock(0);

        lv_obj_t *box = lv_obj_create(_screen);
        lv_obj_set_size(box, width, height);
        lv_obj_set_pos(box, x, y);
        lv_obj_set_style_radius(box, 0, 0);
        lv_obj_set_style_border_color(box, colour, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(box, 2, LV_STATE_DEFAULT);       // Set the border width as needed
        lv_obj_set_style_bg_opa(box, LV_OPA_TRANSP, LV_STATE_DEFAULT); // Make the background transparent

        lvgl_port_unlock();
    }
}

/**
 * @brief Draw a new label on the screen.
 *
 * @param x X offset of the label.
 * @param y Y offset of the label.
 * @param text Label text.
 * @return lv_obj_t* Pointer to the label object.
 */
lv_obj_t *Display::DrawLabel(uint32_t x, uint32_t y, const char *text)
{
    return DrawLabel(x, y, text, LV_FONT_DEFAULT, lv_color_white());
}

/**
 * @brief Draw a new label on the screen.
 *
 * @param x X offset of the label.
 * @param y Y offset of the label.
 * @param text Label text.
 * @param colour Colour of the label (default to white).
 * @return lv_obj_t* Pointer to the label object.
 */
lv_obj_t *Display::DrawLabel(uint32_t x, uint32_t y, const char *text, lv_color_t colour)
{
    return DrawLabel(x, y, text, LV_FONT_DEFAULT, colour);
}

/**
 * @brief Draw a new label on the screen.
 *
 * @param x X offset of the label.
 * @param y Y offset of the label.
 * @param text Label text.
 * @param font Font to use for the label (default to default font specified in config).
 * @param colour Colour of the label (default to white).
 * @return lv_obj_t* Pointer to the label object.
 */
lv_obj_t *Display::DrawLabel(uint32_t x, uint32_t y, const char *text, const lv_font_t *font, lv_color_t colour)
{
    lv_obj_t *label = nullptr;

    if (_screen)
    {
        lvgl_port_lock(0);

        label = lv_label_create(_screen);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label, colour, LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label, font, LV_STATE_DEFAULT);
        lv_obj_set_pos(label, x, y);

        lvgl_port_unlock();
    }
    return (label);
}

/**
 * @brief Draw a filled circle on the screen.
 *
 * @param x X coordinate of the circle center.
 * @param y Y coordinate of the circle center.
 * @param radius Radius of the circle.
 * @param colour Colour of the circle.
 * @return lv_obj_t* Pointer to the circle object.
 */
lv_obj_t *Display::DrawFilledCircle(uint32_t x, uint32_t y, uint32_t radius, lv_color_t colour)
{
    lv_obj_t *circle = nullptr;

    if (_screen)
    {
        lvgl_port_lock(0);

        circle = lv_obj_create(_screen);
        lv_obj_set_size(circle, 2 * radius, 2 * radius);
        lv_obj_set_pos(circle, x - radius, y - radius);
        lv_obj_set_style_radius(circle, radius, 0);
        lv_obj_set_style_bg_color(circle, colour, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(circle, 0, LV_STATE_DEFAULT);

        lvgl_port_unlock();
    }

    return (circle);
}

/**
 * @brief Draw a new button on the screen.
 *
 * @param x X offset of the button.
 * @param y Y offset of the button.
 * @param width Width of the button.
 * @param height Height of the button.
 * @param text Button text.
 * @param callback Event callback function for the button.
 * @return tuple<lv_obj_t *, lv_obj_t *> Pointer to the button object and a pointer to the label on the button.
 */
std::tuple<lv_obj_t *, lv_obj_t *> Display::DrawButton(uint32_t x, uint32_t y, uint32_t width, uint32_t height, const char *text, lv_event_cb_t callback)
{
    lv_obj_t *button = nullptr;
    lv_obj_t *label = nullptr;

    if (_screen)
    {
        lvgl_port_lock(0);

        button = lv_btn_create(_screen);
        lv_obj_set_size(button, width, height);
        lv_obj_set_pos(button, x, y);
        lv_obj_add_event_cb(button, callback, LV_EVENT_ALL, button);

        label = lv_label_create(button);
        lv_label_set_text(label, text);
        lv_obj_set_style_text_color(label, lv_color_make(255, 255, 0), LV_STATE_DEFAULT);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        lvgl_port_unlock();
    }
    return {button, label};
}

/**
 * @brief Set the color of an LVGL object.
 *
 * @param object Pointer to the LVGL object.
 * @param colour Colour to set.
 */
void Display::SetColour(lv_obj_t *object, lv_color_t colour)
{
    if (object)
    {
        lvgl_port_lock(0);
        lv_obj_set_style_bg_color(object, colour, LV_STATE_DEFAULT);
        lvgl_port_unlock();
    }
}

/**
 * @brief Enable an LVGL object.
 *
 * @param obj Pointer to the LVGL object.
 */
void Display::Enable(lv_obj_t *object)
{
    if (object)
    {
        lvgl_port_lock(0);
        lv_obj_clear_state(object, LV_STATE_DISABLED);
        lvgl_port_unlock();
    }
}

/**
 * @brief Disable an LVGL object.
 *
 * @param obj Pointer to the LVGL object.
 */
void Display::Disable(lv_obj_t *object)
{
    if (object)
    {
        lvgl_port_lock(0);
        lv_obj_add_state(object, LV_STATE_DISABLED);
        lvgl_port_unlock();
    }
}

/**
 * @brief Get the current screen object.
 *
 * @return lv_obj_t* Pointer to the current screen object.
 */
lv_obj_t *Display::GetCurrentScreen()
{
    return _screen;
}

/**
 * @brief Create a new screen object.
 *
 * @return lv_obj_t* Pointer to the new screen object.
 */
lv_obj_t *Display::CreateNewScreen()
{
    ESP_LOGI(COMPONENT_NAME, "CreateNewScreen - Enter");

    lv_obj_t *new_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(new_screen, lv_color_black(), LV_PART_MAIN);
    _screen = new_screen;
    lv_screen_load(new_screen);

    ESP_LOGI(COMPONENT_NAME, "CreateNewScreen - Exit");
    return new_screen;
}

/**
 * @brief Delete the specified screen object.
 *
 * @param screen Pointer to the screen object to delete.
 */
void Display::DeleteScreen(lv_obj_t *screen)
{
    ESP_LOGI(COMPONENT_NAME, "DeleteScreen - Enter");

    if (screen)
    {
        lvgl_port_lock(0);
        lv_obj_del(screen);
        lvgl_port_unlock();
    }

    ESP_LOGI(COMPONENT_NAME, "DeleteScreen - Exit");
}

/**
 * @brief Configure the display and initialise all components.
 */
void Display::Configure()
{
    lvgl_port_cfg_t lvglConfig = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvglConfig);

    lvgl_port_display_cfg_t disp_cfg = {};
    disp_cfg.io_handle = static_cast<HalTab5 *>(_halBase)->GetIoHandle();
    disp_cfg.panel_handle = static_cast<HalTab5 *>(_halBase)->GetPanelHandle();
    disp_cfg.control_handle = static_cast<HalTab5 *>(_halBase)->GetControlHandle();
    disp_cfg.buffer_size = _halBase->GetDisplayHeight() * _halBase->GetDisplayWidth();
    disp_cfg.double_buffer = true;
    disp_cfg.hres = _halBase->GetDisplayWidth();
    disp_cfg.vres = _halBase->GetDisplayHeight();
    disp_cfg.monochrome = false;
    disp_cfg.rotation = {};
    disp_cfg.rotation.swap_xy = false;
    disp_cfg.rotation.mirror_x = false;
    disp_cfg.rotation.mirror_y = false;
    disp_cfg.color_format = LV_COLOR_FORMAT_RGB565;
    disp_cfg.flags = {};
    disp_cfg.flags.buff_dma = true;
    disp_cfg.flags.buff_spiram = true;
    disp_cfg.flags.swap_bytes = _halBase->IsDisplayBigEndian();
    disp_cfg.flags.sw_rotate = true;

    lvgl_port_display_dsi_cfg_t dpi_cfg = {};
    dpi_cfg.flags.avoid_tearing = false;

    _displayHandle = lvgl_port_add_disp_dsi(&disp_cfg, &dpi_cfg);
    Rotate(_displayHandle, LV_DISPLAY_ROTATION_90);
    _halBase->SetDisplayBrightness(100);

    _screen = lv_scr_act();

    lv_obj_set_style_bg_color(_screen, lv_color_black(), LV_PART_MAIN);

    lvgl_port_unlock();
}
