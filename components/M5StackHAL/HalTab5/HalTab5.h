/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <sdkconfig.h>

#include <cstdint>
#include <memory>
#include <queue>
#include <string>
#include <mutex>
#include <vector>

#include <lvgl.h>

#include "HalBase.h"

/**
 * @brief Hardware abstraction layer
 */
namespace HAL
{
    /**
     * @brief
     */
    class HalTab5 : public HalBase
    {
    public:
        /**
         * @brief Name of this class.
         *
         * This is used for logging and debugging purposes.
         */
        static constexpr const char *COMPONENT_NAME = "HalTab5";

        /**
         * @brief Default constructor for this class.
         */
        HalTab5();

        /**
         * @brief Destructor for this class.
         */
        ~HalTab5();

        /**
         * @brief Get the instance object
         * 
         * @return HalTab5* Pointer to the singleton instance of HalTab5.
         */
        static HalTab5 *GetInstance();

        /**
         * @brief Perform any class initialisation.
         */
        void init() override;

        /**
         * @brief Get the CPU temperature
         *
         * @return float CPU temperature in degrees Celsius
         */
        float GetCpuTemperatureC() override
        {
            return 0.0f;
        }

        /* -------------------------------------------------------------------------- */
        /*                            Display Methods                                 */
        /* -------------------------------------------------------------------------- */

        /**
         * @brief Configure the display.
         * 
         * @return esp_err_t Error code indicating the result of the operation.
         */
        esp_err_t ConfigureDisplay() override;

        /**
         * @brief Get the display width in pixels.
         * 
         * @return int Display width in pixels.
         */
        int GetDisplayWidth() override
        {
            return 1280;
        }

        /**
         * @brief Get the display height in pixels.
         * 
         * @return int Display height in pixels.
         */
        int GetDisplayHeight() override
        {
            return 720;
        }

        /**
         * @brief Configure the display brightness control interface.
         */
        esp_err_t ConfigureDisplayBrightnessControl() override;

        /**
         * @brief Set the display brightness.
         * 
         * @param brightness Brightness level (0-100).
         */
        esp_err_t SetDisplayBrightness(uint8_t brightnessPercent) override;

        /**
         * @brief Get the display brightness.
         * 
         * @return uint8_t Display brightness level (0-100).
         */
        uint8_t GetDisplayBrightness() override;

        /* ---------------------------------- Lvgl ---------------------------------- */
        lv_indev_t *lvTouchpad = nullptr;

        /* --------------------------------- SD Card -------------------------------- */

        esp_err_t Mount(std::string mountPoint = MOUNT_POINT, size_t maximumFiles = 25) override;

        esp_err_t Unmount(std::string mountPoint) override;

        bool IsSdCardMounted() override;

    private:
        /**
         * @brief SDMMC bus width.
         */
        const int SDMMC_BUS_WIDTH = 4;

        /**
         * @brief SDMMC card detect GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_DET = GPIO_NUM_NC;

        /**
         * @brief SDMMC clock GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_CLK = GPIO_NUM_43;

        /**
         * @brief SDMMC command GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_CMD = GPIO_NUM_44;

        /**
         * @brief SDMMC data 0 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D0 = GPIO_NUM_39;

        /**
         * @brief SDMMC data 1 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D1 = GPIO_NUM_40;

        /**
         * @brief SDMMC data 2 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D2 = GPIO_NUM_41;

        /**
         * @brief SDMMC data 3 GPIO pin.
         */
        const gpio_num_t GPIO_SDMMC_D3 = GPIO_NUM_42;

        /**
         * @brief Display backlight control GPIO pin.
         */
        const gpio_num_t GPIO_LCD_BACKLIGHT = GPIO_NUM_22;

        /**
         * @brief SDMMC mount point.
         */
        // static const std::string MOUNT_POINT;

        // Prevent copying
        HalTab5(const HalTab5 &) = delete;
        HalTab5 &operator=(const HalTab5 &) = delete;

        // Prevent moving
        HalTab5(HalTab5 &&) = delete;
        HalTab5 &operator=(HalTab5 &&) = delete;

        /**
         * @brief Singleton instance of HalTab5
         */
        static HalTab5 *_instance;

        /**
         * @brief Pointer to the SD card structure.
         */
        sdmmc_card_t *_sdCard = nullptr;

        /**
         * @brief Pointer to the SD card power control handle.
         */
        sd_pwr_ctrl_handle_t _sdCardPowerControlHandle = nullptr;

        /**
         * @brief Display brightness control.
         */
        uint8_t _displayBrightness = 100;
    };
} // namespace hal



        // /* ---------------------------------- Power --------------------------------- */
        // struct PMData_t
        // {
        //     float busVoltage = 0.0f;
        //     float busPower = 0.0f;
        //     float shuntVoltage = 0.0f;
        //     float shuntCurrent = 0.0f;
        // };

        // PMData_t powerMonitorData;

        // virtual void updatePowerMonitorData()
        // {
        // }

        // virtual void setChargeQcEnable(bool enable)
        // {
        // }

        // virtual bool getChargeQcEnable()
        // {
        //     return false;
        // }

        // virtual void setChargeEnable(bool enable)
        // {
        // }

        // virtual bool getChargeEnable()
        // {
        //     return false;
        // }

        // virtual void setUsb5vEnable(bool enable)
        // {
        // }

        // virtual bool getUsb5vEnable()
        // {
        //     return false;
        // }

        // virtual void setExt5vEnable(bool enable)
        // {
        // }

        // virtual bool getExt5vEnable()
        // {
        //     return false;
        // }

        // virtual void powerOff()
        // {
        // }

        // virtual void sleepAndTouchWakeup()
        // {
        // }

        // virtual void sleepAndShakeWakeup()
        // {
        // }

        // virtual void sleepAndRtcWakeup()
        // {
        // }

        // /* ----------------------------------- IMU ---------------------------------- */
        // struct IMUData_t
        // {
        //     float accelX = 0.0f;
        //     float accelY = 0.0f;
        //     float accelZ = 0.0f;
        //     float gyroX = 0.0f;
        //     float gyroY = 0.0f;
        //     float gyroZ = 0.0f;
        // };

        // IMUData_t imuData;

        // virtual void updateImuData()
        // {
        // }

        // virtual void clearImuIrq()
        // {
        // }

        // /* ----------------------------------- RTC ---------------------------------- */
        // virtual void getRtcTime(tm *time)
        // {
        // }

        // virtual void setRtcTime(tm time)
        // {
        // }

        // virtual void clearRtcIrq()
        // {
        // }

        // /* --------------------------------- Camera --------------------------------- */
        // virtual void startCameraCapture(lv_obj_t *imgCanvas)
        // {
        // }

        // virtual void stopCameraCapture()
        // {
        // }

        // virtual bool isCameraCapturing()
        // {
        //     return false;
        // }

        // /* ---------------------------------- USB-A --------------------------------- */
        // struct HidMouseData_t
        // {
        //     std::mutex mutex;
        //     int x = 0;
        //     int y = 0;
        //     bool btnLeft = false;
        //     bool btnRight = false;
        // };

        // HidMouseData_t hidMouseData;

        // /* ---------------------------------- Audio --------------------------------- */
        // virtual void setSpeakerVolume(uint8_t volume)
        // {
        // }

        // virtual uint8_t getSpeakerVolume()
        // {
        //     return 0;
        // }

        // // [MIC-L, AEC, MIC-R, MIC-HP]
        // virtual void audioRecord(std::vector<int16_t> &data, uint16_t durationMs, float gain = 80.0f)
        // {
        // }

        // virtual void audioPlay(std::vector<int16_t> &data, bool async = true)
        // {
        // }

        // // Mic record test
        // enum MicTestState_t {
        //     MIC_TEST_IDLE,
        //     MIC_TEST_RECORDING,
        //     MIC_TEST_PLAYING,
        // };

        // virtual void startDualMicRecordTest()
        // {
        // }

        // virtual MicTestState_t getDualMicRecordTestState()
        // {
        //     return MIC_TEST_IDLE;
        // }

        // virtual void startHeadphoneMicRecordTest()
        // {
        // }

        // virtual MicTestState_t getHeadphoneMicRecordTestState()
        // {
        //     return MIC_TEST_IDLE;
        // }

        // // Play music test
        // enum MusicPlayState_t {
        //     MUSIC_PLAY_IDLE,
        //     MUSIC_PLAY_PLAYING,
        // };

        // virtual void startPlayMusicTest()
        // {
        // }

        // virtual MusicPlayState_t getMusicPlayTestState()
        // {
        //     return MUSIC_PLAY_IDLE;
        // }

        // virtual void stopPlayMusicTest()
        // {
        // }

        // // Sfx
        // virtual void playStartupSfx()
        // {
        // }

        // virtual void playShutdownSfx()
        // {
        // }

        // /* --------------------------------- Network -------------------------------- */
        // virtual void setExtAntennaEnable(bool enable)
        // {
        // }

        // virtual bool getExtAntennaEnable()
        // {
        //     return false;
        // }

        // virtual void startWifiAp()
        // {
        // }

    //     /* -------------------------------- Interface ------------------------------- */
    //     virtual bool usbCDetect()
    //     {
    //         return false;
    //     }

    //     virtual bool usbADetect()
    //     {
    //         return false;
    //     }

    //     virtual bool headPhoneDetect()
    //     {
    //         return false;
    //     }

    //     virtual std::vector<uint8_t> i2cScan(bool isInternal)
    //     {
    //         return {};
    //     }

    //     virtual void initPortAI2c()
    //     {
    //     }

    //     virtual void deinitPortAI2c()
    //     {
    //     }

    //     virtual void gpioInitOutput(uint8_t pin)
    //     {
    //     }

    //     virtual void gpioSetLevel(uint8_t pin, bool level)
    //     {
    //     }

    //     virtual void gpioReset(uint8_t pin)
    //     {
    //     }

    //     /* ------------------------------ UART monitor ------------------------------ */
    //     struct UartMonitorData_t
    //     {
    //         std::mutex mutex;
    //         std::queue<uint8_t> rxQueue;
    //         std::queue<uint8_t> txQueue;
    //     };

    //     UartMonitorData_t uartMonitorData;

    //     virtual void uartMonitorSend(std::string msg, bool newLine = true)
    //     {
    //         std::lock_guard<std::mutex> lock(uartMonitorData.mutex);
    //         for (auto c: msg)
    //         {
    //             uartMonitorData.txQueue.push(c);
    //         }
    //         if (newLine)
    //         {
    //             uartMonitorData.txQueue.push('\n');
    //         }
    //     }

