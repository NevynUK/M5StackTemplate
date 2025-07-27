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

void set_gpio_output_capability()
{
    // gpio_set_drive_capability((gpio_num_t)48, GPIO_DRIVE_CAP_0);
    for (int i = 0; i < sizeof(_driver_gpios) / sizeof(_driver_gpios[0]); i++) {
        gpio_num_t gpio = _driver_gpios[i];
        esp_err_t ret   = gpio_set_drive_capability(gpio, GPIO_DRIVE_CAP_0);
        if (ret == ESP_OK) {
            printf("GPIO %d drive capability set to GPIO_DRIVE_CAP_0\n", gpio);
        } else {
            printf("Failed to set GPIO %d drive capability: %s\n", gpio, esp_err_to_name(ret));
        }
    }
}

/**
 * @brief Configure and initialise the hardware.
 */
void HalTab5::Configure()
{
    ConfigureI2C();

    ESP_ERROR_CHECK(ConfigureIoExpanders());
    ESP_ERROR_CHECK(ConfigureDisplayBrightnessControl());
    ESP_ERROR_CHECK(ConfigureSdCard());
    ESP_ERROR_CHECK(ConfigureTouchPanel());
    ESP_ERROR_CHECK(ConfigureDisplay());
    set_gpio_output_capability();
}
