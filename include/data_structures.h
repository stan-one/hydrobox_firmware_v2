/**
 * @file data_structures.h
 * @defgroup data_struct RTOS Data Structures
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief 
 * @version 0.1
 * @date 2022-10-01
 * 
 * @copyright Copyright (c) 2022
 * @{
 */


#ifndef DATA_STRUCTURES_H__
#define DATA_STRUCTURES_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define ELEMENTS_IN_QUEUE 3

typedef struct env{
    float temp;
    float hum;
    float ph;
    int uptime;
}env_t;

typedef struct led_cycle{
    uint32_t t_on;
    uint32_t t_off;
}led_cycle_t;

typedef struct prev_state{
    bool fan;
    bool airpump;
    int8_t led_intens;
    char str_cycle[10];
}prev_state_t;


void init_semaphores();



/**
 * @defgroup Queues
 * @brief FreeRTOS queues
 * @{
 */

/**
 * @brief Initialize RTOS semaphores
 * @author Stanimir Hristov
 * @date 1-10-2022
 */
void init_queues();

#endif