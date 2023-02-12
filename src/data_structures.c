/**
 * @file data_structures.c
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief 
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "data_structures.h"

/**
 * @brief Initialize RTOS queues
 * @author Stanimir Hristov
 * @date 1-10-2022
 */

QueueHandle_t hum_now_q;
QueueHandle_t temp_now_q;
QueueHandle_t ph_now_q;
QueueHandle_t led_cycle_q;
QueueHandle_t timer_q;
QueueHandle_t led_intens_q;
QueueHandle_t uptime_now_q;

/**@}*/

/**
 * @defgroup Semaphores
 * @brief FreeRTOS semaphores
 * @{
 */

SemaphoreHandle_t fan_sem;  
SemaphoreHandle_t airpump_sem;
SemaphoreHandle_t ph7_sem;
SemaphoreHandle_t ph4_sem;
SemaphoreHandle_t timer_sem;
SemaphoreHandle_t button_sem;
SemaphoreHandle_t ph4_sem;
SemaphoreHandle_t ph7_sem;


/**@}*/




void init_queues(){
    led_intens_q = xQueueCreate(ELEMENTS_IN_QUEUE, sizeof(int8_t));
    hum_now_q = xQueueCreate(ELEMENTS_IN_QUEUE, sizeof(float));
    temp_now_q = xQueueCreate(ELEMENTS_IN_QUEUE, sizeof(float));
    led_cycle_q = xQueueCreate(ELEMENTS_IN_QUEUE, sizeof(led_cycle_t));
    timer_q = xQueueCreate(ELEMENTS_IN_QUEUE, sizeof(uint64_t));
    ph_now_q = xQueueCreate(ELEMENTS_IN_QUEUE, sizeof(float));
}

void init_semaphores(){
    fan_sem = xSemaphoreCreateBinary();
    airpump_sem = xSemaphoreCreateBinary();   
    timer_sem = xSemaphoreCreateBinary();
    button_sem = xSemaphoreCreateBinary();
    ph4_sem = xSemaphoreCreateBinary();
    ph7_sem = xSemaphoreCreateBinary();
}

