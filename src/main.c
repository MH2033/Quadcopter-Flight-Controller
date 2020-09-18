#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"

#include "motor_controll.h"

/*A queue for storing power factors for each motor in order to balance the quadcopter*/
QueueHandle_t power_factor_q = NULL;
QueueHandle_t motor_speed_q = NULL;

void app_main() {
    /**/
    power_factor_q = xQueueCreate(2, sizeof(struct motor_data));
    motor_speed_q = xQueueCreate(2, sizeof(struct motor_data));

    xTaskCreate(motor_control, "Motor Controll Task", 2048, NULL, tskIDLE_PRIORITY + 2, NULL);
}