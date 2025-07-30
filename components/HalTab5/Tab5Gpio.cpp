/**
 * @file Tab5Gpio.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <sdkconfig.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"

#include "HalTab5.hpp"

using namespace HAL;

static const gpio_num_t _driver_gpios[] = {
    // EXT I2C
    GPIO_NUM_0,
    GPIO_NUM_1,
    // esp-hosted esp32c6
    GPIO_NUM_8,
    GPIO_NUM_9,
    GPIO_NUM_10,
    GPIO_NUM_11,
    GPIO_NUM_12,
    GPIO_NUM_13,
    GPIO_NUM_15,
    // Display
    GPIO_NUM_22,
    GPIO_NUM_23,
    // Audio
    GPIO_NUM_26,
    GPIO_NUM_27,
    GPIO_NUM_28,
    GPIO_NUM_29,
    GPIO_NUM_30,
    // SYS I2C
    GPIO_NUM_31,
    GPIO_NUM_32,
    // uSD card
    GPIO_NUM_39,
    GPIO_NUM_40,
    GPIO_NUM_41,
    GPIO_NUM_42,
    GPIO_NUM_43,
    GPIO_NUM_44,
};

/**
 * @brief Set GPIO drive capability.
 */
void HalTab5::SetGpioOutputCapability()
{
    ESP_LOGI(COMPONENT_NAME, "Setting GPIO output capability for driver GPIOs");
    for (int i = 0; i < sizeof(_driver_gpios) / sizeof(_driver_gpios[0]); i++) {
        gpio_num_t gpio = _driver_gpios[i];
        esp_err_t ret   = gpio_set_drive_capability(gpio, GPIO_DRIVE_CAP_0);
        if (ret != ESP_OK)
        {
            ESP_LOGE(COMPONENT_NAME, "Failed to set GPIO %d drive capability: %s", gpio, esp_err_to_name(ret));
        }
    }
}

/**
 * @brief Initialize GPIO pin as output.
 */
void HalTab5::GpioConfiguresAsOutput(uint8_t pin)
{
    gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t) pin, GPIO_MODE_OUTPUT);
}

/**
 * @brief Set GPIO level.
 *
 * @param pin Pin to set the level.
 * @param level Level to set (true = high, false = low).
 */
void HalTab5::GpioSetLevel(uint8_t pin, bool level)
{
    gpio_set_level((gpio_num_t) pin, level);
}

/**
 * @brief Reset the GPIO pin to low.
 * 
 * @param pin Pin to set as low.
 */
void HalTab5::GpioReset(uint8_t pin)
{
    gpio_set_level((gpio_num_t) pin, false);
}
