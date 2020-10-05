#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_attr.h"
#include "esp_log.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#include "motor_controll.h"

#define PWM_FREQ 1000

#define FL_MOTOR 32
#define FR_MOTOR 17

#define RL_MOTOR 25
#define RR_MOTOR 16

extern QueueHandle_t power_factor_q;
extern QueueHandle_t motor_speed_q;

void set_motor_speed(enum motor_id id, double power_multiplier, double speed){

    switch (id) {
        case front_left:
            ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0A, power_multiplier * speed));
            break;
        case front_right:
            ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM0B, power_multiplier * speed));
            break;
        
        case rear_left:
            ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM0A, power_multiplier * speed));
            break;

        case rear_right:
            ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM0B, power_multiplier * speed));
            break;

        default:
            break;
    }
}

void motor_control(void *pvParameters){

    /*Initialize motor's power factors*/
    struct motor_data power_factors;
    power_factors.fl = 1.0;
    power_factors.fr = 1.0;
    power_factors.rl = 1.0;
    power_factors.rr = 1.0;

    /*Initilize motor's speed*/
    struct motor_data motor_speeds;
    motor_speeds.fl = 0.0;
    motor_speeds.fr = 0.0;
    motor_speeds.rl = 0.0;
    motor_speeds.rr = 0.0;

    /*Motor Controll pin initialization*/
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, FL_MOTOR));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, FR_MOTOR));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, RL_MOTOR));
    ESP_ERROR_CHECK(mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, RR_MOTOR));

    /*Configuring mcpwm parameters*/
    mcpwm_config_t pwm_config;
    pwm_config.frequency = PWM_FREQ;    
    pwm_config.cmpr_a = 0;  
    pwm_config.cmpr_b = 0;   
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config));
    ESP_ERROR_CHECK(mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config));

    for(;;){
        
        /*Recieve new power factors for each motor*/
        if(uxQueueMessagesWaiting(power_factor_q) > 0){
            ESP_LOGI("Motor", "Recived %f", motor_speeds.fl);
            xQueueReceive(power_factor_q, &power_factors, 0);
        }

        if(uxQueueMessagesWaiting(motor_speed_q) > 0){
            
            xQueueReceive(motor_speed_q, &motor_speeds, 0);
            
        }
        
        set_motor_speed(front_left, power_factors.fl, motor_speeds.fl);
        set_motor_speed(front_right, power_factors.fr, motor_speeds.fr);
        set_motor_speed(rear_left, power_factors.rl, motor_speeds.rl);
        set_motor_speed(rear_right, power_factors.rr, motor_speeds.rr);

        vTaskDelay(20 / portTICK_PERIOD_MS);

    }



    vTaskDelete(NULL);
}