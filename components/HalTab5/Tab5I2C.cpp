/**
 * @file Tab5I2C.cpp
 * @author Mark Stevens
 * @brief Implementation of I2C methods for the Tab5 device.
 * @version 0.1
 * @date 2025-07-19
 *
 * @copyright Copyright (c) 2025
 */
#include <sdkconfig.h>

#include "esp_log.h"
#include "esp_err.h"

#include "HalTab5.hpp"

using namespace HAL;

/**
 * @brief Configure the I2C bus.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::ConfigureI2C()
{
    if (_i2cHandle)
    {
        return ESP_OK;
    }

    i2c_master_bus_config_t i2c_bus_conf = {};
    i2c_bus_conf.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_conf.sda_io_num = MASTER_I2C_SDA_GPIO;
    i2c_bus_conf.scl_io_num = MASTER_I2C_SCL_GPIO;
    i2c_bus_conf.i2c_port = MASTER_I2C_PORT_NUMBER;
    i2c_bus_conf.flags.enable_internal_pullup = true;
    esp_err_t result = i2c_new_master_bus(&i2c_bus_conf, &_i2cHandle);
    if (result != ESP_OK)
    {
        ESP_LOGE(COMPONENT_NAME, "Failed to create I2C master bus");
        _i2cHandle = nullptr;
    }

    return result;
}
