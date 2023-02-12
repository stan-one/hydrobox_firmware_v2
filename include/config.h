/**
 * @file config.h
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief 
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef CONFIG_H
#define CONFIG_H
#include "hardware_init.h"



#define QUEUE_TIMEOUT_MS 50


#define WIFI_NAME_LEN 50
#define WIFI_PASS_LEN 50

#define WIFI_CHANNEL 11
#define MAX_CONN_CNT 1

#define MAX_RETRY 15

#define LED_MAX_INTENSE 127
#define PWM_LED_FREQ 400

#define TICKS_TO_TIMEOUT_Q (TickType_t)50
#define TICKS_TO_TIMEOUT_S (TickType_t)10

#define HOURS_TO_SECONDS 1

#define ADC_SAMPLE_COUNT 64

#define FLAG_VALUE_OFF -1
#define FLAG_VALUE_ON -2
#define LEDS_OFF 128

#define BUTTON_TICKS_THRESHOLD 200

#define SDA_PIN 9
#define SCL_PIN 7

#endif