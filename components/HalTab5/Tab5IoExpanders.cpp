/**
 * @file Tab5IoExpanders.cpp
 * @author your name (you@domain.com)
 * @brief Implementation of I/O expander methods for the Tab5 device.
 * @version 0.1
 * @date 2025-07-19
 * 
 * @copyright Copyright (c) 2025
 */
#include <sdkconfig.h>

#include "esp_err.h"
#include "esp_check.h"

#include "driver/gpio.h"

#include "HalTab5.hpp"

using namespace HAL;

/**
 * @brief Configure the IO Expanders.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::ConfigureIoExpanders()
{
    ConfigureI2C();

    uint8_t write_buf[2] = {};
    uint8_t read_buf[1] = {};

    i2c_device_config_t deviceConfig = {};
    deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    deviceConfig.device_address = I2C_DEV_ADDR_PI4IOE1;
    deviceConfig.scl_speed_hz = 400000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(_i2cHandle, &deviceConfig, &_pi4ioe1Handle));

    write_buf[0] = PI4IO_REG_CHIP_RESET;
    write_buf[1] = 0xff;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    write_buf[0] = PI4IO_REG_CHIP_RESET;
    i2c_master_transmit_receive(_pi4ioe1Handle, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the port direction (0 = input, 1 = output).
    //
    write_buf[0] = PI4IO_REG_IO_DIR;
    write_buf[1] = 0b01111111;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Disable High-Impedance for used pins.
    //
    write_buf[0] = PI4IO_REG_OUT_H_IM;
    write_buf[1] = 0b00000000;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the pull-up/pull-down resistors for the pins (0 = pull down, 1 = pull up).
    //
    write_buf[0] = PI4IO_REG_PULL_SEL;
    write_buf[1] = 0b01111111;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set pull up/down enable ability (0 = enable, 1 = disable).
    //  Port 7 enabled, all others disabled.
    //
    write_buf[0] = PI4IO_REG_PULL_EN;
    write_buf[1] = 0b01111111;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the output ports high.
    //
    //  P1(SPK_EN), P2(EXT5V_EN), P4(LCD_RST), P5(TP_RST), P6(CAM)RST
    //
    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = 0b01110110;
    i2c_master_transmit(_pi4ioe1Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Now for the second IO expander.
    //
    deviceConfig.device_address = I2C_DEV_ADDR_PI4IOE2;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(_i2cHandle, &deviceConfig, &_pi4ioe2Handle));

    write_buf[0] = PI4IO_REG_CHIP_RESET;
    write_buf[1] = 0xFF;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    write_buf[0] = PI4IO_REG_CHIP_RESET;
    i2c_master_transmit_receive(_pi4ioe2Handle, write_buf, 1, read_buf, 1, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the port direction (0 = input, 1 = output).
    //
    write_buf[0] = PI4IO_REG_IO_DIR;
    write_buf[1] = 0b10111001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Disable High-Impedance for used pins.
    //
    write_buf[0] = PI4IO_REG_OUT_H_IM;
    write_buf[1] = 0b00000110;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the pull-up/pull-down resistors for the pins (0 = pull down, 1 = pull up).
    //
    write_buf[0] = PI4IO_REG_PULL_SEL;
    write_buf[1] = 0b10111001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set pull up/down enable ability (0 = enable, 1 = disable).
    //
    write_buf[0] = PI4IO_REG_PULL_EN;
    write_buf[1] = 0b11111001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the input default state (0 = low, 1 = high).
    //
    write_buf[0] = PI4IO_REG_IN_DEF_STA;
    write_buf[1] = 0b01000000;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS); // P6 default high level
    //
    //  Set interrupt ability (0 = enable, 1 = disable).
    //  Port 6 enabled, all others disabled.
    //
    write_buf[0] = PI4IO_REG_INT_MASK;
    write_buf[1] = 0b10111111;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);
    //
    //  Set the output ports high.
    //
    //  P0(WLAN_PWR_EN), P3(USB5V_EN)
    //
    write_buf[0] = PI4IO_REG_OUT_SET;
    write_buf[1] = 0b00001001;
    i2c_master_transmit(_pi4ioe2Handle, write_buf, 2, I2C_MASTER_TIMEOUT_MS);

    return ESP_OK;
}
