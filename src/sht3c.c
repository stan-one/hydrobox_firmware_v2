/**
 * @file sht3c.c
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief 
 * @version 0.1
 * @date 2022-10-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sht3c.h"

const static char* TAG = "sensor";

/**
 * @ingroup shtc3
 * @defgroup i2c I2C
 * @brief I2C peripheral setup 
 * @{
 */
i2c_config_t conf_i2c = {
    .mode = I2C_MODE_MASTER,
    .scl_io_num = SCL_PIN,
    .sda_io_num = SDA_PIN,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = CLOCK_SPEED,
    .clk_flags = 0
};

void setup_i2c(){
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_param_config(i2c_master_port, &conf_i2c);
    i2c_driver_install(i2c_master_port, conf_i2c.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    i2c_set_timeout(I2C_MASTER_NUM, I2C_TIMEOUT);
}

/**
 * @}
 */
esp_err_t read_sht3c_id(){
    uint8_t id_lsb = SHTC3_ID&0x00ff;
    uint8_t id_msb = SHTC3_ID>>8;
    uint8_t id[2] = {id_msb, id_lsb};
    uint8_t answer[2]; memset(answer, 0, 2*sizeof(uint8_t));
    esp_err_t err;
    err = i2c_master_write_read_device(I2C_MASTER_NUM, SHTC3_ADRESS, id, 2,answer, 2, TICKS_TO_TIMEOUT);
    ESP_LOGI(TAG, "msb: %02x lsb: %02x", answer[0], answer[1]);
    return err;
}

esp_err_t wake_up_sht3c(){
    uint8_t wakeup[2] = {SHTC3_WAKEUP>>8, SHTC3_WAKEUP&0x00ff};
    return i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_ADRESS, wakeup, sizeof(wakeup), TICKS_TO_TIMEOUT);

}

esp_err_t sleep_sht3c(){
    uint8_t sleep[2] = {SHTC3_SLEEP>>8, SHTC3_SLEEP&0x00ff};
    return i2c_master_write_to_device(I2C_MASTER_NUM, SHTC3_ADRESS, sleep, 2, TICKS_TO_TIMEOUT);
}

esp_err_t read_data_sht3c(float *temperature, float *humidity){
    //i2c_cmd_handle_t cmd;
    uint8_t sensor_response[6]; memset(sensor_response, 0, 6*sizeof(uint8_t));
    uint8_t command_mes[2] = {SHTC3_MEAUSURE_CMD>>8, SHTC3_MEAUSURE_CMD&0x00ff};
    //cmd = i2c_cmd_link_create();
    //WAKEUP

    //
    i2c_master_write_read_device(I2C_MASTER_NUM, SHTC3_ADRESS, command_mes, sizeof(command_mes),sensor_response, sizeof(sensor_response), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    /*
    //WRITE
    ESP_LOGI(TAG, "%d",i2c_master_start(cmd));
    ESP_LOGI(TAG, "%d",i2c_master_write_byte(cmd, (SHTC3_ADRESS<<1) | I2C_MASTER_WRITE, I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_write(cmd,command_mes, sizeof(command_mes),I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_start(cmd));
    ESP_LOGI(TAG, "write finished");
    //READ
    ESP_LOGI(TAG, "%d",i2c_master_write_byte(cmd, (SHTC3_ADRESS<<1) | I2C_MASTER_READ, I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_read_byte(cmd,&sensor_response[0], I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_read_byte(cmd,&sensor_response[1], I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_read_byte(cmd,&sensor_response[2], I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_read_byte(cmd,&sensor_response[3], I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_read_byte(cmd,&sensor_response[4], I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_read_byte(cmd,&sensor_response[5], I2C_MASTER_ACK));
    ESP_LOGI(TAG, "%d",i2c_master_stop(cmd));
    ESP_LOGI(TAG, "%d",i2c_master_cmd_begin(I2C_MASTER_NUM,cmd,TICKS_TO_TIMEOUT));
    i2c_cmd_link_delete(cmd);
    */
    //SLEEP
    //sleep_sht3c();
    /*
    for(int i= 0; i<6; i++){
        ESP_LOGI(TAG, "i:%d %02x",i,sensor_response[i]);
    }
    */
   uint16_t raw_temp = (sensor_response[0]<<8)|sensor_response[1];
   uint16_t raw_hum = (sensor_response[3]<<8)|sensor_response[4];
   *temperature = -45+175*((float)raw_temp/65536);
   *humidity = 100*((float)raw_hum/65536);
    
    return ESP_OK;
}