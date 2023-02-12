/**
 * @file sht3c.h
 * @defgroup shtc3 Sensor
 * @brief  In this project the SHT3C sensor is used. Is a small scale and low cost humidity and temperature sensor.
 * The SHTC3 sensor has various option for i2c communication.<br>
 * It can comunicate with or without clock streching and also depending of the request command the sensor will answer
 * either with temperature or humidity first. Check the <a href="https://sensirion.com/media/documents/643F9C8E/6164081E/Sensirion_Humidity_Sensors_SHTC3_Datasheet.pdf">datasheet</a>  for more info
 * @todo Implement the CRC checksum.
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @version 0.1
 * @date 2022-10-12
 * @copyright Copyright (c) 2022
 * @{
 * 
 */

#ifndef SHT3C_H
#define SHT3C_H

#include <driver/i2c.h>
#include "config.h"
#include <esp_log.h>

#define SHTC3_ADRESS 0x70
#define SHTC3_ID 0xEFC8
#define SHTC3_WAKEUP 0x3517
#define SHTC3_SLEEP 0xB098
#define SHTC3_MEAUSURE_CMD 0x7CA2 // STRECHING TEMPERATURE FIRST

#define CLOCK_SPEED 400000
#define I2C_MASTER_NUM 0
/**
 * @warning During measurment the sensor pulls low the CLK line (clock streching). The defualt timeout for the ESP32C3 is to short thats why
 * its updated from 10 (default) 30. Numbers reference the APB clock frequency of the ESP32C3.
 * @todo The value of 30 was found to work, but no further tests were done. Check to see if its optimal/roboust.
 */
#define I2C_TIMEOUT 30

#define I2C_MASTER_TIMEOUT_MS 1000

#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TX_BUF_DISABLE 0

#define TICKS_TO_TIMEOUT 10000

/**
 * @defgroup i2c I2C
 * @ingroup shtc3
 * @brief Set the up i2c pheriperal 
 * @{
 */
void setup_i2c();
/**@}*/

/**
 * @brief Read the ID of the sensor. Checks if the sensor is avalible.
 * 
 * @return esp_err_t 
 */
esp_err_t read_sht3c_id();
/**
 * @brief Send the wake-up command to the sensor
 * 
 * @return esp_err_t 
 */
esp_err_t wake_up_sht3c();
/**
 * @brief Send the sleep command to the sensor
 * 
 * @return esp_err_t 
 */
esp_err_t sleep_sht3c();
/**
 * @brief Read the temperature and the humidity from the sensor
 * 
 * @param temperature 
 * @param humidity 
 * @return esp_err_t 
 */
esp_err_t read_data_sht3c(float *temperature, float *humidity);

#endif

/**@}*/