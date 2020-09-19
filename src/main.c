#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "motor_controll.h"
#include "wifi_comunication.h"
#include "udp_server.h"

/*A queue for storing power factors for each motor in order to balance the quadcopter*/
QueueHandle_t power_factor_q = NULL;
QueueHandle_t motor_speed_q = NULL;

void app_main() {
  esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /*Initializing WiFi comunication*/
    wifi_init_softap();

    /*Creating queues for motor control*/
    power_factor_q = xQueueCreate(2, sizeof(struct motor_data));
    motor_speed_q = xQueueCreate(2, sizeof(struct motor_data));

    /*Starting motor control task*/
    xTaskCreate(motor_control, "Motor Controll", 2048, NULL, tskIDLE_PRIORITY + 2, NULL);

    /*Starting udp server in order to recieve control signals from application*/
    xTaskCreate(udp_server, "UPD Server", 4096, NULL, tskIDLE_PRIORITY + 2, NULL);
}