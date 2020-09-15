#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#include "motor_controll.h"

void app_main() {
    xTaskCreate(motor_control, "Motor Controll Task", 1024, NULL, tskIDLE_PRIORITY + 2, NULL);
}