#pragma once

#include <esp_err.h>
#include "HalBase.hpp"
#include "HalTab5.hpp"

class TouchPanel
{
private:
    esp_err_t Configure(esp_lcd_touch_handle_t *tp);

public:
}; // class TouchPanel
