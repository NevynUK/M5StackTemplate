/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */

#include <sdkconfig.h>

#include "HalTab5.h"
#include <memory>
#include <string>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

#include "HalTab5.h"

using namespace HAL;

/* -------------------------------------------------------------------------- */
/*                            Static Data Members                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Static pointer used to ensure that only one instance of HalTab5 exists.
 */
HalTab5 *HalTab5::_instance = nullptr;

/**
 * @brief Mount point for the SD card.
 */
// const std::string HalTab5::MOUNT_POINT = "/sdcard";

#define CONFIG_BSP_ERROR_CHECK 1
#define CONFIG_BSP_I2C_NUM 1
#define CONFIG_BSP_I2C_CLK_SPEED_HZ 100000
#define CONFIG_BSP_I2S_NUM 1
#define CONFIG_BSP_SPIFFS_MOUNT_POINT "/spiffs"
#define CONFIG_BSP_SPIFFS_PARTITION_LABEL "storage"
#define CONFIG_BSP_SPIFFS_MAX_FILES 5
#define CONFIG_BSP_LCD_DPI_BUFFER_NUMS 1
#define CONFIG_BSP_DISPLAY_BRIGHTNESS_LEDC_CH 1
#define CONFIG_BSP_LCD_COLOR_FORMAT_RGB565 1

#define BSP_LDO_PROBE_SD_CHAN 4
#define BSP_LDO_PROBE_SD_VOLCOMPONENT_NAMEE_MV 3300

/* -------------------------------------------------------------------------- */
/*                         Constructors / Destructor                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Default constructor for this class.
 *
 * This constructor is private to enforce the singleton pattern.
 */
HalTab5::HalTab5()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 instance created");
}

/**
 * @brief Delete the default destructor for this class as we should not need it.
 *
 * This destructor is private to enforce the singleton pattern.
 */
HalTab5::~HalTab5()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 instance destroyed");
}

/**
 * @brief Get the singleton instance of HalTab5.
 *
 * If the instance does not exist, it will create a new one.
 *
 * @return HalTab5* Pointer to the singleton instance of HalTab5.
 */
HalTab5 *HalTab5::GetInstance()
{
    if (!_instance)
    {
        _instance = new HalTab5();
    }
    return _instance;
}

/* -------------------------------------------------------------------------- */
/*                                Methods                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize the HalTab5 instance.
 */
void HalTab5::init()
{
    ESP_LOGI(COMPONENT_NAME, "HalTab5 initialized");
}

/* -------------------------------------------------------------------------- */
/*                            SD Card Methods                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure the SD card interface.
 * 
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::Mount(std::string mountPoint, size_t maximumFiles)
{
    esp_err_t result = ESP_OK;

    if (_sdCard)
    {
        return ESP_ERR_INVALID_STATE;
    }

    /**
     * @brief Use settings defined above to initialize SD card and mount FAT filesystem.
     *   Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
     *   Please check its source code and implement error recovery when developing
     *   production applications.
     */
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sd_pwr_ctrl_ldo_config_t ldo_config =
    {
        .ldo_chan_id = BSP_LDO_PROBE_SD_CHAN,   // `LDO_VO4` is used as the SDMMC IO power
    };

    if (!_sdCardPowerControlHandle)
    {
        result = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &_sdCardPowerControlHandle);
        if (result != ESP_OK)
        {
            ESP_LOGE(COMPONENT_NAME, "Failed to new an on-chip ldo power control driver");
            return result;
        }
    }
    host.pwr_ctrl_handle = _sdCardPowerControlHandle;

    /**
     * @brief This initializes the slot without card detect (CD) and write protect (WP) signals.
     *   Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
     */
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = SDMMC_BUS_WIDTH;
    slot_config.clk = GPIO_SDMMC_CLK;
    slot_config.cmd = GPIO_SDMMC_CMD;
    slot_config.d0 = GPIO_SDMMC_D0;
    slot_config.d1 = GPIO_SDMMC_D1;
    slot_config.d2 = GPIO_SDMMC_D2;
    slot_config.d3 = GPIO_SDMMC_D3;

    /**
     * @brief Options for mounting the filesystem.
     *   If format_if_mount_failed is set to true, SD card will be partitioned and
     *   formatted in case when mounting fails.
     */
    esp_vfs_fat_sdmmc_mount_config_t mount_config =
    {
        .format_if_mount_failed = false,
        .max_files = (int) maximumFiles,
        .allocation_unit_size = 16 * 1024,
        .disk_status_check_enable = true,
        .use_one_fat = false,
    };

    result = esp_vfs_fat_sdmmc_mount(mountPoint.c_str(), &host, &slot_config, &mount_config, &_sdCard);

    /* Check for SDMMC mount result. */
    if (result != ESP_OK)
    {
        ESP_LOGE(COMPONENT_NAME, "Failed to initialize the card (%s).  Make sure SD card lines have pull-up resistors in place.",  esp_err_to_name(result));
        return result;
    }

    return result;
}

/**
 * @brief Unmount the SD card.
 * 
 * @param mountPoint The mount point of the SD card.
 * @return esp_err_t Error code indicating the result of the operation.
 */
esp_err_t HalTab5::Unmount(std::string mountPoint)
{
    ESP_LOGI(COMPONENT_NAME, "Unmounting SD card");
    esp_err_t result = esp_vfs_fat_sdcard_unmount(mountPoint.c_str(), _sdCard);
    _sdCard = nullptr;

    return result;
}

/**
 * @brief Check if the SD card is mounted.
 * 
 * @return true If the SD card is mounted.
 * @return false If the SD card is not mounted.
 */
bool HalTab5::IsSdCardMounted()
{
    ESP_LOGI(COMPONENT_NAME, "Checking if SD card is mounted");
    return _sdCard != nullptr;
}

/* -------------------------------------------------------------------------- */
/*                             Display Methods                                */
/* -------------------------------------------------------------------------- */
