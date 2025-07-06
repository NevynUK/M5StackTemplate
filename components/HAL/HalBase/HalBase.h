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

/**
 * @brief Hardware abstraction layer
 */
namespace HAL
{
    /**
     * @brief
     */
    class HalBase
    {
    public:
        /**
         * @brief Name of this class.
         *
         * This is used for logging and debugging purposes.
         */
        static constexpr const char *COMPONENT_NAME = "HalBase";

        /**
         * @brief Default constructor for this class.
         */
        HalBase();

        /**
         * @brief Delete the default destructor for this class as we should not need it.
         */
        ~HalBase();

        static HalBase *GetInstance();

        /**
         * @brief Perform any necessary initialization for the M5Stack Tab5 HAL.
         */
        virtual void init()
        {
        }

        /**
         * @brief Get the CPU temperature
         *
         * @return float CPU temperature in degrees Celsius
         */
        virtual float GetCpuTemperatureC()
        {
            return 0.0f;
        }

        /* --------------------------------- Display -------------------------------- */
        virtual int GetDisplayWidth()
        {
            return 0;
        }

        virtual int GetDisplayHeight()
        {
            return 0;
        }

        virtual void SetDisplayBrightness(uint8_t brightness)
        {
        }

        virtual uint8_t GetDisplayBrightness()
        {
            return 0;
        }

        /* ---------------------------------- Lvgl ---------------------------------- */
        lv_indev_t *lvTouchpad = nullptr;

        /* --------------------------------- SD Card -------------------------------- */

        virtual bool Mount(std::string mountPoint)
        {
            return false;
        }

        virtual bool Unmount()
        {
            return false;
        }

        virtual bool IsSdCardMounted()
        {
            return false;
        }

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
    // };

    // /**
    //  * @brief Get the HAL instance
    //  *
    //  * @return HalBase&
    //  */
    // HalBase *Get();

    // /**
    //  * @brief Inject the HAL, which will call init() to initialize the HAL
    //  *
    //  * @param hal
    //  */
    // void Inject(std::unique_ptr<HalBase> hal);

    // /**
    //  * @brief Destroy the HAL instance
    //  *
    //  */
    // void Destroy();

    // /**
    //  * @brief Check if the HAL instance exists
    //  *
    //  * @return true
    //  * @return false
    //  */
    // bool Check();

    private:
        // Prevent copying
        HalBase(const HalBase &) = delete;
        HalBase &operator=(const HalBase &) = delete;

        // Prevent moving
        HalBase(HalBase &&) = delete;
        HalBase &operator=(HalBase &&) = delete;

        /**
         * @brief Singleton instance of HalBase
         */
        static HalBase *_instance;
    };
} // namespace hal
