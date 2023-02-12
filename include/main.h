
/**
 * 
 * @mainpage 
 * This project aims to create the first low cost, self continent and open
hydroponics system for educational and recreational purposes alike.<br><br>
 * **Electronics**:<br>
The PCB contains everything needed for hydroponics in the smallest possible
package. <br>This includes:<br>
-# Full WiFi connectivity. User interface provided over WiFi with a webpage.
Powered by an ESP32-C3.
-# High power led driver. Step-up constant current led driver that powers 11 high
brightness leds outputting a maximum of 11W.
-# Led panel SMD-soldered on the backside of the PCB.
-# Temperature and humidity sensor.
-# Switching of two 12V dc fans.
-# Switching one 12V air pump.
-# Analog input of a pH probe (using an external pcb)
-# 12V power supply
-# General purpose input button.
 * 
* ![Front of the pcb](/home/user/PlatformIO/Projects/minibox_firmware/docs/images/pcb_front_re.jpg) 
<br><br>
 * ![Back of the pcb](/home/user/PlatformIO/Projects/minibox_firmware/docs/imagespcb_back_re.jpg) 
 * 
 * @file main.h
 * @defgroup main Main
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief Main section of the firmware, conteins the code of the tasks
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * @{
 * 
 */
#ifndef MAIN_H
#define MAIN_H

#include "hardware_init.h"
#include "config.h"
#include "web_server.h"
#include "data_structures.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <esp_system.h>
#include "sht3c.h"
#include <string.h>
#include <esp_err.h>

#define DEFAULT_WIFI_SSID "ALCATRAZ wifi"
#define DEFAULT_WIFI_PASSWORD "x%rAdEOn%x"

#define MAX_VALUE (uint32_t)0xffffffff


/**
 * @defgroup wifi WiFi Setup
 * @brief Wifi provisioning thru AP mode. If the WiFi was set once at reboot will use the previously known WiFi.
 * If connection is not possible, AP mode will be activated and provision will start. 
 * @{
 */


/**
 * @brief WiFi initialization function. Contains all the configurations that control the WiFi peripheral
 * 
 */
void init_wifi(void);

/**
 * @brief WiFi handle function. During the connection process (either AP or STA) events happen. This function handles them thru the defaulteven loop implemented in the ESP-IDE
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
void handle_wifi_connection(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

/**@}*/

/**
 * @brief Main function. Initializes the hardware and starts FreeRTOS kernel.
 * 
 */
void app_main();

/**
 * @brief Sensor Task.
 * 
 */
void sensor_task();

/**
 * @brief Task that sets the PWM duty cycle and controls the LEDC peripheral.
 * 
 */

void led_control_task();

/**
 * @brief Fan task. Sets on/off the fan.
 * 
 */
void fan_task();

/**
 * @brief Air pump task. Sets the airpump on/off.
 * 
 */
void airpump_task();

/**
 * @brief Callback function of the RTOS software timer. Counts the seconds.
 * 
 */
void timer_callback();

/**
 * @brief Task that sets the sets the light cycle. Sends commands to led_control_task
 * 
 */
void timer_task();

/**
 * @brief ADC task. Reads the ADC peripheral and converts the raw voltage to a ph reading 
 * 
 */
void adc_task();


/**@}*/
#endif