/**
 * @file web_server.h
 * 
 * 
 * 
 * @defgroup webserver Web Service
 * @author Stanimir Hristov (stanimirhristov@openbotanics.org)
 * @brief Setup of the webserver used as a user interface.
 * Process incoming data from the front-end.<br>
 * In this firmware there are no query paramters.
 * @version 0.1
 * @date 2022-10-01
 * @copyright Copyright (c) 2022
 * @{
 */

#ifndef WEB_SERVER_H__
#define WEB_SERVER_H__

#include <esp_http_server.h>
#include <esp_err.h>
#include <esp_log.h>
#include "cJSON.h"
//#include "mdns.h"
#include <string.h>
#include "nvs.h"
#include "nvs_flash.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "data_structures.h"

#define HTTPD_401 "401 UNAUTHORIZED"

#define MDNS_HOST_NAME "hydrobox"

#define DEF_VALUE -1
#define ERR_STR "ERR"

/**
 * @defgroup http_get HTTP GET handles
 * @brief Handles that process HTTP GET requests. From the frond-end to the back-end
 * @{
 */


/**
 * @brief Handle request for the current envoirment inside the box
 * @date 01-10-2022
 * @param req 
 * @return esp_err_t 
 */
esp_err_t env_get_handle(httpd_req_t *req);


/**
 * @brief Load the current state of the mainbox and set it to the front end
 * @author Stanimir Hristov
 * @date 12-10-2022
 * @param req 
 * @return esp_err_t 
 */
esp_err_t load_prev_get_handle(httpd_req_t *req);


/**@}*/


/**
 * @brief 
 * @defgroup http_post HTTP POST handles
 * @brief Handles that process HTTP POST requests. From the frond-end to the back-end
 * @{
 * 
 */

/**
 * @brief Receives the control option for the fan
 * @date 02-12-2023
 * @param req 
 * @return esp_err_t 
 */
esp_err_t fan_post_handle(httpd_req_t *req);

/**
 * @brief Receives the control option for the airpump
 * @date 02-12-2023
 * @param req 
 * @return esp_err_t 
 */
esp_err_t airpump_post_handle(httpd_req_t *req);

/**
 * @brief Receives the control option for the led
 * @date 02-12-2023
 * @param req 
 * @return esp_err_t 
 */
esp_err_t led_post_handle(httpd_req_t *req);

/**
 * @brief Receives the control option for the light cycle
 * @date 02-12-2023
 * @param req 
 * @return esp_err_t 
 */
esp_err_t cycle_post_handle(httpd_req_t *req);

/**
 * @brief Receives calibration request for the ph probe
 * @date 02-12-2023
 * @param req 
 * @return esp_err_t 
 */
esp_err_t config_ph_post_handle(httpd_req_t *req);

/**@}*/

/**
 * @defgroup json JSON object processing
 * @brief Create outcomming JSONs and read incomming JSONs
 * @{
 */


/**
 * @brief Load the sensor data and create a JSON object
 * @date 12-10-2022
 * @author Stanimir Hristov
 * @param env 
 * @return char* 
 */
char* create_env_json(env_t env);

/**
 * @brief Convert the previous configuration ot json. used to load the state of the webpage
 * @author Stanimir Hristov
 * @date 12-10-2022
 * @param prev 
 * @return char* 
 */
char *create_prev_json(prev_state_t prev);

/**@}*/

/**
 * @brief initialize mDNS. This allows the user to set in the browser a name and not a ip adress
 * @author Stanimir Hristov
 * @date 12-10-2022
 */
//void initialise_mdns();


/**
 * @brief Register URIs and start the web service
 * @author Stanimir Hristov
 * @date 12-10-2022
 * @return httpd_handle_t 
 */
httpd_handle_t start_webserver();

/**
 * @brief Save in NVS the current light cycle
 * @author Stanimir Hristov
 * @date 12-10-2022
 * @param t_on 
 * @param t_off 
 */
void safe_cycle(int t_on, int t_off);


#endif 

/**@}*/