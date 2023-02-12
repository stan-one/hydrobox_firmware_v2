/**
 * @file web_server.c
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief 
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../include/web_server.h"
#include "../include/config.h"
extern nvs_handle_t permanent_data_handle;

extern SemaphoreHandle_t fan_sem;  
extern SemaphoreHandle_t airpump_sem;
extern SemaphoreHandle_t ph4_sem;
extern SemaphoreHandle_t ph7_sem;

extern QueueHandle_t led_intens_q;
extern QueueHandle_t led_cycle_q;
extern QueueHandle_t uptime_now_q;

extern QueueHandle_t hum_now_q;
extern QueueHandle_t temp_now_q;
extern QueueHandle_t ph_now_q;

static const char *TAG = "webconfig";

char* create_env_json(env_t env){
    char *string = NULL;
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temp", env.temp);
    cJSON_AddNumberToObject(root, "hum", env.hum);
    cJSON_AddNumberToObject(root, "ph", env.ph);
    cJSON_AddNumberToObject(root, "uptime", env.uptime);
    string = cJSON_Print(root);
    cJSON_Delete(root);
    return string;  
}

void safe_cycle(int t_on, int t_off){
    char cycle[10];
    memset(cycle, 0, 10);
    sprintf(cycle, "%d/%d", t_on, t_off);
    nvs_set_str(permanent_data_handle, KEY_CYCLE, cycle);
    nvs_commit(permanent_data_handle);
}


char *create_prev_json(prev_state_t prev){
    char *string = NULL;
    cJSON *prev_json = cJSON_CreateObject();
    cJSON_AddBoolToObject(prev_json, "fan", prev.fan);
    cJSON_AddBoolToObject(prev_json, "airpump", prev.airpump);
    cJSON_AddNumberToObject(prev_json, "led_intens", prev.led_intens);
    cJSON_AddStringToObject(prev_json, "led_cycle", prev.str_cycle);
    string = cJSON_Print(prev_json);
    cJSON_Delete(prev_json);
    return string;
}

/*
void initialise_mdns(void){
    mdns_init();
    mdns_hostname_set(MDNS_HOST_NAME);
    mdns_instance_name_set("growbox server instance");

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}
*/


const httpd_uri_t env_uri = {
    .uri       = "/sensors",
    .method    = HTTP_GET,
    .handler   = env_get_handle,
    .user_ctx  = (void*)"sensor data"
};

const httpd_uri_t fan_uri = {
    .uri       = "/fan",
    .method    = HTTP_POST,
    .handler   = fan_post_handle,
    .user_ctx  = (void*)"set fan"
};

const httpd_uri_t airpump_uri = {
    .uri       = "/airpump",
    .method    = HTTP_POST,
    .handler   = airpump_post_handle,
    .user_ctx  = (void*)"set airpump"
};

const httpd_uri_t led_uri = {
    .uri       = "/led",
    .method    = HTTP_POST,
    .handler   = led_post_handle,
    .user_ctx  = (void*)"set led"
};

const httpd_uri_t cycle_uri = {
    .uri       = "/cycle",
    .method    = HTTP_POST,
    .handler   = cycle_post_handle,
    .user_ctx  = (void*)"set cycle"
};

const httpd_uri_t prev_uri = {
    .uri       = "/prev",
    .method    = HTTP_GET,
    .handler   = load_prev_get_handle,
    .user_ctx  = (void*)"prev"
};


const httpd_uri_t config_ph_uri = {
    .uri       = "/config_ph",
    .method    = HTTP_GET,
    .handler   = config_ph_post_handle,
    .user_ctx  = (void*)"config_ph"
};

esp_err_t config_ph_post_handle(httpd_req_t *req){
    //GET RAW DATA
    cJSON *control;
    char* buf;
    int ret;
    buf = (char*)malloc(req->content_len);
    ret = httpd_req_recv(req,buf,req->content_len);
    if(ret == 0){
        ESP_LOGI(TAG, "ERROR CONFIG PH DATA EMPTY");
        httpd_resp_send(req, HTTPD_400, HTTPD_RESP_USE_STRLEN);
        return ESP_ERR_HTTPD_HANDLER_EXISTS;
    }
    //CONVERT TO JSON AND PROCESS
    control = cJSON_Parse(buf);
    cJSON *ph_js = cJSON_GetObjectItemCaseSensitive(control, "ph");
    if(cJSON_IsNumber(ph_js)){
        if(ph_js->valueint == 4){
            xSemaphoreGive(ph4_sem);
        }
        else{
             xSemaphoreGive(ph7_sem);
        }
       
    }
    free(buf);
    httpd_resp_send(req, HTTPD_200, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t env_get_handle(httpd_req_t *req){
    char *resp;
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    env_t env_now = {.hum = DEF_VALUE, .ph = DEF_VALUE, .temp = DEF_VALUE, .uptime = DEF_VALUE};
    xQueueReceive(temp_now_q, &env_now.temp, pdMS_TO_TICKS(QUEUE_TIMEOUT_MS));
    xQueueReceive(hum_now_q, &env_now.hum, pdMS_TO_TICKS(QUEUE_TIMEOUT_MS));
    xQueueReceive(ph_now_q, &env_now.ph, pdMS_TO_TICKS(QUEUE_TIMEOUT_MS));
    xQueueReceive(uptime_now_q, &env_now.uptime, pdMS_TO_TICKS(QUEUE_TIMEOUT_MS));
    resp = create_env_json(env_now);
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t fan_post_handle(httpd_req_t *req){
    //GET RAW DATA
    cJSON *control;
    char* buf;
    int ret;
    buf = (char*)malloc(req->content_len);
    ret = httpd_req_recv(req,buf,req->content_len);
    if(ret == 0){
        ESP_LOGI(TAG, "ERROR FAN DATA EMPTY");
        httpd_resp_send(req, HTTPD_400, HTTPD_RESP_USE_STRLEN);
        return ESP_ERR_HTTPD_HANDLER_EXISTS;
    }
    //CONVERT TO JSON AND PROCESS
    control = cJSON_Parse(buf);
    cJSON *fan_js = cJSON_GetObjectItemCaseSensitive(control, "fan");
    if(cJSON_IsBool(fan_js)){
        nvs_set_i8(permanent_data_handle, KEY_FAN, (uint8_t)fan_js->valueint);
        nvs_commit(permanent_data_handle);
        xSemaphoreGive(fan_sem);
        ESP_LOGI(TAG, "sem fan send");
    }
    free(buf);
    httpd_resp_send(req, HTTPD_200, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t airpump_post_handle(httpd_req_t *req){
    //GET RAW DATA
    cJSON *control;
    char* buf;
    int ret;
    buf = (char*)malloc(req->content_len);
    ret = httpd_req_recv(req,buf,req->content_len);
    if(ret == 0){
        ESP_LOGI(TAG, "ERROR AIRPURMP DATA EMPTY");
        httpd_resp_send(req, HTTPD_400, HTTPD_RESP_USE_STRLEN);
        return ESP_ERR_HTTPD_HANDLER_EXISTS;
    }
    //CONVERT TO JSON AND PROCESS
    control = cJSON_Parse(buf);
    cJSON *pump_js = cJSON_GetObjectItemCaseSensitive(control, "airpump");
    if(cJSON_IsBool(pump_js)){
        nvs_set_i8(permanent_data_handle, KEY_AIRPUMP, (uint8_t)pump_js->valueint);
        nvs_commit(permanent_data_handle);
        xSemaphoreGive(airpump_sem);
        ESP_LOGI(TAG, "sem airpump send");
    }
    free(buf);
    httpd_resp_send(req, HTTPD_200, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}



esp_err_t led_post_handle(httpd_req_t *req){
    //GET RAW DATA
    cJSON *control;
    char* buf;
    int ret;
    buf = (char*)malloc(req->content_len);
    ret = httpd_req_recv(req,buf,req->content_len);
    if(ret == 0){
        ESP_LOGI(TAG, "ERROR LED INTENS DATA EMPTY");
        httpd_resp_send(req, HTTPD_400, HTTPD_RESP_USE_STRLEN);
        return ESP_ERR_HTTPD_HANDLER_EXISTS;
    }
    //CONVERT TO JSON AND PROCESS
    control = cJSON_Parse(buf);
    cJSON *led_js = cJSON_GetObjectItemCaseSensitive(control, "leds");
    if(cJSON_IsBool(led_js)){
        nvs_set_i8(permanent_data_handle, KEY_LED, (uint8_t)led_js->valueint);
        nvs_commit(permanent_data_handle);
        uint8_t hold = (uint8_t)led_js->valueint;
        xQueueSendToBack(led_intens_q, &hold, pdMS_TO_TICKS(QUEUE_TIMEOUT_MS));
        ESP_LOGI(TAG, "led intens send to queue");
    }
    free(buf);
    httpd_resp_send(req, HTTPD_200, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t cycle_post_handle(httpd_req_t *req){
    //GET RAW DATA
    cJSON *control;
    char* buf;
    int ret;
    buf = (char*)malloc(req->content_len);
    ret = httpd_req_recv(req,buf,req->content_len);
    if(ret == 0){
        ESP_LOGI(TAG, "ERROR LED CYCLE DATA EMPTY");
        httpd_resp_send(req, HTTPD_400, HTTPD_RESP_USE_STRLEN);
        return ESP_ERR_HTTPD_HANDLER_EXISTS;
    }
    //CONVERT TO JSON AND PROCESS
    control = cJSON_Parse(buf);
    led_cycle_t led;
    cJSON *t_on = cJSON_GetObjectItemCaseSensitive(control, "t_on");
    cJSON *t_off = cJSON_GetObjectItemCaseSensitive(control, "t_off");
    if(cJSON_IsNumber(t_on) && cJSON_IsNumber(t_off)){
        led.t_on = t_on->valueint;
        led.t_off = t_off->valueint;
        safe_cycle(led.t_on, led.t_off);
        xQueueSendToBack(led_cycle_q, &led, TICKS_TO_TIMEOUT_Q);
    }
    free(buf);
    httpd_resp_send(req, HTTPD_200, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t load_prev_get_handle(httpd_req_t *req){
    prev_state_t prev;
    size_t textlen_1 = 10;
    memset(prev.str_cycle, 0, 10);
    int8_t hold;
    char *string; 
    char cycle[10]; memset(cycle, 0, 10);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    nvs_get_i8(permanent_data_handle, KEY_FAN, &hold);
    prev.fan = (bool)hold;
    nvs_get_i8(permanent_data_handle, KEY_AIRPUMP, &hold);
    prev.airpump = (bool)hold;
    nvs_get_i8(permanent_data_handle, KEY_LED, &prev.led_intens);
    nvs_get_str(permanent_data_handle, KEY_CYCLE, prev.str_cycle, &textlen_1);
    string = create_prev_json(prev);
    httpd_resp_send(req, string, HTTPD_RESP_USE_STRLEN);
    free(string);
    return ESP_OK;
}


httpd_handle_t start_webserver(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &env_uri);
        httpd_register_uri_handler(server, &prev_uri);
        httpd_register_uri_handler(server, &fan_uri);
        httpd_register_uri_handler(server, &led_uri);
        httpd_register_uri_handler(server, &airpump_uri);
        httpd_register_uri_handler(server, &cycle_uri);
        httpd_register_uri_handler(server, &config_ph_uri);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}