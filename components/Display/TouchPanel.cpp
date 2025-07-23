#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_timer.h>
#include <esp_lcd_touch.h>
#include <esp_lcd_touch_xpt2046.h>

#include <driver/spi_master.h>
#include <driver/gpio.h>

#include "Display.hpp"
#include "TouchPanel.hpp"
#include "Utilities.hpp"

class TouchPanel
{
    /**
     * @brief Setup the touch display.
     *
     * @param tp Pointer to the touch panel handle.
     * @return esp_err_t ESP_OK if successful, otherwise an error code.
     */
    esp_err_t Setup(esp_lcd_touch_handle_t *tp)
    {
    public:
        esp_err_t Configure()
        {
        }

        esp_lcd_panel_io_handle_t tp_io_handle = NULL;

        esp_lcd_panel_io_spi_config_t tp_io_config = {};
        tp_io_config.cs_gpio_num = TOUCH_CS;
        tp_io_config.dc_gpio_num = TOUCH_DC;
        tp_io_config.spi_mode = 0;
        tp_io_config.pclk_hz = TOUCH_CLOCK_HZ;
        tp_io_config.trans_queue_depth = 3;
        tp_io_config.on_color_trans_done = NULL;
        tp_io_config.user_ctx = NULL;
        tp_io_config.lcd_cmd_bits = 8;
        tp_io_config.lcd_param_bits = 8;
        tp_io_config.flags = {};
        tp_io_config.flags.dc_low_on_data = 0;
        tp_io_config.flags.octal_mode = 0;
        tp_io_config.flags.sio_mode = 0;
        tp_io_config.flags.lsb_first = 0;
        tp_io_config.flags.cs_high_active = 0;

        spi_bus_config_t buscfg_touch = {};
        buscfg_touch.mosi_io_num = TOUCH_SPI_MOSI;
        buscfg_touch.miso_io_num = TOUCH_SPI_MISO;
        buscfg_touch.sclk_io_num = TOUCH_SPI_CLK;
        buscfg_touch.quadwp_io_num = GPIO_NUM_NC;
        buscfg_touch.quadhd_io_num = GPIO_NUM_NC;
        buscfg_touch.data4_io_num = GPIO_NUM_NC;
        buscfg_touch.data5_io_num = GPIO_NUM_NC;
        buscfg_touch.data6_io_num = GPIO_NUM_NC;
        buscfg_touch.data7_io_num = GPIO_NUM_NC;
        buscfg_touch.max_transfer_sz = SPI_MAX_TRANSFER_SIZE;
        buscfg_touch.flags = SPICOMMON_BUSFLAG_SCLK | SPICOMMON_BUSFLAG_MISO | SPICOMMON_BUSFLAG_MOSI | SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS;
        buscfg_touch.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
        buscfg_touch.intr_flags = ESP_INTR_FLAG_LOWMED | ESP_INTR_FLAG_IRAM;

        esp_lcd_touch_config_t tp_cfg = {};
        tp_cfg.x_max = Display::HORIZONTAL_RESOLUTION;
        tp_cfg.y_max = Display::VERTICAL_RESOLUTION;
        tp_cfg.rst_gpio_num = TOUCH_RST;
        tp_cfg.int_gpio_num = TOUCH_IRQ;
        tp_cfg.levels = {.reset = 0, .interrupt = 0};
        tp_cfg.flags = {};
        tp_cfg.flags.swap_xy = false;
        tp_cfg.flags.mirror_x = Display::LCD_MIRROR_X;
        tp_cfg.flags.mirror_y = Display::LCD_MIRROR_Y;
        tp_cfg.process_coordinates = ProcessCoordinates;
        tp_cfg.interrupt_callback = NULL;

        ESP_ERROR_CHECK(spi_bus_initialize(TOUCH_SPI, &buscfg_touch, SPI_DMA_CH_AUTO));

        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t) TOUCH_SPI, &tp_io_config, &tp_io_handle));
        ESP_ERROR_CHECK(esp_lcd_touch_new_spi_xpt2046(tp_io_handle, &tp_cfg, tp));

        return ESP_OK;
    }

private:
    /**
     * @brief Process a touch panel coordinate event.
     *
     * @param tp Pointer to the touch panel handle.
     * @param x Pointer to the x co-ordinate.
     * @param y Pointer to the y co-ordinate.
     * @param strength Strength of the touch.
     * @param point_num Point number.
     * @param max_point_num Maximum point number.
     */
    void ProcessCoordinates(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
    {
        *x = Utilities::Map(*x, TOUCH_X_RES_MIN, TOUCH_X_RES_MAX, 0, Display::HORIZONTAL_RESOLUTION);
        *y = Utilities::Map(*y, TOUCH_Y_RES_MIN, TOUCH_Y_RES_MAX, 0, Display::VERTICAL_RESOLUTION);
    }
}; // class TouchPanel
