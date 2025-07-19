/**
 * @file HalTab5.cpp
 * @author Mark Stevens
 * @brief Generic HAL methods for the Tab5 device.
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */
#include <sdkconfig.h>

#include "esp_err.h"
#include "esp_check.h"

#include "HalTab5.hpp"

using namespace HAL;

/**
 * @brief Configure and initialise the hardware.
 */
void HalTab5::Configure()
{
    ConfigureI2C();

    ConfigureIoExpanders();
    InitialiseBrightnessControl();
    ESP_ERROR_CHECK(ConfigureDisplay());
}

