/**
 * @file main.c
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief 
 * @version 0.1
 * @date 2023-02-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "main.h"


#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

extern QueueHandle_t hum_now_q;
extern QueueHandle_t temp_now_q;
extern QueueHandle_t ph_now_q;
extern QueueHandle_t led_cycle_q;
extern QueueHandle_t timer_q;
extern QueueHandle_t led_intens_q;

extern SemaphoreHandle_t fan_sem;  
extern SemaphoreHandle_t airpump_sem;
extern SemaphoreHandle_t timer_sem;
extern SemaphoreHandle_t button_sem;
extern SemaphoreHandle_t ph7_sem;
extern SemaphoreHandle_t ph4_sem;

extern adc_oneshot_unit_handle_t adc1_handle;

TaskHandle_t air_pump_task_handle;
TaskHandle_t i2c_task_handle;
TaskHandle_t led_control_task_handle;
TaskHandle_t fan_task_handle;
TaskHandle_t adc_task_handle;
TaskHandle_t timer_task_handle;
TaskHandle_t button_task_handle;

TimerHandle_t base_timer;


uint32_t seconds = 0;

int32_t v_ph7 = 0;
int32_t v_ph4 = 0;

extern nvs_handle_t permanent_data_handle;
extern ledc_channel_config_t ledc_channel_driver;

static EventGroupHandle_t wifi_events;

bool WIFI_PROV_FLAG = true;

int retry_cnt = 0;

int val_off = FLAG_VALUE_OFF;
int val_on = FLAG_VALUE_ON;
int val_lights_off = LEDS_OFF;

static const char* TAG = "main";

float process_raw_ph(int ph){
    if(v_ph4 == 0 && v_ph7 == 0){
        return -1;
    }
    float slope = (7.0-4.0)/((v_ph7-1500.0)/3.0 - (v_ph4-1500.0)/3.0);
    float intercept =  7.0 - slope*(v_ph7-1500.0)/3.0;
    return slope*(ph-1500.0)/3.0+intercept;
}

void init_wifi(void){
    ESP_LOGI(TAG, "IN WIFI");
    size_t textlen_1 = WIFI_PASS_LEN;
    size_t textlen_2 = WIFI_NAME_LEN;
    esp_netif_t *wifi_sta = NULL; 
    char wifi_name[WIFI_NAME_LEN]; memset(wifi_name, 0, WIFI_NAME_LEN);
    char wifi_pass[WIFI_PASS_LEN]; memset(wifi_pass, 0, WIFI_PASS_LEN);
    
    wifi_events = xEventGroupCreate();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &handle_wifi_connection, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &handle_wifi_connection, NULL);
    //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    nvs_get_str(permanent_data_handle, KEY_WIFI_PASS, wifi_pass, &textlen_1);
    nvs_get_str(permanent_data_handle, KEY_WIFI_NAME, wifi_name, &textlen_2);
    ESP_LOGI(TAG, "name %s pass %s", wifi_name, wifi_pass);
    if(strlen(wifi_name) != 0 && strlen(wifi_pass) != 0){
        WIFI_PROV_FLAG = false;
    }

        EventBits_t bits = xEventGroupWaitBits(wifi_events,
                                            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT){
            ESP_LOGI(TAG, "wifi connected");
        }
        else{
        ESP_LOGI(TAG, "wifi connection error");
            esp_wifi_stop();
            esp_netif_destroy(wifi_sta);
            esp_wifi_deinit();
            ESP_LOGI(TAG, "WIFI_CLEAN");
            esp_restart();
        }
}

void handle_wifi_connection(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data){
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_cnt++ < MAX_RETRY)
        {
            esp_wifi_connect();
            ESP_LOGI(TAG, "wifi connect retry: %d", retry_cnt);
        }
        else
        {
            xEventGroupSetBits(wifi_events, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG,"ip: %d.%d.%d.%d", IP2STR(&event->ip_info.ip));
        retry_cnt = 0;
        xEventGroupSetBits(wifi_events, WIFI_CONNECTED_BIT);
    }
}
void airpump_task(){
    bool state = false;
    nvs_set_i8(permanent_data_handle, KEY_AIRPUMP, 0);
    for(;;){
        xSemaphoreTake(airpump_sem, portMAX_DELAY);
        ESP_LOGI(TAG, "AIR PUMP TASK RECEIVED");
        if(state){
            gpio_set_level(AIRPUMP, 0);
            state = false;
        }
        else{
            gpio_set_level(AIRPUMP, 1);
            state = true;
        }
    }
}

void fan_task(){
    bool state = false;
    nvs_set_i8(permanent_data_handle, KEY_FAN, 0);
    for(;;){
        xSemaphoreTake(fan_sem, portMAX_DELAY);
        ESP_LOGI(TAG, "FAN TASK RECEIVED");
        if(state){
            gpio_set_level(FAN, 0);
            state = false;
        }
        else{
            gpio_set_level(FAN, 1);
            state = true;
        }
    }
}

void led_control_task(){
    int8_t led_intens = 0;
    int value_ans = 0;
    nvs_get_i8(permanent_data_handle, KEY_LED, &led_intens);
    xQueueSendToBack(led_intens_q, &led_intens, TICKS_TO_TIMEOUT_Q);
    for(;;){
        xQueueReceive(led_intens_q, &led_intens, portMAX_DELAY);
        ESP_LOGI(TAG, "LED VALUE:%d", led_intens);
        if(led_intens == val_off){
            //ESP_LOGI(TAG, "ON CYCLE ENDED, LEDS OFF");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel_driver.channel, LEDS_OFF);
            ledc_update_duty(ledc_channel_driver.speed_mode, ledc_channel_driver.channel);
        }
        else if(led_intens == val_on){
            //ESP_LOGI(TAG, "OFF CYCLE ENDED, LEDS ON");
            ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel_driver.channel, value_ans);
            ledc_update_duty(ledc_channel_driver.speed_mode, ledc_channel_driver.channel);
        }
        else{
            value_ans = led_intens;
            //ESP_LOGI(TAG, "LED INTENSITY UPDATED");
            nvs_set_i8(permanent_data_handle, KEY_LED, led_intens);
            ledc_set_duty(LEDC_LOW_SPEED_MODE, ledc_channel_driver.channel, led_intens);
            ledc_update_duty(ledc_channel_driver.speed_mode, ledc_channel_driver.channel);
        }
    }
}

void sensor_task(){
    float temperature = 27.4;
    float humidity = 60.2;
    float dummy = 0;
    //time_t t;
    //srand((unsigned) time(&t));
    setup_i2c();
    wake_up_sht3c();
    for(;;){
        read_data_sht3c(&temperature, &humidity);
        if(temperature>0 && humidity>0){
            //temperature = (float)(rand() % 50);
            //humidity = (float)(rand() % 50);
            //ESP_LOGI(TAG, "Temperature: %.2f °C", temperature);
            //ESP_LOGI(TAG, "Humidity: %.2f %%", humidity);
            if(xQueueSendToBack(temp_now_q, &temperature, TICKS_TO_TIMEOUT_Q)==pdFALSE){
                if(xQueueReceive(temp_now_q, &dummy, TICKS_TO_TIMEOUT_Q)==pdTRUE){
                    if(xQueueSendToBack(temp_now_q, &temperature, TICKS_TO_TIMEOUT_Q)==pdFALSE){
                        ESP_LOGI(TAG, "cant access temperature queue");
                    }
                }
            }
            if(xQueueSendToBack(hum_now_q, &humidity, TICKS_TO_TIMEOUT_Q)==pdFALSE){
                if(xQueueReceive(hum_now_q, &dummy, TICKS_TO_TIMEOUT_Q)==pdTRUE){
                    if(xQueueSendToBack(hum_now_q, &humidity, TICKS_TO_TIMEOUT_Q)==pdFALSE){
                        ESP_LOGI(TAG, "cant access humidity queue");
                    }
                }
            }
        }
            vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void adc_task(){
    int ph_raw = 0;
    float ph;
    float dummy;
    nvs_get_i32(permanent_data_handle, KEY_PH_4, &v_ph4);
    nvs_get_i32(permanent_data_handle, KEY_PH_7, &v_ph7);
    for(;;){
        for(int i = 0; i<ADC_SAMPLE_COUNT; i++){
            int hold = 0;
            ph_raw += adc_oneshot_read(adc1_handle, ADC_CHANNEL_3, &hold);
            ph_raw+=hold;
        }
        ph_raw /= ADC_SAMPLE_COUNT;

        if(xSemaphoreTake(ph7_sem, TICKS_TO_TIMEOUT_S)){
            v_ph7 = ph_raw;
            nvs_set_i32(permanent_data_handle, KEY_PH_7, v_ph7);
            nvs_commit(permanent_data_handle);
        }
        if(xSemaphoreTake(ph4_sem, TICKS_TO_TIMEOUT_S)){
             v_ph4 = ph_raw;
             nvs_set_i32(permanent_data_handle, KEY_PH_4, v_ph4);
             nvs_commit(permanent_data_handle);
             
        }
        ph = process_raw_ph(ph_raw);
        if(xQueueSendToBack(ph_now_q, &ph, TICKS_TO_TIMEOUT_Q)==pdFALSE){
            if(xQueueReceive(ph_now_q, &dummy, TICKS_TO_TIMEOUT_Q)==pdTRUE){
                if(xQueueSendToBack(ph_now_q, &ph, TICKS_TO_TIMEOUT_Q)==pdFALSE){
                    ESP_LOGI(TAG, "cant access ph queue");
                }
            }
        }
        vTaskDelay(100);
    }
}

void timer_task(){
    led_cycle_t led_cycle = {.t_off = DEF_VALUE, .t_on = MAX_VALUE};
    led_cycle_t led_cycle_dummy = {.t_off = DEF_VALUE, .t_on = MAX_VALUE};
    uint32_t time_now = 0;
    uint32_t time_future = MAX_VALUE;
    bool cycle_side = true;
    for(;;){
        xQueueReceive(timer_q, &time_now, portMAX_DELAY);
         //ESP_LOGI(TAG, "light cycle updated: t_on: %d, t_off: %d", led_cycle.t_on, led_cycle.t_off);
        if(xQueueReceive(led_cycle_q, &led_cycle_dummy, TICKS_TO_TIMEOUT_Q)==pdTRUE){
            //ESP_LOGI(TAG, "light cycle updated: t_on: %d, t_off: %d", led_cycle.t_on, led_cycle.t_off);
            led_cycle = led_cycle_dummy;
            if(led_cycle.t_on != DEF_VALUE && led_cycle.t_off != DEF_VALUE){
                time_future = time_now+led_cycle.t_on*HOURS_TO_SECONDS;
                //ESP_LOGI(TAG, "Time future:%d", time_future);
                cycle_side = true;
                xQueueSendToBack(led_intens_q, &val_on, TICKS_TO_TIMEOUT_Q);
            }
        }
        if(time_now>=time_future){
            if(cycle_side){
                cycle_side = false;
                //ESP_LOGI(TAG, "send off");
                xQueueSendToBack(led_intens_q, &val_off, TICKS_TO_TIMEOUT_Q);
                time_future = time_now + led_cycle.t_off*HOURS_TO_SECONDS;
                //ESP_LOGI(TAG, "t_off: %d Time future on:%d", time_future, led_cycle.t_off);
            }
            else{
                cycle_side = true;
                //ESP_LOGI(TAG, "send on");
                xQueueSendToBack(led_intens_q, &val_on, TICKS_TO_TIMEOUT_Q);
                time_future = time_now + led_cycle.t_on*HOURS_TO_SECONDS;
                //ESP_LOGI(TAG, "t_on: %d Time future off:%d", led_cycle.t_on,time_future);
            }
        }
    }
}

void timer_callback(){
    seconds++;
    ESP_LOGI(TAG, "SECONDS:%lu", seconds);
    nvs_set_u32(permanent_data_handle, KEY_SECONDS, seconds);
    xQueueSendToBack(timer_q, &seconds, TICKS_TO_TIMEOUT_Q);
}

void app_main() {
    ESP_LOGI(TAG, "FIRMWARE STARTED");
    init_queues();
    init_semaphores();
    esp_event_loop_create_default();
    init_nvs();
    init_ledc();
    init_gpio();
    init_adc();
    esp_err_t err;
    err = nvs_set_str(permanent_data_handle, KEY_WIFI_NAME, DEFAULT_WIFI_SSID);
    ESP_LOGI(TAG, "wifi name err %d",err);
    err = nvs_set_str(permanent_data_handle, KEY_WIFI_PASS, DEFAULT_WIFI_PASSWORD);
    ESP_LOGI(TAG, "wifi pass err %d",err);
    nvs_commit(permanent_data_handle);

    init_wifi();
    nvs_get_u32(permanent_data_handle, KEY_SECONDS, &seconds);
    base_timer = xTimerCreate("timer_growbox",1000/portTICK_PERIOD_MS,pdTRUE,(void*)1,timer_callback);
    xTimerStart(base_timer, 100);
    xTaskCreate(airpump_task, "air_pump_task", 2000, NULL, 1, &air_pump_task_handle);
    xTaskCreate(adc_task, "adc_task", 2000, NULL, 2, &adc_task_handle);
    xTaskCreate(fan_task, "fan_task", 2000, NULL, 1, &fan_task_handle);
    xTaskCreate(sensor_task, "sensor_task", 3500, NULL, 3, &i2c_task_handle);
    xTaskCreate(led_control_task, "led_control", 2500, NULL, 2, &led_control_task_handle);
    xTaskCreate(timer_task, "led_timer_task", 3500, NULL, 3, &timer_task_handle);

    for(;;){
        vTaskDelay(300);
    }
}
