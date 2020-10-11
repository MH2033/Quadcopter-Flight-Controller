#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <cJSON.h>
#include <motor_controll.h>

extern QueueHandle_t motor_speed_q;

void handle_control_data(char *str){
    //Parse the recieved JSON string
    cJSON *json = cJSON_Parse(str);
    // ESP_LOGI(TAG, "%s", str);
    //Extract different JSON objects
    cJSON *power_obj = cJSON_GetObjectItemCaseSensitive(json, "power");
    cJSON *pitch_obj = cJSON_GetObjectItemCaseSensitive(json, "pitch");
    cJSON *roll_obj = cJSON_GetObjectItemCaseSensitive(json, "roll");
    cJSON *yaw_obj = cJSON_GetObjectItemCaseSensitive(json, "yaw");
    
    //Creating a motor_data instance to save the controll data
    struct motor_data *control_data = malloc(sizeof(struct motor_data));

    //Getting control data from objects
    double power = 0, pitch = 0, roll = 0, yaw = 0;

    //Error flag for reporting error in the sent data
    esp_err_t err = ESP_OK;

    power = power_obj -> valueint;
    pitch = pitch_obj -> valueint;
    roll = roll_obj -> valueint;
    yaw = yaw_obj -> valueint;

    if(err == ESP_OK){
        control_data -> fl = control_data -> fr = control_data -> rl = control_data -> rr = power;
        if(pitch >= 0){
            control_data -> rl += pitch;
            control_data -> rr += pitch;
        }

        else {
            control_data -> fr += -pitch;
            control_data -> fl += -pitch;
        }
        
        if(roll >= 0){
            control_data -> fl += roll;
            control_data -> rl += roll;
        }

        else {
            control_data -> fr += -roll;
            control_data -> rr += -roll;
        }

        if(yaw >= 0){
            control_data -> fl += yaw;
            control_data -> rr += yaw;
        }

        else {
            control_data -> fr += -yaw;
            control_data -> rl += -yaw;
        }
        xQueueSend(motor_speed_q, (void *)control_data, 0);
    }
    cJSON_Delete(json);
    free(control_data);
}