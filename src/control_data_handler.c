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
    
    // ESP_LOGI(TAG, "Json objects created");
    //Creating a motor_data instance to save the controll data
    struct motor_data *control_data = malloc(sizeof(struct motor_data));

    // ESP_LOGI(TAG, "Control Data structure allocated");
    //Getting control data from objects
    double power = -1, pitch = -1, roll = -1, yaw = -1;

    //Error flag for reporting error in the sent data
    esp_err_t err = ESP_OK;
    // ESP_LOGI(TAG, "power value: %f", power_obj -> valuedouble);
    if((power_obj -> valuedouble) <= 100 && (power_obj -> valuedouble) >= 0){
        power = power_obj -> valuedouble;
    }

    else {
        err = ESP_FAIL;
    }
    
    if((pitch_obj -> valuedouble) <= 20 && (pitch_obj -> valuedouble) >= -20){
        pitch = pitch_obj -> valuedouble;
    }

    else {
        err = ESP_FAIL;
    }

    if((roll_obj -> valuedouble) <= 20 && (roll_obj -> valuedouble) >= -20){
        roll = roll_obj -> valuedouble;
    }

    else {
        err = ESP_FAIL;
    }

    if((yaw_obj -> valuedouble) <= 20 && (yaw_obj -> valuedouble) >= -20){
        yaw = yaw_obj -> valuedouble;
    }
    
    else {
        err = ESP_FAIL;
    }

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
        // ESP_LOGI(TAG, "Reached Queue");
        xQueueSend(motor_speed_q, (void *)control_data, 0);
    }
    cJSON_Delete(json);
    free(control_data);
}