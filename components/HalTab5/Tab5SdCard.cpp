/**
 * @file SDCard.cpp
 * @author Mark Stevens
 * @brief SD Card functionality.
 * @date 2025-05-25
 *
 * @copyright Copyright (c) 2025
 */
#include "sdkconfig.h"

#include <mutex>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "sd_pwr_ctrl_by_on_chip_ldo.h"

#include "HalTab5.hpp"

using namespace HAL;

/*****************************************************************************
 * 
 *                      Static member initializations.
 * 
 *****************************************************************************/

/**
 * @brief Component name for logging.
 */
const std::string HalTab5::DEFAULT_MOUNT_POINT = "/sdcard";

/**
 * @brief Mutex for thread safety.
 * 
 * This is used to ensure that the SD card operations are thread safe.
 */
std::mutex HalTab5::_mutex;

/*****************************************************************************
 * 
 *                                Methods.
 * 
 *****************************************************************************/

/**
 * @brief Setup the SD card and mount the filesystem.
 * 
 * @param mountPoint The mount point for the filesystem.
 * @param maxFiles The maximum number of files that can be opened simultaneously.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t HalTab5::ConfigureSdCard(const std::string &mountPoint, const size_t maxFiles)
{
    esp_err_t result = ESP_OK;

    //
    //  Lock the mutex to ensure thread safety.
    //
    std::lock_guard<std::mutex> lock(_mutex);

    //
    //  Check if the SD card is already configured.
    //
    if (_card)
    {
        ESP_LOGW(COMPONENT_NAME, "SD card already configured.");
        return ESP_OK;
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
        .ldo_chan_id = LDO_PROBE_SD_CHAN, // `LDO_VO4` is used as the SDMMC IO power
    };

    if (!pwr_ctrl_handle)
    {
        result = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
        if (result != ESP_OK)
        {
            ESP_LOGE(COMPONENT_NAME, "Failed to new an on-chip ldo power control driver");
            return result;
        }
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;

    /**
     * @brief This initialises the slot without card detect (CD) and write protect (WP) signals.
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

    esp_vfs_fat_sdmmc_mount_config_t mount_config = VFS_FAT_MOUNT_DEFAULT_CONFIG();
    mount_config.max_files = (int) maxFiles;

    char *base_path;
    if (mountPoint.empty())
    {
        base_path = const_cast<char *>(DEFAULT_MOUNT_POINT.c_str());
    }
    else
    {
        base_path = const_cast<char *>(mountPoint.c_str());
    }
    result = esp_vfs_fat_sdmmc_mount(base_path, &host, &slot_config, &mount_config, &_card);

    if (result != ESP_OK)
    {
        if (result == ESP_FAIL)
        {
            ESP_LOGE(COMPONENT_NAME, "Failed to mount filesystem.");
        }
        else
        {
            ESP_LOGE(COMPONENT_NAME, "Failed to initialize the card (%s).", esp_err_to_name(result));
        }
        return result;
    }

    /* Card has been initialized, print its properties. */
    sdmmc_card_print_info(stdout, _card);

    //
    //  Save the mount point and maximum number of simultaneously open files.
    //
    _mountPoint = mountPoint;
    _maxFiles = maxFiles;

    return result;
}
