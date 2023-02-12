/**
 * @file hardware_init.h
 * @defgroup hardware Hardware Initialization
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief Initialize the hardware peripheral 
 * @version 0.1
 * @date 2022-10-03
 * 
 * @copyright Copyright (c) 2022
 * @{
 */

#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

#include "soc/soc_caps.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <esp_adc/adc_oneshot.h>
#include <driver/ledc.h>
#include <driver/gpio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include <esp_spiffs.h>
#include <string.h>
#include "config.h"

//PINS
#define LED_DRIVER 4
#define FAN 5
#define AIRPUMP 6
#define BUTTON 10
#define PH 3
#define I2C_SDA 18
#define I2C_SCL 19

#define GPIO_MASK_OUTPUT ((1ULL<<AIRPUMP) | (1ULL<<FAN))
#define GPIO_INTERRUPT_MASK (1ULL<<BUTTON)

#define PH_ADC ADC1_CHANNEL_3

#define RESOLUTION_LED LEDC_TIMER_7_BIT

/**
 * @brief NVS setup. Namespaces and keys.<br>
 * In ESP-IDE the nvs is organized in namespaces. This namespaces con
 * @defgroup nvs NVS
 * @{
 */
#define NAMESPECE_CONTROL "control"
#define KEY_FAN "fan"
#define KEY_AIRPUMP "airpump"
#define KEY_LED "led"
#define KEY_PH_7 "ph7"
#define KEY_PH_4 "ph4"

#define NAMESPACE_TIME "time"
#define KEY_SECONDS "seconds"
#define KEY_CYCLE "cycle"

#define NAMESPACE_WIFI "wifi"
#define KEY_WIFI_NAME "wifi_name"
#define KEY_WIFI_PASS "wifi_pass"

/**
 * @brief NVS initialization function. NVS stands for Non-Volatile Storage; in the ESP-IDE this library is used to store small amounts of data in flash.
 * 
 */
void init_nvs();
/**@}*/



/**
 * @brief LEDC initialization function
 * 
 */

/**
 * @brief PWM generation. In ESP-IDE the general PWM generation is handled by the LEDC peripheral 
 * There are 4 timers [0..3] that provide the ESP32-C3 with 5 individual [0..5]<br>
 * The ESP-IDE handles by itself what prescalers to apply to get the trigger to fire at the right frequency.<br>
 * The same timer can be used for mutiple channels and frequencies.
 * @defgroup ledc LEDC
 * Aplication   | Channels  | Timer     | Frequency | Resolution
 * ----------   | -------   | --------- | --------- | ----------
 * LED PWM      |  1        | TIMER_0   |  450Hz    |  7_BIT(128)
 * @{
 */

/**
 * @brief LEDC initialization function
 * 
 */

void init_ledc();
/**@}*/

/**
 * @brief General Purpuse In Out controller. Used for reading logic inputs and outputing logic levels.<br>
 * The configuration is done using a bitmask. See configuration parameters.
 * @defgroup gpio GPIO
 * @{
 */

/**
 * @brief GPIO initialization function
 * 
 */


void init_gpio();
/**@}*/

/**
 * @brief Analogic to Digital converter. Translates voltages to other magnitudes.<br>
 * @defgroup adc ADC
 * @{
 */

/**
 * @brief ADC initialization function
 * 
 */
void init_adc();
/**@}*/

/**@}*/
#endif