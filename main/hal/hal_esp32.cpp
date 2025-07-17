/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "hal/hal_esp32.h"
extern "C"
{
#include <rx8130.h>
}

#include <mooncake_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <bsp/m5stack_tab5.h>
#include <lv_demos.h>

extern esp_lcd_touch_handle_t _lcd_touch_handle;

static const std::string _tag = "hal";

static void lvgl_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (_lcd_touch_handle == NULL)
    {
        data->state = LV_INDEV_STATE_REL;
        return;
    }

    uint16_t touch_x[1];
    uint16_t touch_y[1];
    uint16_t touch_strength[1];
    uint8_t touch_cnt = 0;

    esp_lcd_touch_read_data(_lcd_touch_handle);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(_lcd_touch_handle, touch_x, touch_y, touch_strength, &touch_cnt, 1);
    // mclog::tagInfo(_tag, "touchpad pressed: {}", touchpad_pressed);

    if (!touchpad_pressed)
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;
        data->point.x = touch_x[0];
        data->point.y = touch_y[0];
    }
}

/* -------------------------------------------------------------------------- */
/*                                    I2C                                     */
/* -------------------------------------------------------------------------- */

extern "C" void bsp_fake_i2c_init(i2c_master_bus_handle_t bus_handle);

/**
 * @brief Configure the I2C bus.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalEsp32::ConfigureI2C()
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

    bsp_fake_i2c_init(_i2cHandle);
    return result;
}

/* -------------------------------------------------------------------------- */
/*                          IO Expander Methods                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the IO Expanders.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalEsp32::ConfigureIoExpanders()
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

void HalEsp32::init()
{
    mclog::tagInfo(_tag, "init");

    // mclog::tagInfo(_tag, "camera init");
    // bsp_cam_osc_init();

    mclog::tagInfo(_tag, "i2c init");
    ConfigureI2C();
    // bsp_i2c_init();

    mclog::tagInfo(_tag, "io expander init");
    // i2c_master_bus_handle_t i2c_bus_handle = bsp_i2c_get_handle();
    // bsp_io_expander_pi4ioe_init(i2c_bus_handle);
    ConfigureIoExpanders();

    // setChargeQcEnable(true);
    // delay(50);
    // setChargeEnable(true);
    // setChargeEnable(false);

    // mclog::tagInfo(_tag, "i2c scan");
    // bsp_i2c_scan();

    // mclog::tagInfo(_tag, "codec init");
    // delay(200);
    // bsp_codec_init();

    // mclog::tagInfo(_tag, "imu init");
    // imu_init();

    // mclog::tagInfo(_tag, "ina226 init");
    // ina226.begin(i2c_bus_handle, 0x41);
    // ina226.configure(INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    // ina226.calibrate(0.005, 8.192);
    // mclog::tagInfo(_tag, "bus voltage: {}", ina226.readBusVoltage());

    // mclog::tagInfo(_tag, "rx8130 init");
    // rx8130.begin(i2c_bus_handle, 0x32);
    // rx8130.initBat();
    // clearRtcIrq();
    // update_system_time();

    mclog::tagInfo(_tag, "display init");
    bsp_reset_tp();
    bsp_display_cfg_t cfg =
    {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * BSP_LCD_V_RES,
        .double_buffer = true,
        .flags = 
        {
#if CONFIG_BSP_LCD_COLOR_FORMAT_RGB888
            .buff_dma = false,
#else
            .buff_dma = true,
#endif
            .buff_spiram = true,
            .sw_rotate = true,
        }
    };
    printf("Buffer size: %u\n", (unsigned int) cfg.buffer_size);
    lvDisp = bsp_display_start_with_config(&cfg);
    lv_display_set_rotation(lvDisp, LV_DISPLAY_ROTATION_90);
    bsp_display_backlight_on();

    // Touchpad lvgl indev
    mclog::tagInfo(_tag, "create lvgl touchpad indev");
    lvTouchpad = lv_indev_create();
    lv_indev_set_type(lvTouchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(lvTouchpad, lvgl_read_cb);
    lv_indev_set_display(lvTouchpad, lvDisp);

    // mclog::tagInfo(_tag, "usb host init");
    // bsp_usb_host_start(BSP_USB_HOST_POWER_MODE_USB_DEV, true);

    // mclog::tagInfo(_tag, "hid init");
    // hid_init();

    // mclog::tagInfo(_tag, "rs485 init");
    // rs485_init();

    // mclog::tagInfo(_tag, "set gpio output capability");
    // set_gpio_output_capability();

    bsp_display_unlock();
}

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

// void HalEsp32::set_gpio_output_capability()
// {
//     // gpio_set_drive_capability((gpio_num_t)48, GPIO_DRIVE_CAP_0);
//     for (int i = 0; i < sizeof(_driver_gpios) / sizeof(_driver_gpios[0]); i++)
//     {
//         gpio_num_t gpio = _driver_gpios[i];
//         esp_err_t ret = gpio_set_drive_capability(gpio, GPIO_DRIVE_CAP_0);
//         if (ret == ESP_OK)
//         {
//             printf("GPIO %d drive capability set to GPIO_DRIVE_CAP_0\n", gpio);
//         }
//         else
//         {
//             printf("Failed to set GPIO %d drive capability: %s\n", gpio, esp_err_to_name(ret));
//         }
//     }
// }

/* -------------------------------------------------------------------------- */
/*                                   Display                                  */
/* -------------------------------------------------------------------------- */
void HalEsp32::setDisplayBrightness(uint8_t brightness)
{
    _current_lcd_brightness = std::clamp((int) brightness, 0, 100);
    bsp_display_brightness_set(_current_lcd_brightness);
}

uint8_t HalEsp32::getDisplayBrightness()
{
    return _current_lcd_brightness;
}

void HalEsp32::lvglLock()
{
    lvgl_port_lock(0);
}

void HalEsp32::lvglUnlock()
{
    lvgl_port_unlock();
}

/* -------------------------------------------------------------------------- */
/*                                  Interface                                 */
/* -------------------------------------------------------------------------- */
bool HalEsp32::usbCDetect()
{
    return bsp_usb_c_detect();
    // return false;
}

bool HalEsp32::headPhoneDetect()
{
    return bsp_headphone_detect();
}

std::vector<uint8_t> HalEsp32::i2cScan(bool isInternal)
{
    i2c_master_bus_handle_t i2c_bus_handle;
    std::vector<uint8_t> addrs;

    if (isInternal)
    {
        i2c_bus_handle = bsp_i2c_get_handle();
    }
    else
    {
        i2c_bus_handle = bsp_ext_i2c_get_handle();
    }

    esp_err_t ret;
    uint8_t address;

    for (int i = 16; i < 128; i += 16)
    {
        for (int j = 0; j < 16; j++)
        {
            fflush(stdout);
            address = i + j;
            ret = i2c_master_probe(i2c_bus_handle, address, 50);
            if (ret == ESP_OK)
            {
                addrs.push_back(address);
            }
        }
    }

    return addrs;
}

void HalEsp32::initPortAI2c()
{
    mclog::tagInfo(_tag, "init port a i2c");
    bsp_ext_i2c_init();
}

void HalEsp32::deinitPortAI2c()
{
    mclog::tagInfo(_tag, "deinit port a i2c");
    bsp_ext_i2c_deinit();
}

void HalEsp32::gpioInitOutput(uint8_t pin)
{
    gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY);
    gpio_set_direction((gpio_num_t) pin, GPIO_MODE_OUTPUT);
}

void HalEsp32::gpioSetLevel(uint8_t pin, bool level)
{
    gpio_set_level((gpio_num_t) pin, level);
}

void HalEsp32::gpioReset(uint8_t pin)
{
    gpio_set_level((gpio_num_t) pin, false);
}
