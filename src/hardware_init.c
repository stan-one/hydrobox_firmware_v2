/**
 * @file hardware_init.c
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief
 * @version 0.1
 * @date 2022-10-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "config.h"
#include "hardware_init.h"
#include <esp_log.h>

nvs_handle_t permanent_data_handle;

static const char *TAG = "hardware_init";
/**
 * @ingroup ledc
 * @{
 */
ledc_timer_config_t ledc_led_timer = {
    .duty_resolution = RESOLUTION_LED,
    .freq_hz = PWM_LED_FREQ,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .timer_num = LEDC_TIMER_0,
    .clk_cfg = LEDC_AUTO_CLK,
};
ledc_channel_config_t ledc_channel_driver;
/**@}*/

/**
 * @ingroup adc
 * @{
 */
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
/**@}*/

/**
 * @ingroup gpio
 * @{
 */
gpio_config_t io_conf_output = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = GPIO_MASK_OUTPUT,
    .pull_up_en = 0,
    .pull_down_en = 0,
};

gpio_config_t io_conf_input = {
    .intr_type = GPIO_INTR_NEGEDGE,
    .mode = GPIO_MODE_INPUT,
    .pin_bit_mask = GPIO_MASK_OUTPUT,
    .pull_up_en = 1,
    .pull_down_en = 0,
};
/**@}*/

void init_gpio()
{
    gpio_config(&io_conf_output);
    gpio_config(&io_conf_input);
}

void init_adc()
{
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
}

void init_nvs()
{
    ESP_LOGI(TAG, "INITIALIZING NVS");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    err = nvs_open(NAMESPECE_CONTROL, NVS_READWRITE, &permanent_data_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "CONTROL NAMESPACE ERROR: %s", esp_err_to_name(err));
    }
    err = nvs_open(NAMESPACE_TIME, NVS_READWRITE, &permanent_data_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "TIME NAMESPACE ERROR: %s", esp_err_to_name(err));
    }
    err = nvs_open(NAMESPACE_WIFI, NVS_READWRITE, &permanent_data_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG, "WIFI NAMESPACE ERROR: %s", esp_err_to_name(err));
    }
}

void init_ledc()
{
    ledc_timer_config(&ledc_led_timer);
    ledc_channel_driver.channel = LEDC_CHANNEL_0;
    ledc_channel_driver.duty = LEDS_OFF;
    ledc_channel_driver.gpio_num = LED_DRIVER;
    ledc_channel_driver.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel_driver.hpoint = 0;
    ledc_channel_driver.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ledc_channel_driver);
}