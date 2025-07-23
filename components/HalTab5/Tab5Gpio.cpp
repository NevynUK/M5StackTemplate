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

void HalTab5::gpioInitOutput(uint8_t pin)
{
    gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t) pin, GPIO_MODE_OUTPUT);
}

void HalTab5::gpioSetLevel(uint8_t pin, bool level)
{
    gpio_set_level((gpio_num_t) pin, level);
}

void HalTab5::gpioReset(uint8_t pin)
{
    gpio_set_level((gpio_num_t) pin, false);
}
