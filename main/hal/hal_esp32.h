/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "esp_err.h"
#include "esp_log.h"

#include <HalBase.h>
#include <ina226.hpp>
#include <lvgl.h>
#include <rx8130.h>

class HalEsp32 : public hal::HalBase
{
public:
    const char *COMPONENT_NAME = "HalEsp32";

    std::string type() override
    {
        return "Tab5";
    }

    /* -------------------------------------------------------------------------- */
    /*                                    I2C                                     */
    /* -------------------------------------------------------------------------- */

    /**
     * @brief Configure the I2C bus.
     *
     * @return esp_err_t ESP_OK on success, or an error code on failure.
     */
    esp_err_t ConfigureI2C();

    /* -------------------------------------------------------------------------- */
    /*                               IO Expanders                                 */
    /* -------------------------------------------------------------------------- */

    /**
     * @brief Configure the IO Expanders.
     *
     * @return esp_err_t ESP_OK on success, or an error code on failure.
     */
    esp_err_t ConfigureIoExpanders();

    /* -------------------------------------------------------------------------- */
    /*                            Display Methods                                 */
    /* -------------------------------------------------------------------------- */

    void init() override;

    INA226 ina226;
    RX8130_Class rx8130;
    lv_disp_t *lvDisp = nullptr;
    lv_indev_t *lvKeyboard = nullptr;

    void setDisplayBrightness(uint8_t brightness) override;
    uint8_t getDisplayBrightness() override;

    void lvglLock() override;
    void lvglUnlock() override;

    void updatePowerMonitorData() override;
    void updateImuData() override;
    void clearImuIrq() override;

    void clearRtcIrq() override;
    void setRtcTime(tm time) override;

    void setChargeQcEnable(bool enable) override;
    bool getChargeQcEnable() override;
    void setChargeEnable(bool enable) override;
    bool getChargeEnable() override;
    void setUsb5vEnable(bool enable) override;
    bool getUsb5vEnable() override;
    void setExt5vEnable(bool enable) override;
    bool getExt5vEnable() override;
    void powerOff() override;
    void sleepAndTouchWakeup() override;
    void sleepAndShakeWakeup() override;
    void sleepAndRtcWakeup() override;

    void startCameraCapture(lv_obj_t *imgCanvas) override;
    void stopCameraCapture() override;
    bool isCameraCapturing() override;

    void setSpeakerVolume(uint8_t volume) override;
    uint8_t getSpeakerVolume() override;
    void audioRecord(std::vector<int16_t> &data, uint16_t durationMs, float gain = 80.0f) override;
    void audioPlay(std::vector<int16_t> &data, bool async = true) override;
    void startDualMicRecordTest() override;
    MicTestState_t getDualMicRecordTestState() override;
    void startHeadphoneMicRecordTest() override;
    MicTestState_t getHeadphoneMicRecordTestState() override;
    void startPlayMusicTest() override;
    MusicPlayState_t getMusicPlayTestState() override;
    void stopPlayMusicTest() override;
    void playStartupSfx() override;
    void playShutdownSfx() override;

    void setExtAntennaEnable(bool enable) override;
    bool getExtAntennaEnable() override;
    void startWifiAp() override;

    bool isSdCardMounted() override;
    std::vector<FileEntry_t> scanSdCard(const std::string &dirPath) override;

    bool usbCDetect() override;
    bool usbADetect() override;
    bool headPhoneDetect() override;
    std::vector<uint8_t> i2cScan(bool isInternal) override;
    void initPortAI2c() override;
    void deinitPortAI2c() override;
    void gpioInitOutput(uint8_t pin) override;
    void gpioSetLevel(uint8_t pin, bool level) override;
    void gpioReset(uint8_t pin) override;

private:
    /* -------------------------------------------------------------------------- */
    /*                        Private I2C Methods and Data                        */
    /* -------------------------------------------------------------------------- */

    /**
     * @brief Port number for the I2C master bus.
     */
    const i2c_port_num_t MASTER_I2C_PORT_NUMBER = 0;

    /**
     * @brief GPIO number for the I2C master SDA line.
     */
    const gpio_num_t MASTER_I2C_SDA_GPIO = GPIO_NUM_31;

    /**
     * @brief GPIO number for the I2C master SCL line.
     */
    const gpio_num_t MASTER_I2C_SCL_GPIO = GPIO_NUM_32;

    /**
     * @brief I2C master bus handle.
     */
    i2c_master_bus_handle_t _i2cHandle = nullptr;

    /* -------------------------------------------------------------------------- */
    /*                    Private IO Expanders Data and Methods                   */
    /*                                                                            */
    /* Chip: PI4IOE5V6416                                                         */
    /* Datasheet: https://www.diodes.com/datasheet/download/PI4IOE5V6416.pdf      */
    /*                                                                            */
    /* -------------------------------------------------------------------------- */

    /**
     * @brief I2C device addresses for the first PI4IOE expander.
     */
    const uint8_t I2C_DEV_ADDR_PI4IOE1 = 0x43;

    /**
     * @brief I2C device address for the second PI4IOE expander.
     */
    const uint8_t I2C_DEV_ADDR_PI4IOE2 = 0x44;

    /**
     * @brief I2C master timeout in milliseconds.
     */
    const uint8_t I2C_MASTER_TIMEOUT_MS = 50;

    /**
     * @brief Reset register address
     */
    const uint8_t PI4IO_REG_CHIP_RESET = 0x01;

    /**
     * @brief IO register address.
     */
    const uint8_t PI4IO_REG_IO_DIR = 0x03;


    const uint8_t PI4IO_REG_OUT_SET = 0x05;

    /**
     * @brief Output high impedance register address.
     */
    const uint8_t PI4IO_REG_OUT_H_IM = 0x07;

    /**
     * @brief Default state register address.
     */
    const uint8_t PI4IO_REG_IN_DEF_STA = 0x09;

    /**
     * @brief Pull up/down enable register address.
     */
    const uint8_t PI4IO_REG_PULL_EN = 0x0B;

    /**
     * @brief Pull up/down selection.
     */
    const uint8_t PI4IO_REG_PULL_SEL = 0x0D;

    /**
     * @brief Set the state for a pin.
     */
    const uint8_t PI4IO_REG_IN_STA = 0x0F;

    /**
     * @brief Interrupt mask register address.
     */
    const uint8_t PI4IO_REG_INT_MASK = 0x11;

    /**
     * @brief Interrupt status register address.
     */
    const uint8_t PI4IO_REG_IRQ_STA = 0x13;

    /**
     * @brief Handle for the first PI4IOE expander.
     */
    i2c_master_dev_handle_t _pi4ioe1Handle = nullptr;

    /**
     * @brief Handle for the second PI4IOE expander.
     */
    i2c_master_dev_handle_t _pi4ioe2Handle = nullptr;


    void set_gpio_output_capability();
    void hid_init();
    void rs485_init();
    bool wifi_init();
    void imu_init();
    void update_system_time();

    uint8_t _current_lcd_brightness = 100;
    bool _charge_qc_enable = false;
    bool _charge_enable = true;
    bool _ext_5v_enable = true;
    bool _usba_5v_enable = true;
    bool _ext_antenna_enable = false;
    bool _sd_card_mounted = false;
};
